/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2021. All rights reserved.
 * Description: 报文解析模块
 */
#define LOG_TAG "StreamParser"

#include "StreamParser.h"
#include <chrono>
#include <arpa/inet.h>
#include <array>
#include <cassert>
#include <cstring>
#include <thread>
#include <unistd.h>
#include <chrono>
#include "CommonDefine.h"
#include "Communication.h"
#include "logging.h"
#include "SmartPtrMacro.h"
#include "EngineEventHandler.h"

using namespace std;

namespace Vmi {
namespace {
    constexpr int VMI_THREAD_RUNNING = 1;
    constexpr int VMI_THREAD_EXIT = -1;
    constexpr size_t STREAM_HEAD_SIZE = 16;
    constexpr size_t STREAM_MAX_BUF_LEN = FRAGMENT_SIZE_FULL;
}

/**
 * @brief: constructor
 */
StreamParse::StreamParse()
{
    m_serviceHandle.fill(nullptr);
}

StreamParse::~StreamParse()
{
    m_msgTypeCount.clear();
}

/**
 * @brief: set handle with type
 * @param [in] type: type
 * @param [in] serviceHandle: handle
 */
void StreamParse::SetServiceHandle(VMIMsgType type, std::shared_ptr<ServiceHandle> serviceHandle)
{
    if (type <= VMIMsgType::INVALID || type >= VMIMsgType::END) {
        ERR("Failed to set service handle, Invalid message type:%u", type);
        return;
    }
    INFO("Message type:(%u) is setting the service handle", type);
    m_serviceHandle[type] = serviceHandle;
}

/**
 * @brief: get handle with type
 * @param [in] type: type
 * @param [in] serviceHandle: handle
 */
ServiceHandle* StreamParse::GetServiceHandle(VMIMsgType type)
{
    if (type <= VMIMsgType::INVALID || type >= VMIMsgType::END) {
        ERR("Failed to get service handle, Invalid message type:%u", type);
        return nullptr;
    }
    return m_serviceHandle[type].get();
}

/**
 * @brief: check packet
 * @param [in] packetBuffer: packet data buffer
 * @return: packet length or error code
 */
int StreamParse::PacketDelimiter(const std::pair<uint8_t*, size_t> &packetBuffer)
{
    uint8_t *packet = packetBuffer.first;
    size_t len = packetBuffer.second;
    if (packet == nullptr) {
        ERR("Failed to delimit packet, packet is nullptr");
        return -1;
    }
    if (len < sizeof(StreamMsgHead)) {
        ERR("Failed to delimit packet, data len(%zu) < head len(%zu), data must start with stream message head",
            len, sizeof(StreamMsgHead));
        return -1;
    }

    StreamMsgHead *messageHead = reinterpret_cast<StreamMsgHead *>(packet);
    if (messageHead->GetType() >= VMIMsgType::END || messageHead->GetType() <= VMIMsgType::INVALID ||
        !messageHead->VerifyMsgHead()) {
        ERR("Failed to delimit packet, Stream message head verification failed, type:%d, verify:%d",
            messageHead->GetType(), messageHead->VerifyMsgHead());
        return -1;
    }

    return static_cast<int>(messageHead->GetPayloadSize());
}

/**
 * @brief: statistic not process message type count for unit time
 * @param [in] msgType: message type
 */
void StreamParse::NoProcessMsgCount(VMIMsgType msgType)
{
    if (msgType <= VMIMsgType::INVALID || msgType >= VMIMsgType::END) {
        ERR("Failed to statistic no process msg count, Message type is invalid, type:%d,", msgType);
        return;
    }

    // 统计单位时间内未处理消息的数量
    constexpr int64_t rateLimitTime = 1000;
    constexpr int64_t rateLimitTimeMax = 1500;
    auto now = std::chrono::system_clock::now();
    static auto lastSecond = now;
    int64_t timeInterval = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastSecond).count();
    if (timeInterval == 0) { // 首次统计
        m_msgTypeCount[msgType]++;
        WARN("First statistic no process msg count, msg(type %u) handle is nullptr", msgType);
    } else if (timeInterval < rateLimitTime) {
        m_msgTypeCount[msgType]++;
    } else if (timeInterval < rateLimitTimeMax) {
        for (auto& it: m_msgTypeCount) {
            WARN("Msg(type %u) handle is nullptr, no process message type count(%u)",
                it.first, it.second);
        }
        m_msgTypeCount.clear();
        m_msgTypeCount[msgType]++;
        lastSecond = now;
    } else { // 大于1.5秒重新开始统计
        m_msgTypeCount.clear();
        m_msgTypeCount[msgType]++;
        lastSecond = now;
        WARN("Reset no process msg count, msg(type %u) handle is nullptr", msgType);
    }
}


/**
 * @brief: process packet
 * @param [in] packetBuffer: packet data buffer
 * @return: true if success, or false
 */
bool StreamParse::ProcessMsg(const std::pair<uint8_t*, size_t> &packetBuffer)
{
    uint8_t *packetData = packetBuffer.first;
    size_t packetLength = packetBuffer.second;
    if (packetData == nullptr || packetLength == 0 || packetLength > MAX_MSG_SIZE) {
        ERR("Failed to process message, invalid packet, data %s null, len:%zu, max size:%d",
            (packetData == nullptr) ? "is" : "is not", packetLength, MAX_MSG_SIZE);
        return false;
    }
    StreamMsgHead* msgHead = reinterpret_cast<StreamMsgHead *>(packetData);
    if (m_isMsgHeadVerify &&
        !msgHead->VerifyMurmurHash(make_pair(packetData, static_cast<uint32_t>(packetLength)))) {
        ERR("Failed to process message, message data verify hash value failed, pre-message Num(%u)", m_preRecvMsgSeq);
        return false;
    }
    uint32_t curRecvMsgSeq = msgHead->GetMsgSeq();
    DBG("Recv msg with seq:%u", curRecvMsgSeq);
    if (curRecvMsgSeq != m_preRecvMsgSeq + 1) {
        ERR("Failed to process message, message sequence number error, pre-message Num(%u)->cur-message Num(%u)",
            m_preRecvMsgSeq, curRecvMsgSeq);
        return false;
    }
    m_preRecvMsgSeq = curRecvMsgSeq;
    ServiceHandle* pserviceHandle = GetServiceHandle(msgHead->GetType());
    if (pserviceHandle != nullptr) {
        pserviceHandle->Handle(std::make_pair(packetData, packetLength));
    } else {
        NoProcessMsgCount(msgHead->GetType());
        free(packetData);
        packetData = nullptr;
    }

    return true;
}

/**
 * @brief: parse socket stream
 * @param: [in] packetBuffer: packet data buffer
 * @return: true if success, or false
 */
bool StreamParse::ParseStream(const std::pair<uint8_t*, size_t> &packetBuffer)
{
    size_t packetSize = packetBuffer.second;
    if (packetSize <= sizeof(StreamMsgHead)) {
        ERR("Failed to parse stream, packet size:%zu less than stream msg header:%zu",
            packetSize, sizeof(StreamMsgHead));
        return false;
    }
    int packetLength = StreamParse::PacketDelimiter(packetBuffer);
    if (packetLength <= 0) {
        ERR("Failed to parse stream, invalid packet length:%d", packetLength);
        return false;
    }
    packetLength += static_cast<int>(sizeof(StreamMsgHead));
    if (static_cast<size_t>(packetLength) != packetSize) {
        ERR("Failed to parse stream, packet length (%d) not equal to recv packet size (%zu)",
            packetLength, packetSize);
        return false;
    }
    if (!ProcessMsg(packetBuffer)) {
        ERR("Failed to parse stream, pre-message Num(%u)", m_preRecvMsgSeq);
        return false;
    }
    return true;
}

/**
 * @brief: Reset m_preRecvMsgSeq to zero
 */
void StreamParse::ResetRecvSeq()
{
    m_preRecvMsgSeq = 0;
}

/**
 * @brief: get the last successfully received message sequence number
 * @return: message sequence number
 */
uint32_t StreamParse::GetCurRecvMsgSeq() const
{
    return m_preRecvMsgSeq;
}

void StreamParse::SetHeadVerify(bool flag)
{
    m_isMsgHeadVerify = flag;
}

bool StreamParse::GetHeadVerify() const
{
    return m_isMsgHeadVerify;
}

/**
 * @brief: receive fixed size data
 * @param: [in] recvBuffer: buffer to recv data
 * @return: true if success, or false
 */
bool StreamParseThread::RecvFixedSizeData(const std::pair<uint8_t*, size_t> &recvBuffer)
{
    uint8_t* packetRecvBuffer = recvBuffer.first;
    size_t bufLen = recvBuffer.second;
    size_t recvSize = 0;
    size_t offset = 0;
    size_t dataLen = bufLen;
    size_t byte = 0;
    do {
        ssize_t ret = m_vmiSocket.Recv(std::pair<uint8_t*, size_t> {packetRecvBuffer + offset, dataLen});
        if ((ret > 0) && (static_cast<size_t>(ret) <= dataLen)) {
            byte = static_cast<size_t>(ret);
            recvSize += byte;
            offset += byte;
            dataLen -= byte;
        } else if (ret == SOCKET_RECV_FAIL_RETRY) {
            continue;
        } else {
            ERR("Failed to receive fixed size data, socketfd(%d), errno(%zd)",
                m_vmiSocket.GetFd(), ret);
            return false;
        }
    } while (recvSize < bufLen && m_threadStatus == VMI_THREAD_RUNNING);

    return true;
}

/**
 * @brief: cloud phone mode receive data
 * @param: [in] streamParse: streamParse object
 * @param: [in] recvBuffer: recv buffer
 * @return: true if success, or false
 */
bool StreamParseThread::RecvCloudPhoneData()
{
    size_t bufferLen = STREAM_MAX_BUF_LEN;
    uint8_t *recvBufPtr = reinterpret_cast<uint8_t *>(malloc(bufferLen));
    if (recvBufPtr == nullptr) {
        ERR("Failed to receive cloud phone data, alloc receive buf(%zu) failed", bufferLen);
        return false;
    }

    // 接收消息头数据
    if (!RecvFixedSizeData(std::make_pair(recvBufPtr, STREAM_HEAD_SIZE))) {
        ERR("Failed to receive cloud phone data, Socketfd(%d) receive head failed", m_vmiSocket.GetFd());
        free(recvBufPtr);
        recvBufPtr = nullptr;
        return false;
    }

    // 验证消息头，并返回负载数据大小
    int payloadSize = StreamParse::PacketDelimiter(std::make_pair(recvBufPtr, STREAM_HEAD_SIZE));
    if (payloadSize <= 0 || static_cast<size_t>(payloadSize) > STREAM_MAX_BUF_LEN - STREAM_HEAD_SIZE) {
        ERR("Failed to receive cloud phone data, Socketfd(%d) parse msg head occur error, payload size(%d)",
            m_vmiSocket.GetFd(), payloadSize);
        free(recvBufPtr);
        recvBufPtr = nullptr;

        EngineEvent engineEvent = {VMI_ENGINE_EVENT_PKG_BROKEN, 0, 0, 0, 0};
        EngineEventHandler::GetInstance().CreateEvent(engineEvent);
        return false;
    }

    // 接收消息负载数据
    if (!RecvFixedSizeData(std::make_pair(recvBufPtr + STREAM_HEAD_SIZE, static_cast<size_t>(payloadSize)))) {
        ERR("Failed to receive cloud phone data, Socketfd(%d) recv payload failed", m_vmiSocket.GetFd());
        free(recvBufPtr);
        recvBufPtr = nullptr;
        return false;
    }

    // 下发包数据处理
    std::pair<uint8_t*, size_t> packetBuffer =
        std::make_pair(recvBufPtr, STREAM_HEAD_SIZE + static_cast<size_t>(payloadSize));
    if (!m_streamParse->ParseStream(packetBuffer)) {
        ERR("Failed to receive cloud phone data, Socketfd(%d) parse packet data failed", m_vmiSocket.GetFd());
        free(recvBufPtr);
        recvBufPtr = nullptr;
        return false;
    }

    return true;
}

/**
 * @brief: process recv message
 * @param [in] streamParse: streamParse object
 */
void StreamParseThread::ProcessRecvMsg()
{
    while (m_threadStatus == VMI_THREAD_RUNNING) {
        if (!RecvCloudPhoneData()) {
            ERR("Failed to process receive message, receive cloud phone data failed");
            break;
        }

        // 主动释放CPU，避免网络报文过多导致其他线程无法得到调度
        std::this_thread::yield();
    }
}

/**
 * @brief: thread process
 */
void StreamParseThread::Taskentry()
{
    if (m_streamParse == nullptr) {
        ERR("Failed to enter task, stream parse is null");
        return;
    }

    INFO("Stream Parse Thread entering");
    ProcessRecvMsg();
    INFO("The sequence number of the last successfully received message, current receive message sequence(%u)",
         m_streamParse->GetCurRecvMsgSeq());
    INFO("Stream Parse Thread  exited");
}

/**
 * @brief: stream parse thread
 * @param [in] vmiSocket: socket
 */
StreamParseThread::StreamParseThread(VmiSocket& vmiSocket)
    : m_vmiSocket(vmiSocket)
{
}

/**
 * @brief: deconstructor
 */
StreamParseThread::~StreamParseThread()
{
    INFO("Stop parsing message stream");
    (void)Stop();
}

/**
 * @brief: set m_streamParse
 */
void StreamParseThread::SetStreamParse(std::shared_ptr<StreamParse> streamParse)
{
    m_streamParse = streamParse;
    if (m_streamParse != nullptr) {
        m_streamParse->ResetRecvSeq();
    }
}

/**
 * @brief: start thread
 * @return: return 0 if success, -1 falied
 */
int StreamParseThread::Start()
{
    lock_guard<mutex> lck(m_lock);
    m_threadStatus = VMI_THREAD_RUNNING;
    MAKE_UNIQUE_NOTHROW(m_task, std::thread, std::bind(&StreamParseThread::Taskentry, this));
    if (m_task == nullptr) {
        ERR("Failed to start, out of memory to alloc stream parse thread");
        m_threadStatus = VMI_THREAD_EXIT;
        return -1;
    }
    return 0;
}

/**
 * @brief: stop thread
 * @return: return 0 if success, -1 falied
 */
int StreamParseThread::Stop()
{
    lock_guard<mutex> lck(m_lock);
    m_threadStatus = VMI_THREAD_EXIT;
    if (m_task != nullptr) {
        INFO("Set thread to exit");
        if (m_task->joinable()) {
            m_task->join();
        }
        m_task = nullptr;
        m_vmiSocket.CloseSocket();  // cancel SOCKET recv block
        return 0;
    } else {
        INFO("Thread is not running");
        return -1;
    }
}
} // namespace Vmi
