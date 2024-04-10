/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2021. All rights reserved.
 * Description: 报文重组
 */
#ifndef MSGREASSEMBLE_H
#define MSGREASSEMBLE_H

#include <deque>
#include "VmiMsgHead.h"
#include "Socket/VmiSocket.h"

namespace Vmi {
class __attribute__ ((visibility ("default"))) MsgReassemble {
public:
    /**
     * @brief: constructor
     * @param [in] type: type
     */
    explicit MsgReassemble(VMIMsgType type);

    /**
     * @brief: deconstructor, clear queue and free memory
     */
    ~MsgReassemble();

    /**
     * @brief: packet reassemble
     * @param [in] packet: packet
     * @return: Reassemble packet if reassemble ok, or nullptr
     */
    std::pair<uint8_t*, uint32_t> Reassemble(StreamMsgHead &packet);

    /**
     * @brief: clear queue and free memory
     */
    void Clear();

    /**
     * @brief: Set whether to send the whole package without going through the fragmentation process.
     * @param [in] flag：True means that the whole packet is sent regardless of the data.
     */
    void SetSingleSend(bool flag);

private:
    /**
     * @brief: check packet
     * @param [in] packet: packet
     * @return: true if ok
     */
    bool CheckPacket(StreamMsgHead &packet);

    /**
     * @brief: handle single fragment
     * @param [in] packet: packet
     * @return: this packet
     */
    std::pair<uint8_t *, uint32_t> ProcessSingleFragment(StreamMsgHead &packet);

    /**
     * @brief: handle first fragment
     * @param [in] packet: packet
     * @return: nullptr, No complete package was received.
     */
    std::pair<uint8_t *, uint32_t> ProcessFirstFragment(StreamMsgHead &packet);

    /**
     * @brief: handle middle fragment
     * @param [in] packet: packet
     * @return: nullptr, No complete package was received.
     */
    std::pair<uint8_t *, uint32_t> ProcessMiddleFragment(StreamMsgHead &packet);

    /**
     * @brief: handle end fragment
     * @param [in] packet: packet
     * @return: Reassemble packet if reassemble ok, or nullptr
     */
    std::pair<uint8_t *, uint32_t> ProcessEndFragment(StreamMsgHead &packet);

    std::deque<StreamMsgHead *> m_reassembleQueue = {};
    VMIMsgType m_type = VMIMsgType::HEARTBEAT;
    uint32_t m_totalSize = 0;
    bool m_singleSend = false;
    uint32_t m_fragmentSize = FRAGMENT_SIZE;
};
} // namespace Vmi
#endif
