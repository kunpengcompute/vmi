/*
 * 功能说明: 视频解码器功能接口实现
 */

#define LOG_TAG "VideoDecoderApi"
#include "android/log.h"
#include "VideoDecoder.h"
#include "VideoDecoderQuadra.h"
#include "VideoDecoderNetint.h"
#include "Property.h"

using namespace MediaCore;

namespace {
enum DecoderType : uint32_t {
    SOFT_TYPE = 0,
    T432_TYPE = 1,
    VASTAI_TYPE = 2,
    QUADRA_TYPE = 3,
};

#define ALOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define ALOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define ALOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define ALOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define ALOGF(...) __android_log_assert("assert", LOG_TAG, __VA_ARGS__)
}

DecoderRetCode CreateVideoDecoder(VideoDecoder** decoder)
{
    // kbox默认使能T432硬解，不使能quadra
    uint32_t decType = GetIntEncParam("ro.vmi.hardware.vpu");
    switch (decType) {
        case DecoderType::SOFT_TYPE:
            ALOGI("create soft video decoder");
            break;
        case DecoderType::T432_TYPE:
            *decoder = new (std::nothrow) VideoDecoderNetint();
            break;
        case DecoderType::VASTAI_TYPE:
            ALOGI("create video decoder failed: unsupported decoder type %u", decType);
            break;
        case DecoderType::QUADRA_TYPE:
            *decoder = new (std::nothrow) VideoDecoderQuadra();
            break;
        default:
            ALOGE("create video decoder failed: unknown decoder type %u", decType);
            return VIDEO_DECODER_CREATE_FAIL;
    }

    if (*decoder == nullptr) {
        ALOGE("create video decoder failed: decoder type %u", decType);
        return VIDEO_DECODER_CREATE_FAIL;
    }
    return VIDEO_DECODER_SUCCESS;
}

DecoderRetCode DestroyVideoDecoder(VideoDecoder* decoder)
{
    if (decoder == nullptr) {
        ALOGW("input decoder is null.");
        return VIDEO_DECODER_SUCCESS;
    }
    delete decoder;
    decoder = nullptr;
    return VIDEO_DECODER_SUCCESS;
}
