/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2021. All rights reserved.
 * Description: 报文重组
 */
#define LOG_TAG "MessageReassemble"

#include "MsgReassemble.h"
#include <cstring>
#include "logging.h"

namespace Vmi {
/**
 * @brief: constructor
 * @param [in] type: type
 */
MsgReassemble::MsgReassemble(VMIMsgType type) : m_type(type), m_totalSize(0)
{
}

/**
 * @brief: deconstructor, clear queue and free memory
 */
MsgReassemble::~MsgReassemble()
{
    Clear();
}

/**
 * @brief: clear queue and free memory
 */
void MsgReassemble::Clear()
{
    void *packet = nullptr;

    while (!(m_reassembleQueue.empty())) {
        packet = m_reassembleQueue.front();
        m_reassembleQueue.pop_front();
        free(packet);
        packet = nullptr;
    }

    m_totalSize = 0;
}

/*
 * @brief: Set whether to send the whole package without going through the fragmentation process.
 * @param [in] flag：True means that the whole packet is sent regardless of the data.
 */
void MsgReassemble::SetSingleSend(bool flag)
{
    INFO("Set singleSend flag %d", flag);
    m_singleSend = flag;
}

/**
 * @brief: check packet, packet may be free int this function
 * @param [in] packet: packet
 * @return: true if ok
 */
bool MsgReassemble::CheckPacket(StreamMsgHead& packet)
{
    if (packet.GetType() != m_type) {
        ERR("Failed to check packet, msgtype(%u) is error, this type is %u", packet.GetType(), m_type);
        free(&packet);
        return false;
    }

    if (packet.GetPayloadSize() > m_fragmentSize && (!m_singleSend || packet.GetFlag() != SINGLE_FRAGMENT)) {
        ERR("Failed to check packet, fragment error, payloadSize:%u, FRAGMENT_SIZE:%u, stream_type:%u, flag:%u",
            packet.GetPayloadSize(), m_fragmentSize, m_type, packet.GetFlag());
        free(&packet);
        return false;
    }
    return true;
}

/**
 * @brief: handle single fragment, packet may be free int this function
 * @param [in] packet: packet
 * @return: this packet
 */
std::pair<uint8_t *, uint32_t> MsgReassemble::ProcessSingleFragment(StreamMsgHead &packet)
{
    std::pair<uint8_t *, uint32_t> nullPair(nullptr, 0);
    if (!(m_reassembleQueue.empty())) {
        ERR("Failed to process single fragment, reassemble queue is not empty, clear queue(size %u), this type is %u",
            m_totalSize, m_type);
        Clear();
    }
    uint32_t payloadSize = packet.GetPayloadSize();
    StreamMsgHead *head = &packet;
    if (payloadSize == 0) {
        ERR("Failed to process single fragment, payloadSize is 0, can't malloc message!");
        free(head);
        head = nullptr;
        return nullPair;
    }
    uint8_t* message = reinterpret_cast<uint8_t *>(malloc(payloadSize));
    if (message == nullptr) {
        ERR("Failed to process single fragment, alloc message(%u) failed.", payloadSize);
        free(head);
        head = nullptr;
        return nullPair;
    }

    memcpy(message, reinterpret_cast<uint8_t *>(head + 1), payloadSize);
    free(head);
    head = nullptr;
    return std::pair<uint8_t *, uint32_t>(message, payloadSize);
}

/**
 * @brief: handle first fragment, packet may be free int this function
 * @param [in] packet: packet
 * @return: nullptr, No complete package was received.
 */
std::pair<uint8_t *, uint32_t> MsgReassemble::ProcessFirstFragment(StreamMsgHead &packet)
{
    if (!(m_reassembleQueue.empty())) {
        ERR("Failed to process first fragment, reassemble queue is not empty, clear queue(size %u), this type is %u",
            m_totalSize, m_type);
        Clear();
    }
    m_totalSize = packet.GetPayloadSize();
    m_reassembleQueue.push_back(&packet);
    return std::pair<uint8_t *, uint32_t>(nullptr, 0);
}

/**
 * @brief: handle middle fragment, packet may be free int this function
 * @param [in] packet: packet
 * @return: nullptr, No complete package was received.
 */
std::pair<uint8_t *, uint32_t> MsgReassemble::ProcessMiddleFragment(StreamMsgHead &packet)
{
    std::pair<uint8_t *, uint32_t> nullPair(nullptr, 0);

    if (m_reassembleQueue.empty()) {
        ERR("Failed to process middle fragment, reassemble queue is empty, drop packet(size %u), this type is %u",
            packet.GetPayloadSize(), m_type);
        free(&packet);
    } else {
        uint32_t remainSize = MAX_MSG_SIZE - sizeof(StreamMsgHead) - m_totalSize;
        if (packet.GetPayloadSize() > remainSize) {
            ERR("Failed to process middle fragment, packet size:%u > remain:%u, clear queue(size %u), this type is %u",
                packet.GetPayloadSize(), remainSize, m_totalSize, m_type);
            Clear();
            return nullPair;
        }

        m_totalSize += packet.GetPayloadSize();
        m_reassembleQueue.push_back(&packet);
    }
    return nullPair;
}

/**
 * @brief: handle end fragment, packet may be free int this function
 * @param [in] packet: packet
 * @return: Reassemble packet if reassemble ok, or nullptr
 */
std::pair<uint8_t *, uint32_t> MsgReassemble::ProcessEndFragment(StreamMsgHead &packet)
{
    std::pair<uint8_t *, uint32_t> nullPair(nullptr, 0);
    if (m_reassembleQueue.empty()) {
        ERR("Failed to process end fragment, reassemble queue is empty, drop packet(size %u), this type is %u",
            packet.GetPayloadSize(), m_type);
        free(&packet);
        return nullPair;
    }

    uint32_t remainSize = MAX_MSG_SIZE - sizeof(StreamMsgHead) - m_totalSize;
    if (packet.GetPayloadSize() > remainSize) {
        ERR("Failed to process end fragment, packet size:%u > remain:%u, clear queue(size %u), this type is %u",
            packet.GetPayloadSize(), remainSize, m_totalSize, m_type);
        free(&packet);
        Clear();
        return nullPair;
    }

    m_reassembleQueue.push_back(&packet);
    m_totalSize += packet.GetPayloadSize();
    if (m_totalSize == 0) {
        ERR("Failed to process end fragment, total size is 0, can't malloc message! Clear queue, this type is %u",
            m_type);
        Clear();
        return nullPair;
    }

    uint8_t *message = reinterpret_cast<uint8_t *>(malloc(m_totalSize));
    if (message == nullptr) {
        ERR("Failed to process end fragment, alloc buf failed, clear queue(size %u), this type is %u",
            m_totalSize, m_type);
        Clear();
        return nullPair;
    }

    uint8_t *messageData = message;
    uint32_t leftSize = m_totalSize;
    while (!(m_reassembleQueue.empty())) {
        StreamMsgHead *msgHead = m_reassembleQueue.front();
        if (leftSize < msgHead->GetPayloadSize()) {
            ERR("Failed to process end fragment, lefe size:%u less than copy size:%u, clear queue, this type is %u",
                leftSize, msgHead->GetPayloadSize(), m_type);
            free(message);
            message = nullptr;
            Clear();
            return nullPair;
        }
        memcpy(messageData, reinterpret_cast<uint8_t *>(msgHead + 1), msgHead->GetPayloadSize());
        m_reassembleQueue.pop_front();
        messageData += msgHead->GetPayloadSize();
        leftSize -= msgHead->GetPayloadSize();
        free(msgHead);
        msgHead = nullptr;
    }

    return std::pair<uint8_t *, uint32_t>(message, m_totalSize);
}

/**
 * @brief: packet reassemble, packet may be free int this function
 * @param [in] packet: packet
 * @return: Reassemble packet if reassemble ok, or nullptr
 */
std::pair<uint8_t *, uint32_t> MsgReassemble::Reassemble(StreamMsgHead &packet)
{
    std::pair<uint8_t *, uint32_t> resultMsg(nullptr, 0);
    if (!CheckPacket(packet)) {
        return resultMsg;
    }

    switch (packet.GetFlag()) {
        case SINGLE_FRAGMENT: {
            resultMsg = ProcessSingleFragment(packet);
            break;
        }
        case FIRST_FRAGMENT: {
            resultMsg = ProcessFirstFragment(packet);
            break;
        }
        case MIDDLE_FRAGMENT: {
            resultMsg = ProcessMiddleFragment(packet);
            break;
        }
        case END_FRAGMENT: {
            resultMsg = ProcessEndFragment(packet);
            Clear();
            break;
        }
        default:
            ERR("Failed to reassemble packet, unknown packet's flag: %u", packet.GetFlag());
            free(&packet);
            break;
    }

    return resultMsg;
}
} // namespace Vmi
