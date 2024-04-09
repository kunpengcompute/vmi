/*
 * 版权所有 (c) 华为技术有限公司 2013-2023
 * 功能说明: 适配QUADRA硬件视频编码器，包括编码器初始化、启动、编码、停止、销毁等
 */

#define LOG_TAG "VideoDecoderQuadra"
#include "VideoDecoderQuadra.h"
#include <string>
#include <algorithm>
#include <unordered_map>
#include <chrono>
#include <dlfcn.h>
#include <unistd.h>
#include "android/log.h"
#include <sys/time.h>

namespace MediaCore {
namespace {
    const std::string NI_DECODER_INIT_DEFAULT_PARAMS  = "ni_decoder_init_default_params";
    const std::string NI_RSRC_ALLOCATE_AUTO           = "ni_rsrc_allocate_auto";
    const std::string NI_RSRC_RELEASE_RESOURCE        = "ni_rsrc_release_resource";
    const std::string NI_RSRC_FREE_DEVICE_CONTEXT     = "ni_rsrc_free_device_context";
    const std::string NI_DEVICE_OPEN                  = "ni_device_open";
    const std::string NI_DEVICE_CLOSE                 = "ni_device_close";
    const std::string NI_DEVICE_SESSION_CONTEXT_INIT  = "ni_device_session_context_init";
    const std::string NI_DEVICE_SESSION_OPEN          = "ni_device_session_open";
    const std::string NI_DEVICE_SESSION_READ          = "ni_device_session_read";
    const std::string NI_DEVICE_SESSION_FLUSH         = "ni_device_session_flush";
    const std::string NI_DEVICE_DEC_SESSION_FLUSH     = "ni_device_dec_session_flush";
    const std::string NI_DEVICE_DEC_SESSION_SAVE_HDRS = "ni_device_dec_session_save_hdrs";
    const std::string NI_DEVICE_SESSION_WRITE         = "ni_device_session_write";
    const std::string NI_DEVICE_SESSION_CLOSE         = "ni_device_session_close";
    const std::string NI_PACKET_BUFFER_ALLOC          = "ni_packet_buffer_alloc";
    const std::string NI_PACKET_COPY                  = "ni_packet_copy";
    const std::string NI_PACKET_BUFFER_FREE           = "ni_packet_buffer_free";
    const std::string NI_DECODER_FRAME_BUFFER_ALLOC   = "ni_decoder_frame_buffer_alloc";
    const std::string NI_DECODER_FRAME_BUFFER_FREE    = "ni_decoder_frame_buffer_free";
    const std::string NI_DECODER_PARAMS_SET_VALUE     = "ni_decoder_params_set_value";

    using NiDecInitDefaultParamsFunc = ni_retcode_t (*)(ni_xcoder_params_t *param, int fpsNum,
        int fpsDenom, long bitRate, int width, int height);

    using NiRsrcAllocateAutoFunc = ni_device_context_t* (*)(ni_device_type_t devType, ni_alloc_rule_t rule,
        ni_codec_t codec, int width, int height, int frameRate, unsigned long *load);
    using NiRsrcReleaseResourceFunc = void (*)(ni_device_context_t *devCtx, ni_codec_t codec, unsigned long load);
    using NiRsrcFreeDeviceContextFunc = void (*)(ni_device_context_t *devCtx);

    using NiDeviceOpenFunc = ni_device_handle_t (*)(const char *dev, uint32_t *maxIoSizeOut);
    using NiDeviceCloseFunc = void (*)(ni_device_handle_t devHandle);
    using NiDeviceSessionContextInitFunc = void (*)(ni_session_context_t *sessionCtx);
    using NiDeviceSessionOpenFunc = ni_retcode_t (*)(ni_session_context_t *sessionCtx,
        ni_device_type_t devType);
    using NiDeviceSessionReadFunc = int (*)(ni_session_context_t *sessionCtx,
        ni_session_data_io_t *sessionDataIo, ni_device_type_t devType);
    using NiDeviceSessionFlushFunc = ni_retcode_t (*)(ni_session_context_t *sessionCtx,
        ni_device_type_t devType);
    using NiDeviceDecSessionFlushFunc = ni_retcode_t (*)(ni_session_context_t *sessionCtx);
    using NiDeviceDecSessionSaveHdrsFunc =
        ni_retcode_t (*)(ni_session_context_t *sessionCtx, uint8_t *hdrData, uint8_t hdrSize);
    using NiDeviceSessionWriteFunc = int (*)(ni_session_context_t *sessionCtx,
        ni_session_data_io_t *data, ni_device_type_t devType);
    using NiDeviceSessionCloseFunc =
        ni_retcode_t (*)(ni_session_context_t *sessionCtx, int eosRecieved, ni_device_type_t devType);

    using NiPacketBufferAllocFunc = ni_retcode_t (*)(ni_packet_t *packet, int packetSize);
    using NiPacketCopyFunc =
        int (*)(void *destination, const void * const source, int curSize, void *leftover, int *prevSize);
    using NiPacketBufferFreeFunc = ni_retcode_t (*)(ni_packet_t *packet);

    using NiDecoderFrameBufferAllocFunc = ni_retcode_t (*)(ni_buf_pool_t *pool, ni_frame_t *frame,
        int allocMem, int videoWidth, int videoHeight, int alignment, int factor);
    using NiDecoderFrameBufferFreeFunc = ni_retcode_t (*)(ni_frame_t *frame);
    using NiDecoderParamsSetValueFunc = ni_retcode_t (*)(ni_xcoder_params_t *p_params,
                                         const char *name, char *value);

    std::unordered_map<std::string, void*> g_funcMap = {
        { NI_DECODER_INIT_DEFAULT_PARAMS, nullptr },
        { NI_RSRC_ALLOCATE_AUTO, nullptr },
        { NI_RSRC_RELEASE_RESOURCE, nullptr },
        { NI_RSRC_FREE_DEVICE_CONTEXT, nullptr },
        { NI_DEVICE_OPEN, nullptr },
        { NI_DEVICE_CLOSE, nullptr },
        { NI_DEVICE_SESSION_CONTEXT_INIT, nullptr },
        { NI_DEVICE_SESSION_OPEN, nullptr },
        { NI_DEVICE_SESSION_READ, nullptr },
        { NI_DEVICE_SESSION_FLUSH, nullptr },
        { NI_DEVICE_DEC_SESSION_FLUSH, nullptr },
        { NI_DEVICE_DEC_SESSION_SAVE_HDRS, nullptr },
        { NI_DEVICE_SESSION_WRITE, nullptr },
        { NI_DEVICE_SESSION_CLOSE, nullptr },
        { NI_PACKET_BUFFER_ALLOC, nullptr },
        { NI_PACKET_COPY, nullptr },
        { NI_PACKET_BUFFER_FREE, nullptr },
        { NI_DECODER_FRAME_BUFFER_ALLOC, nullptr },
        { NI_DECODER_FRAME_BUFFER_FREE, nullptr },
        { NI_DECODER_PARAMS_SET_VALUE, nullptr }
    };

    // H.264 NAL unit types in T-REC-H.264-201906
    enum class H264NaluType {
        UNSPECIFIED     = 0,
        SLICE           = 1,
        DPA             = 2,
        DPB             = 3,
        DPC             = 4,
        IDR_SLICE       = 5,
        SEI             = 6,
        SPS             = 7,
        PPS             = 8,
        AUD             = 9,
        END_SEQUENCE    = 10,
        END_STREAM      = 11,
        FILLER_DATA     = 12,
        SPS_EXT         = 13,
        PREFIX          = 14,
        SUB_SPS         = 15,
        DPS             = 16,
        AUXILIARY_SLICE = 19,
    };

    // HEVC NAL unit types in T-REC-H.265-201906
    enum class H265NaluType {
        TRAIL_N        = 0,
        TRAIL_R        = 1,
        TSA_N          = 2,
        TSA_R          = 3,
        STSA_N         = 4,
        STSA_R         = 5,
        RADL_N         = 6,
        RADL_R         = 7,
        RASL_N         = 8,
        RASL_R         = 9,
        VCL_N10        = 10,
        VCL_R11        = 11,
        VCL_N12        = 12,
        VCL_R13        = 13,
        VCL_N14        = 14,
        VCL_R15        = 15,
        BLA_W_LP       = 16,
        BLA_W_RADL     = 17,
        BLA_N_LP       = 18,
        IDR_W_RADL     = 19,
        IDR_N_LP       = 20,
        CRA_NUT        = 21,
        RSV_IRAP_VCL22 = 22,
        RSV_IRAP_VCL23 = 23,
        RSV_VCL24      = 24,
        RSV_VCL25      = 25,
        RSV_VCL26      = 26,
        RSV_VCL27      = 27,
        RSV_VCL28      = 28,
        RSV_VCL29      = 29,
        RSV_VCL30      = 30,
        RSV_VCL31      = 31,
        VPS            = 32,
        SPS            = 33,
        PPS            = 34,
        AUD            = 35,
        EOS_NUT        = 36,
        EOB_NUT        = 37,
        FD_NUT         = 38,
        SEI_PREFIX     = 39,
        SEI_SUFFIX     = 40,
        RSV_NVCL41     = 41,
        RSV_NVCL42     = 42,
        RSV_NVCL43     = 43,
        RSV_NVCL44     = 44,
        RSV_NVCL45     = 45,
        RSV_NVCL46     = 46,
        RSV_NVCL47     = 47,
        UNSPEC48       = 48,
        UNSPEC49       = 49,
        UNSPEC50       = 50,
        UNSPEC51       = 51,
        UNSPEC52       = 52,
        UNSPEC53       = 53,
        UNSPEC54       = 54,
        UNSPEC55       = 55,
        UNSPEC56       = 56,
        UNSPEC57       = 57,
        UNSPEC58       = 58,
        UNSPEC59       = 59,
        UNSPEC60       = 60,
        UNSPEC61       = 61,
        UNSPEC62       = 62,
        UNSPEC63       = 63,
    };

    constexpr uint32_t QUADRA_WIDTH_ALIGN = 32;
    constexpr uint32_t QUADRA_WIDTH_ALIGN_PLAIN = 128;
    constexpr uint32_t QUADRA_HEIGHT_ALIGN_H264 = 16;
    constexpr uint32_t QUADRA_HEIGHT_ALIGN_H265 = 8;
    constexpr long DEFAULT_BITRATE = 2000000; // 2Mbps
    constexpr uint32_t NAL_START_CODE_MIN_LEN = 3;
    constexpr uint32_t NAL_START_CODE_1ST_BYTE = 0;
    constexpr uint32_t NAL_START_CODE_2ST_BYTE = 1;
    constexpr uint32_t NAL_START_CODE_3ST_BYTE = 2;
    constexpr uint32_t NAL_START_CODE_4ST_BYTE = 3;
    constexpr uint32_t QUADRA_UV_HALF_LEN = 2;
    constexpr uint32_t PARAMS_OPTION_NUMS = 64;
    const std::string SHARED_LIB_NAME = "libxcoder.so";
    std::atomic<bool> g_quadraLoaded = { false };
    void *g_libHandle = nullptr;

    inline uint32_t AlignUp(uint32_t val, uint32_t align)
    {
        return (val + (align - 1)) & ~(align - 1);
    }

    #define ALOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
    #define ALOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
    #define ALOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
    #define ALOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
    #define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
    #define ALOGF(...) __android_log_assert("assert", LOG_TAG, __VA_ARGS__)
}

VideoDecoderQuadra::~VideoDecoderQuadra()
{
    DestroyDecoder();
    ALOGI("decoder destructed.");
}

DecoderRetCode VideoDecoderQuadra::CreateDecoder(MediaStreamFormat decType)
{
    ALOGI("create decoder.");
    if (decType == STREAM_FORMAT_AVC) {
        m_codec = EN_H264;
    } else if (decType == STREAM_FORMAT_HEVC) {
        m_codec = EN_H265;
    } else {
        ALOGE("create decoder failed!");
        return VIDEO_DECODER_CREATE_FAIL;
    }
    ALOGI("quadra decoder constructed %s", (m_codec == EN_H264) ? "h.264" : "h.265");

    return VIDEO_DECODER_SUCCESS;
}

DecoderRetCode VideoDecoderQuadra::InitDecoder()
{
    ALOGI("init decoder.");
    return VIDEO_DECODER_SUCCESS;
}

DecoderRetCode VideoDecoderQuadra::SendStreamData(uint8_t *buffer, uint32_t filledLen)
{
    if (m_stop) {
        ALOGE("send stream data, stop status.");
        return VIDEO_DECODER_DECODE_FAIL;
    }

    return DecoderWriteData(buffer, filledLen);
}

DecoderRetCode VideoDecoderQuadra::RetrieveFrameData(uint8_t *buffer, uint32_t maxLen, uint32_t *filledLen)
{
    if (m_stop) {
        ALOGE("retrieve frame data, stop status.");
        return VIDEO_DECODER_DECODE_FAIL;
    }

    return DecoderReadData(buffer, maxLen, filledLen);
}

DecoderRetCode VideoDecoderQuadra::SetCallbacks(std::function<void(DecodeEventIndex, uint32_t, void *)> eventCallBack)
{
    m_eventCallBack = eventCallBack;
    return VIDEO_DECODER_SUCCESS;
}

DecoderRetCode VideoDecoderQuadra::SetCopyFrameFunc(
    std::function<uint32_t(uint8_t*, uint8_t*, const PicInfoParams &, uint32_t)> copyFrame)
{
    m_copyFrame = copyFrame;
    return VIDEO_DECODER_SUCCESS;
}

DecoderRetCode VideoDecoderQuadra::SetDecodeParams(DecodeParamsIndex index, void *decParams)
{
    ALOGI("set decode params, index:%u", index);
    switch (index) {
        case INDEX_PIC_INFO: {
            auto params = static_cast<PicInfoParams *>(decParams);
            ALOGI("set decode params, index:%u, params width:%u, params height:%u, params stride:%d",
                index, params->width, params->height, params->stride);
            m_writeWidth = params->width;
            m_writeHeight = params->height;
            m_stride = params->stride;
            break;
        }
        default:
            break;
    }
    return VIDEO_DECODER_SUCCESS;
}

DecoderRetCode VideoDecoderQuadra::GetDecodeParams(DecodeParamsIndex index, void *decParams)
{
    ALOGI("get decode params, index:%u.", index);
    switch (index) {
        case INDEX_PIC_INFO: {
            auto params = static_cast<PicInfoParams *>(decParams);
            params->width = m_writeWidth;
            params->stride = static_cast<int32_t>(m_writeWidth);
            params->height = params->scanLines = m_writeHeight;
            break;
        }
        case INDEX_PORT_FORMAT_INFO: {
            auto params = static_cast<PortFormatParams *>(decParams);
            if (params->port == OUT_PORT) {
                params->format = PIXEL_FORMAT_FLEX_YUV_420P;
            } else if (params->port == IN_PORT) {
                params->format = m_codec;
            } else {
                return VIDEO_DECODER_GET_DECODE_PARAMS_FAIL;
            }
            break;
        }
        case INDEX_ALIGN_INFO: {
            auto params = static_cast<AlignInfoParams *>(decParams);
            params->widthAlign = QUADRA_WIDTH_ALIGN;
            if (m_codec == EN_H264) {
                params->heightAlign = QUADRA_HEIGHT_ALIGN_H264;
            } else if (m_codec == EN_H265) {
                params->heightAlign = QUADRA_HEIGHT_ALIGN_H265;
            } else {
                params->heightAlign = 1;
            }
            break;
        }
        default:
            break;
    }
    return VIDEO_DECODER_SUCCESS;
}

DecoderRetCode VideoDecoderQuadra::Flush()
{
    ALOGI("decoder flush.");
    auto deviceDecSessionFlush =
        reinterpret_cast<NiDeviceDecSessionFlushFunc>(g_funcMap[NI_DEVICE_DEC_SESSION_FLUSH]);
    ni_retcode_t ret = (*deviceDecSessionFlush)(m_sessionCtx);
    if (ret != NI_RETCODE_SUCCESS) {
        ALOGE("device dec session flush error.");
        return VIDEO_DECODER_RESET_FAIL;
    }
    return VIDEO_DECODER_SUCCESS;
}

DecoderRetCode VideoDecoderQuadra::StartDecoder()
{
    if (!LoadQuadraSharedLib()) {
        ALOGE("load quadra so error.");
        return VIDEO_DECODER_START_FAIL;
    }

    if (!InitContext()) {
        ALOGE("init context error.");
        return VIDEO_DECODER_START_FAIL;
    }

    m_stop = false;
    ALOGI("start decoder success");
    return VIDEO_DECODER_SUCCESS;
}

DecoderRetCode VideoDecoderQuadra::StopDecoder()
{
    if (m_stop) {
        ALOGI("stop decoder, stop already.");
        return VIDEO_DECODER_SUCCESS;
    }

    m_packet.data.packet.end_of_stream = 1;
    ALOGI("stop decoder, session ctx ready to close is %u, frame end of stream is %u",
        m_sessionCtx->ready_to_close, m_frame.data.frame.end_of_stream);

    DestroyContext();

    m_stop = true;

    return VIDEO_DECODER_SUCCESS;
}

void VideoDecoderQuadra::DestroyDecoder()
{
    ALOGI("destroy decoder.");

    if (g_libHandle == nullptr) {
        ALOGI("decoder has been destroyed.");
        return;
    }

    if (!m_stop) {
        ALOGI("destroy decoder, stop decoder.");
        (void) StopDecoder();
    }

    ALOGI("destroy decoder done.");
}

bool VideoDecoderQuadra::LoadQuadraSharedLib() const
{
    if (g_quadraLoaded) {
        ALOGI("quadra has loaded.");
        return true;
    }

    ALOGI("load %s", SHARED_LIB_NAME.c_str());
    g_libHandle = dlopen(SHARED_LIB_NAME.c_str(), RTLD_LAZY);
    if (g_libHandle == nullptr) {
        const char *errStr = dlerror();
        ALOGE("load %s error:%s", SHARED_LIB_NAME.c_str(), (errStr != nullptr) ? errStr : "unknown");
        return false;
    }

    for (auto &symbol : g_funcMap) {
        void *func = dlsym(g_libHandle, symbol.first.c_str());
        if (func == nullptr) {
            ALOGE("failed to load %s", symbol.first.c_str());
            return false;
        }
        symbol.second = func;
    }

    g_quadraLoaded = true;
    return true;
}

bool VideoDecoderQuadra::InitContext()
{
    ALOGI("init context start.");
    if (!InitCtxParams()) {
        ALOGE("init context params failed.");
        return false;
    }

    m_startOfStream = 1;

    m_sessionCtx = (ni_session_context_t *)malloc(sizeof(ni_session_context_t));
    if (m_sessionCtx == nullptr) {
        ALOGE("context alloc failed");
        return false;
    }
    memset(m_sessionCtx, 0, sizeof(ni_session_context_t));

    auto deviceSessionContextInit =
        reinterpret_cast<NiDeviceSessionContextInitFunc>(g_funcMap[NI_DEVICE_SESSION_CONTEXT_INIT]);
    (*deviceSessionContextInit)(m_sessionCtx);

    m_sessionCtx->p_session_config = nullptr;
    m_sessionCtx->session_id = NI_INVALID_SESSION_ID;
    m_sessionCtx->codec_format = (m_codec == EN_H264) ? NI_CODEC_FORMAT_H264 : NI_CODEC_FORMAT_H265;

    auto rsrcAllocateAuto = reinterpret_cast<NiRsrcAllocateAutoFunc>(g_funcMap[NI_RSRC_ALLOCATE_AUTO]);
    m_devCtx = (*rsrcAllocateAuto)(NI_DEVICE_TYPE_DECODER, EN_ALLOC_LEAST_INSTANCE, m_codec, m_writeWidth,
        m_writeHeight, m_frameRate, &m_load);
    if (m_devCtx == nullptr) {
        ALOGE("rsrc allocate auto failed.");
        return false;
    }

    std::string xcoderGuid = m_devCtx->p_device_info->dev_name;
    std::string xcoderNsid = m_devCtx->p_device_info->blk_name;

    auto deviceOpen = reinterpret_cast<NiDeviceOpenFunc>(g_funcMap[NI_DEVICE_OPEN]);
    ni_device_handle_t devHandle = (*deviceOpen)(xcoderNsid.c_str(), &m_sessionCtx->max_nvme_io_size);
    ni_device_handle_t blkHandle = (*deviceOpen)(xcoderNsid.c_str(), &m_sessionCtx->max_nvme_io_size);
    if ((devHandle == NI_INVALID_DEVICE_HANDLE) || (blkHandle == NI_INVALID_DEVICE_HANDLE)) {
        ALOGE("init context, device open failed.");
        return false;
    }

    m_sessionCtx->device_handle = devHandle;
    m_sessionCtx->blk_io_handle = blkHandle;
    m_sessionCtx->hw_id = 0;
    m_sessionCtx->p_session_config = &m_decApiParams;
    m_sessionCtx->src_bit_depth = m_bitDepth;
    m_sessionCtx->src_endian = NI_FRAME_LITTLE_ENDIAN;
    m_sessionCtx->bit_depth_factor = 1;

    auto deviceSessionOpen = reinterpret_cast<NiDeviceSessionOpenFunc>(g_funcMap[NI_DEVICE_SESSION_OPEN]);
    ni_retcode_t ret = (*deviceSessionOpen)(m_sessionCtx, NI_DEVICE_TYPE_DECODER);
    if (ret != NI_RETCODE_SUCCESS) {
        ALOGE("init decoder failed: device session open error %d", ret);
        return false;
    }

    return true;
}

bool VideoDecoderQuadra::InitCtxParams()
{
    ALOGI("init ctx params start.");

    auto decInitDefaultParams = reinterpret_cast<NiDecInitDefaultParamsFunc>(g_funcMap[NI_DECODER_INIT_DEFAULT_PARAMS]);
    ni_retcode_t ret =
        (*decInitDefaultParams)(&m_decApiParams, m_frameRate, 1, DEFAULT_BITRATE, m_writeWidth, m_writeHeight);
    if (ret != NI_RETCODE_SUCCESS) {
        ALOGE("decoder init default params error %d", ret);
        return false;
    }

    char enableOption[PARAMS_OPTION_NUMS];
    enableOption[0] = '1';
    enableOption[1] = '\0';

    auto niDecoderParamsSetValue = reinterpret_cast<NiDecoderParamsSetValueFunc>
        (g_funcMap[NI_DECODER_PARAMS_SET_VALUE]);
    ret = (*niDecoderParamsSetValue)(&m_decApiParams, "multicoreJointMode", &enableOption[0]);
    if (ret != NI_RETCODE_SUCCESS) {
        ALOGE("decoder set params value error %d", ret);
        return false;
    }

    return true;
}

int VideoDecoderQuadra::InitPacketData(const uint8_t *src, const uint32_t inputSize)
{
    if (src == nullptr) {
        ALOGE("decoder write data: input data buffer is nullptr.");
        return NI_RETCODE_FAILURE;
    }

    int sendSize;
    int saveSize = 0;
    bool newPacket = false;
    ni_packet_t *inPacket = &(m_packet.data.packet);

    if (inPacket->data_len == 0) {
        std::fill_n(reinterpret_cast<uint8_t *>(inPacket), sizeof(ni_packet_t), 0);

        inPacket->p_data = nullptr;
        inPacket->data_len = inputSize;

        if (inputSize + m_sessionCtx->prev_size > 0) {
            auto packetBufferAlloc = reinterpret_cast<NiPacketBufferAllocFunc>(g_funcMap[NI_PACKET_BUFFER_ALLOC]);
            ni_retcode_t ret = (*packetBufferAlloc)(inPacket, inputSize + m_sessionCtx->prev_size);
            if (ret != NI_RETCODE_SUCCESS) {
                ALOGE("decoder write data: packet buffer alloc failed.");
                return NI_RETCODE_FAILURE;
            }
        }

        newPacket = true;
        sendSize = inputSize + m_sessionCtx->prev_size;
        saveSize = m_sessionCtx->prev_size;
    } else {
        sendSize = static_cast<int>(inPacket->data_len);
    }

    inPacket->start_of_stream = m_startOfStream;
    inPacket->end_of_stream = 0;
    inPacket->video_width = m_writeWidth;
    inPacket->video_height = m_writeHeight;

    auto packetCopy = reinterpret_cast<NiPacketCopyFunc>(g_funcMap[NI_PACKET_COPY]);
    if (sendSize == 0) {
        if (newPacket) {
            sendSize = (*packetCopy)(inPacket->p_data, src, 0, m_sessionCtx->p_leftover, &m_sessionCtx->prev_size);
        }
        inPacket->data_len = static_cast<uint32_t>(sendSize);
        inPacket->end_of_stream = 1;
        ALOGI("decoder write data: sending last packet, size:%d + eos", sendSize);
    } else {
        if (newPacket) {
            sendSize =
                (*packetCopy)(inPacket->p_data, src, inputSize, m_sessionCtx->p_leftover, &m_sessionCtx->prev_size);
            inPacket->data_len += saveSize;
        }
    }

    return sendSize;
}

bool VideoDecoderQuadra::InitFrameData()
{
    uint32_t width = m_sessionCtx->active_video_width > 0 ? m_sessionCtx->active_video_width : m_writeWidth;
    uint32_t height = m_sessionCtx->active_video_height > 0 ? m_sessionCtx->active_video_height : m_writeHeight;
    int allocMem = (m_sessionCtx->active_video_width > 0 && m_sessionCtx->active_video_height > 0) ? 1 : 0;
    auto decoderFrameBufferAlloc =
        reinterpret_cast<NiDecoderFrameBufferAllocFunc>(g_funcMap[NI_DECODER_FRAME_BUFFER_ALLOC]);

    if (width > INT_MAX || height > INT_MAX) {
        ALOGE("receiving data error, width:%u or height:%u out of range!", width, height);
        return false;
    }

    ni_retcode_t ret = (*decoderFrameBufferAlloc)(m_sessionCtx->dec_fme_buf_pool, &(m_frame.data.frame),
        allocMem, static_cast<int>(width), static_cast<int>(height),
        m_sessionCtx->codec_format == NI_CODEC_FORMAT_H264, m_sessionCtx->bit_depth_factor);
    if (ret != NI_RETCODE_SUCCESS) {
        ALOGE("receiving data error, decoder frame buffer alloc error. ret:%d", ret);
        return false;
    }

    return true;
}

DecoderRetCode VideoDecoderQuadra::DecoderWriteData(const uint8_t *buffer, const uint32_t filledLen)
{
    if (m_sessionCtx->ready_to_close != 0) {
        ALOGE("decoder write data: session ctx ready to close is 1, no send.");
        return VIDEO_DECODER_DECODE_FAIL;
    }

    int sendSize = InitPacketData(buffer, filledLen);
    m_startOfStream = 0;

    if (sendSize == NI_RETCODE_FAILURE) {
        ALOGE("decoder write data: send size is failure.");
        return VIDEO_DECODER_DECODE_FAIL;
    }

    // 数据写入quadra
    int txSize = DeviceDecSessionWrite();
    if (txSize < 0) {
        ALOGE("decoder write data: sending data error. txSize:%d", txSize);
        (void) StopDecoder();
        return VIDEO_DECODER_DECODE_FAIL;
    } else if (txSize == 0 && filledLen != 0) {
        ALOGW("decoder write data: 0 byte sent this time, sleep and will re-try.");
        return VIDEO_DECODER_WRITE_OVERFLOW;
    } else {
        auto packetBufferFree = reinterpret_cast<NiPacketBufferFreeFunc>(g_funcMap[NI_PACKET_BUFFER_FREE]);
        ni_retcode_t ret = (*packetBufferFree)(&(m_packet.data.packet));
        if (ret != NI_RETCODE_SUCCESS) {
            ALOGW("decoder write data: packet buffer free failed, ret:%d", ret);
        }
    }

    return VIDEO_DECODER_SUCCESS;
}

void VideoDecoderQuadra::DecodeFpsStat()
{
    auto clockTimeNow = std::chrono::steady_clock::now().time_since_epoch();
    auto endTime = std::chrono::duration_cast<std::chrono::milliseconds>(clockTimeNow).count();
    int64_t period = endTime - m_lastTime;
    if (period >= 1000) { // 1000: 1000ms, 即1s
        float fps = static_cast<float>(m_frameCount) * 1000 / period;
        if (m_lastTime != 0) {
            ALOGI("PERF-DEC-FPS: %0.2f", fps);
        }
        m_lastTime = endTime;
        m_frameCount = 0;
    }
}

DecoderRetCode VideoDecoderQuadra::DecoderReadData(uint8_t *buffer, const uint32_t maxLen, uint32_t *filledLen)
{
    if (!InitFrameData()) {
        *filledLen = 0;
        (void) StopDecoder();
        return VIDEO_DECODER_DECODE_FAIL;
    }

    auto deviceSessionRead = reinterpret_cast<NiDeviceSessionReadFunc>(g_funcMap[NI_DEVICE_SESSION_READ]);
    // 从quadra获取解码后数据
    int rxSize = (*deviceSessionRead)(m_sessionCtx, &m_frame, NI_DEVICE_TYPE_DECODER);

    if (rxSize < 0) {
        ALOGE("decoder read data: receiving data error. rxSize:%d", rxSize);
        auto decoderFrameBufferFree =
            reinterpret_cast<NiDecoderFrameBufferFreeFunc>(g_funcMap[NI_DECODER_FRAME_BUFFER_FREE]);
        (void) (*decoderFrameBufferFree)(&(m_frame.data.frame));
        *filledLen = 0;
        (void) StopDecoder();
        return VIDEO_DECODER_DECODE_FAIL;
    }

    if (rxSize == 0) {
        ALOGW("decoder read data: no decoded frame is available now. rxSize:%d", rxSize);
        auto decoderFrameBufferFree =
            reinterpret_cast<NiDecoderFrameBufferFreeFunc>(g_funcMap[NI_DECODER_FRAME_BUFFER_FREE]);
        (void) (*decoderFrameBufferFree)(&(m_frame.data.frame));
        *filledLen = 0;

        if (m_frame.data.frame.end_of_stream == 1) {
            ALOGI("decoder read data: frame end of stream is 1, rxSize is 0.");
            return VIDEO_DECODER_EOS;
        }

        return VIDEO_DECODER_READ_UNDERFLOW;
    }
    // 增加计数位置
    m_frameCount++;
    DecodeFpsStat();

    return DecoderHandleData(buffer, maxLen, filledLen);
}

void VideoDecoderQuadra::DecoderPreHandleData(uint8_t *inputDataArr[])
{
    int tmpWriteHeight = m_frame.data.frame.video_height;
    int tmpWriteWidth = AlignUp(m_frame.data.frame.video_width * m_sessionCtx->bit_depth_factor, QUADRA_WIDTH_ALIGN);

    if (!m_mallocFlag) {
        const int intputSize = tmpWriteHeight * tmpWriteWidth * NAL_START_CODE_MIN_LEN / QUADRA_UV_HALF_LEN;
        m_inputData = static_cast<uint8_t *>(malloc(intputSize));
        m_mallocFlag = true;
    }

    uint8_t *tmpSrc;
    uint8_t *tmpDst;
    uint32_t cnt = 0;

    // 遍历y,u,v分量
    for (int i = 0; i < NAL_START_CODE_MIN_LEN; i++) {
        tmpSrc = reinterpret_cast<uint8_t *>(m_frame.data.frame.p_data[i]);
        int planeHeight = m_frame.data.frame.video_height;
        int planeWidth = AlignUp(m_frame.data.frame.video_width * m_sessionCtx->bit_depth_factor,
            QUADRA_WIDTH_ALIGN_PLAIN);
        
        int writeHeight = m_frame.data.frame.video_height;
        int writeWidth = AlignUp(m_frame.data.frame.video_width * m_sessionCtx->bit_depth_factor, QUADRA_WIDTH_ALIGN);

        if (i == NAL_START_CODE_2ST_BYTE || i == NAL_START_CODE_3ST_BYTE) {
            planeHeight /= QUADRA_UV_HALF_LEN;
            // U/V stride size is multiple of 128, following the calculation
            // in ni_decoder_frame_buffer_alloc
            planeWidth = AlignUp(m_frame.data.frame.video_width / QUADRA_UV_HALF_LEN * m_sessionCtx->bit_depth_factor,
                QUADRA_WIDTH_ALIGN_PLAIN);
            writeHeight /= QUADRA_UV_HALF_LEN;
            writeWidth /= QUADRA_UV_HALF_LEN;
        }

        tmpDst = m_inputData + cnt;
        inputDataArr[i] = tmpDst;

        for (int j = 0; j < planeHeight; j++) {
                (void)std::copy_n(tmpSrc, writeWidth, tmpDst);
                tmpSrc += planeWidth;
                tmpDst += writeWidth;
        }
        cnt += writeHeight * writeWidth;
    }
}

DecoderRetCode VideoDecoderQuadra::DecoderHandleData(uint8_t *buffer, const uint32_t maxLen, uint32_t *filledLen)
{
    m_planeWidth = AlignUp(m_frame.data.frame.video_width, QUADRA_WIDTH_ALIGN);
    m_planeHeight = m_frame.data.frame.video_height;

    if (m_planeWidth != m_writeWidth || m_planeHeight != m_writeHeight) {
        PicInfoParams decParams = {
            .width = m_planeWidth,
            .height = m_planeHeight,
            .stride = static_cast<int32_t>(m_planeWidth),
            .scanLines = m_planeHeight,
            .cropWidth = m_frame.data.frame.crop_right,
            .cropHeight = m_frame.data.frame.crop_bottom
        };
        ALOGI("decoder handle data, plane width is %u, plane height is %u", m_planeWidth, m_planeHeight);
        m_eventCallBack(INDEX_PIC_INFO_CHANGE, 0, &decParams);
        return VIDEO_DECODER_BAD_PIC_SIZE;
    }

    PicInfoParams params = {m_planeWidth, m_writeHeight, m_stride, m_writeHeight};

    // 初始化数组维度为4，前三列分别对应y,u,v三个分量
    uint8_t *inputDataArr[4];
    DecoderPreHandleData(inputDataArr);
    
    auto convertSize = m_copyFrame(reinterpret_cast<uint8_t *>(inputDataArr), buffer, params, maxLen);
    *filledLen = convertSize;

    auto decoderFrameBufferFree =
        reinterpret_cast<NiDecoderFrameBufferFreeFunc>(g_funcMap[NI_DECODER_FRAME_BUFFER_FREE]);
    (void) (*decoderFrameBufferFree)(&(m_frame.data.frame));

    if (m_frame.data.frame.end_of_stream == 1) {
        ALOGI("Receiving data end! frame end of stream is %u", m_frame.data.frame.end_of_stream);
        return VIDEO_DECODER_EOS;
    }
    return VIDEO_DECODER_SUCCESS;
}

void VideoDecoderQuadra::DestroyContext()
{
    ALOGI("destroy context.");

    auto deviceSessionFlush = reinterpret_cast<NiDeviceSessionFlushFunc>(g_funcMap[NI_DEVICE_SESSION_FLUSH]);
    auto deviceSessionClose = reinterpret_cast<NiDeviceSessionCloseFunc>(g_funcMap[NI_DEVICE_SESSION_CLOSE]);

    (void) (*deviceSessionFlush)(m_sessionCtx, NI_DEVICE_TYPE_DECODER);
    (void) (*deviceSessionClose)(m_sessionCtx, 1, NI_DEVICE_TYPE_DECODER);

    if (m_devCtx != nullptr) {
        ALOGI("destroy rsrc start.");
        auto rsrcReleaseResource = reinterpret_cast<NiRsrcReleaseResourceFunc>(g_funcMap[NI_RSRC_RELEASE_RESOURCE]);
        auto rsrcFreeDeviceContext =
            reinterpret_cast<NiRsrcFreeDeviceContextFunc>(g_funcMap[NI_RSRC_FREE_DEVICE_CONTEXT]);
        (*rsrcReleaseResource)(m_devCtx, m_codec, m_load);
        (*rsrcFreeDeviceContext)(m_devCtx);

        m_devCtx = nullptr;
        ALOGI("destroy rsrc done.");
    }

    auto packetBufferFree = reinterpret_cast<NiPacketBufferFreeFunc>(g_funcMap[NI_PACKET_BUFFER_FREE]);
    auto decoderFrameBufferFree =
        reinterpret_cast<NiDecoderFrameBufferFreeFunc>(g_funcMap[NI_DECODER_FRAME_BUFFER_FREE]);
    auto deviceClose = reinterpret_cast<NiDeviceCloseFunc>(g_funcMap[NI_DEVICE_CLOSE]);

    (void) (*packetBufferFree)(&(m_packet.data.packet));
    (void) (*decoderFrameBufferFree)(&(m_frame.data.frame));
    (*deviceClose)(m_sessionCtx->device_handle);
    (*deviceClose)(m_sessionCtx->blk_io_handle);

    free(m_sessionCtx);
    m_sessionCtx = nullptr;
    if (m_inputData != nullptr) {
        free(m_inputData);
        m_inputData = nullptr;
        m_mallocFlag = false;
    }

    ALOGI("destroy context done.");
}

int VideoDecoderQuadra::DeviceDecSessionWrite()
{
    uint8_t streamHeaders[4 * 1024];   // 4 * 1024：4K足够大的内存存储码流头信息
    uint8_t *hdr = streamHeaders;
    uint8_t *buf = reinterpret_cast<uint8_t *>(m_packet.data.packet.p_data);
    uint32_t dataSize = m_packet.data.packet.data_len;
    int nalType = -1;
    int headerSize = 0;
    bool spsFound = false;
    bool ppsFound = false;
    bool vpsFound = false;
    bool headersFound = false;

    // parse the packet and when SPS/PPS/VPS are found, save/update the stream
    // header info; stop searching as soon as VCL is encountered
    int nalSize = FindNextNonVclNalu(std::pair<uint8_t*, uint32_t>(buf, dataSize), m_sessionCtx->codec_format, nalType);
    while (dataSize > NAL_START_CODE_MIN_LEN && nalSize > 0) {
        if (m_sessionCtx->codec_format == NI_CODEC_FORMAT_H264) {
            if (headersFound && (nalType != static_cast<int>(H264NaluType::SPS)) &&
                (nalType != static_cast<int>(H264NaluType::PPS))) {
                break;
            }
            spsFound = (spsFound || (nalType == static_cast<int>(H264NaluType::SPS)));
            ppsFound = (ppsFound || (nalType == static_cast<int>(H264NaluType::PPS)));
            headersFound = (headersFound || (spsFound && ppsFound));
        } else if (m_sessionCtx->codec_format == NI_CODEC_FORMAT_H265) {
            if (headersFound && (nalType != static_cast<int>(H265NaluType::VPS)) &&
                (nalType != static_cast<int>(H265NaluType::PPS)) && (nalType != static_cast<int>(H265NaluType::PPS))) {
                break;
            }
            vpsFound = (vpsFound || (nalType == static_cast<int>(H265NaluType::VPS)));
            spsFound = (spsFound || (nalType == static_cast<int>(H265NaluType::SPS)));
            ppsFound = (ppsFound || (nalType == static_cast<int>(H265NaluType::PPS)));
            headersFound = (headersFound || (vpsFound && spsFound && ppsFound));
        }

        if ((m_sessionCtx->codec_format == NI_CODEC_FORMAT_H264 && (nalType == static_cast<int>(H264NaluType::SPS)
            || nalType == static_cast<int>(H264NaluType::PPS))) || (m_sessionCtx->codec_format == NI_CODEC_FORMAT_H265
            && (nalType == static_cast<int>(H265NaluType::VPS) || nalType == static_cast<int>(H265NaluType::SPS)
            || nalType == static_cast<int>(H265NaluType::PPS)))) {
            headerSize += nalSize;
            (void)std::copy_n(buf, nalSize, hdr);
            hdr += nalSize;
        }

        buf += nalSize;
        dataSize -= nalSize;

        nalSize = FindNextNonVclNalu(std::pair<uint8_t*, uint32_t>(buf, dataSize), m_sessionCtx->codec_format, nalType);
    }

    if (headersFound) {
        (void)DecSessionSaveHdrs(streamHeaders, headerSize);
    }

    auto deviceSessionWrite = reinterpret_cast<NiDeviceSessionWriteFunc>(g_funcMap[NI_DEVICE_SESSION_WRITE]);
    int txSize = (*deviceSessionWrite)(m_sessionCtx, &m_packet, NI_DEVICE_TYPE_DECODER);
    return txSize;
}

void VideoDecoderQuadra::DecSessionSaveHdrs(uint8_t streamHeaders[], int headerSize)
{
        auto deviceDecSessionSaveHdrs =
            reinterpret_cast<NiDeviceDecSessionSaveHdrsFunc>(g_funcMap[NI_DEVICE_DEC_SESSION_SAVE_HDRS]);
        ni_retcode_t ret = (*deviceDecSessionSaveHdrs)(m_sessionCtx, streamHeaders, headerSize);
        if (ret != NI_RETCODE_SUCCESS) {
            ALOGE("DeviceDecSessionWrite save hdrs failed: %d", ret);
        }
}

int VideoDecoderQuadra::FindNextNonVclNalu(std::pair<uint8_t*, uint32_t> inBuf, uint32_t codec, int &nalType)
{
    int dataSize = 0;
    uint8_t *inData = inBuf.first;
    uint32_t inSize = inBuf.second;
    nalType = -1;

    if (inSize <= NAL_START_CODE_MIN_LEN) {
        return 0;
    }

    int i = FindNalStartCode(inBuf);
    if (i == -1) {
        return 0;
    }

    // found start code, advance to NAL unit start based on actual start code
    if (inData[i + NAL_START_CODE_1ST_BYTE] != 0x00 || inData[i + NAL_START_CODE_2ST_BYTE] != 0x00 ||
        inData[i + NAL_START_CODE_3ST_BYTE] != 0x01) {
        i++;
    }
    i += NAL_START_CODE_MIN_LEN;

    // get the NAL type
    if (codec == NI_CODEC_FORMAT_H264) {
        nalType = (inData[i] & 0x1f);
        if (nalType >= static_cast<int>(H264NaluType::SLICE) &&
            nalType <= static_cast<int>(H264NaluType::IDR_SLICE)) {
            return 0;
        }
    } else if (codec == NI_CODEC_FORMAT_H265) {
        nalType = (inData[i] >> 1);
    } else {
        ALOGE("Codec format is invalid");
        return 0;
    }

    // advance to the end of NAL or stream
    while ((inData[i + NAL_START_CODE_1ST_BYTE] != 0x00 || inData[i + NAL_START_CODE_2ST_BYTE] != 0x00 ||
        inData[i + NAL_START_CODE_3ST_BYTE] != 0x00) &&
        (inData[i + NAL_START_CODE_1ST_BYTE] != 0x00 || inData[i + NAL_START_CODE_2ST_BYTE] != 0x00 ||
        inData[i + NAL_START_CODE_3ST_BYTE] != 0x01)) {
        i++;
        // if reaching/passing the stream end, return the whole data chunk size
        if (i + NAL_START_CODE_MIN_LEN > inSize) {
            dataSize = inSize;
            return inSize;
        }
    }

    return i;
}

int VideoDecoderQuadra::FindNalStartCode(std::pair<uint8_t*, uint32_t> &inBuf)
{
    uint8_t *inData = inBuf.first;
    uint32_t inSize = inBuf.second;
    int i = 0;
    // search for start code 0x000001 or 0x00000001
    while ((inData[i + NAL_START_CODE_1ST_BYTE] != 0x00 || inData[i + NAL_START_CODE_2ST_BYTE] != 0x00 ||
        inData[i + NAL_START_CODE_3ST_BYTE] != 0x01) &&
        (inData[i + NAL_START_CODE_1ST_BYTE] != 0x00 || inData[i + NAL_START_CODE_2ST_BYTE] != 0x00 ||
        inData[i + NAL_START_CODE_3ST_BYTE] != 0x00 || inData[i + NAL_START_CODE_4ST_BYTE] != 0x01)) {
        i++;
        if (i + NAL_START_CODE_MIN_LEN > inSize) {
            return -1;
        }
    }
    return i;
}
} // namespace MediaCore
