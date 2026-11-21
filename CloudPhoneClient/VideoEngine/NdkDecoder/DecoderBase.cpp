/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * vmi is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */

/*
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