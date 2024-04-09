/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2021. All rights reserved.
 * Description: 报文分片
 */
#ifndef MSGFRAGMENT_H
#define MSGFRAGMENT_H

#include "VmiMsgHead.h"
#include "Socket/VmiSocket.h"

namespace Vmi {
class MsgFragment {
public:
    /**
     * @brief: constructor
     */
    MsgFragment();
    ~MsgFragment();

    /**
     * @brief: Network message fragmentation. Note that the parameter buf points to the stream_msg_head header.
     *         Datalen only contains the data part. When successful, the return value is the length of the data
     *         part, i.e. datalen. If the return value is less than 0, it fails.
     * @param [in] type: type
     * @param [in] socket: socket
     * @param [in] buf: data
     * @param [in] dataLen: data length
     * @return: dataLen or error code
     */
    int FragmentSend(VMIMsgType type, VmiSocket &socket, StreamMsgHead &buf, size_t dataLen);

    /*
     * @brief: Set whether to send the whole package without going through the fragmentation process.
     * @param [in] flag：True means that the whole packet is sent regardless of the data.
     */
    void SetSingleSend(bool flag);

private:
    /**
     * @功能描述: 发送单片数据
     * @参数: messageHead，需要发送的消息头
     * @参数: flag, 标记发送消息的单片类型，包括SINGLE_FRAGMENT，FIRST_FRAGMENT，MIDDLE_FRAGMENT，END_FRAGMENT
     * @参数: type，发送的消息类型
     * @参数: dataLen，发送的消息负载大小
     * @返回值: true，发送成功
     * @返回值: false，发送失败
     */
    bool SendMessage(VmiSocket &socket, StreamMsgHead &messageHead, uint8_t flag, VMIMsgType type, uint32_t dataLen);
    /**
     * @brief: init message what to be send
     * @param [in] messageHead: message
     * @prama [in] type: type
     * @param [in] dataLen: data length
     */
    void InitSendMsg(StreamMsgHead &messageHead, uint8_t flag, VMIMsgType type, uint32_t dataLen);
    bool m_singleSend = false;
    uint32_t m_fragmentSize = FRAGMENT_SIZE;
};
} // namespace Vmi
#endif
