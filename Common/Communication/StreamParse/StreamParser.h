/* *
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2021. All rights reserved.
 * Description: 报文解析模块
 */
#ifndef STREAM_PARSE_H
#define STREAM_PARSE_H
#include <unordered_map>
#include <string>
#include <array>
#include <mutex>
#include <thread>
#include "VmiMsgHead.h"
#include "Socket/VmiSocket.h"

namespace Vmi {
class ServiceHandle {
public:
    /* *
     * @brief: interface
     * @param [in] pPkt: packet
     */
    virtual void Handle(const std::pair<uint8_t*, size_t> &packet) = 0;
    virtual ~ServiceHandle() {}
    /* *
     * @brief: interface
     * @return: packet
     */
    virtual uint8_t *GetNextPkt()
    {
        return nullptr;
    }
    /* *
     * @brief: interface
     * @param [in] packet: compelete message
     * @return: return reassemble message
     */
    virtual uint8_t *Reassemble(const std::pair<uint8_t*, size_t> &packet)
    {
        return packet.first;
    }
};

class StreamParse {
public:
    StreamParse();
    ~StreamParse();

    /* *
     * @brief: set handle with type
     * @param [in] type: type
     * @param [in] serviceHandle: handle
     */
    void SetServiceHandle(VMIMsgType type, std::shared_ptr<ServiceHandle> serviceHandle);

    /* *
     * @brief: get handle with type
     * @param [in] type: type
     * @param [in] serviceHandle: handle
     */
    ServiceHandle *GetServiceHandle(VMIMsgType type);

    /* *
     * @brief: parse socket stream
     * @param: [in] packetBuffer: packet data buffer
     * @return: true if success, or false
     */
    bool ParseStream(const std::pair<uint8_t*, size_t> &packetBuffer);

    /* *
     * @brief: check packet
     * @param [in] packetBuffer: packet data buffer
     * @return: packet length or error code
     */
    static int PacketDelimiter(const std::pair<uint8_t*, size_t> &packetBuffer);

    /* *
     * @brief: Reset m_preRecvMsgSeq to zero
     */
    void ResetRecvSeq();

    /* *
     * @brief: get the last successfully received message sequence number
     * @return: message sequence number
     */
    uint32_t GetCurRecvMsgSeq() const;

    /* *
     * @brief: set the stream whether to check the hash value of data,
     * to prevent data from being damaged during transmission.
     * @param [in] flag: true is to check hash value, false is not.
     */
    void SetHeadVerify(bool flag);

    bool GetHeadVerify() const;

private:
    /* *
     * @brief: process packet
     * @param [in] packetBuffer: packet data buffer
     * @return: true if success, or false
     */
    bool ProcessMsg(const std::pair<uint8_t*, size_t> &packetBuffer);

    /**
     * @brief: statistic not process message type count for unit time
     * @param [in] msgType: message type
     */
    void NoProcessMsgCount(VMIMsgType msgType);

    std::array<std::shared_ptr<ServiceHandle>, VMIMsgType::END> m_serviceHandle = {};
    uint32_t m_preRecvMsgSeq = 0;
    bool m_isMsgHeadVerify = true;
    std::unordered_map<VMIMsgType, uint32_t> m_msgTypeCount = {};
};

class StreamParseThread {
public:
    /* *
     * @brief: constructor
     * @param [in] vmiSocket: socket
     */
    explicit StreamParseThread(VmiSocket& vmiSocket);

    ~StreamParseThread();

    /* *
     * @brief: start thread
     * @return: return 0 if success, -1 falied
     */
    int Start();

    /* *
     * @brief: stop thread
     * @return: return 0 if success, -1 falied
     */
    int Stop();

    /* *
     * @brief: set m_streamParse
     */
    void SetStreamParse(std::shared_ptr<StreamParse> streamParse);
protected:
    /* *
     * @brief: process recv message
     * @param [in] streamParse: streamParse object
     */
    void ProcessRecvMsg();

    /* *
     * @brief: receive fixed size data
     * @param: [in] recvBuffer: buffer to recv data
     * @return: true if success, or false
     */
    bool RecvFixedSizeData(const std::pair<uint8_t*, size_t> &recvBuffer);

    /* *
     * @brief: cloud phone mode receive data
     * @param: [in] streamParse: streamParse object
     * @param: [in] recvBuffer: recv buffer
     * @return: true if success, or false
     */
    bool RecvCloudPhoneData();

    /* *
     * @brief: thread process
     */
    void Taskentry();

    std::mutex m_lock = {};
    VmiSocket& m_vmiSocket;
    std::unique_ptr<std::thread> m_task = nullptr;
    std::shared_ptr<StreamParse> m_streamParse = nullptr;
    volatile int m_threadStatus = -1;
};
} // namespace Vmi
#endif
