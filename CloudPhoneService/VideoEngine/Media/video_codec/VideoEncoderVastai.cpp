/*
 * 功能说明: 适配Vastai硬件视频编码器，包括编码器初始化、启动、编码、停止、销毁等
 */

#define CONFIG_VASTAPI 1
#define LOG_TAG "VideoEncoderVastai"
#include "VideoEncoderVastai.h"
#include "dirent.h"
#include <dlfcn.h>
#include <algorithm>
#include <cstring>
#include <string>
#include <atomic>
#include <unistd.h>
#include "MediaLog.h"
#include "Property.h"
#include <chrono>

namespace {
    constexpr int NUM_OF_PLANES = 3;
    constexpr int COMPRESS_RATIO = 2;
    constexpr int ENCODE_BUFFER_POOL_SIZE = 32;    //缓冲池的大小性能最优建议为32
    constexpr uint32_t MAX_RECEIVE_TIMES = 500;
    constexpr int RECEIVE_INTERVAL = 1000;  // 1000us = 1ms
    constexpr uint32_t US_TO_MS = 1000;
    const std::string ENCODER_TYPE_VASTAI_H264 = "h264_vastapi";
    const std::string ENCODER_TYPE_VASTAI_H265 = "hevc_vastapi";
    const std::string ENCODER_DEVICE_PATH = "/dev/";

    const std::string AV_HWFRAME_CTX_ALLOC = "av_hwframe_ctx_alloc";
    const std::string AV_HWFRAME_CTX_INIT = "av_hwframe_ctx_init";
    const std::string AV_BUFFER_REF = "av_buffer_ref";
    const std::string AV_BUFFER_UNREF = "av_buffer_unref";
    const std::string AVCODEC_FIND_ENCODER_BY_NAME = "avcodec_find_encoder_by_name";
    const std::string AVCODEC_ALLOC_CONTEXT3 = "avcodec_alloc_context3";
    const std::string AVCODEC_FREE_CONTEXT = "avcodec_free_context";
    const std::string AV_OPT_SET = "av_opt_set";
    const std::string AV_OPT_SET_INT = "av_opt_set_int";
    const std::string AVCODEC_OPEN2 = "avcodec_open2";
    const std::string AV_FRAME_ALLOC = "av_frame_alloc";
    const std::string AV_FRAME_ALLOC_VASTAI = "av_frame_alloc_vastai";
    const std::string AV_FRAME_FREE = "av_frame_free";
    const std::string AV_LOG_SET_LEVEL = "av_log_set_level";
    const std::string AV_INIT_PACKET = "av_init_packet";
    const std::string AV_PACKET_UNREF = "av_packet_unref";
    const std::string AV_PACKET_ALLOC = "av_packet_alloc";
    const std::string AV_PACKET_FREE = "av_packet_free";
    const std::string AVCODEC_SEND_FRAME = "avcodec_send_frame";
    const std::string AVCODEC_RECEIVE_PACKET = "avcodec_receive_packet";
    const std::string AV_FRAME_GET_BUFFER = "av_frame_get_buffer";
    const std::string AV_HWFRAME_TRANSFER_DATA = "av_hwframe_transfer_data";
    const std::string AV_HWFRAME_TRANSFER_DATA_EX = "av_hwframe_transfer_data_ex";
    const std::string AV_HWDEVICE_CTX_CREATE = "av_hwdevice_ctx_create";
    const std::string AV_HWFRAME_GET_BUFFER = "av_hwframe_get_buffer";
    
    using AvcodecSendFrameFunc = int (*)(AVCodecContext *avctx, const AVFrame *frame);
    using AvcodecReceivePacketFunc = int (*)(AVCodecContext *avctx, AVPacket *avpkt);
    using AvInitPacketFunc = void(*)(AVPacket *pkt);
    using AvPacketUnrefFunc = void (*)(AVPacket *pkt);
    using AvPacketAllocFunc = AVPacket *(*)(void);
    using AvPacketFreeFunc = void (*)(AVPacket **pkt);
    using AvHwframeCtxAllocFunc = AVBufferRef *(*)(AVBufferRef *device_ctx);
    using AvHwframeCtxInitFunc = int (*)(AVBufferRef *ref);
    using AvBufferUnrefFunc = void(*)(AVBufferRef **buf);
    using AvcodecFindEncoderByNameFunc = AVCodec *(*)(const char *name);
    using AvcodecAllocContext3Func = AVCodecContext *(*)(const AVCodec *codec);
    using AvcodecFreeContextFunc = void (*)(AVCodecContext **pavctx);
    using AvBufferRefFunc = AVBufferRef *(*)(AVBufferRef *buf);
    using AvOptSetFunc = int (*)(void *obj, const char *name, const char *val, int search_flags);
    using AvOptSetIntFunc = int (*)(void *obj, const char *name, int64_t val, int search_flags);
    using AvcodecOpen2Func = int (*)(AVCodecContext *avctx, const AVCodec *codec, AVDictionary **options);
    using AvFrameAllocFunc = AVFrame *(*)(void);
    using AvFrameAllocVastaiFunc = AVFrame *(*)(AVBufferRef *hw_frame_ctx, VastFrameInfo *frame_info);
    using AvFrameFreeFunc = void(*)(AVFrame **frame);
    using AvFrameGetBufferFunc = int (*)(AVFrame *frame, int align);
    using AvHwframeTransferDataFunc = int (*)(AVFrame *dst, const AVFrame *src, int flags);
    using AvHwframeTransferDataExFunc = int (*)(AVFrame *dst, const AVFrame *src, int srcType);
    using AvHwdeviceCtxCreateFunc = int (*)(AVBufferRef **device_ctx, enum AVHWDeviceType type, const char *device,
        AVDictionary *opts, int flags);
    using AvHwframeGetBufferFunc = int (*)(AVBufferRef *hwframe_ctx, AVFrame *frame, int flags);
    using AVLogSetLevelFunc = void (*)(int level);


    std::unordered_map<std::string, std::string> g_transProfile = {
        {"baseline", "66"},
        {"main", "77"},
        {"high", "100"}};

    std::unordered_map<std::string, std::string> g_transProfile_h265 = {
        {"main", "1"}};

    std::unordered_map<std::string, void*> g_funcMap_avcodec = {
        { AVCODEC_FIND_ENCODER_BY_NAME, nullptr },
        { AVCODEC_ALLOC_CONTEXT3, nullptr },
        { AVCODEC_FREE_CONTEXT, nullptr },
        { AVCODEC_OPEN2, nullptr },
        { AV_INIT_PACKET, nullptr },
        { AV_PACKET_UNREF, nullptr },
        { AV_PACKET_ALLOC, nullptr },
        { AV_PACKET_FREE, nullptr },
        { AVCODEC_SEND_FRAME, nullptr },
        { AVCODEC_RECEIVE_PACKET, nullptr }
        };

    std::unordered_map<std::string, void*> g_funcMap_avutil = {
        { AV_HWFRAME_CTX_ALLOC, nullptr },
        { AV_HWFRAME_CTX_INIT, nullptr },
        { AV_OPT_SET, nullptr },
        { AV_OPT_SET_INT, nullptr },
        { AV_FRAME_GET_BUFFER, nullptr },
        { AV_HWFRAME_TRANSFER_DATA, nullptr },
        { AV_HWFRAME_TRANSFER_DATA_EX, nullptr },
        { AV_HWDEVICE_CTX_CREATE, nullptr },
        { AV_HWFRAME_GET_BUFFER, nullptr },
        { AV_FRAME_ALLOC, nullptr },
        { AV_FRAME_ALLOC_VASTAI, nullptr },
        { AV_FRAME_FREE, nullptr },
        { AV_BUFFER_REF, nullptr },
        { AV_BUFFER_UNREF, nullptr },
        { AV_LOG_SET_LEVEL, nullptr }
        };

    const std::string SHARED_LIB_AVCODEC = "libavcodec.so";
    const std::string SHARED_LIB_AVUTIL = "libavutil.so";
    const std::string basePath = "/dev/";
    std::string m_renderName;
    std::atomic<bool> g_vastaiLoaded = { false };
    void *g_libHandleAvcodec = nullptr;
    void *g_libHandleAvutil = nullptr;
}

VideoEncoderVastai::VideoEncoderVastai(VACodecType codecType, EncoderFormat codecFormat)
{
    if (codecType == VA_CODEC_TYPE_H264) {
        m_codec = ENCODER_TYPE_VASTAI_H264;
    } else {
        m_codec = ENCODER_TYPE_VASTAI_H265;
        m_encParams.bitrate = static_cast<uint32_t>(BITRATE_DEFAULT_265);
        m_encParams.profile = ENCODE_PROFILE_MAIN;
    }
    m_codecFormat = codecFormat;
    INFO("VideoEncoderVastai constructed %s", (m_codec == ENCODER_TYPE_VASTAI_H264) ? "h.264" : "h.265");
}
 
VideoEncoderVastai::~VideoEncoderVastai()
{
    DestroyEncoder();
    INFO("VideoEncoderVastai destructed");
}

void VideoEncoderVastai::ReadDevicePath()
{
    DIR *dir= opendir(basePath.c_str());
    struct dirent *ptr;

    while ((ptr = readdir(dir)) != NULL) {
        if ((ptr->d_name == ".") || (ptr->d_name == ".."))
            continue;
        if (strstr(ptr->d_name, "vastai_video")) {
            break;
        }
    }
    std::string nodeName;
    nodeName = ptr->d_name;
    m_renderName = basePath + nodeName;
    INFO("RenderName=%s",m_renderName.c_str());
    closedir(dir);
}

void VideoEncoderVastai::FreeEncoderCtx()
{
    if (m_encoderCtx) {
        auto AvcodecFreeContext = reinterpret_cast<AvcodecFreeContextFunc>(g_funcMap_avcodec[AVCODEC_FREE_CONTEXT]);
        AvcodecFreeContext(&m_encoderCtx);
        m_encoderCtx = nullptr;
    }
}

void VideoEncoderVastai::FreeHwDeviceCtx()
{
    if (m_hwDeviceCtx) {
        auto avBufferUnref = reinterpret_cast<AvBufferUnrefFunc>(g_funcMap_avutil[AV_BUFFER_UNREF]);
        avBufferUnref(&m_hwDeviceCtx);
        m_hwDeviceCtx = nullptr;
    }
}

void VideoEncoderVastai::FreeEncPkt()
{
    if (m_encPkt) {
        auto avPacketFree = reinterpret_cast<AvPacketFreeFunc>(g_funcMap_avcodec[AV_PACKET_FREE]);
        avPacketFree(&m_encPkt);
        m_encPkt = nullptr;
    }
}

EncoderRetCode VideoEncoderVastai::VastaiCreateEncoder()
{
    auto avHwdeviceCtxCreate = reinterpret_cast<AvHwdeviceCtxCreateFunc>(g_funcMap_avutil[AV_HWDEVICE_CTX_CREATE]);
    int ret = avHwdeviceCtxCreate(&m_hwDeviceCtx, AV_HWDEVICE_TYPE_VASTAPI, m_renderName.c_str(), nullptr, 0); 
    if (ret < 0) {
        ERR("Could not create hwdevice, %d", ret);
        (void)FreeEncPkt();
        return VIDEO_ENCODER_INIT_FAIL;
    }

    auto avcodecFindEncoderByName = reinterpret_cast<AvcodecFindEncoderByNameFunc>
        (g_funcMap_avcodec[AVCODEC_FIND_ENCODER_BY_NAME]);
    m_encCodec = avcodecFindEncoderByName(m_codec.c_str());
    if (m_encCodec == nullptr) {
        ERR("Could not find encoder");
        (void)FreeEncPkt();
        (void)FreeHwDeviceCtx();
        return VIDEO_ENCODER_INIT_FAIL;
    }

    auto avcodecAllocContext3 = reinterpret_cast<AvcodecAllocContext3Func>(g_funcMap_avcodec[AVCODEC_ALLOC_CONTEXT3]);
    m_encoderCtx = avcodecAllocContext3(m_encCodec);
    if (m_encoderCtx == nullptr) {
        ERR("Could not create encoder context");
        (void)FreeEncPkt();
        (void)FreeHwDeviceCtx();
        return VIDEO_ENCODER_INIT_FAIL;
    }
    return VIDEO_ENCODER_SUCCESS;
}

EncoderRetCode VideoEncoderVastai::VastaiEncodeFrame(uint8_t **outputData, uint32_t *outputSize)
{
    if (m_encPkt == nullptr) {
        ERR("avPacketAlloc fail");
        return VIDEO_ENCODER_INIT_FAIL;
    }

    auto avcodecSendFrame = reinterpret_cast<AvcodecSendFrameFunc>(g_funcMap_avcodec[AVCODEC_SEND_FRAME]);   
    auto avFrameFree = reinterpret_cast<AvFrameFreeFunc>(g_funcMap_avutil[AV_FRAME_FREE]);                    
    int ret = avcodecSendFrame(m_encoderCtx, m_hwFrame);           
    if (ret != 0) {
        ERR("avcodecSendFrame Error during encoding.");
        (void)avFrameFree(&m_swFrame);
        (void)avFrameFree(&m_hwFrame);
        (void)FreeEncPkt();
        return VIDEO_ENCODER_ENCODE_FAIL;
    }
    auto avcodecReceivePacket = reinterpret_cast<AvcodecReceivePacketFunc>(g_funcMap_avcodec[AVCODEC_RECEIVE_PACKET]);
    uint32_t count = 0;
    while (count < MAX_RECEIVE_TIMES) {
        ret = avcodecReceivePacket(m_encoderCtx, m_encPkt);
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
            (void)avFrameFree(&m_swFrame);
            (void)avFrameFree(&m_hwFrame);
            (void)FreeEncPkt();
            return VIDEO_ENCODER_ENCODE_FAIL;
        }
    }
    if (count == MAX_RECEIVE_TIMES) {
        WARN("avcodecReceivePacket no data during %u ms", MAX_RECEIVE_TIMES * RECEIVE_INTERVAL / US_TO_MS);
        return VIDEO_ENCODER_ENCODE_FAIL;
    }

    (void)avFrameFree(&m_swFrame);
    (void)avFrameFree(&m_hwFrame);

    return VIDEO_ENCODER_SUCCESS;
}

EncoderRetCode VideoEncoderVastai::InitEncoder()
{
    if ((!GetRoEncParam()) || (!GetPersistEncParam())) {
        ERR("init encoder failed: GetEncParam failed");
        return VIDEO_ENCODER_INIT_FAIL;
    }
    m_encParams = m_tmpEncParams;
    if (m_codec == ENCODER_TYPE_VASTAI_H265) {
        m_encParams.profile = ENCODE_PROFILE_MAIN;
        m_tmpEncParams.profile = ENCODE_PROFILE_MAIN;
    }
    if (!LoadVastaiSharedLib()) {
        ERR("init encoder failed: load Vastai so error");
        return VIDEO_ENCODER_INIT_FAIL;
    }
    int ret = VIDEO_ENCODER_SUCCESS;
    m_width = static_cast<int>(m_encParams.width);
    m_height = static_cast<int>(m_encParams.height);

    (void)ReadDevicePath();
    auto avPacketAlloc = reinterpret_cast<AvPacketAllocFunc>(g_funcMap_avcodec[AV_PACKET_ALLOC]);
    m_encPkt = avPacketAlloc();
    if (m_encPkt == nullptr) {
        ERR("avPacketAlloc fail");
        return VIDEO_ENCODER_INIT_FAIL;
    }

    if (VastaiCreateEncoder() != VIDEO_ENCODER_SUCCESS) {
        ERR("VastaiCreateEncoder fail");
        return VIDEO_ENCODER_INIT_FAIL;
    }

    if (!InitCodec(m_encoderCtx, m_hwDeviceCtx)) {
        ERR("Encoder InitCodec fail");
        (void)FreeEncPkt();
        (void)FreeEncoderCtx();
        (void)FreeHwDeviceCtx();
        return VIDEO_ENCODER_INIT_FAIL;
    }

    auto avcodecOpen2 = reinterpret_cast<AvcodecOpen2Func>(g_funcMap_avcodec[AVCODEC_OPEN2]);
    if ((ret = avcodecOpen2(m_encoderCtx, m_encCodec, nullptr)) < 0) {
        ERR("Failed to open encode codec. Error code: %d", ret);
        (void)FreeEncPkt();
        (void)FreeEncoderCtx();
        (void)FreeHwDeviceCtx();
        return VIDEO_ENCODER_INIT_FAIL;
    }
    m_isInited = true;
    INFO("init encoder success");
    return VIDEO_ENCODER_SUCCESS;
}

bool VideoEncoderVastai::LoadVastaiSharedLib()
{
    if (g_vastaiLoaded) {
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
    g_vastaiLoaded = true;
    return true;
}

bool VideoEncoderVastai:: InitCodec(AVCodecContext *ctx, AVBufferRef *m_hwDeviceCtx)
{
    if (!InitCtxParams()) {
        ERR("Encoder InitCtxParams fail");
        return VIDEO_ENCODER_INIT_FAIL;
    }
    auto avHwframeCtxAlloc = reinterpret_cast<AvHwframeCtxAllocFunc>(g_funcMap_avutil[AV_HWFRAME_CTX_ALLOC]);
    if (!(m_hwFramesRef = avHwframeCtxAlloc(m_hwDeviceCtx))) {
        ERR("Failed to create VASTAPI frame context.");
        return false;
    }

    m_framesCtx = (AVHWFramesContext *)(m_hwFramesRef->data);
    m_framesCtx->sw_format = AV_PIX_FMT_YUV420P;
    m_framesCtx->format    = AV_PIX_FMT_VASTAPI;
    m_framesCtx->width     = ctx->width;
    m_framesCtx->height    = ctx->height;
    m_framesCtx->initial_pool_size = ENCODE_BUFFER_POOL_SIZE;

    auto avHwframeCtxInit = reinterpret_cast<AvHwframeCtxInitFunc>(g_funcMap_avutil[AV_HWFRAME_CTX_INIT]);
    int ret = avHwframeCtxInit(m_hwFramesRef);
    if (ret < 0) {
        ERR("Failed to initialize VASTAPI frame context. Error code: %d",ret);
        auto avBufferUnref = reinterpret_cast<AvBufferUnrefFunc>(g_funcMap_avutil[AV_BUFFER_UNREF]);
        (void)avBufferUnref(&m_hwFramesRef);
        return false;
    }

    auto avBufferRef = reinterpret_cast<AvBufferRefFunc>(g_funcMap_avutil[AV_BUFFER_REF]);
    ctx->hw_frames_ctx = avBufferRef(m_hwFramesRef);
    if (ctx->hw_frames_ctx == nullptr) {
        return false;
    }

    auto avBufferUnref = reinterpret_cast<AvBufferUnrefFunc>(g_funcMap_avutil[AV_BUFFER_UNREF]);
    (void)avBufferUnref(&m_hwFramesRef);

    return true;
}
bool VideoEncoderVastai:: InitCtxParams()
{
    m_encoderCtx->sw_pix_fmt = AV_PIX_FMT_YUV420P;
    m_encoderCtx->pix_fmt = AV_PIX_FMT_VASTAPI;
    m_encoderCtx->width = m_width;
    m_encoderCtx->height = m_height;
    m_encoderCtx->time_base = (AVRational){1, static_cast<int>(m_encParams.framerate)};
    m_encoderCtx->framerate = (AVRational){static_cast<int>(m_encParams.framerate), 1};
    m_encoderCtx->bit_rate = m_encParams.bitrate;

    auto avOptSet = reinterpret_cast<AvOptSetFunc>(g_funcMap_avutil[AV_OPT_SET]);
    int ret = avOptSet(m_encoderCtx->priv_data, "vast-params", "preset=bronze_quality:miniGopSize=1:lookaheadLength=0:\
        llRc=5:inputAlignmentExp=1:refAlignmentExp=1:refChromaAlignmentExp=1", 0);
    if (ret != 0) { 
        ERR("Set EncodeParams error.Error code: %d",ret);
        return false;
    }
    auto tmpProfile = StrToInt(g_transProfile[m_encParams.profile]);
    if ((m_codec == ENCODER_TYPE_VASTAI_H265)) {
        tmpProfile = StrToInt(g_transProfile_h265[m_encParams.profile]);
    } 
    auto avOptSetInt = reinterpret_cast<AvOptSetIntFunc>(g_funcMap_avutil[AV_OPT_SET_INT]);
    ret = avOptSetInt(m_encoderCtx->priv_data, "profile", tmpProfile , 0);
    if (ret != 0) {             
        ERR("Set profile error.Error code: %d",ret);
        return false;
    }
    return true;
}
EncoderRetCode VideoEncoderVastai::StartEncoder()
{
    INFO("start encoder success");
    return VIDEO_ENCODER_SUCCESS;
}

EncoderRetCode VideoEncoderVastai::EncodeOneFrame(const uint8_t *inputData, uint32_t inputSize,
    uint8_t **outputData, uint32_t *outputSize)
{
    uint32_t frameSize = static_cast<uint32_t>(m_width * m_height * NUM_OF_PLANES / COMPRESS_RATIO);
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

    if (VastaiEncodeFrame(outputData, outputSize) != VIDEO_ENCODER_SUCCESS) {
        return VIDEO_ENCODER_ENCODE_FAIL;
    }

    return VIDEO_ENCODER_SUCCESS;
}

bool VideoEncoderVastai::InitFrameData(const uint8_t *src)
{
    if (src == nullptr) {
        ERR("input data buffer is null");
        return false;
    }

    auto avFrameAlloc = reinterpret_cast<AvFrameAllocFunc>(g_funcMap_avutil[AV_FRAME_ALLOC]);
    m_hwFrame = avFrameAlloc();
    if (m_hwFrame == nullptr) {
        ERR("Alloc m_hwFrame failed.");
        return false;
    }

    auto avHwframeGetBuffer = reinterpret_cast<AvHwframeGetBufferFunc>(g_funcMap_avutil[AV_HWFRAME_GET_BUFFER]);
    int ret = avHwframeGetBuffer(m_encoderCtx->hw_frames_ctx, m_hwFrame, 0);
    if (ret != 0) {
        ERR("avHwframeGetBuffer failed.");
        return false;
    }

    m_swFrame = avFrameAlloc();
    if (m_swFrame == nullptr) {
        ERR("Alloc m_swFrame failed.");
        auto avFrameFree = reinterpret_cast<AvFrameFreeFunc>(g_funcMap_avutil[AV_FRAME_FREE]);
        (void)avFrameFree(&m_hwFrame);
        return false;
    }

    m_swFrame->width = m_encoderCtx->width;
    m_swFrame->height = m_encoderCtx->height;
    m_swFrame->linesize[0] = m_encoderCtx->width;
    m_swFrame->linesize[1] = m_encoderCtx->width / COMPRESS_RATIO;
    m_swFrame->opaque = const_cast<unsigned char *>(src);
    m_hwFrame->opaque = const_cast<unsigned char *>(src);

    auto avHwframeTransferDataEx =
        reinterpret_cast<AvHwframeTransferDataExFunc>(g_funcMap_avutil[AV_HWFRAME_TRANSFER_DATA_EX]);
    if (avHwframeTransferDataEx(m_hwFrame, m_swFrame, 1) != VIDEO_ENCODER_SUCCESS) {
        ERR("avHwframeTransferDataEx failed.Error code: %d", ret);
        auto avFrameFree = reinterpret_cast<AvFrameFreeFunc>(g_funcMap_avutil[AV_FRAME_FREE]);
        (void)avFrameFree(&m_swFrame);
        (void)avFrameFree(&m_hwFrame);
        return false;
    }

    return true;
}

EncoderRetCode VideoEncoderVastai::StopEncoder()
{
    INFO("stop encoder success");
    return VIDEO_ENCODER_SUCCESS;
}

void VideoEncoderVastai::CheckFuncPtr()
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

void VideoEncoderVastai::DestroyEncoder()
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
    auto avPacketFree = reinterpret_cast<AvPacketFreeFunc>(g_funcMap_avcodec[AV_PACKET_FREE]);
    (void)avPacketFree(&m_encPkt);
    auto AvcodecFreeContext = reinterpret_cast<AvcodecFreeContextFunc>(g_funcMap_avcodec[AVCODEC_FREE_CONTEXT]);
    AvcodecFreeContext(&m_encoderCtx);
    auto avBufferUnref = reinterpret_cast<AvBufferUnrefFunc>(g_funcMap_avutil[AV_BUFFER_UNREF]);
    (void)avBufferUnref(&m_hwDeviceCtx);

    if (m_funPtrError) {
        UnLoadVastaiSharedLib();
    }
    m_isInited = false;
}

void VideoEncoderVastai::UnLoadVastaiSharedLib()
{
    INFO("UnLoadVastaiSharedLib");
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
    g_vastaiLoaded = false;
    m_funPtrError = false;
}

EncoderRetCode VideoEncoderVastai::ResetEncoder()
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

EncoderRetCode VideoEncoderVastai::ForceKeyFrame()
{
    INFO("force key frame success");
    return VIDEO_ENCODER_SUCCESS;
}

/**
* @功能描述: 初始化视频流模式下的编码参数
*/
EncoderRetCode VideoEncoderVastai::Config(const VmiEncoderConfig& config)
{
    return GeneralConfig(config);
}

/**
* @功能描述: 参数有效性由上层检验，动态修改编码相关参数
*/
EncoderRetCode VideoEncoderVastai::SetParams(const VmiEncoderParams& params)
{
    return GeneralSetParams(params);
}