/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2023. All rights reserved.
 * Description: Video Engine Client - DECODERBASE
 */

#ifndef DECODERBASE_H
#define DECODERBASE_H

#include <cstdint>

namespace Vmi {
enum DecoderRetCode : uint32_t {
    VMI_DECODER_SUCCESS = 0,
    VMI_DECODER_INIT_ERR = 0x01,
    VMI_DECODER_START_ERR = 0x02,
    VMI_DECODER_INPUT_ERR = 0x03,
    VMI_DECODER_OUTPUT_RETRY = 0x04,
    VMI_DECODER_OUTPUT_ERR = 0x05,
    VMI_DECODER_SDK_UNSUPPORTED = 0x06
};

// Decoder Type
enum class DecoderType {
    DEC_TYPE_UNKNOWN = 0x0,
    DEC_TYPE_H264 = 0x01,
    DEC_TYPE_H265 = 0x02
};

class DecoderBase {
public:
    static DecoderBase &GetInstance();
    DecoderBase()= default;
    virtual ~DecoderBase()= default;
    virtual DecoderRetCode Init(DecoderType type, uint64_t surface, int32_t rotation) = 0;
    virtual DecoderRetCode Start() = 0;
    virtual DecoderRetCode Input(std::pair<uint8_t *, uint32_t> &packetPair) = 0;
    virtual DecoderRetCode OutputAndDisplay() = 0;
    virtual uint32_t GetFps() = 0;
    virtual void Destroy() = 0;
};

extern "C" {
DecoderRetCode CreateVideoDecoder(DecoderBase** decoder);
DecoderRetCode DestroyVideoDecoder(DecoderBase* decoder);
}
}
#endif // DECODERBASE_H
