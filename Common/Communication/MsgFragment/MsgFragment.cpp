/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2021. All rights reserved.
 * Description: 报文分片
 */
#define LOG_TAG "MessageFragment"

#include "MsgFragment.h"
#include "logging.h"

namespace Vmi {
/**
 * @brief: constructor
 */
MsgFragment::MsgFragment() {}

/**
 * @brief: deconstructor
 */
MsgFragment::~MsgFragment() {}

/**
 * @brief: init message what to be send
 * @param [in] messageHead: message
 * @prama [in] type: type
 * @param [in] dataLen: data length
 */
void MsgFragment::InitSendMsg(StreamMsgHead &messageHead, uint8_t flag, VMIMsgType type, uint32_t dataLen)
{
    messageHead.SetMagicWord();
    messageHead.SetType(type);
    messageHead.SetFlag(flag);
    messageHead.SetPayloadSize(dataLen);
}

/*
 * @brief: Set whether to send the whole package without going through the fragmentation process.
 * @param [in] flag：True means that the whole packet is sent regardless of the data.
 */
void MsgFragment::SetSingleSend(bool flag)
{
    INFO("set singleSend flag %d", flag);
    m_singleSend = flag;
}

bool MsgFragment::SendMessage(VmiSocket &socket, StreamMsgHead &messageHead,
                              uint8_t flag, VMIMsgType type, uint32_t dataLen)
{
    InitSendMsg(messageHead, flag, type, dataLen);
    std::pair<uint8_t*, size_t> sendBuf(reinterpret_cast<uint8_t *>(&messageHead), dataLen + sizeof(StreamMsgHead));
    int ret = socket.Send(sendBuf);
    if (ret < 0) {
        LOG_RATE_LIMIT(ANDROID_LOG_ERROR, 1,
            "Failed to send message, VMI socket send failed, flag:%d, type:%u, dataLen:%u", flag, type, dataLen);
        return false;
    }
    return true;
}

/**
 * @brief: Network message fragmentation. Note that the parameter buf points to the stream_msg_head header.
 *         Datalen only contains the data part. When successful, the return value is the length of the data
 *         part, i.e. datalen. If the return value is less than 0, it fails.
 * @param [in] type: type
 * @param [in] socket: socket
 * @param [in] buf: data
 * @param [in] dataLen: data length, no contain StreamMsgHead
 * @return: dataLen or error code
 */
int MsgFragment::FragmentSend(VMIMsgType type, VmiSocket &socket, StreamMsgHead &buf, size_t dataLen)
{
    if ((dataLen == 0) || (dataLen > (static_cast<uint64_t>(MAX_MSG_SIZE) - sizeof(StreamMsgHead))) ||
        (type <= VMIMsgType::INVALID) || (type >= VMIMsgType::END)) {
        ERR("Failed to send fragment, message type(%u) or data length(%zu) is invalid", type, dataLen);
        return -1;
    }

    StreamMsgHead *messageHead = &buf;

    if (dataLen <= static_cast<uint64_t>(m_fragmentSize) || m_singleSend) {
        if (!SendMessage(socket, *messageHead, SINGLE_FRAGMENT, type, static_cast<uint32_t>(dataLen))) {
            LOG_RATE_LIMIT(ANDROID_LOG_ERROR, 1,
                "Failed to send fragment, send single message failed, type:%u, dataLen:%zu", type, dataLen);
            return -1;
        }
    } else {
        if (!SendMessage(socket, *messageHead, FIRST_FRAGMENT, type, m_fragmentSize)) {
            LOG_RATE_LIMIT(ANDROID_LOG_ERROR, 1,
                "Failed to send fragment, send first message failed, type:%u, dataLen:%zu", type, dataLen);
            return -1;
        }

        uint32_t len = dataLen - m_fragmentSize;
        messageHead = reinterpret_cast<StreamMsgHead *>(reinterpret_cast<uint8_t *>(messageHead) + m_fragmentSize);
        while (len > m_fragmentSize) {
            if (!SendMessage(socket, *messageHead, MIDDLE_FRAGMENT, type, m_fragmentSize)) {
                LOG_RATE_LIMIT(ANDROID_LOG_ERROR, 1,
                    "Failed to send fragment, send middle message failed, type:%u, dataLen:%zu", type, dataLen);
                return -1;
            }
            len -= m_fragmentSize;
            messageHead = reinterpret_cast<StreamMsgHead *>(reinterpret_cast<uint8_t *>(messageHead) + m_fragmentSize);
        }

        if (!SendMessage(socket, *messageHead, END_FRAGMENT, type, len)) {
            LOG_RATE_LIMIT(ANDROID_LOG_ERROR, 1,
                "Failed to send fragment, send end message failed, type:%u, dataLen:%zu", type, dataLen);
            return -1;
        }
    }

    return dataLen;
}
} // namespace Vmi
