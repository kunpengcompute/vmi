/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2021. All rights reserved.
 * Description: Defines type and header of message
 */

#ifndef VMI_MSGHEAD_H
#define VMI_MSGHEAD_H

#include <string>
#include "MurmurHash.h"
#include "CommonDefine.h"
#include "VmiEngine.h"

namespace Vmi {

enum FRAGMENT : uint8_t {
    SINGLE_FRAGMENT = 0x0, // 两位标志位00表示完整消息，没有分片
    FIRST_FRAGMENT  = 0x1, // 两位标志位01表示首分片
    MIDDLE_FRAGMENT = 0x2, // 两位标志位10表示中间分片
    END_FRAGMENT    = 0x3, // 两位标志位11表示尾分片
};

struct StreamMsgHead {
public:
    enum MAGICWORD : uint16_t {
        VMI_STREAM_DELIMITER_MAGICWORD = 0x5A5A
    };

    bool VerifyMsgHead() const
    {
        return this->magicword == VMI_STREAM_DELIMITER_MAGICWORD;
    }

    void SetMagicWord()
    {
        this->magicword = VMI_STREAM_DELIMITER_MAGICWORD;
    }

    uint32_t GetPayloadSize() const
    {
        return this->size;
    }

    void SetPayloadSize(uint32_t size)
    {
        this->size = size;
    }

    VMIMsgType GetType() const
    {
        return this->type;
    }

    void SetType(VMIMsgType type)
    {
        this->type = type;
    }

    uint8_t GetFlag() const
    {
        return this->flag;
    }

    void SetFlag(uint8_t flag)
    {
        this->flag = flag;
    }

    uint32_t GetMsgSeq() const
    {
        return this->msgSeq;
    }

    void SetMsgSeq(uint32_t msgSeq)
    {
        this->msgSeq = msgSeq;
    }

    void SetMurmurHash(std::pair<const uint8_t*, uint32_t> key)
    {
        this->murmurHash = 0;
        uint32_t murmurHashValue = 0;
        (void)MurmurHash2(key, murmurHashValue);
        this->murmurHash = murmurHashValue;
    }

    bool VerifyMurmurHash(std::pair<uint8_t*, uint32_t> key)
    {
        uint8_t *data = key.first;
        StreamMsgHead *head = reinterpret_cast<StreamMsgHead *>(data);
        uint32_t recvHashValue = head->murmurHash;
        this->murmurHash = 0;
        uint32_t murmurHashValue = 0;
        (void)MurmurHash2(key, murmurHashValue);
        return recvHashValue == murmurHashValue;
    }

private:
    uint16_t magicword = 0;
    uint8_t flag = 0;
    VMIMsgType type = VMIMsgType::INVALID;
    uint32_t size = 0;
    uint32_t msgSeq = 0;
    uint32_t murmurHash = 0;
} __attribute__((packed));

enum MESSAGE_HEAD {
    FRAGMENT_SIZE_FULL = 1420,

    // 分片长度设置不可过大，按照带宽计算发送一个分片时间不可超过毫秒级，否则就失去了分片的意义
    FRAGMENT_SIZE = FRAGMENT_SIZE_FULL - sizeof(StreamMsgHead),

    // 包括消息头，最大的消息长度, 32 * 1024 * 1024
    MAX_MSG_SIZE = 33554432,
};

} // namespace Vmi

#endif  // VMI_MSGHEAD_H
