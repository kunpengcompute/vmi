/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2021. All rights reserved.
 * Description: 报文管理队列
 */
#ifndef PACKET_QUEUE_H
#define PACKET_QUEUE_H

#include <condition_variable>
#include <deque>
#include <mutex>

namespace Vmi {
class __attribute__ ((visibility ("default"))) PacketQueue {
public:
    /**
     * @brief: construct
     * @param [in] isBlock: queue is blocked
     */
    explicit PacketQueue(bool isBlock);
    /**
     * @brief: constructor.
     */
    PacketQueue();

    /**
    * @brief: deconstruct
    */
    ~PacketQueue();

    /**
     * @brief: put data into data queue
     * @param [in] packet: data
     */
    void PutPkt(const std::pair<uint8_t *, uint32_t> &packet);

    /**
     * @brief: 获取队列里的第一个报文，但是报文不出队列
     * @return : a pair that consist of by pointer and size of data packet,
     *           or a pair consist of nullptr and 0 if there are no data packet in the queue
     */
    std::pair<uint8_t *, uint32_t> FrontPkt();
    /**
     * @brief: 队列里的第一个报文出队列
     * @return : a pair that consist of by pointer and size of data packet,
     *           or a pair consist of nullptr and 0 if there are no data packet in the queue
     */
    std::pair<uint8_t *, uint32_t> PopFrontPkt();

    /**
     * @brief: get a data from queue
     * @return : a pair that consist of by pointer and size of data packet,
     *           or a pair consist of nullptr and 0 if there are no data packet in the queue
     */
    std::pair<uint8_t *, uint32_t> GetNextPkt();

    /**
     * @brief: get a data from queue with timeout
     * @param [in] timeoutMillis: unit is ms
     * @return : a pair that consist of by pointer and size of data packet,
     *           or a pair consist of nullptr and 0 if there are no data packet in the queue
     */
    std::pair<uint8_t *, uint32_t> GetNextPktWait(int timeoutMillis);

    /**
     * @brief: get queue size
     * @return : number of items in the queue
     */
    size_t GetNumItems();

    /**
     * @brief: get total size
     * @return : total data size
     */
    uint32_t GetTotalSize();

    /**
     * @brief: If you want to release the communication object between threads, you should
     *         call this interface first, especially in blocking mode, and release the object
     *         after the blocked receiving thread exits.
     */
    void Cancel();

    /**
     * @brief: resume running
     */
    void Continue();

    /**
     * @brief: clear queue and free memory
     */
    void Clear();

    /**
     * @brief: if queue is waiting packet that mean queue is empty and render thread do not deal with packet
     * @return : if queue is waiting packet return false
     */
    bool HasPacket();
private:
    uint32_t m_totalDataSize = 0;
    std::deque<std::pair<uint8_t *, uint32_t>> m_packetDeque = {};
    std::mutex m_lock = {};
    std::condition_variable m_conditionVariable = {};
    bool m_block = false;
    volatile bool m_status = true;
};
} // namespace Vmi
#endif
