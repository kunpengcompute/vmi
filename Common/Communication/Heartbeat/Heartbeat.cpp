/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2021. All rights reserved.
 * Description: 通信心跳模块
 */
#define LOG_TAG "Heartbeat"

#include "Heartbeat.h"
#include <ctime>
#include <cstring>
#include <unistd.h>
#include <cinttypes>
#include "logging.h"
#include "SystemProperty.h"
#include "SmartPtrMacro.h"
#include "EngineEventHandler.h"

namespace {
    constexpr unsigned int MICROSECOND_OFFSET = 500;
    constexpr int CHECK_DURATION = 30;   // check duration
    constexpr int AVERAGE_NUM = 10;      // calculate network average delay with the last 10 heartbeat data
    constexpr int HEARTBEAT_SEND_INTERVAL = 100;   // send heartbeat interval
    constexpr int SECOND_TO_MICRO = 1000000;
    constexpr int SECOND_TO_MILL = 1000;
    constexpr int NANO_TO_MICRO = 1000;
    constexpr int GET_DATA_FROM_QUEUE_MIN_WAIT_TIME = 1000;
}

namespace Vmi {
/**
 * @brief: get microsecond
 * @param [in] t: time
 * @return: microsecond
 */
static inline uint64_t GetMicroSecond(struct timespec t)
{
    return (static_cast<uint64_t>(t.tv_sec) * SECOND_TO_MICRO +
        (static_cast<uint64_t>(t.tv_nsec) + MICROSECOND_OFFSET) / NANO_TO_MICRO);
}

/**
 * @brief: heartbeat has for function:
 *               1、timing send heartbeat (SendHeartbeatRequest);
 *               2、handle recv heartbeat (RecvHeartbeatResponse);
 *               3、compute net loop;
 *               4、check net status.
 */
void Heartbeat::TaskentryHeartbeat()
{
    INFO("HEARTBEAT thread entering");
    StreamHeartbeatMsg msg {};
    struct timespec sendTime = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &sendTime);
    m_nextSendTime = GetMicroSecond(sendTime);

    while (!m_stopFlag) {
        SendHeartbeatRequest(msg);
        RecvHeartbeatResponse();
        CheckNetStatus();
    }
    INFO("HEARTBEAT thread exited");
}

/**
 * @brief: send heartbeat request
 * @param [in] streamMessage: socket package
 */
void Heartbeat::SendHeartbeatRequest(StreamHeartbeatMsg &streamMessage)
{
    struct timespec sendTime = {0, 0};

    clock_gettime(CLOCK_MONOTONIC, &sendTime);
    streamMessage.heartbeatMsg.sendMicroSecond = GetMicroSecond(sendTime);
    if ((streamMessage.heartbeatMsg.sendMicroSecond + MICROSECOND_OFFSET) < m_nextSendTime) { // Rounding
        return;
    }
    if (m_latencyQueue.size() >= static_cast<size_t>(CHECK_DURATION)) {
        m_latencyQueue.pop_front();
    }

    streamMessage.heartbeatMsg.type = HeartbeatPackageType::HEARTBEAT_REQ;
    streamMessage.heartbeatMsg.seqNum = ++m_sendSeqNum;

    uint64_t sendMicroSecond = streamMessage.heartbeatMsg.sendMicroSecond;
    m_latencyQueue.emplace_back(std::make_pair(sendMicroSecond, static_cast<uint64_t>(0)));
    m_nextSendTime += HEARTBEAT_SEND_INTERVAL * SECOND_TO_MILL;

    if (m_msgFragment.FragmentSend(VMIMsgType::HEARTBEAT, m_vmiSocket, streamMessage.header, sizeof(HeartbeatMsg))
        != static_cast<int>(sizeof(HeartbeatMsg))) {
        ERR("Failed to send heartbeat request, send seqNum: %ju", m_sendSeqNum);
    }
}

/**
 * @brief: handle recv heartbeat message
 * @param [in] reassembleMsg: recv message
 * @param [in] recvTime: recvTime
 */
void Heartbeat::HandleRecvMsg(const std::pair<uint8_t *, uint32_t> &reassembleMsg, struct timespec recvTime)
{
    uint8_t *msgData = reassembleMsg.first;
    uint32_t msgSize = reassembleMsg.second;
    auto heartbeatMsg = reinterpret_cast<HeartbeatMsg *>(msgData);
    if (heartbeatMsg->type == HeartbeatPackageType::HEARTBEAT_REQ) {
        heartbeatMsg->type = HeartbeatPackageType::HEARTBEAT_RES;

        StreamHeartbeatMsg loopbackMsg = { { }, { 0, 0, HeartbeatPackageType::HEARTBEAT_REQ, {0} } };
        size_t destSize = sizeof(loopbackMsg) - sizeof(StreamMsgHead);
        if (destSize < msgSize) {
            ERR("Failed to handle recv message, msg size:%u is larger than dest size:%zu", msgSize, destSize);
            return;
        }
        memcpy(reinterpret_cast<uint8_t *>(&loopbackMsg) + sizeof(StreamMsgHead), msgData, msgSize);

        auto head = reinterpret_cast<StreamMsgHead *>(&loopbackMsg);
        if (m_msgFragment.FragmentSend(VMIMsgType::HEARTBEAT, m_vmiSocket,
            *head, sizeof(HeartbeatMsg))
            != static_cast<int>(sizeof(HeartbeatMsg))) {
            ERR("Failed to handle recv message, loopback heartbeat msg failed, seqnum(%ju), recvSeqNum(%ju)",
                heartbeatMsg->seqNum, m_recvSeqNum);
        }
    } else if (heartbeatMsg->type == HeartbeatPackageType::HEARTBEAT_RES) {
        CalcLatency(*heartbeatMsg, GetMicroSecond(recvTime));
    } else {
        ERR("Failed to handle recv message, message type:%u is error!", static_cast<uint8_t>(heartbeatMsg->type));
    }
}

/**
 * @brief: recv heartbeat response
 */
void Heartbeat::RecvHeartbeatResponse()
{
    struct timespec recvTime = {0, 0};
    StreamMsgHead *recvMessage = nullptr;
    std::pair<uint8_t *, uint32_t> responseMessage(nullptr, 0);
    uint64_t waitTime = 0;

    do {
        clock_gettime(CLOCK_MONOTONIC, &recvTime);
        waitTime = GetMicroSecond(recvTime);
        if (waitTime < m_nextSendTime) {
            waitTime = m_nextSendTime - waitTime;
        } else {
            waitTime = 0;
        }

        // avoid waitTime is 0 to m_packetQueue.GetNextPktWait
        if (waitTime < GET_DATA_FROM_QUEUE_MIN_WAIT_TIME) {
            waitTime = GET_DATA_FROM_QUEUE_MIN_WAIT_TIME;
        }

        std::pair<uint8_t *, uint32_t> pkt = m_packetQueue.GetNextPktWait(waitTime / SECOND_TO_MILL);
        recvMessage = reinterpret_cast<StreamMsgHead *>(pkt.first);
        if (recvMessage == nullptr) {
            break;
        }

        clock_gettime(CLOCK_MONOTONIC, &recvTime);
        responseMessage = m_msgReassemble.Reassemble(*recvMessage);
        uint8_t* msgData = responseMessage.first;
        uint32_t msgSize = responseMessage.second;
        if (msgData == nullptr) {
            break;
        }

        if (msgSize == sizeof(HeartbeatMsg)) {
            HandleRecvMsg(responseMessage, recvTime);
        } else {
            ERR("Failed to recv heartbeat response, msg size(%u) is not valid size:(%zu)",
                msgSize, sizeof(HeartbeatMsg));
        }

        free(msgData);
        msgData = nullptr;
        responseMessage = std::pair<uint8_t *, uint32_t>(nullptr, 0);
    } while (recvMessage != nullptr);
}

/**
 * @brief: compute net loop
 * @param [in] heartbeatResponseMsg: heartbeat package
 * @param [in] recvTime: recvTime
 */
void Heartbeat::CalcLatency(const HeartbeatMsg &heartbeatResponseMsg, uint64_t recvTime)
{
    if (heartbeatResponseMsg.seqNum != m_recvSeqNum + 1) {
        ERR("Failed to calculate latency, heartbeat response msg order(%ju) is error, expected value is %ju." \
            " There may be a packet loss on the network.!",
            heartbeatResponseMsg.seqNum, m_recvSeqNum + 1);
    }

    m_recvSeqNum = heartbeatResponseMsg.seqNum;
    uint64_t latencyMicroSec = (recvTime > heartbeatResponseMsg.sendMicroSecond) ?
        (recvTime - heartbeatResponseMsg.sendMicroSecond) : 0;
    int size = static_cast<int>(m_latencyQueue.size());

    for (int i = size - 1; i >= 0; --i) {
        if (m_latencyQueue.at(i).first == heartbeatResponseMsg.sendMicroSecond) {
            m_latencyQueue.at(i).second = latencyMicroSec;
            DBG("heartbeat msg: seq = %ju, send_time = %ju, latency = %ju us",
                heartbeatResponseMsg.seqNum,
                heartbeatResponseMsg.sendMicroSecond,
                latencyMicroSec);
            break;
        }
    }  // If not found, it may be a network exception and does not process the data.

    latencyMicroSec = 0;
    uint64_t maxMicroSec = 0;
    int receivePacketCount = 0;
    for (int i = size - 1; (i >= 0) && (i > size - 1 - AVERAGE_NUM); --i) {
        uint64_t tmp = m_latencyQueue.at(i).second;
        latencyMicroSec += tmp;
        maxMicroSec = (maxMicroSec < tmp) ? tmp : maxMicroSec;
        receivePacketCount += (tmp == 0) ? 0 : 1;
    }

    receivePacketCount = (receivePacketCount > 0) ? receivePacketCount : 1;

    m_netLoopbackLatencyMax = maxMicroSec;
    if (latencyMicroSec < INT64_MAX) {
        m_netLoopbackLatencyAverage = static_cast<int64_t>(latencyMicroSec) / static_cast<int64_t>(receivePacketCount);
    } else {
        m_netLoopbackLatencyAverage = -1;
    }
    LOG_RATE_LIMIT(ANDROID_LOG_INFO, 1, "MaxLatency = %ju us, AverageLatency = %jd us",
                   maxMicroSec, m_netLoopbackLatencyAverage);
    int64_t tooBadLatency = m_maxAveLatency * SECOND_TO_MICRO;
    if (m_netLoopbackLatencyAverage > tooBadLatency) {
        ERR("Failed to calculate latency, network average latency:%d is larger than bad latency:%" PRId64 "",
            static_cast<int32_t>(m_netLoopbackLatencyAverage), tooBadLatency);
        EngineEvent engineEvent = {VMI_ENGINE_EVENT_BAD_LATENCY, 0, 0, 0, 0};
        EngineEventHandler::GetInstance().CreateEvent(engineEvent);
    }

    SetProperty("vmi.sys.network.latency.average", std::to_string(m_netLoopbackLatencyAverage));
}

/**
 * @brief: check net status
 */
void Heartbeat::CheckNetStatus()
{
    int size = static_cast<int>(m_latencyQueue.size());
    uint64_t receivePacketCount = 0;
    for (int i = size - 1; i >= 0; --i) {
        receivePacketCount += (m_latencyQueue.at(i).second == 0) ? 0 : 1;
    }

    if ((size >= CHECK_DURATION) && (receivePacketCount == 0)) {
        ERR("Failed to check net status, network may be interrupted or delayed too much." \
            " Lantency queue size:%d, check duration:%d, receive packet count is 0", size, CHECK_DURATION);
        m_stopFlag = true;
        if (m_callback != nullptr) {
            INFO("execute m_callback");
            m_callback(m_vmiSocket);
        } else {
            ERR("Failed to check net status, heartbeat m_callback is nullptr!");
        }
    }
}

/**
 * @brirf: construct
 * @param [in] sock: socket
 * @param [in] fnCallback: check net exception, callbak it
 */
Heartbeat::Heartbeat(VmiSocket& socket, std::function<void(const VmiSocket &)> fnCallback)
    : m_vmiSocket(socket), m_callback(fnCallback)
{
}

Heartbeat::~Heartbeat()
{
    ForceStop();
}

/**
 * @brief: start thread
 */
bool Heartbeat::Start()
{
    std::lock_guard<std::mutex> lck(m_lock);
    int32_t maxAveageLatency = GetPropertyWithDefault("heartbeat.max.aveage.latency", 1); // 单位：秒
    constexpr int32_t latencyLimtit = 10; // 单位：秒
    if (maxAveageLatency <= 0 || maxAveageLatency > latencyLimtit) {
        WARN("Failed to set max aveage latency:%d, latency limit:%d, set it default value 1",
            maxAveageLatency, latencyLimtit);
        maxAveageLatency = 1;
    }
    m_maxAveLatency = maxAveageLatency;
    INFO("Set heartbeat max aveage latency:%d", m_maxAveLatency);
    m_stopFlag = false;
    MAKE_UNIQUE_NOTHROW(m_task, std::thread, std::bind(&Heartbeat::TaskentryHeartbeat, this));
    if (m_task == nullptr) {
        ERR("Failed to start heatbeat, out of memory to alloc heartbeat task");
        return false;
    }
    INFO("Heartbeat start successful.");
    return true;
}

/**
 * @brief: Stop the thread and the caller waits for the thread to exit and hear.
 */
void Heartbeat::ForceStop()
{
    std::lock_guard<std::mutex> lck(m_lock);
    if (m_task != nullptr) {
        m_stopFlag = true;
        m_packetQueue.Cancel();
        m_task->join();
        m_task = nullptr;
    }
    m_netLoopbackLatencyAverage = -1;
    INFO("heartbeat thread exit");
}

/**
 * @brief: put heartbeat data into queue
 * @param [in] packet: heartbeat data
 */
void Heartbeat::Handle(const std::pair<uint8_t*, size_t> &packet)
{
    std::lock_guard<std::mutex> lck(m_lock);
    if (m_task != nullptr) {
        m_packetQueue.PutPkt(std::make_pair(packet.first, static_cast<uint32_t>(packet.second)));
    } else {
        free(packet.first);
    }
}

/**
 * @brief: Get network average latency
 * @return: latency, -1 is Infinity
 */
int64_t Heartbeat::GetNetLoopbackLatencyAverage() const
{
    return m_netLoopbackLatencyAverage;
}

/**
 * @brief: Get the maximum network latency
 * @return: latency, 0 is Infinity
 */
uint64_t Heartbeat::GetNetLoopbackLatencyMax() const
{
    return m_netLoopbackLatencyMax;
}
} // namespace Vmi
