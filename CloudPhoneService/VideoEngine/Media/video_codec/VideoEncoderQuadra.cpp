/*
 * 版权所有 (c) 华为技术有限公司 2023-2023
 * 功能说明: 适配Quadra硬件视频编码器，包括编码器初始化、启动、编码、停止、销毁等
 */
#define CONFIG_VASTAPI 0
#define CONFIG_NETINT 1
#define LOG_TAG "VideoEncoderQuadra"
#include "VideoEncoderQuadra.h"
#include <dlfcn.h>
#include <algorithm>
#include <cstring>
#include <string>
#include <atomic>
#include <unistd.h>
#include "logging.h"
#include <string>
#include <sstream>

namespace {
    constexpr int NUM_OF_PLANES = 3;
    constexpr int COMPRESS_RATIO = 2;
    constexpr int FRAME_ALIGN_U_INDEX = 2;
    constexpr int FRAME_ALIGN_V_INDEX = 4;
    const std::string ENCODER_TYPE_QUADRA_H264 = "h264_ni_quadra_enc";
    const std::string ENCODER_TYPE_QUADRA_H265 = "h265_ni_quadra_enc";
    const std::string ENCODER_TYPE_T432_H264 = "h264_ni_logan_enc";
    const std::string ENCODER_TYPE_T432_H265 = "h265_ni_logan_enc";
    constexpr uint32_t MAX_RECEIVE_TIMES = 500;
    constexpr int RECEIVE_INTERVAL = 1000;  // 1000us = 1ms
    constexpr uint32_t US_TO_MS = 1000;
    constexpr uint32_t NI_FRAME_AUX_DATA_VBV_BUFFER_SIZE = 20;
    constexpr uint32_t NI_FRAME_AUX_DATA_CRF = 17;


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
    const std::string AV_FRAME_NEW_SIDE_DATA = "av_frame_new_side_data";
    const std::string AV_FRAME_GET_SIDE_DATA = "av_frame_get_side_data";
    const std::string AV_FRAME_REMOVE_SIDE_DATA = "av_frame_remove_side_data";
    const std::string AV_FILTER_GET_BY_NAME = "avfilter_get_by_name";
    const std::string AV_FILTER_IN_OUT_ALLOC = "avfilter_inout_alloc";
    const std::string AV_FILTER_GRAPH_ALLOC = "avfilter_graph_alloc";
    const std::string AV_FILTER_GRAPH_CREATE_FILTER = "avfilter_graph_create_filter";
    const std::string AV_BUFFERSRC_PARAMTERS_SET = "av_buffersrc_parameters_set";
    const std::string AV_OPT_SET_INT_LIST = "av_opt_set_int_list";
    const std::string AV_OPT_SET_BIN = "av_opt_set_bin";
    const std::string AV_FILTER_GRAPH_PARSE_PTR = "avfilter_graph_parse_ptr";
    const std::string AV_FILTER_GRAPH_CONFIG = "avfilter_graph_config";
    const std::string AV_FILTER_INOUT_FREE = "avfilter_inout_free";
    const std::string AV_BUFFERSRC_ADD_FRAME_FLAGS = "av_buffersrc_add_frame_flags";
    const std::string AV_BUFFERSINK_GET_FRAME = "av_buffersink_get_frame";
    const std::string AV_STR_DUP = "av_strdup";
    const std::string AV_FILTER_GRAPH_FREE = "avfilter_graph_free";
    const std::string AV_FRAME_UNREF = "av_frame_unref";
    using AVFrameUnrefFunc = void (*)(AVFrame *frame);
    using AVLogSetCallbackFunc = void (*)(void (*callback)(void*, int, const char*, va_list));
    using AVLogSetFunc = void (*)(int level);
    using AVFilterGraphFreeFunc = void (*)(AVFilterGraph **graph);
    using AVStrDupFunc = char *(*)(const char *s);
    using AVOptSetBinFunc = int (*)(void *obj, const char *name, const uint8_t *val, int size, int search_flags);
    using AVBuffersrcAddFrameFlagsFunc = int (*)(AVFilterContext *buffer_src, AVFrame *frame, int flags);
    using AVBuffersinkGetFrameFunc = int (*)(AVFilterContext *ctx, AVFrame *frame);
    using AVFilterInOutFreeFunc = void (*)(AVFilterInOut **inout);
    using AVFilterGraphConfigFunc = int (*)(AVFilterGraph *graphctx, void *log_ctx);
    using AVFilterGraphParsePtrFunc = int (*)(AVFilterGraph *graph, const char *filters,
                             AVFilterInOut **inputs, AVFilterInOut **outputs,
                             void *log_ctx);
    using AVBuffersrcParametersSetFunc = int (*)(AVFilterContext *ctx, AVBufferSrcParameters *param);
    using AVFilterGraphCreateFilterFunc = int (*)(AVFilterContext **filt_ctx, const AVFilter *filt,
                                 const char *name, const char *args, void *opaque,
                                 AVFilterGraph *graph_ctx);
    using AVFilterGraphAllocFunc = AVFilterGraph *(*)(void);
    using AVFilterInOutAllocFunc = AVFilterInOut *(*)(void);
    using AVFilterGetByNameFunc = AVFilter *(*)(const char *name);
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
    using AvFrameNewSideDataFunc = AVFrameSideData *(*)(AVFrame *frame, enum AVFrameSideDataType type, int size);
    using AvFrameGetSideDataFunc = AVFrameSideData *(*)(const AVFrame *frame, enum AVFrameSideDataType type);
    using AvFrameRemoveSideDataFunc = void (*)(AVFrame *frame, enum AVFrameSideDataType type);
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
        { AV_BUFFER_UNREF, nullptr },
        { AV_FRAME_NEW_SIDE_DATA, nullptr },
        { AV_FRAME_GET_SIDE_DATA, nullptr },
        { AV_FRAME_REMOVE_SIDE_DATA, nullptr },
        { AV_STR_DUP, nullptr},
        { AV_LOG_SET_LEVEL, nullptr},
        { AV_LOG_SET_CALLBACK, nullptr},
        { AV_FRAME_UNREF, nullptr}
        };

    std::unordered_map<std::string, void*> g_funcMap_avfilter = {
        { AV_FILTER_GET_BY_NAME, nullptr },
        { AV_FILTER_IN_OUT_ALLOC, nullptr },
        { AV_FILTER_GRAPH_ALLOC, nullptr },
        { AV_FILTER_GRAPH_CREATE_FILTER, nullptr },
        { AV_BUFFERSRC_PARAMTERS_SET, nullptr },
        { AV_FILTER_GRAPH_PARSE_PTR, nullptr },
        { AV_FILTER_GRAPH_CONFIG, nullptr },
        { AV_FILTER_INOUT_FREE, nullptr },
        { AV_BUFFERSRC_ADD_FRAME_FLAGS, nullptr },
        { AV_OPT_SET_BIN, nullptr},
        { AV_BUFFERSINK_GET_FRAME, nullptr},
        { AV_FILTER_GRAPH_FREE, nullptr}
        };

    const std::string SHARED_LIB_AVCODEC = "libavcodec.so";
    const std::string SHARED_LIB_AVUTIL = "libavutil.so";
    const std::string SHARED_LIB_AVFILTER = "libavfilter.so";

    std::atomic<bool> g_quadraLoaded = { false };
    void *g_libHandleAvcodec = nullptr;
    void *g_libHandleAvutil = nullptr;
    void *g_libHandleAvfilter = nullptr;

    uint32_t m_tmpFrameWidth = 720;
    uint32_t m_tmpFrameHeight = 1280;
    std::mutex m_lock = {};
}

VideoEncoderQuadra::VideoEncoderQuadra(QuaCodecType codecType)
{
    INFO("VideoEncoderQuadra::VideoEncoderQuadra codecType: %d", codecType);

    if (codecType == QUA_CODEC_TYPE_H264) {
        m_codec = ENCODER_TYPE_QUADRA_H264;
    } else if (codecType == QUA_CODEC_TYPE_H265){
        m_codec = ENCODER_TYPE_QUADRA_H265;
        m_encParams.bitrate = BITRATE_DEFAULT_265;
        m_encParams.profile = ENCODE_PROFILE_MAIN;
    } else if (codecType == T432_CODEC_TYPE_H264){
        m_codec = ENCODER_TYPE_T432_H264;
        m_vpuType = "T432";
    } else {
        m_codec = ENCODER_TYPE_T432_H265;
        m_vpuType = "T432";
    }
    INFO("VideoEncoderQuadra constructed %s", (m_codec == ENCODER_TYPE_QUADRA_H264) ? "h.264" : "h.265");
}

VideoEncoderQuadra::~VideoEncoderQuadra()
{
    INFO("VideoEncoderQuadra::~VideoEncoderQuadra");
    DestroyEncoder();
    INFO("VideoEncoderQuadra destructed");
}

EncoderRetCode FrameScaling(uint32_t width, uint32_t height) {
    std::unique_lock<std::mutex> lock(m_lock);
    m_tmpFrameWidth = width;
    m_tmpFrameHeight = height;
    return VIDEO_ENCODER_SUCCESS;
}

bool VideoEncoderQuadra::InitFilter()
{
    auto avfilterGetByName = reinterpret_cast<AVFilterGetByNameFunc>(g_funcMap_avfilter[AV_FILTER_GET_BY_NAME]);
    m_bufferSrc = avfilterGetByName("buffer");
    m_bufferSink = avfilterGetByName("buffersink");
    if (m_bufferSrc == nullptr || m_bufferSink == nullptr) {
        ERR("Could not find filter");
        return false;
    }

    auto avfilterInOutAlloc = reinterpret_cast<AVFilterInOutAllocFunc>
        (g_funcMap_avfilter[AV_FILTER_IN_OUT_ALLOC]);
    m_outputs = avfilterInOutAlloc();
    m_inputs = avfilterInOutAlloc();
    auto avfilterGraphAlloc = reinterpret_cast<AVFilterGraphAllocFunc>
        (g_funcMap_avfilter[AV_FILTER_GRAPH_ALLOC]);
    m_filterGraph = avfilterGraphAlloc();
    if (m_outputs == nullptr || m_inputs == nullptr || m_filterGraph == nullptr) {
        ERR("Could not alloc filter");
        return false;
    }

    char args[512];    // 此处适配第三方厂商逻辑，采用char数组对filter创建函数入参赋值
    snprintf(args, sizeof(args), "video_size=%dx%d:pix_fmt=%d:time_base=1/%d:pixel_aspect=0/1",
            m_encParams.width, m_encParams.height, AV_PIX_FMT_YUV420P, m_encParams.framerate);

    auto avfilterGraphCreateFilter = reinterpret_cast<AVFilterGraphCreateFilterFunc>
        (g_funcMap_avfilter[AV_FILTER_GRAPH_CREATE_FILTER]);
    (void)avfilterGraphCreateFilter(&m_buffersrcCtx, m_bufferSrc, "in", args, nullptr, m_filterGraph);
    (void)avfilterGraphCreateFilter(&m_buffersinkCtx, m_bufferSink, "out", nullptr, nullptr, m_filterGraph);

    auto avStrDup = reinterpret_cast<AVStrDupFunc>(g_funcMap_avutil[AV_STR_DUP]);
    m_outputs->name       = avStrDup("in");
    m_outputs->filter_ctx = m_buffersrcCtx;
    m_outputs->pad_idx    = 0;
    m_outputs->next       = nullptr;

    m_inputs->name       = avStrDup("out");
    m_inputs->filter_ctx = m_buffersinkCtx;
    m_inputs->pad_idx    = 0;
    m_inputs->next       = nullptr;

    // 此处参数字符串为约定参数，不建议变为可配置，默认在初始化缩放滤镜时使能
    std::string tmp = "ni_quadra_hwupload=-1,ni_quadra_scale=" + std::to_string(m_encParams.width) + "x" +
            std::to_string(m_encParams.height) + ",hwdownload,format=0";
    if (m_tmpFrameWidth != m_encParams.width ||  m_tmpFrameHeight != m_encParams.height) {
        tmp = "ni_quadra_hwupload=-1,ni_quadra_scale=" + std::to_string(m_tmpFrameWidth) + "x" +
            std::to_string(m_tmpFrameHeight) + ",hwdownload,format=0";
    }
    auto avfilterGraphParsePtr = reinterpret_cast<AVFilterGraphParsePtrFunc>
        (g_funcMap_avfilter[AV_FILTER_GRAPH_PARSE_PTR]);
    (void)avfilterGraphParsePtr(m_filterGraph, tmp.c_str(), &m_inputs, &m_outputs, nullptr);

    auto avfilterGraphConfig = reinterpret_cast<AVFilterGraphConfigFunc>
        (g_funcMap_avfilter[AV_FILTER_GRAPH_CONFIG]);
    (void)avfilterGraphConfig(m_filterGraph, nullptr);

    auto avfilterInOutFree = reinterpret_cast<AVFilterInOutFreeFunc>
        (g_funcMap_avfilter[AV_FILTER_INOUT_FREE]);
    (void)avfilterInOutFree(&m_outputs);
    (void)avfilterInOutFree(&m_inputs);

    if (m_tmpFrameWidth != m_encParams.width ||  m_tmpFrameHeight != m_encParams.height) {
        m_encoderCtx->width = m_tmpFrameWidth;
        m_encoderCtx->height = m_tmpFrameHeight;
    } else {
        m_encoderCtx->width = m_encParams.width;
        m_encoderCtx->height = m_encParams.height;
    }

    m_frameWidth = m_tmpFrameWidth;
    m_frameHeight = m_tmpFrameHeight;
    return true;
}

EncoderRetCode VideoEncoderQuadra::InitEncoder()
{
    m_encParams = m_videoParams;
    if (m_codec == ENCODER_TYPE_QUADRA_H265 || m_codec == ENCODER_TYPE_T432_H265) {
        m_encParams.profile = ENCODE_PROFILE_MAIN;
        m_videoParams.profile = ENCODE_PROFILE_MAIN;
    }
    if (!LoadQuadraSharedLib()) {
        ERR("init encoder failed: load Quadra so error");
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

bool VideoEncoderQuadra::LoadQuadraSharedLib()
{
    if (g_quadraLoaded) {
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
    INFO("load %s", SHARED_LIB_AVFILTER.c_str());
    g_libHandleAvfilter = dlopen(SHARED_LIB_AVFILTER.c_str(), RTLD_LAZY);
    if (g_libHandleAvfilter == nullptr) {
        const char *errStr = dlerror();
        ERR("load %s error:%s", SHARED_LIB_AVFILTER.c_str(), (errStr != nullptr) ? errStr : "unknown");
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
    for (auto &symbol : g_funcMap_avfilter) {
        void *func = dlsym(g_libHandleAvfilter, symbol.first.c_str());
        if (func == nullptr) {
            ERR("failed to load %s", symbol.first.c_str());
            return false;
        }
        symbol.second = func;
    }
    g_quadraLoaded = true;
    return true;
}

bool VideoEncoderQuadra::InitCtxParams()
{
    if (!InitFilter()) {
        ERR("Encoder InitFilter fail");
        return false;
    }
    m_encoderCtx->pix_fmt = AV_PIX_FMT_YUV420P;
    m_encoderCtx->time_base = (AVRational){1, static_cast<int>(m_encParams.framerate)};
    m_encoderCtx->framerate = (AVRational){static_cast<int>(m_encParams.framerate), 1};

    std::string crfValue = "";
    std::string maxCrfRateValue = "";
    std::string vbvBufferSizeValue = "";
    if (m_encParams.rcMode == CAPPED_CRF_RCMODE) {
        crfValue = "crf=" + std::to_string(m_encParams.crf);
        maxCrfRateValue = "bitrate=" + std::to_string(m_encParams.maxCrfRate);
        vbvBufferSizeValue = "vbvBufferSize=" + std::to_string(m_encParams.vbvBufferSize);
    } else if (m_encParams.rcMode == CBR_RCMODE) {
        m_encoderCtx->bit_rate = m_encParams.bitrate;
    }

    const std::string xcoderParams = "xcoder-params";
    std::string extraPramOpt = "profile=";
    std::string extraPramValue = "";
    std::string lowDelayPocTypeOpt = "useLowDelayPocType=1";
    std::string intraPeriodValue = "intraPeriod=" + std::to_string(m_encParams.gopsize);
    if (m_codec == ENCODER_TYPE_QUADRA_H264 || m_codec == ENCODER_TYPE_T432_H264) {
        extraPramValue = extraPramOpt + g_transProfile[m_encParams.profile] + ":" + lowDelayPocTypeOpt
            + ":" + intraPeriodValue;
    } else if (m_codec == ENCODER_TYPE_QUADRA_H265 || m_codec == ENCODER_TYPE_T432_H265) {
        extraPramValue = extraPramOpt + g_transProfile_h265[m_encParams.profile]
            + ":" + intraPeriodValue;
    }

    std::string encodeParams = "gopPresetIdx=9:lowDelay=1";
    encodeParams = encodeParams + ":" + extraPramValue;
    if (m_encParams.rcMode == CAPPED_CRF_RCMODE) {
        std::string rcEnable = "RcEnable=0:cuLevelRCEnable=1";
        encodeParams = encodeParams + ":" + crfValue + ":" + maxCrfRateValue + ":" + vbvBufferSizeValue + ":"+ rcEnable;
    } else if (m_encParams.rcMode == CBR_RCMODE) {
        std::string rcEnable = "RcEnable=1";
        encodeParams = encodeParams + ":" + rcEnable;
    }

    auto avOptSet = reinterpret_cast<AvOptSetFunc>(g_funcMap_avutil[AV_OPT_SET]);
    int ret = avOptSet(m_encoderCtx->priv_data, xcoderParams.c_str(), encodeParams.c_str(), 0);
    if (ret != 0) {
        ERR("Set EncodeParams error.Error code: %d", ret);
        return false;
    }

    return true;
}
EncoderRetCode VideoEncoderQuadra::StartEncoder()
{
    INFO("start encoder success");
    return VIDEO_ENCODER_SUCCESS;
}

bool VideoEncoderQuadra::SendOneFrame(AVFrame *swFrame)
{
    if (m_paramsChangeNoResetFlag && (m_vpuType == "Quadra")) {
        if(m_encParams.rcMode == CBR_RCMODE) {
            auto avFrameNewSideData = reinterpret_cast<AvFrameNewSideDataFunc>
                (g_funcMap_avutil[AV_FRAME_NEW_SIDE_DATA]);
            AVFrameSideData *tmpSideData = avFrameNewSideData(swFrame, AV_FRAME_DATA_NETINT_BITRATE, sizeof(int32_t));
            *((int32_t*)tmpSideData->data) = m_encParams.bitrate;
        } else if (m_encParams.rcMode == CAPPED_CRF_RCMODE) {
            auto avFrameNewSideData = reinterpret_cast<AvFrameNewSideDataFunc>(g_funcMap_avutil[AV_FRAME_NEW_SIDE_DATA]);
            AVFrameSideData *tmpSideData = avFrameNewSideData(swFrame, AV_FRAME_DATA_NETINT_BITRATE, sizeof(int32_t));
            *((int32_t*)tmpSideData->data) = m_encParams.maxCrfRate;

            AVFrameSideData *tmpSideData2 = avFrameNewSideData(swFrame, AV_FRAME_DATA_NETINT_GENERAL_SIDE_DATA, sizeof(AVNetintGeneralSideData));
            AVNetintGeneralSideData *niTmpSideData = (AVNetintGeneralSideData*)tmpSideData2->data;
            niTmpSideData->count = 0;
            niTmpSideData->type[niTmpSideData->count] = NI_FRAME_AUX_DATA_VBV_BUFFER_SIZE;
            void *curData = (uint32_t *)&(niTmpSideData->data[niTmpSideData->count]);
            ((uint32_t *)curData)[0] = m_encParams.vbvBufferSize;

            niTmpSideData->count = 1;
            niTmpSideData->type[niTmpSideData->count] = NI_FRAME_AUX_DATA_CRF;
            curData = (uint32_t *)&(niTmpSideData->data[niTmpSideData->count]);
            ((uint32_t *)curData)[0] = m_encParams.crf;

            niTmpSideData->count++;
        }
    }

    auto avcodecSendFrame = reinterpret_cast<AvcodecSendFrameFunc>(g_funcMap_avcodec[AVCODEC_SEND_FRAME]);
    int ret = avcodecSendFrame(m_encoderCtx, swFrame);
    if (ret != 0) {
        ERR("avcodecSendFrame failed,ret=%d", ret);
        return false;
    }

    if (m_paramsChangeNoResetFlag && (m_vpuType == "Quadra")) {
        auto avFrameGetSideData = reinterpret_cast<AvFrameGetSideDataFunc>
            (g_funcMap_avutil[AV_FRAME_GET_SIDE_DATA]);
        AVFrameSideData *tmpSideData = avFrameGetSideData(swFrame, AV_FRAME_DATA_NETINT_BITRATE);
        if (tmpSideData) {
            auto avFrameRemoveSideData = reinterpret_cast<AvFrameRemoveSideDataFunc>
                (g_funcMap_avutil[AV_FRAME_REMOVE_SIDE_DATA]);
            avFrameRemoveSideData(swFrame, AV_FRAME_DATA_NETINT_BITRATE);
        }

        AVFrameSideData *tmpSideData2 = avFrameGetSideData(swFrame, AV_FRAME_DATA_NETINT_GENERAL_SIDE_DATA);
        if (tmpSideData2) {
            auto avFrameRemoveSideData = reinterpret_cast<AvFrameRemoveSideDataFunc>
                (g_funcMap_avutil[AV_FRAME_REMOVE_SIDE_DATA]);
            avFrameRemoveSideData(swFrame, AV_FRAME_DATA_NETINT_GENERAL_SIDE_DATA);
        }

        m_paramsChangeNoResetFlag = false;
    }

    auto avFrameFree = reinterpret_cast<AvFrameFreeFunc>(g_funcMap_avutil[AV_FRAME_FREE]);
    (void)avFrameFree(&swFrame);

    return true;
}

bool VideoEncoderQuadra::VerifyParamsBeforeEncode()
{
    if (EncodeParamsCheck() != VIDEO_ENCODER_SUCCESS) {
        ERR("init encoder failed: GetEncParam failed");
        return false;
    }

    if (m_paramsChangeFlag) {
        if ((m_vpuType == "Quadra" && !m_paramsChangeNoResetFlag) || (m_vpuType == "T432")) {
            if (ResetEncoder() != VIDEO_ENCODER_SUCCESS) {
                ERR("reset encoder failed while encoding");
                return false;
            }
        }
        m_paramsChangeFlag = false;
    }
    return true;
}

bool VideoEncoderQuadra::FilterAndSendOneFrame () {
    if (m_frameWidth == m_encParams.width &&  m_frameHeight == m_encParams.height) {
        if (!SendOneFrame(m_swFrame)) {
            return false;
        }
        return true;
    }
    auto avFrameAlloc = reinterpret_cast<AvFrameAllocFunc>(g_funcMap_avutil[AV_FRAME_ALLOC]);
    m_filtFrame = avFrameAlloc();
    if (m_filtFrame == nullptr) {
        ERR("Alloc m_filtFrame failed.");
        return VIDEO_ENCODER_ENCODE_FAIL;
    }

    auto avBuffersrcAddFrameFlags = reinterpret_cast<AVBuffersrcAddFrameFlagsFunc>
        (g_funcMap_avfilter[AV_BUFFERSRC_ADD_FRAME_FLAGS]);
    auto ret = avBuffersrcAddFrameFlags(m_buffersrcCtx, m_swFrame, AV_BUFFERSRC_FLAG_KEEP_REF);
    if (ret != 0) {
        ERR("avBuffersrcAddFrameFlags failed,ret=%d", ret);
        return false;
    }
    auto avFrameFree = reinterpret_cast<AvFrameFreeFunc>(g_funcMap_avutil[AV_FRAME_FREE]);
    (void)avFrameFree(&m_swFrame);

    while (true) {
        auto avBuffersinkGetFrame = reinterpret_cast<AVBuffersinkGetFrameFunc>
            (g_funcMap_avfilter[AV_BUFFERSINK_GET_FRAME]);
        auto ret = avBuffersinkGetFrame(m_buffersinkCtx, m_filtFrame);
        if (ret == 0) {
            if (!SendOneFrame(m_filtFrame)) {
                return false;
            }
        } else if (ret == AVERROR(EAGAIN)) {
            break;
        } else if (ret == AVERROR_EOF) {
            auto avcodecSendFrame = reinterpret_cast<AvcodecSendFrameFunc>(g_funcMap_avcodec[AVCODEC_SEND_FRAME]);
            ret = avcodecSendFrame(m_encoderCtx, nullptr);
            if (ret != 0) {
                ERR("avcodecSendFrame failed,ret=%d", ret);
                return false;
            }
            break;
        } else {
            ERR("avBuffersinkGetFrame failed, ret=%d", ret);
            return false;
        }
    }

    return true;
}

EncoderRetCode VideoEncoderQuadra::EncodeOneFrame(const uint8_t *inputData, uint32_t inputSize,
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

    if (!InitFrameData(inputData)) {
        return VIDEO_ENCODER_ENCODE_FAIL;
    }

    if (m_nextKeyFrameDistance > 0) {
        (void)ForceKeyFrame();
        m_nextKeyFrameDistance = 0;
    }

    std::unique_lock<std::mutex> lock(m_lock);
    if (m_frameHeight != m_tmpFrameHeight || m_frameWidth != m_tmpFrameWidth) {
        if (ResetEncoder() != VIDEO_ENCODER_SUCCESS) {
            ERR("reset encoder failed while encoding");
            return VIDEO_ENCODER_ENCODE_FAIL;
        }
    }
   lock.unlock();

    if (!FilterAndSendOneFrame()) {
        return VIDEO_ENCODER_ENCODE_FAIL;
    }

    if (!ReceiveOneFrame(outputData, outputSize)) {
        return VIDEO_ENCODER_ENCODE_FAIL;
    }

    return VIDEO_ENCODER_SUCCESS;
}

bool VideoEncoderQuadra::ReceiveOneFrame(uint8_t **outputData, uint32_t *outputSize)
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

bool VideoEncoderQuadra::InitFrameData(const uint8_t *src)
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

EncoderRetCode VideoEncoderQuadra::StopEncoder()
{
    INFO("stop encoder success");
    return VIDEO_ENCODER_SUCCESS;
}

void VideoEncoderQuadra::CheckFuncPtr()
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
    for (auto &symbol : g_funcMap_avfilter) {
        if (symbol.second == nullptr) {
            m_funPtrError = true;
            ERR("%s ptr is nullptr", symbol.first.c_str());
        }
    }
}

void VideoEncoderQuadra::DestroyEncoder()
{
    if (!m_isInited) {
        INFO("Destroy encoder already triggered, return");
        return;
    }
    INFO("destroy encoder start");
    if ((g_libHandleAvcodec) == nullptr && (g_libHandleAvutil) == nullptr && (g_libHandleAvfilter) == nullptr) {
        WARN("encoder has been destroyed");
        return;
    }
    CheckFuncPtr();
    auto AvcodecFreeContext = reinterpret_cast<AvcodecFreeContextFunc>(g_funcMap_avcodec[AVCODEC_FREE_CONTEXT]);
    AvcodecFreeContext(&m_encoderCtx);
    auto avPacketFree = reinterpret_cast<AvPacketFreeFunc>(g_funcMap_avcodec[AV_PACKET_FREE]);
    (void)avPacketFree(&m_encPkt);
    auto avFilterGraphFree = reinterpret_cast<AVFilterGraphFreeFunc>(g_funcMap_avfilter[AV_FILTER_GRAPH_FREE]);
    (void)avFilterGraphFree(&m_filterGraph);
    if (m_funPtrError) {
        UnLoadQuadraSharedLib();
    }
    m_isInited = false;
    INFO("destroy encoder done");
}

void VideoEncoderQuadra::UnLoadQuadraSharedLib()
{
    INFO("UnLoadQuadraSharedLib");
    for (auto &symbol : g_funcMap_avcodec) {
        symbol.second = nullptr;
    }
    for (auto &symbol : g_funcMap_avutil) {
        symbol.second = nullptr;
    }
    for (auto &symbol : g_funcMap_avfilter) {
        symbol.second = nullptr;
    }
    (void)dlclose(g_libHandleAvcodec);
    (void)dlclose(g_libHandleAvutil);
    (void)dlclose(g_libHandleAvfilter);
    g_libHandleAvcodec = nullptr;
    g_libHandleAvutil = nullptr;
    g_libHandleAvfilter = nullptr;
    g_quadraLoaded = false;
    m_funPtrError = false;
}

EncoderRetCode VideoEncoderQuadra::ResetEncoder()
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

EncoderRetCode VideoEncoderQuadra::ForceKeyFrame()
{
    m_swFrame->pict_type = AV_PICTURE_TYPE_I;
    m_swFrame->key_frame = 1;
    INFO("force key frame success");
    return VIDEO_ENCODER_SUCCESS;
}

/**
* @功能描述: 初始化视频流模式下的编码参数
*/
EncoderRetCode VideoEncoderQuadra::Config(const VmiEncoderConfig& config)
{
    return GeneralConfig(config);
}

/**
* @功能描述: 参数有效性由上层检验，动态修改编码相关参数
*/
EncoderRetCode VideoEncoderQuadra::SetParams(const VmiEncoderParams& params)
{
    return GeneralSetParams(params);
}
