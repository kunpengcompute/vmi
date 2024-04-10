/*
 * 版权所有 (c) 华为技术有限公司 2023-2023
 * 功能说明: 适配T432硬件视频编码器，包括编码器初始化、启动、编码、停止、销毁等
 */
#define LOG_TAG "VideoEncoderT432"
#include "VideoEncoderT432.h"
#include <dlfcn.h>
#include <algorithm>
#include <cstring>
#include <string>
#include <atomic>
#include <unistd.h>
#include "MediaLog.h"
#include "Property.h"

namespace {
    constexpr int NUM_OF_PLANES = 3;
    constexpr int COMPRESS_RATIO = 2;
    constexpr int FRAME_ALIGN_U_INDEX = 2;
    constexpr int FRAME_ALIGN_V_INDEX = 4;
    const std::string ENCODER_TYPE_T432_H264 = "h264_ni_logan_enc";
    const std::string ENCODER_TYPE_T432_H265 = "h265_ni_logan_enc";
    constexpr uint32_t MAX_RECEIVE_TIMES = 500;
    constexpr int RECEIVE_INTERVAL = 1000;  // 1000us = 1ms
    constexpr uint32_t US_TO_MS = 1000;

    const std::string AV_BUFFER_REF = "av_buffer_ref";
    const std::string AV_BUFFER_UNREF = "av_buffer_unref";
    const std::string AVCODEC_FIND_ENCODER_BY_NAME = "avcodec_find_encoder_by_name";
    const std::string AVCODEC_ALLOC_CONTEXT3 = "avcodec_alloc_context3";
    const std::string AVCODEC_FREE_CONTEXT = "avcodec_free_context";
    const std::string AV_OPT_SET = "av_opt_set";
    const std::string AV_OPT_SET_INT = "av_opt_set_int";
    const std::string AVCODEC_OPEN2 = "avcodec_open2";
    const std::string AV_FRAME_ALLOC = "av_frame_alloc";
    const std::string AV_FRAME_FREE = "av_frame_free";
    const std::string AV_PACKET_ALLOC = "av_packet_alloc";
    const std::string AV_PACKET_FREE = "av_packet_free";
    const std::string AV_PACKET_UNREF = "av_packet_unref";
    const std::string AVCODEC_SEND_FRAME = "avcodec_send_frame";
    const std::string AVCODEC_RECEIVE_PACKET = "avcodec_receive_packet";
    const std::string AV_FRAME_GET_BUFFER = "av_frame_get_buffer";
    const std::string AV_LOG_SET_LEVEL = "av_log_set_level";
    const std::string AV_LOG_SET_CALLBACK = "av_log_set_callback";
    const std::string AV_INIT_PACKET = "av_init_packet";

    using AvInitPacketFunc = void(*)(AVPacket *pkt);
    using AvPacketAllocFunc = AVPacket *(*)(void);
    using AvPacketFreeFunc = void (*)(AVPacket **pkt);
    using AvcodecSendFrameFunc = int (*)(AVCodecContext *avctx, const AVFrame *frame);
    using AvcodecReceivePacketFunc = int (*)(AVCodecContext *avctx, AVPacket *avpkt);
    using AvPacketUnrefFunc = void (*)(AVPacket *pkt);
    using AvHwframeCtxAllocFunc = AVBufferRef *(*)(AVBufferRef *device_ctx);
    using AvHwframeCtxInitFunc = int (*)(AVBufferRef *ref);
    using AvBufferUnrefFunc = void(*)(AVBufferRef **buf);
    using AvcodecFindEncoderByNameFunc = AVCodec *(*)(const char *name);
    using AvcodecAllocContext3Func = AVCodecContext *(*)(const AVCodec *codec);
    using AvcodecFreeContextFunc = void (*)(AVCodecContext **pavctx);
    using AvBufferRefFunc = AVBufferRef *(*)(AVBufferRef *buf);
    using AvOptSetFunc = int (*)(void *obj, const char *name, const char *val, int searchFlags);
    using AvOptSetIntFunc = int (*)(void *obj, const char *name, int64_t val, int searchFlags);
    using AvcodecOpen2Func = int (*)(AVCodecContext *avctx, const AVCodec *codec, AVDictionary **options);
    using AvFrameAllocFunc = AVFrame *(*)(void);
    using AvFrameFreeFunc = void(*)(AVFrame **frame);
    using AvFrameGetBufferFunc = int (*)(AVFrame *frame, int align);
    using AvHwframeTransferDataFunc = int (*)(AVFrame *dst, const AVFrame *src, int flags);
    using AvHwframeGetBufferFunc = int (*)(AVBufferRef *hwframe_ctx, AVFrame *frame, int flags);

    std::unordered_map<std::string, std::string> g_transProfile = {
        {"baseline", "1"},
        {"main", "2"},
        {"high", "4"}};

    std::unordered_map<std::string, std::string> g_transProfile_h265 = {
        {"main", "1"}};

    std::unordered_map<std::string, void*> g_funcMap_avcodec = {
        { AVCODEC_FIND_ENCODER_BY_NAME, nullptr },
        { AVCODEC_ALLOC_CONTEXT3, nullptr },
        { AVCODEC_FREE_CONTEXT, nullptr },
        { AVCODEC_OPEN2, nullptr },
        { AV_PACKET_ALLOC, nullptr },
        { AV_PACKET_FREE, nullptr },
        { AVCODEC_SEND_FRAME, nullptr },
        { AVCODEC_RECEIVE_PACKET, nullptr },
        { AV_PACKET_UNREF, nullptr },
        { AV_INIT_PACKET, nullptr }
        };

    std::unordered_map<std::string, void*> g_funcMap_avutil = {
        { AV_OPT_SET, nullptr },
        { AV_OPT_SET_INT, nullptr },
        { AV_FRAME_GET_BUFFER, nullptr },
        { AV_FRAME_ALLOC, nullptr },
        { AV_FRAME_FREE, nullptr },
        { AV_BUFFER_REF, nullptr },
        { AV_BUFFER_UNREF, nullptr }
        };

    const std::string SHARED_LIB_AVCODEC = "libavcodec.so";
    const std::string SHARED_LIB_AVUTIL = "libavutil.so";
    std::atomic<bool> g_t432Loaded = { false };
    void *g_libHandleAvcodec = nullptr;
    void *g_libHandleAvutil = nullptr;
}

VideoEncoderT432::VideoEncoderT432(T432CodecType codecType, EncoderFormat codecFormat)
{
    INFO("VideoEncoderT432::VideoEncoderT432 codecType: %d", codecType);

    if (codecType == T432_CODEC_TYPE_H264) {
        m_codec = ENCODER_TYPE_T432_H264;
    } else {
        m_codec = ENCODER_TYPE_T432_H265;
        m_encParams.bitrate = static_cast<uint32_t>(BITRATE_DEFAULT_265);
        m_encParams.profile = ENCODE_PROFILE_MAIN;
    }
    m_codecFormat = codecFormat;
    INFO("VideoEncoderT432 constructed %s", (m_codec == ENCODER_TYPE_T432_H264) ? "h.264" : "h.265");
}

VideoEncoderT432::~VideoEncoderT432()
{
    INFO("VideoEncoderT432::~VideoEncoderT432");
    DestroyEncoder();
    INFO("VideoEncoderT432 destructed");
}

EncoderRetCode VideoEncoderT432::InitEncoder()
{
    if ((!GetRoEncParam()) || (!GetPersistEncParam())) {
        ERR("init encoder failed: GetEncParam failed");
        return VIDEO_ENCODER_INIT_FAIL;
    }
    m_encParams = m_tmpEncParams;
    if (m_codec == ENCODER_TYPE_T432_H265) {
        m_encParams.profile = ENCODE_PROFILE_MAIN;
        m_tmpEncParams.profile = ENCODE_PROFILE_MAIN;
    }
    if (!LoadT432SharedLib()) {
        ERR("init encoder failed: load T432 so error");
        return VIDEO_ENCODER_INIT_FAIL;
    }

    auto avcodecFindEncoderByName = reinterpret_cast<AvcodecFindEncoderByNameFunc>
        (g_funcMap_avcodec[AVCODEC_FIND_ENCODER_BY_NAME]);
    m_encCodec = avcodecFindEncoderByName(m_codec.c_str());
    if (m_encCodec == nullptr) {
        ERR("Could not find encoder");
        return VIDEO_ENCODER_INIT_FAIL;
    }

    auto avcodecAllocContext3 = reinterpret_cast<AvcodecAllocContext3Func>(g_funcMap_avcodec[AVCODEC_ALLOC_CONTEXT3]);
    m_encoderCtx = avcodecAllocContext3(m_encCodec);
    if (m_encoderCtx == nullptr) {
        ERR("Could not create encoder context");
        return VIDEO_ENCODER_INIT_FAIL;
    }

    auto avPacketAlloc = reinterpret_cast<AvPacketAllocFunc>(g_funcMap_avcodec[AV_PACKET_ALLOC]);
    m_encPkt = avPacketAlloc();
    if (m_encPkt == nullptr) {
        ERR("avcodecPacketAlloc Error during encoding.");
        return VIDEO_ENCODER_ENCODE_FAIL;
    }

    if (!InitCtxParams()) {
        ERR("Encoder InitCtxParams fail");
        return VIDEO_ENCODER_INIT_FAIL;
    }

    auto avcodecOpen2 = reinterpret_cast<AvcodecOpen2Func>(g_funcMap_avcodec[AVCODEC_OPEN2]);
    int ret = avcodecOpen2(m_encoderCtx, m_encCodec, nullptr);
    if (ret < 0) {
        ERR("Failed to open encode codec. Error code: %d", ret);
        return VIDEO_ENCODER_INIT_FAIL;
    }

    m_isInited = true;
    INFO("init encoder success");
    return VIDEO_ENCODER_SUCCESS;
}

bool VideoEncoderT432::LoadT432SharedLib()
{
    if (g_t432Loaded) {
        return true;
    }
    INFO("load %s", SHARED_LIB_AVCODEC.c_str());
    g_libHandleAvcodec = dlopen(SHARED_LIB_AVCODEC.c_str(), RTLD_LAZY);
    if (g_libHandleAvcodec == nullptr) {
        const char *errStr = dlerror();
        ERR("load %s error:%s", SHARED_LIB_AVCODEC.c_str(), (errStr != nullptr) ? errStr : "unknown");
        return false;
    }
    INFO("load %s", SHARED_LIB_AVUTIL.c_str());
    g_libHandleAvutil = dlopen(SHARED_LIB_AVUTIL.c_str(), RTLD_LAZY);
    if (g_libHandleAvutil == nullptr) {
        const char *errStr = dlerror();
        ERR("load %s error:%s", SHARED_LIB_AVUTIL.c_str(), (errStr != nullptr) ? errStr : "unknown");
        return false;
    }
    for (auto &symbol : g_funcMap_avcodec) {
        void *func = dlsym(g_libHandleAvcodec, symbol.first.c_str());
        if (func == nullptr) {
            ERR("failed to load %s", symbol.first.c_str());
            return false;
        }
        symbol.second = func;
    }
    for (auto &symbol : g_funcMap_avutil) {
        void *func = dlsym(g_libHandleAvutil, symbol.first.c_str());
        if (func == nullptr) {
            ERR("failed to load %s", symbol.first.c_str());
            return false;
        }
        symbol.second = func;
    }
    g_t432Loaded = true;
    return true;
}

bool VideoEncoderT432::InitCtxParams()
{
    m_encoderCtx->pix_fmt = AV_PIX_FMT_YUV420P;
    m_encoderCtx->width = m_encParams.width;
    m_encoderCtx->height = m_encParams.height;
    m_encoderCtx->time_base = (AVRational){1, static_cast<int>(m_encParams.framerate)};
    m_encoderCtx->framerate = (AVRational){static_cast<int>(m_encParams.framerate), 1};
    m_encoderCtx->bit_rate = m_encParams.bitrate;

    const std::string xcoderParams = "xcoder-params";
    std::string profileOpt = "profile=";
    std::string profileValue = "";
    if (m_codec == ENCODER_TYPE_T432_H264) {
        profileValue = profileOpt + g_transProfile[m_encParams.profile];
    } else if (m_codec == ENCODER_TYPE_T432_H265) {
        profileValue = profileOpt + g_transProfile_h265[m_encParams.profile];
    }

    std::string encodeParams = "gopPresetIdx=9:RcEnable=1:lowDelay=1";
    encodeParams = encodeParams + ":" + profileValue;

    auto avOptSet = reinterpret_cast<AvOptSetFunc>(g_funcMap_avutil[AV_OPT_SET]);
    int ret = avOptSet(m_encoderCtx->priv_data, xcoderParams.c_str(), encodeParams.c_str(), 0);
    if (ret != 0) {
        ERR("Set EncodeParams error.Error code: %d", ret);
        return false;
    }

    return true;
}
EncoderRetCode VideoEncoderT432::StartEncoder()
{
    INFO("start encoder success");
    return VIDEO_ENCODER_SUCCESS;
}

EncoderRetCode VideoEncoderT432::EncodeOneFrame(const uint8_t *inputData, uint32_t inputSize,
    uint8_t **outputData, uint32_t *outputSize)
{
    uint32_t frameSize = static_cast<uint32_t>(m_encParams.width * m_encParams.height * NUM_OF_PLANES / COMPRESS_RATIO);
    if (inputSize < frameSize) {
        ERR("input size error: size(%u) < frame size(%u)", inputSize, frameSize);
        return VIDEO_ENCODER_ENCODE_FAIL;
    }
    if (!VerifyParamsBeforeEncode()) {
        ERR("Before encode frame, params check fail.");
        return VIDEO_ENCODER_ENCODE_FAIL;
    }
    if (m_nextKeyFrameDistance > 0) {
        (void)ForceKeyFrame();
        m_nextKeyFrameDistance = 0;
    }

    if (!InitFrameData(inputData)) {
        return VIDEO_ENCODER_ENCODE_FAIL;
    }

    auto avcodecSendFrame = reinterpret_cast<AvcodecSendFrameFunc>(g_funcMap_avcodec[AVCODEC_SEND_FRAME]);
    int ret = avcodecSendFrame(m_encoderCtx, m_swFrame);
    auto avFrameFree = reinterpret_cast<AvFrameFreeFunc>(g_funcMap_avutil[AV_FRAME_FREE]);
    (void)avFrameFree(&m_swFrame);
    if (ret != 0) {
        ERR("avcodecSendFrame failed,ret=%d", ret);
        return VIDEO_ENCODER_ENCODE_FAIL;
    }

    if (!ReceiveOneFrame(outputData, outputSize)) {
        return VIDEO_ENCODER_ENCODE_FAIL;
    }

    return VIDEO_ENCODER_SUCCESS;
}

bool VideoEncoderT432::ReceiveOneFrame(uint8_t **outputData, uint32_t *outputSize)
{
    auto avcodecReceivePacket = reinterpret_cast<AvcodecReceivePacketFunc>(g_funcMap_avcodec[AVCODEC_RECEIVE_PACKET]);
    uint32_t count = 0;
    while (count < MAX_RECEIVE_TIMES) {
        int ret = avcodecReceivePacket(m_encoderCtx, m_encPkt);
        if (ret == 0) {
            *outputData = m_encPkt->data;
            *outputSize = m_encPkt->size;
            break;
        } else if (ret == AVERROR(EAGAIN)) {
            (void)usleep(RECEIVE_INTERVAL);
            ++count;
            continue;
        } else {
            ERR("avcodecReceivePacket failed, ret=%d", ret);
            return false;
        }
    }
    if (count == MAX_RECEIVE_TIMES) {
        WARN("avcodecReceivePacket no data during %u ms", MAX_RECEIVE_TIMES * RECEIVE_INTERVAL / US_TO_MS);
    }
    return true;
}

bool VideoEncoderT432::InitFrameData(const uint8_t *src)
{
    if (src == nullptr) {
        ERR("input data buffer is null");
        return false;
    }
    auto avFrameAlloc = reinterpret_cast<AvFrameAllocFunc>(g_funcMap_avutil[AV_FRAME_ALLOC]);
    m_swFrame = avFrameAlloc();
    if (m_swFrame == nullptr) {
        ERR("Alloc m_swFrame failed.");
        return false;
    }
    m_swFrame->width = m_encParams.width;
    m_swFrame->height = m_encParams.height;
    m_swFrame->format = AV_PIX_FMT_YUV420P;

    auto avFrameGetBuffer = reinterpret_cast<AvFrameGetBufferFunc>(g_funcMap_avutil[AV_FRAME_GET_BUFFER]);
    int ret = avFrameGetBuffer(m_swFrame, 32);
    if (ret != 0) {
        ERR("avFrameGetBuffer failed.Error code: %d", ret);
        auto avFrameFree = reinterpret_cast<AvFrameFreeFunc>(g_funcMap_avutil[AV_FRAME_FREE]);
        (void)avFrameFree(&m_swFrame);
        return false;
    }

    for (int i = 0; i < m_swFrame->height; i++) {
        std::copy_n(src + i * m_swFrame->width, m_swFrame->width, m_swFrame->data[0] + i * m_swFrame->linesize[0]);
    }

    for (int i = 0; i < m_swFrame->height / FRAME_ALIGN_U_INDEX; i++) {
        std::copy_n(src + m_swFrame->width * (m_swFrame->height + i / FRAME_ALIGN_U_INDEX),
                    m_swFrame->width / FRAME_ALIGN_U_INDEX,
                    m_swFrame->data[1] + i * m_swFrame->linesize[1]);
    }

    for (int i = 0; i < m_swFrame->height / FRAME_ALIGN_U_INDEX; i++) {
        std::copy_n(src + m_swFrame->width * m_swFrame->height +
                    m_swFrame->width * m_swFrame->height / FRAME_ALIGN_V_INDEX +
                    i * m_swFrame->width / FRAME_ALIGN_U_INDEX,
                    m_swFrame->width / FRAME_ALIGN_U_INDEX,
                    m_swFrame->data[FRAME_ALIGN_U_INDEX] + i * m_swFrame->linesize[FRAME_ALIGN_U_INDEX]);
    }
    return true;
}

EncoderRetCode VideoEncoderT432::StopEncoder()
{
    INFO("stop encoder success");
    return VIDEO_ENCODER_SUCCESS;
}

void VideoEncoderT432::CheckFuncPtr()
{
    for (auto &symbol : g_funcMap_avcodec) {
        if (symbol.second == nullptr) {
            m_funPtrError = true;
            ERR("%s ptr is nullptr", symbol.first.c_str());
        }
    }
    for (auto &symbol : g_funcMap_avutil) {
        if (symbol.second == nullptr) {
            m_funPtrError = true;
            ERR("%s ptr is nullptr", symbol.first.c_str());
        }
    }
}

void VideoEncoderT432::DestroyEncoder()
{
    if (!m_isInited) {
        INFO("Destroy encoder already triggered, return");
        return;
    }
    INFO("destroy encoder start");
    if ((g_libHandleAvcodec) == nullptr && (g_libHandleAvutil) == nullptr) {
        WARN("encoder has been destroyed");
        return;
    }
    CheckFuncPtr();
    auto AvcodecFreeContext = reinterpret_cast<AvcodecFreeContextFunc>(g_funcMap_avcodec[AVCODEC_FREE_CONTEXT]);
    AvcodecFreeContext(&m_encoderCtx);
    auto avPacketFree = reinterpret_cast<AvPacketFreeFunc>(g_funcMap_avcodec[AV_PACKET_FREE]);
    (void)avPacketFree(&m_encPkt);
    if (m_funPtrError) {
        UnLoadT432SharedLib();
    }
    m_isInited = false;
    INFO("destroy encoder done");
}

void VideoEncoderT432::UnLoadT432SharedLib()
{
    INFO("UnLoadT432SharedLib");
    for (auto &symbol : g_funcMap_avcodec) {
        symbol.second = nullptr;
    }
    for (auto &symbol : g_funcMap_avutil) {
        symbol.second = nullptr;
    }
    (void)dlclose(g_libHandleAvcodec);
    (void)dlclose(g_libHandleAvutil);
    g_libHandleAvcodec = nullptr;
    g_libHandleAvutil = nullptr;
    g_t432Loaded = false;
    m_funPtrError = false;
}

EncoderRetCode VideoEncoderT432::ResetEncoder()
{
    INFO("resetting encoder");
    DestroyEncoder();
    EncoderRetCode ret = InitEncoder();
    if (ret != VIDEO_ENCODER_SUCCESS) {
        ERR("init encoder failed %#x while resetting", ret);
        return VIDEO_ENCODER_RESET_FAIL;
    }
    ret = StartEncoder();
    if (ret != VIDEO_ENCODER_SUCCESS) {
        ERR("start encoder failed %#x while resetting", ret);
        return VIDEO_ENCODER_RESET_FAIL;
    }
    INFO("reset encoder success");
    return VIDEO_ENCODER_SUCCESS;
}

EncoderRetCode VideoEncoderT432::ForceKeyFrame()
{
    INFO("force key frame success");
    return VIDEO_ENCODER_SUCCESS;
}

/**
* @功能描述: 初始化视频流模式下的编码参数
*/
EncoderRetCode VideoEncoderT432::Config(const VmiEncoderConfig& config)
{
    return GeneralConfig(config);
}

/**
* @功能描述: 参数有效性由上层检验，动态修改编码相关参数
*/
EncoderRetCode VideoEncoderT432::SetParams(const VmiEncoderParams& params)
{
    return GeneralSetParams(params);
}