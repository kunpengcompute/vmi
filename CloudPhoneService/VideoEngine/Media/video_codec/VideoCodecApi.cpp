/*
 * 版权所有 (c) 华为技术有限公司 2021-2021
 * 功能说明: 视频编解码器对外接口实现
 */

#define LOG_TAG "VideoCodecApi"
#include <unordered_map>
#include "VideoCodecApi.h"
#include "VideoEncoderCommon.h"
#include "VideoEncoderNetint.h"
#include "VideoEncoderOpenH264.h"
#include "VideoEncoderQuadra.h"
#include "VideoEncoderVastai.h"
#include "logging.h"

std::unordered_map<int32_t, VideoEncoder *> g_encoderMap;
int32_t g_index = 0;

EncoderRetCode CreateVideoEncoder(int32_t* fd, EncoderFormat encoderFormat)
{
    INFO("create video encoder: encoder type %u", encoderFormat);
    VideoEncoder *encoder = nullptr;
    switch (encoderFormat) {
        case ENCODER_TYPE_OPENH264:
            encoder = new (std::nothrow) VideoEncoderOpenH264();
            break;
        case ENCODER_TYPE_NETINTH264:
            encoder = new (std::nothrow) VideoEncoderNetint(NI_CODEC_TYPE_H264);
            break;
        case ENCODER_TYPE_NETINTH265:
            encoder = new (std::nothrow) VideoEncoderNetint(NI_CODEC_TYPE_H265);
            break;
        case ENCODER_TYPE_VASTAIH264:
            encoder = new (std::nothrow) VideoEncoderVastai(VA_CODEC_TYPE_H264);
            break;
        case ENCODER_TYPE_VASTAIH265:
            encoder = new (std::nothrow) VideoEncoderVastai(VA_CODEC_TYPE_H265);
            break;
        case ENCODER_TYPE_QUADRAH264:
            encoder = new (std::nothrow) VideoEncoderQuadra(QUA_CODEC_TYPE_H264);
            break;
        case ENCODER_TYPE_QUADRAH265:
            encoder = new (std::nothrow) VideoEncoderQuadra(QUA_CODEC_TYPE_H265);
            break;
        case ENCODER_TYPE_T432H264:
            encoder = new (std::nothrow) VideoEncoderQuadra(T432_CODEC_TYPE_H264);
            break;
        case ENCODER_TYPE_T432H265:
            encoder = new (std::nothrow) VideoEncoderQuadra(T432_CODEC_TYPE_H265);
            break;
        default:
            ERR("create video encoder failed: unknown encoder type %u", encoderFormat);
            return VIDEO_ENCODER_CREATE_FAIL;
    }
    if (encoder == nullptr) {
        ERR("create video encoder failed: encoder type %u", encoderFormat);
        return VIDEO_ENCODER_CREATE_FAIL;
    }
    *fd = g_index;
    g_encoderMap[g_index] = encoder;
    g_index++;
    return VIDEO_ENCODER_SUCCESS;
}

EncoderRetCode DestroyVideoEncoder(int32_t fd)
{
    if (g_encoderMap.find(fd) == g_encoderMap.end()) {
        WARN("input encoder is null");
        return VIDEO_ENCODER_SUCCESS;
    }
    VideoEncoder *encoder = g_encoderMap[fd];
    delete encoder;
    encoder = nullptr;
    g_encoderMap.erase(fd);
    return VIDEO_ENCODER_SUCCESS;
}

EncoderRetCode InitEncoder(int32_t fd)
{
    if (g_encoderMap.find(fd) == g_encoderMap.end()) {
        WARN("input encoder is null");
        return VIDEO_ENCODER_REGISTER_FAIL;
    }
    return g_encoderMap[fd]->InitEncoder();
}

EncoderRetCode StartEncoder(int32_t fd)
{
    if (g_encoderMap.find(fd) == g_encoderMap.end()) {
        WARN("input encoder is null");
        return VIDEO_ENCODER_REGISTER_FAIL;
    }
    return g_encoderMap[fd]->StartEncoder();
}

EncoderRetCode EncodeOneFrame(int32_t fd, const uint8_t *inputData, uint32_t inputSize,
        uint8_t **outputData, uint32_t *outputSize)
{
    if (g_encoderMap.find(fd) == g_encoderMap.end()) {
        WARN("input encoder is null");
        return VIDEO_ENCODER_REGISTER_FAIL;
    }
    return g_encoderMap[fd]->EncodeOneFrame(inputData, inputSize, outputData, outputSize);
}

EncoderRetCode StopEncoder(int32_t fd)
{
    if (g_encoderMap.find(fd) == g_encoderMap.end()) {
        WARN("input encoder is null");
        return VIDEO_ENCODER_REGISTER_FAIL;
    }
    return g_encoderMap[fd]->StopEncoder();
}

EncoderRetCode DestoryEncoder(int32_t fd)
{
    if (g_encoderMap.find(fd) == g_encoderMap.end()) {
        WARN("input encoder is null");
        return VIDEO_ENCODER_REGISTER_FAIL;
    }
    g_encoderMap[fd]->DestroyEncoder();
    return EncoderRetCode::VIDEO_ENCODER_SUCCESS;
}

EncoderRetCode ResetEncoder(int32_t fd)
{
    if (g_encoderMap.find(fd) == g_encoderMap.end()) {
        WARN("input encoder is null");
        return VIDEO_ENCODER_REGISTER_FAIL;
    }
    return g_encoderMap[fd]->ResetEncoder();
}

EncoderRetCode Config(int32_t fd, const VmiEncoderConfig& config)
{
    if (g_encoderMap.find(fd) == g_encoderMap.end()) {
        WARN("input encoder is null");
        return VIDEO_ENCODER_REGISTER_FAIL;
    }
    return g_encoderMap[fd]->Config(config);
}

EncoderRetCode SetParams(int32_t fd, const VmiEncoderParams& params)
{
    if (g_encoderMap.find(fd) == g_encoderMap.end()) {
        WARN("input encoder is null");
        return VIDEO_ENCODER_REGISTER_FAIL;
    }
    return g_encoderMap[fd]->SetParams(params);
}
