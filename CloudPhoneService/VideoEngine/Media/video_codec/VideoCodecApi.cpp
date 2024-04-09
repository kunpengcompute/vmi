/*
 * 版权所有 (c) 华为技术有限公司 2021-2021
 * 功能说明: 视频编解码器对外接口实现
 */

#define LOG_TAG "VideoCodecApi"
#include "VideoCodecApi.h"
#include "VideoEncoderCommon.h"
#include "VideoEncoderNetint.h"
#include "VideoEncoderOpenH264.h"
#include "VideoEncoderQuadra.h"
#include "VideoEncoderVastai.h"
#include "VideoEncoderT432.h"
#include "MediaLog.h"
#include "Property.h"

EncoderRetCode CreateVideoEncoder(VideoEncoder **encoder, EncoderFormat encoderFormat)
{
    INFO("create video encoder: encoder type %u", encoderFormat);
    switch (encoderFormat) {
        case ENCODER_TYPE_OPENH264:
            *encoder = new (std::nothrow) VideoEncoderOpenH264(encoderFormat);
            break;
        case ENCODER_TYPE_NETINTH264:
            *encoder = new (std::nothrow) VideoEncoderNetint(NI_CODEC_TYPE_H264, encoderFormat);
            break;
        case ENCODER_TYPE_NETINTH265:
            *encoder = new (std::nothrow) VideoEncoderNetint(NI_CODEC_TYPE_H265, encoderFormat);
            break;
        case ENCODER_TYPE_VASTAIH264:
            *encoder = new (std::nothrow) VideoEncoderVastai(VA_CODEC_TYPE_H264, encoderFormat);
            break;
        case ENCODER_TYPE_VASTAIH265:
            *encoder = new (std::nothrow) VideoEncoderVastai(VA_CODEC_TYPE_H265, encoderFormat);
            break;
        case ENCODER_TYPE_QUADRAH264:
            *encoder = new (std::nothrow) VideoEncoderQuadra(QUA_CODEC_TYPE_H264, encoderFormat);
            break;
        case ENCODER_TYPE_QUADRAH265:
            *encoder = new (std::nothrow) VideoEncoderQuadra(QUA_CODEC_TYPE_H265, encoderFormat);
            break;
        case ENCODER_TYPE_T432H264:
            *encoder = new (std::nothrow) VideoEncoderT432(T432_CODEC_TYPE_H264, encoderFormat);
            break;
        case ENCODER_TYPE_T432H265:
            *encoder = new (std::nothrow) VideoEncoderT432(T432_CODEC_TYPE_H265, encoderFormat);
            break;
        default:
            ERR("create video encoder failed: unknown encoder type %u", encoderFormat);
            return VIDEO_ENCODER_CREATE_FAIL;
    }
    if (*encoder == nullptr) {
        ERR("create video encoder failed: encoder type %u", encoderFormat);
        return VIDEO_ENCODER_CREATE_FAIL;
    }
    return VIDEO_ENCODER_SUCCESS;
}

EncoderRetCode DestroyVideoEncoder(VideoEncoder *encoder)
{
    if (encoder == nullptr) {
        WARN("input encoder is null");
        return VIDEO_ENCODER_SUCCESS;
    }
    delete encoder;
    encoder = nullptr;
    return VIDEO_ENCODER_SUCCESS;
}
