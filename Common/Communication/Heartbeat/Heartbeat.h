/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2021. All rights reserved.
 * Description: 通信心跳模块
 */
#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#include <string>
#include <deque>
#include <thread>
#include <functional>
#include "VmiMsgHead.h"
#include "PacketQueue.h"
#include "MsgFragment/MsgFragment.h"
#include "MsgFragment/MsgReassemble.h"
#include "Socket/VmiSocket.h"
#include "StreamParse/StreamParser.h"

namespace Vmi {
constexpr int BYTE_ALIGNMENT = 7;

enum class HeartbeatPackageType : uint8_t {
    HEARTBEAT_REQ = 0,
    HEARTBEAT_RES = 1,
    HEARTBEAT_INVLIAD = 2
};

struct HeartbeatMsg {
    uint64_t sendMicroSecond = 0;
    uint64_t seqNum = 0;
    HeartbeatPackageType type = HeartbeatPackageType::HEARTBEAT_REQ;
    uint8_t pad[BYTE_ALIGNMENT] = { 0 };
} __attribute__((packed));

struct StreamHeartbeatMsg {
    struct StreamMsgHead header = {};
    struct HeartbeatMsg heartbeatMsg = {};
};

class Heartbeat : public ServiceHandle {
public:
    /**
     * @brirf: construct
     * @param [in] sock: socket
     * @param [in] fnCallback: check net exception, callbak it
     */
    Heartbeat(VmiSocket& sock, std::function<void(const VmiSocket &)> fnCallback);

    ~Heartbeat() override;

    /**
     * @brief: start thread
     */
    bool Start();

    /**
     * @brief: stop thread, caller wait thread exit in hear.
     */
    void ForceStop();

    /**
     * @brief: put heartbeat data into queue
     * @param [in] packet: heartbeat data
     */
    void Handle(const std::pair<uint8_t*, size_t> &packet) override;

    /**
     * @brief: Get network average latency
     * @return: latency, -1 is Infinity
     */
    int64_t GetNetLoopbackLatencyAverage() const;

    /**
     * @brief: Get network maximum latency
     * @return: latency, 0 is Infinity
     */
    uint64_t GetNetLoopbackLatencyMax() const;

protected:
    /**
     * @brief: send heartbeat request
     * @param [in] head: heartbeat message
     */
    void SendHeartbeatRequest(StreamHeartbeatMsg &streamMessage);

    /**
     * @brief: handle recv heartbeat
     */
    void RecvHeartbeatResponse();

    /**
     * @brief: check net status
     */
    void CheckNetStatus();

    /**
     * @brief: compute net loop
     * @param [in] heartbeatResMsg: heartbeat package
     * @param [in] recvTime: recvTime
     */
    void CalcLatency(const HeartbeatMsg &heartbeatResponseMsg, uint64_t recvTime);

    /**
     * @brief: handle recv heartbeat
     * @param [in] reassembleMsg: heartbeat message
     * @param [in] recvTime: recvTime
     */
    void HandleRecvMsg(const std::pair<uint8_t *, uint32_t> &reassembleMsg, struct timespec recvTime);

    /**
     * @brief: heartbeat has for function:
     *               1、timing send heartbeat (SendHeartbeatRequest);
     *               2、handle recv heartbeat (RecvHeartbeatResponse);
     *               3、compute net loop;
     *               4、check net status.
     */
    void TaskentryHeartbeat();

    std::mutex m_lock = {};
    VmiSocket& m_vmiSocket;
    std::function<void(const VmiSocket &)> m_callback = nullptr;
    std::unique_ptr<std::thread> m_task = nullptr;
    volatile bool m_stopFlag = false;
    MsgFragment m_msgFragment = {};
    MsgReassemble m_msgReassemble {VMIMsgType::HEARTBEAT};
    uint64_t m_nextSendTime = 0;     // mircosecond
    int64_t m_netLoopbackLatencyAverage = -1;
    uint64_t m_netLoopbackLatencyMax = 0;
    std::deque<std::pair<uint64_t, uint64_t>> m_latencyQueue = {};
    PacketQueue m_packetQueue {true};
    uint64_t m_sendSeqNum = 0;
    uint64_t m_recvSeqNum = 0;
    int32_t m_maxAveLatency = 0;
};
} // namespace Vmi
#endif
