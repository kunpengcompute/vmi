/*
 * 版权所有 (c) 华为技术有限公司 2021-2023
 * 功能说明: 视频编解码器对外接口实现
 */

#define LOG_TAG "DecoderBase"
#include "DecoderMedia.h"
#include "logging.h"

namespace Vmi {
DecoderRetCode CreateVideoDecoder(DecoderBase **decoder)
{
    *decoder = new (std::nothrow) DecoderMedia();
    if (*decoder == nullptr) {
        ERR("create video decoder failed: decoder type.");
        return VMI_DECODER_INIT_ERR;
    }
    
    return VMI_DECODER_SUCCESS;
}

DecoderRetCode DestroyVideoDecoder(DecoderBase *decoder)
{
    if (decoder == nullptr) {
        WARN("input decoder is null");
        return VMI_DECODER_SUCCESS;
    }

    delete decoder;
    decoder = nullptr;
    return VMI_DECODER_SUCCESS;
}
}