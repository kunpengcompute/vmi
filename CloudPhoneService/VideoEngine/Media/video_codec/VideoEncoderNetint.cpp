/*
 * 版权所有 (c) 华为技术有限公司 2021-2022
 * 功能说明: 适配NETINT硬件视频编码器，包括编码器初始化、启动、编码、停止、销毁等
 */

#define LOG_TAG "VideoEncoderNetint"
#include "VideoEncoderNetint.h"
#include <dlfcn.h>
#include <algorithm>
#include <cstring>
#include <string>
#include <atomic>
#include "MediaLog.h"
#include "Property.h"

namespace {
    constexpr int Y_INDEX = 0;
    constexpr int U_INDEX = 1;
    constexpr int V_INDEX = 2;
    constexpr int BIT_DEPTH = 8;
    constexpr int NUM_OF_PLANES = 4;
    constexpr int COMPRESS_RATIO = 2;

    const std::string NI_ENCODER_INIT_DEFAULT_PARAMS = "ni_logan_encoder_init_default_params";
    const std::string NI_ENCODER_PARAMS_SET_VALUE = "ni_logan_encoder_params_set_value";
    const std::string NI_RSRC_ALLOCATE_AUTO = "ni_logan_rsrc_allocate_auto";
    const std::string NI_RSRC_RELEASE_RESOURCE = "ni_logan_rsrc_release_resource";
    const std::string NI_RSRC_FREE_DEVICE_CONTEXT = "ni_logan_rsrc_free_device_context";
    const std::string NI_DEVICE_OPEN = "ni_logan_device_open";
    const std::string NI_DEVICE_CLOSE = "ni_logan_device_close";
    const std::string NI_DEVICE_SESSION_CONTEXT_INIT = "ni_logan_device_session_context_init";
    const std::string NI_DEVICE_SESSION_CONTEXT_FREE = "ni_logan_device_session_context_free";
    const std::string NI_DEVICE_SESSION_OPEN = "ni_logan_device_session_open";
    const std::string NI_DEVICE_SESSION_WRITE = "ni_logan_device_session_write";
    const std::string NI_DEVICE_SESSION_READ = "ni_logan_device_session_read";
    const std::string NI_DEVICE_SESSION_CLOSE = "ni_logan_device_session_close";
    const std::string NI_FRAME_BUFFER_ALLOC_V4 = "ni_logan_frame_buffer_alloc_v4";
    const std::string NI_FRAME_BUFFER_FREE = "ni_logan_frame_buffer_free";
    const std::string NI_PACKET_BUFFER_ALLOC = "ni_logan_packet_buffer_alloc";
    const std::string NI_PACKET_BUFFER_FREE = "ni_logan_packet_buffer_free";
    const std::string NI_GET_HW_YUV420P_DIM = "ni_logan_get_hw_yuv420p_dim";
    const std::string NI_COPY_HW_YUV420P = "ni_logan_copy_hw_yuv420p";

    using NiEncInitDefaultParamsFunc =
        ni_logan_retcode_t (*)(ni_logan_encoder_params_t *param, int fpsNum, int fpsDenom, long bitRate, int width, int height);
    using NiEncParamsSetValueFunc =
        ni_logan_retcode_t (*)(ni_logan_encoder_params_t *params, const char *name, const char *value, ni_logan_session_context_t *ctx);
    using NiRsrcAllocateAutoFunc = ni_logan_device_context_t* (*)(ni_logan_device_type_t devType, ni_alloc_rule_t rule,
        ni_codec_t codec, int width, int height, int framerate, unsigned long *load);  
    using NiRsrcReleaseResourceFunc = void (*)(ni_logan_device_context_t *devCtx, ni_codec_t codec, unsigned long load);
    using NiRsrcFreeDeviceContextFunc = void (*)(ni_logan_device_context_t *devCtx);
    using NiDeviceOpenFunc = ni_device_handle_t (*)(const char *dev, uint32_t *maxIoSizeOut);
    using NiDeviceCloseFunc = void (*)(ni_device_handle_t devHandle);
    using NiDeviceSessionContextInitFunc = void (*)(ni_logan_session_context_t *sessionCtx);
    using NiDeviceSessionContextFreeFunc = void (*)(ni_logan_session_context_t *sessionCtx);
    using NiDeviceSessionOpenFunc = ni_logan_retcode_t (*)(ni_logan_session_context_t *sessionCtx, ni_logan_device_type_t devType);
    using NiDeviceSessionWriteFunc =
        int (*)(ni_logan_session_context_t *sessionCtx, ni_logan_session_data_io_t *sessionDataIo, ni_logan_device_type_t devType);
    using NiDeviceSessionReadFunc =
        int (*)(ni_logan_session_context_t *sessionCtx, ni_logan_session_data_io_t *sessionDataIo, ni_logan_device_type_t devType);
    using NiDeviceSessionCloseFunc =
        ni_logan_retcode_t (*)(ni_logan_session_context_t *sessionCtx, int eosRecieved, ni_logan_device_type_t devType);
    using NiFrameBufferAllocV4Func = ni_logan_retcode_t (*)(ni_logan_frame_t *frame, int pixel_format, int videoWidth,
        int videoHeight, int lineSize[], int alignment, int extraLen);
    using NiFrameBufferFreeFunc = ni_logan_retcode_t (*)(ni_logan_frame_t *frame);
    using NiPacketBufferAllocFunc = ni_logan_retcode_t (*)(ni_logan_packet_t *packet, int packetSize);
    using NiPacketBufferFreeFunc = ni_logan_retcode_t (*)(ni_logan_packet_t *packet);
    using NiGetHwYuv420pDimFunc = void (*)(int width, int height, int bitDepthFactor, int isH264,
        int planeStride[NI_LOGAN_MAX_NUM_DATA_POINTERS], int planeHeight[NI_LOGAN_MAX_NUM_DATA_POINTERS]);
    
    using NiCopyHwYuv420pFunc = void (*)(uint8_t *dstPtr[NI_LOGAN_MAX_NUM_DATA_POINTERS],
        uint8_t *srcPtr[NI_LOGAN_MAX_NUM_DATA_POINTERS], int frameWidth, int frameHeight, int bitDepthFactor,
        int dstStride[NI_LOGAN_MAX_NUM_DATA_POINTERS], int dstHeight[NI_LOGAN_MAX_NUM_DATA_POINTERS],
        int srcStride[NI_LOGAN_MAX_NUM_DATA_POINTERS], int srcHeight[NI_LOGAN_MAX_NUM_DATA_POINTERS]);

    std::unordered_map<std::string, void*> g_funcMap = {
        { NI_ENCODER_INIT_DEFAULT_PARAMS, nullptr },
        { NI_ENCODER_INIT_DEFAULT_PARAMS, nullptr },
        { NI_ENCODER_PARAMS_SET_VALUE, nullptr },
        { NI_RSRC_ALLOCATE_AUTO, nullptr },
        { NI_RSRC_RELEASE_RESOURCE, nullptr },
        { NI_RSRC_FREE_DEVICE_CONTEXT, nullptr },
        { NI_DEVICE_OPEN, nullptr },
        { NI_DEVICE_CLOSE, nullptr },
        { NI_DEVICE_SESSION_CONTEXT_INIT, nullptr },
        { NI_DEVICE_SESSION_CONTEXT_FREE, nullptr },
        { NI_DEVICE_SESSION_OPEN, nullptr },
        { NI_DEVICE_SESSION_WRITE, nullptr },
        { NI_DEVICE_SESSION_READ, nullptr },
        { NI_DEVICE_SESSION_CLOSE, nullptr },
        { NI_FRAME_BUFFER_ALLOC_V4, nullptr },
        { NI_FRAME_BUFFER_FREE, nullptr },
        { NI_PACKET_BUFFER_ALLOC, nullptr },
        { NI_PACKET_BUFFER_FREE, nullptr },
        { NI_GET_HW_YUV420P_DIM, nullptr },
        { NI_COPY_HW_YUV420P, nullptr }
    };

    std::unordered_map<std::string, std::string> g_transProfile = {
        {"baseline", "1"},
        {"main", "2"},
        {"high", "4"}};    

    std::unordered_map<std::string, std::string> g_transProfile_h265 = {
        {"main", "1"}};

    const std::string SHARED_LIB_NAME = "libxcoder_logan.so";
    std::atomic<bool> g_netintLoaded = { false };
    void *g_libHandle = nullptr;
}

VideoEncoderNetint::VideoEncoderNetint(NiCodecType codecType, EncoderFormat codecFormat)
{
    if (codecType == NI_CODEC_TYPE_H264) {
        m_codec = EN_H264;
    } else {
        m_codec = EN_H265;
        m_encParams.bitrate = static_cast<uint32_t>(BITRATE_DEFAULT_265);
        m_encParams.profile = ENCODE_PROFILE_MAIN;
    }
    m_codecFormat = codecFormat;
    INFO("VideoEncoderNetint constructed %s", (m_codec == EN_H264) ? "h.264" : "h.265");
}

VideoEncoderNetint::~VideoEncoderNetint()
{
    DestroyEncoder();
    INFO("VideoEncoderNetint destructed");
}

EncoderRetCode VideoEncoderNetint::InitEncoder()
{
    if ((!GetRoEncParam()) || (!GetPersistEncParam())) {
        ERR("init encoder failed: GetEncParam failed");
        return VIDEO_ENCODER_INIT_FAIL;
    }
    m_encParams = m_tmpEncParams;
    if (m_codec == EN_H265) {
        m_encParams.profile = ENCODE_PROFILE_MAIN;
        m_tmpEncParams.profile = ENCODE_PROFILE_MAIN;
    }
    if (!LoadNetintSharedLib()) {
        ERR("init encoder failed: load NETINT so error");
        return VIDEO_ENCODER_INIT_FAIL;
    }
    m_width = static_cast<int>(m_encParams.width);
    m_height = static_cast<int>(m_encParams.height);
    const int align = (m_codec == EN_H264) ? 16 : 8;  // h.264: 16-aligned, h.265: 8-aligned
    m_widthAlign = std::max(((m_width + align - 1) / align) * align, NI_LOGAN_MIN_WIDTH);
    m_heightAlign = std::max(((m_height + align - 1) / align) * align, NI_LOGAN_MIN_HEIGHT);
    if (!InitCodec()) {
        ERR("init encoder failed: init codec error");
        return VIDEO_ENCODER_INIT_FAIL;
    }
    auto deviceSessionOpen = reinterpret_cast<NiDeviceSessionOpenFunc>(g_funcMap[NI_DEVICE_SESSION_OPEN]);
    ni_logan_retcode_t ret = (*deviceSessionOpen)(&m_sessionCtx, NI_LOGAN_DEVICE_TYPE_ENCODER);
    if (ret != NI_LOGAN_RETCODE_SUCCESS) {
        ERR("init encoder failed: device session open error %d", ret);
        return VIDEO_ENCODER_INIT_FAIL;
    }
    m_frame.data.frame.start_of_stream = 1;
    m_isInited = true;
    INFO("init encoder success");
    return VIDEO_ENCODER_SUCCESS;
}

bool VideoEncoderNetint::LoadNetintSharedLib()
{
    if (g_netintLoaded) {
        return true;
    }
    INFO("load %s", SHARED_LIB_NAME.c_str());
    g_libHandle = dlopen(SHARED_LIB_NAME.c_str(), RTLD_LAZY);
    if (g_libHandle == nullptr) {
        const char *errStr = dlerror();
        ERR("load %s error:%s", SHARED_LIB_NAME.c_str(), (errStr != nullptr) ? errStr : "unknown");
        return false;
    }
    for (auto &symbol : g_funcMap) {
        void *func = dlsym(g_libHandle, symbol.first.c_str());
        if (func == nullptr) {
            ERR("failed to load %s", symbol.first.c_str());
            return false;
        }
        symbol.second = func;
    }
    g_netintLoaded = true;
    return true;
}

bool VideoEncoderNetint::InitCodec()
{
    if (!InitCtxParams()) {
        ERR("init context params failed");
        return false;
    }
    auto deviceSessionContextInit =
        reinterpret_cast<NiDeviceSessionContextInitFunc>(g_funcMap[NI_DEVICE_SESSION_CONTEXT_INIT]);
    (*deviceSessionContextInit)(&m_sessionCtx);
    m_sessionCtx.session_id = NI_LOGAN_INVALID_SESSION_ID;
    m_sessionCtx.codec_format = (m_codec == EN_H264) ? NI_LOGAN_CODEC_FORMAT_H264 : NI_LOGAN_CODEC_FORMAT_H265;
    auto rsrcAllocateAuto = reinterpret_cast<NiRsrcAllocateAutoFunc>(g_funcMap[NI_RSRC_ALLOCATE_AUTO]);
    m_devCtx = (*rsrcAllocateAuto)(NI_LOGAN_DEVICE_TYPE_ENCODER, EN_ALLOC_LEAST_LOAD, m_codec,
        m_encParams.width, m_encParams.height, m_encParams.framerate, &m_load);
    if (m_devCtx == nullptr) {
        ERR("rsrc allocate auto failed");
        return false;
    }
    std::string xcoderId = m_devCtx->p_device_info->blk_name;
    INFO("netint xcoder id: %s", xcoderId.c_str());
    auto deviceOpen = reinterpret_cast<NiDeviceOpenFunc>(g_funcMap[NI_DEVICE_OPEN]);
    ni_device_handle_t devHandle = (*deviceOpen)(xcoderId.c_str(), &m_sessionCtx.max_nvme_io_size);
    ni_device_handle_t blkHandle = (*deviceOpen)(xcoderId.c_str(), &m_sessionCtx.max_nvme_io_size);
    if ((devHandle == NI_INVALID_DEVICE_HANDLE) || (blkHandle == NI_INVALID_DEVICE_HANDLE)) {
        ERR("device open failed");
        return false;
    }
    m_sessionCtx.device_handle = devHandle;
    m_sessionCtx.blk_io_handle = blkHandle;
    m_sessionCtx.hw_id = 0;
    m_sessionCtx.p_session_config = &m_niEncParams;
    m_sessionCtx.src_bit_depth = BIT_DEPTH;
    m_sessionCtx.src_endian = NI_LITTLE_ENDIAN_PLATFORM;
    m_sessionCtx.bit_depth_factor = 1;
    return true;
}

bool VideoEncoderNetint::InitCtxParams()
{
    auto encInitDefaultParams = reinterpret_cast<NiEncInitDefaultParamsFunc>(g_funcMap[NI_ENCODER_INIT_DEFAULT_PARAMS]);
    ni_logan_retcode_t ret = (*encInitDefaultParams)(
        &m_niEncParams, m_encParams.framerate, 1, m_encParams.bitrate, m_encParams.width, m_encParams.height);
    if (ret != NI_LOGAN_RETCODE_SUCCESS) {
        ERR("encoder init default params error %d", ret);
        return false;
    }
    const std::string gopPresetOpt = "gopPresetIdx";
    const std::string lowDelayOpt = "lowDelay";
    const std::string rateControlOpt = "RcEnable";
    const std::string profileOpt = "profile";
    const std::string lowDelayPocTypeOpt = "useLowDelayPocType";
    const std::string noBframeOption = "2";
    const std::string enableOption = "1";
    std::string profileValue = "";
    if (m_codec == EN_H264) {
        profileValue = g_transProfile[m_encParams.profile];
    } else if (m_codec == EN_H265) {
        profileValue = g_transProfile_h265[m_encParams.profile];
    }

    std::unordered_map<std::string, std::string> xcoderParams = {
        { gopPresetOpt, noBframeOption },   // GOP: IPPP...
        { lowDelayOpt, enableOption },      // enable low delay mode
        { rateControlOpt, enableOption },   // rate control enable
        { profileOpt, profileValue },     // profile: High(h.264), Main(h.265)
        { lowDelayPocTypeOpt, enableOption} // enable lowDelayPoc
    };
    auto encParamsSetValue = reinterpret_cast<NiEncParamsSetValueFunc>(g_funcMap[NI_ENCODER_PARAMS_SET_VALUE]);
    for (const auto &xParam : xcoderParams) {
        ret = (*encParamsSetValue)(&m_niEncParams, xParam.first.c_str(), xParam.second.c_str(), &m_sessionCtx);
        if (ret != NI_LOGAN_RETCODE_SUCCESS) {
            ERR("encoder params set value error %d: name %s : value %s",
                ret, xParam.first.c_str(), xParam.second.c_str());
            return false;
        }
    }
    if (m_widthAlign > m_width) {
        m_niEncParams.source_width = m_widthAlign;
    }
    if (m_heightAlign > m_height) {
        m_niEncParams.source_height = m_heightAlign;
    }
    return true;
}

EncoderRetCode VideoEncoderNetint::StartEncoder()
{
    INFO("start encoder success");
    return VIDEO_ENCODER_SUCCESS;
}

EncoderRetCode VideoEncoderNetint::EncodeOneFrame(const uint8_t *inputData, uint32_t inputSize,
    uint8_t **outputData, uint32_t *outputSize)
{
    uint32_t frameSize = static_cast<uint32_t>(m_width * m_height * 3 / COMPRESS_RATIO);    //用于字节对齐
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

    auto deviceSessionWrite = reinterpret_cast<NiDeviceSessionWriteFunc>(g_funcMap[NI_DEVICE_SESSION_WRITE]);
    int oneSent = 0;
    uint32_t sentCnt = 0;
    constexpr int maxSentTimes = 3;
    while (oneSent == 0 && sentCnt < maxSentTimes) {
        oneSent = (*deviceSessionWrite)(&m_sessionCtx, &m_frame, NI_LOGAN_DEVICE_TYPE_ENCODER);
        ++sentCnt;
    }
    if (oneSent < 0 || sentCnt == maxSentTimes) {
        ERR("device session write error, return sent size = %d", oneSent);
        return VIDEO_ENCODER_ENCODE_FAIL;
    }
    ni_logan_frame_t dataFrame = m_frame.data.frame;
    uint32_t sentBytes = dataFrame.data_len[Y_INDEX] + dataFrame.data_len[U_INDEX] + dataFrame.data_len[V_INDEX];
    DBG("encoder send data success, total sent data size = %u", sentBytes);

    ni_logan_packet_t *dataPacket = &(m_packet.data.packet);
    auto packetBufferAlloc = reinterpret_cast<NiPacketBufferAllocFunc>(g_funcMap[NI_PACKET_BUFFER_ALLOC]);
    ni_logan_retcode_t ret = (*packetBufferAlloc)(dataPacket, frameSize);
    if (ret != NI_LOGAN_RETCODE_SUCCESS) {
        ERR("packet buffer alloc error %d", ret);
        return VIDEO_ENCODER_ENCODE_FAIL;
    }
    auto deviceSessionRead = reinterpret_cast<NiDeviceSessionReadFunc>(g_funcMap[NI_DEVICE_SESSION_READ]);
    int oneRead = (*deviceSessionRead)(&m_sessionCtx, &m_packet, NI_LOGAN_DEVICE_TYPE_ENCODER);
    const int metaDataSize = NI_LOGAN_FW_ENC_BITSTREAM_META_DATA_SIZE;
    if (oneRead > metaDataSize) {
        if (m_sessionCtx.pkt_num == 0) {
            m_sessionCtx.pkt_num = 1;
        }
    } else if (oneRead != 0) {
        ERR("received %d bytes <= metadata size %d", oneRead, metaDataSize);
        return VIDEO_ENCODER_ENCODE_FAIL;
    }
    *outputData = static_cast<uint8_t *>(dataPacket->p_data) + metaDataSize;
    *outputSize = static_cast<uint32_t>(dataPacket->data_len - metaDataSize);
    return VIDEO_ENCODER_SUCCESS;
}

bool VideoEncoderNetint::InitFrameData(const uint8_t *src)
{
    if (src == nullptr) {
        ERR("input data buffer is null");
        return false;
    }
    ni_logan_frame_t *dataFrame = &(m_frame.data.frame);
    dataFrame->start_of_stream = 0;
    dataFrame->end_of_stream = 0;
    dataFrame->force_key_frame = 0;
    dataFrame->video_width = m_width;
    dataFrame->video_height = m_height;
    dataFrame->extra_data_len = NI_LOGAN_APP_ENC_FRAME_META_DATA_SIZE;

    int dstPlaneStride[NUM_OF_PLANES] = {0};
    int dstPlaneHeight[NUM_OF_PLANES] = {0};
    int srcPlaneStride[NUM_OF_PLANES] = {0};
    int srcPlaneHeight[NUM_OF_PLANES] = {0};
    auto getHwYuv420pDim = reinterpret_cast<NiGetHwYuv420pDimFunc>(g_funcMap[NI_GET_HW_YUV420P_DIM]);
    (*getHwYuv420pDim)(m_width, m_height, m_sessionCtx.bit_depth_factor,
        m_sessionCtx.codec_format == NI_LOGAN_CODEC_FORMAT_H264, dstPlaneStride, dstPlaneHeight);

    auto frameBufferAllocV4 = reinterpret_cast<NiFrameBufferAllocV4Func>(g_funcMap[NI_FRAME_BUFFER_ALLOC_V4]);
    ni_logan_retcode_t ret = (*frameBufferAllocV4)(dataFrame, NI_LOGAN_PIX_FMT_YUV420P, m_width, m_height, dstPlaneStride,
        m_sessionCtx.codec_format == NI_LOGAN_CODEC_FORMAT_H264, dataFrame->extra_data_len);
    if (ret != NI_LOGAN_RETCODE_SUCCESS || !dataFrame->p_data[Y_INDEX]) {
        ERR("frame buffer alloc failed: ret = %d", ret);
        return false;
    }
    srcPlaneStride[Y_INDEX] = m_width * m_sessionCtx.bit_depth_factor;
    srcPlaneStride[U_INDEX] = m_width / COMPRESS_RATIO;
    srcPlaneStride[V_INDEX] = m_width / COMPRESS_RATIO;
    srcPlaneHeight[Y_INDEX] = m_height;
    srcPlaneHeight[U_INDEX] = m_height / COMPRESS_RATIO;
    srcPlaneHeight[V_INDEX] = m_height / COMPRESS_RATIO;
    uint8_t *srcPlanes[NUM_OF_PLANES];
    srcPlanes[Y_INDEX] = const_cast<uint8_t*>(src);
    srcPlanes[U_INDEX] = srcPlanes[Y_INDEX] + srcPlaneStride[Y_INDEX] * srcPlaneHeight[Y_INDEX];
    srcPlanes[V_INDEX] = srcPlanes[U_INDEX] + srcPlaneStride[U_INDEX] * srcPlaneHeight[U_INDEX];

    auto copyHwYuv420p = reinterpret_cast<NiCopyHwYuv420pFunc>(g_funcMap[NI_COPY_HW_YUV420P]);
    (*copyHwYuv420p)((uint8_t**)(dataFrame->p_data), srcPlanes, m_width, m_height, m_sessionCtx.bit_depth_factor,
        dstPlaneStride, dstPlaneHeight, srcPlaneStride, srcPlaneHeight);
    return true;
}

EncoderRetCode VideoEncoderNetint::StopEncoder()
{
    INFO("stop encoder success");
    return VIDEO_ENCODER_SUCCESS;
}

void VideoEncoderNetint::CheckFuncPtr()
{
    for (auto &symbol : g_funcMap) {
        if (symbol.second == nullptr) {
            m_FunPtrError = true;
            ERR("%s ptr is nullptr", symbol.first.c_str());
        }
    }
}

void VideoEncoderNetint::DestroyEncoder()
{
    if (!m_isInited) {
        INFO("Destroy encoder already triggered, return");
        return;
    }
    INFO("destroy encoder start");
    if (g_libHandle == nullptr) {
        WARN("encoder has been destroyed");
        return;
    }
    CheckFuncPtr();
    ni_logan_retcode_t ret;
    if (g_funcMap[NI_DEVICE_SESSION_CLOSE] != nullptr) {
        auto deviceSessionClose = reinterpret_cast<NiDeviceSessionCloseFunc>(g_funcMap[NI_DEVICE_SESSION_CLOSE]);
        ret = (*deviceSessionClose)(&m_sessionCtx, 1, NI_LOGAN_DEVICE_TYPE_ENCODER);
        if (ret != NI_LOGAN_RETCODE_SUCCESS) {
            WARN("device session close failed: ret = %d", ret);
        }
    }
    if (g_funcMap[NI_DEVICE_CLOSE] != nullptr) {
        auto deviceClose = reinterpret_cast<NiDeviceCloseFunc>(g_funcMap[NI_DEVICE_CLOSE]);
        (*deviceClose)(m_sessionCtx.device_handle);
        (*deviceClose)(m_sessionCtx.blk_io_handle);
    }
    if (m_devCtx != nullptr) {
        INFO("destroy rsrc start");
        if (g_funcMap[NI_RSRC_RELEASE_RESOURCE] != nullptr) {
            auto rsrcReleaseResource = reinterpret_cast<NiRsrcReleaseResourceFunc>(g_funcMap[NI_RSRC_RELEASE_RESOURCE]);
            (*rsrcReleaseResource)(m_devCtx, m_codec, m_load);
        }
        if (g_funcMap[NI_RSRC_FREE_DEVICE_CONTEXT] != nullptr) {
            auto rsrcFreeDeviceContext =
            reinterpret_cast<NiRsrcFreeDeviceContextFunc>(g_funcMap[NI_RSRC_FREE_DEVICE_CONTEXT]);
            (*rsrcFreeDeviceContext)(m_devCtx);
        }
        m_devCtx = nullptr;
        INFO("destroy rsrc done");
    }
    if (g_funcMap[NI_DEVICE_SESSION_CONTEXT_FREE] != nullptr) {
        auto deviceSessionContextFree =
            reinterpret_cast<NiDeviceSessionContextFreeFunc>(g_funcMap[NI_DEVICE_SESSION_CONTEXT_FREE]);
        (*deviceSessionContextFree)(&m_sessionCtx);
    }
    if (g_funcMap[NI_FRAME_BUFFER_FREE] != nullptr) {
        auto frameBufferFree = reinterpret_cast<NiFrameBufferFreeFunc>(g_funcMap[NI_FRAME_BUFFER_FREE]);
        ret = (*frameBufferFree)(&(m_frame.data.frame));
        if (ret != NI_LOGAN_RETCODE_SUCCESS) {
            WARN("device session close failed: ret = %d", ret);
        }
    }
    if (g_funcMap[NI_PACKET_BUFFER_FREE] != nullptr) {
        auto packetBufferFree = reinterpret_cast<NiPacketBufferFreeFunc>(g_funcMap[NI_PACKET_BUFFER_FREE]);
        ret = (*packetBufferFree)(&(m_packet.data.packet));
        if (ret != NI_LOGAN_RETCODE_SUCCESS) {
            WARN("device session close failed: ret = %d", ret);
        }
    }
    if (m_FunPtrError) {
        UnLoadNetintSharedLib();
    }
    m_isInited = false;
    INFO("destroy encoder done");
}

void VideoEncoderNetint::UnLoadNetintSharedLib()
{
    INFO("UnLoadNetintSharedLib");
    for (auto &symbol : g_funcMap) {
        symbol.second = nullptr;
    }
    (void)dlclose(g_libHandle);
    g_libHandle = nullptr;
    g_netintLoaded = false;
    m_FunPtrError = false;
}

EncoderRetCode VideoEncoderNetint::ResetEncoder()
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

EncoderRetCode VideoEncoderNetint::ForceKeyFrame()
{
    INFO("force key frame success");
    return VIDEO_ENCODER_SUCCESS;
}

/**
* @功能描述: 初始化视频流模式下的编码参数
*/
EncoderRetCode VideoEncoderNetint::Config(const VmiEncoderConfig& config)
{
    return GeneralConfig(config);
}

/**
* @功能描述: 参数有效性由上层检验，动态修改编码相关参数
*/
EncoderRetCode VideoEncoderNetint::SetParams(const VmiEncoderParams& params)
{
    return GeneralSetParams(params);
}
