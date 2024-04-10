/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2021. All rights reserved.
 * Description: 报文管理队列
 */

#define LOG_TAG "PacketQueue"

#include "PacketQueue.h"
#include "logging.h"

using namespace std;
namespace Vmi {
/**
 * @brief: construct
 * @param [in] isBlock: queue is blocked
 */
PacketQueue::PacketQueue(bool isBlock)
    : m_totalDataSize(0),
      m_packetDeque(),
      m_lock(),
      m_conditionVariable(),
      m_block(isBlock),
      m_status(true)
{
}

/**
* @brief: constructor.
*/
PacketQueue::PacketQueue() {}

/**
 * @brief: deconstruct
 */
PacketQueue::~PacketQueue()
{
    Clear();
}

/**
 * @brief: clear queue and free memory
 */
void PacketQueue::Clear()
{
    unique_lock<mutex> lock(m_lock);
    m_status = false;
    m_conditionVariable.notify_all();
    while (!(m_packetDeque.empty())) {
        auto packet = m_packetDeque.front();
        m_packetDeque.pop_front();
        free(packet.first);
        packet.first = nullptr;
    }
    m_totalDataSize = 0;
}

/**
 * @brief: If you want to release the communication object between threads, you should
 *         call this interface first, especially in blocking mode, and release the object
 *         after the blocked receiving thread exits.
 */
void PacketQueue::Cancel()
{
    unique_lock<mutex> lock(m_lock);
    m_status = false;
    m_conditionVariable.notify_all();
}

/**
 * @brief: resume running
 */
void PacketQueue::Continue()
{
    unique_lock<mutex> lock(m_lock);
    m_status = true;
    m_conditionVariable.notify_all();
}

/**
 * @brief: put data into data queue
 * @param [in] packet: data
 */
void PacketQueue::PutPkt(const std::pair<uint8_t *, uint32_t> &packet)
{
    if (packet.first == nullptr || packet.second == 0) {
        ERR("Failed to put packet data, buf %s null, size:%u",
            (packet.first == nullptr) ? "is" : "is not", packet.second);
        free(packet.first);
        return;
    }
    unique_lock<mutex> lock(m_lock);
    if (m_status) {
        m_totalDataSize += packet.second;
        m_packetDeque.push_back(packet);
        if (m_block) {
            m_conditionVariable.notify_one();
        }
    } else {
        free(packet.first);
    }
}

std::pair<uint8_t *, uint32_t> PacketQueue::FrontPkt()
{
    std::pair<uint8_t *, uint32_t> packet(nullptr, 0);
    unique_lock<mutex> lock(m_lock);
    if (m_status) {
        if (m_block) {
            m_conditionVariable.wait(
                lock, [this]() -> bool { return !m_status || !m_packetDeque.empty(); });
        }
        if (m_status && !m_packetDeque.empty()) {
            packet = m_packetDeque.front();
        }
    }
    return packet;
}

std::pair<uint8_t *, uint32_t> PacketQueue::PopFrontPkt()
{
    std::pair<uint8_t *, uint32_t> packet(nullptr, 0);
    unique_lock<mutex> lock(m_lock);
    if (m_status && !m_packetDeque.empty()) {
        packet = m_packetDeque.front();
        m_packetDeque.pop_front();
        m_totalDataSize -= packet.second;
    }
    return packet;
}

/**
 * @brief: get a data from queue
 * @return : a pair that consist of by pointer and size of data packet,
 *           or a pair consist of nullptr and 0 if there are no data packet in the queue
 */
std::pair<uint8_t *, uint32_t> PacketQueue::GetNextPkt()
{
    std::pair<uint8_t *, uint32_t> packet(nullptr, 0);
    unique_lock<mutex> lock(m_lock);
    if (m_status) {
        if (m_block) {
            m_conditionVariable.wait(
                lock, [this]() -> bool { return !m_status || !m_packetDeque.empty(); });
        }
        if (m_status && !m_packetDeque.empty()) {
            packet = m_packetDeque.front();
            m_packetDeque.pop_front();
            m_totalDataSize -= packet.second;
        }
    }
    return packet;
}

/**
 * @brief: get a data from queue with timeout
 * @param [in] timeoutMillis: unit is ms
 * @return : a pair that consist of by pointer and size of data packet,
 *           or a pair consist of nullptr and 0 if there are no data packet in the queue
 */
std::pair<uint8_t *, uint32_t> PacketQueue::GetNextPktWait(int timeoutMillis)
{
    std::pair<uint8_t *, uint32_t> packet(nullptr, 0);
    unique_lock<mutex> lock(m_lock);
    if (m_status) {
        if (m_block) {
            // 此函数为内部函数，由调用处保证入参 timeoutMillis 的有效性
            m_conditionVariable.wait_for(lock, std::chrono::milliseconds(timeoutMillis),
                [this]() -> bool { return !m_status || !m_packetDeque.empty(); });
        }
        if (m_status && !m_packetDeque.empty()) {
            packet = m_packetDeque.front();
            m_packetDeque.pop_front();
            m_totalDataSize -= packet.second;
        }
    }

    return packet;
}

/**
 * @brief: get queue size
 * @return : number of items in the queue
 */
size_t PacketQueue::GetNumItems()
{
    unique_lock<mutex> lock(m_lock);
    return m_packetDeque.size();
}

/**
 * @brief: get total size
 * @return : total data size
 */
uint32_t PacketQueue::GetTotalSize()
{
    unique_lock<mutex> lock(m_lock);
    return m_totalDataSize;
}

/**
 * @brief: if queue is waiting packet that mean queue is empty and render thread do not deal with packet
 * @return : if queue is waiting packet return false
 */
bool PacketQueue::HasPacket()
{
    unique_lock<mutex> lock(m_lock);
    return !m_packetDeque.empty();
}
} // namespace Vmi
