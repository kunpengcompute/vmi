/*
 * 版权所有 (c) 华为技术有限公司 2022-2022
 * 功能描述：Inno GPU编码模块对外接口
 */

#include "GpuEncoderHantro.h"
#include "logging.h"
#include "AvcodecWrapper.h"
#include "DisplayServer/DisplayServer.h"
#include <cutils/properties.h>
#include <string.h>
#include <sync/sync.h>
#include <sw_sync.h>

#ifdef __cplusplus
extern "C" {
#endif
void *CreateGpuTurbo(uint32_t type)
{
    void *ret = nullptr;
    auto gpuType = static_cast<Vmi::GpuEncoder::GpuType>(type);
    switch (gpuType) {
        case Vmi::GpuEncoder::GpuType::GPU_HANTRO_SG100:
            if (!Vmi::AvcodecWrapper::Get().Init()) {
                ERR("Fail to load avcodec functions, abort");
                return nullptr;
            }
            ret = std::make_unique<Vmi::GpuEncoder::GpuEncoderHantro>().release();
            break;
        default:
            INFO("Cannot create encoder, unsupport gpu type: %u", type);
            return nullptr;
    }
    return ret;
}
#ifdef __cplusplus
}
#endif

/* 规避avcodec头文件中inline函数的链接问题 */
int av_strerror(int errnum, char *errbuf, size_t errbuf_size)
{
    return Vmi::AvcodecWrapper::Get().Call<Vmi::AV_STRERROR>(errnum, errbuf, errbuf_size);
}

namespace {
// 内部使用，需要保证输入宽高小于4096
uint32_t GetBufferSize(uint32_t width, uint32_t height, uint32_t type)
{
    switch (type) {
        case Vmi::GpuEncoder::FRAME_FORMAT_YUV:
        case Vmi::GpuEncoder::FRAME_FORMAT_NV12: {
            uint32_t pixelsNum = width * height;
            auto uvSize = (pixelsNum + 3) >> 2; // 3: (2^2 - 1)用于除4的对齐, 2：右移2用于除4，uv的数据大小为y的1/4
            return pixelsNum + uvSize + uvSize;
        }
        case Vmi::GpuEncoder::FRAME_FORMAT_RGBA:
        case Vmi::GpuEncoder::FRAME_FORMAT_BGRA: {
            return width * height * 4; // 4: RGBA数据每个像素包含四个uint8值
        }
        case Vmi::GpuEncoder::FRAME_FORMAT_H264:
        case Vmi::GpuEncoder::FRAME_FORMAT_HEVC: {
            return width * height; // 码流大小一般不会大于宽*高
        } default: {
            return 0;
        }
    }
}

inline uint32_t AlignUp(uint32_t val, uint32_t align)
{
    return (val + (align - 1)) & ~(align - 1);
}

enum AVPixelFormat get_vastapi_format(AVCodecContext *ctx, const enum AVPixelFormat *pix_fmts) {
    const enum AVPixelFormat *p;
    for (p = pix_fmts; *p != AV_PIX_FMT_NONE; p++) {
        if (*p == AV_PIX_FMT_VASTAPI)
            return *p;
    }
    ERR("Unable to decode this file using VA-API. ctx is null: %d\n", (int)(ctx == nullptr));
    return AV_PIX_FMT_NONE;
}

char* GetVastaiVideoNodeNumber()
{
    char* number = new char[1];
    if (property_get("ro.kernel.va.gpu.id", number, "0") <= 0) {
        ERR("Init vastai encoder, get gpu id failed.");
        return nullptr;
    }
    return number;
}

constexpr uint32_t MAX_WIDTH = 4096;
constexpr uint32_t MAX_HEIGHT = 4096;
constexpr uint32_t WIDTH_ALIGN = 32;
constexpr uint32_t HEIGHT_ALIGN = 32;
}

namespace Vmi {
namespace GpuEncoder {
bool GpuEncoderHantro::CheckAndLockStatus(Status status)
{
    std::lock_guard<std::mutex> lk(m_lock);
    if (m_status != status) {
        ERR("Status check failed, status is %d, expected %d",
            static_cast<int32_t>(m_status), static_cast<int32_t>(status));
        return false;
    }
    m_originalStatus = m_status;
    m_status = Status::BUSY;
    return true;
}

void GpuEncoderHantro::UnlockStatus(Status status)
{
    std::lock_guard<std::mutex> lk(m_lock);
    m_status = status;
}

int32_t GpuEncoderHantro::Init(EncoderConfig &config)
{
    if (!CheckAndLockStatus(Status::INVALID)) {
        return -ERR_INVALID_STATUS;
    }

    if (config.inSize.width > MAX_WIDTH || config.inSize.height > MAX_HEIGHT ||
        config.inSize.width == 0 || config.inSize.height == 0) {
        ERR("Invalid frame size, current[%u x %u], max[%u x %u]",
            config.inSize.width, MAX_WIDTH, config.inSize.height, MAX_HEIGHT);
        UnlockStatus(m_originalStatus);
        return -ERR_INVALID_PARAM;
    }

    m_size = config.inSize;
    m_size.widthAligned = AlignUp(m_size.width, WIDTH_ALIGN);
    m_size.heightAligned = AlignUp(m_size.height, HEIGHT_ALIGN);

    // Allocate
    AVCodecContext* ctx;
    AVCodec* codec;
    if (!AllocContext(ctx, codec, config)) {
        ERR("Alloc encoder context failed!");
        return -ERR_INTERNAL_ERROR;
    }
    if (!CreateAndSetHwDeviceCtx(ctx, codec)) {
        ERR("Create and set device ctx failed!");
        return -ERR_INTERNAL_ERROR;
    }
    // Initialize timeline for Async fence
    if (!m_needRestart) {
        m_fenceTimeline = sw_sync_timeline_create();
    }

    m_capability = config.capability;                  // AllocContext will check capability legal or illegal
    m_avcodec = ctx;
    UnlockStatus(Status::INITED);
    INFO("Gpu encode hantro init success");
    return OK;
}

bool GpuEncoderHantro::AllocContext(AVCodecContext *&ctx, AVCodec *&codec, EncoderConfig &config)
{
    // find AVCodec
    const char* codecName = "h264_vastapi";
    if (config.capability == EncoderCapability::CAP_VA_ENCODE_H264) {
        codecName = "h264_vastapi";
    } else if (config.capability == EncoderCapability::CAP_VA_ENCODE_HEVC) {
        codecName = "hevc_vastapi";
    } else {
        ERR("Hantro video encoder unsupport format: %u", config.capability);
        return false;
    }
    if (!(codec = AvcodecWrapper::Get().Call<AVCODEC_FIND_ENCODER_BY_NAME>(codecName))) {
        ERR("Hantro video encoder: %s not found", codecName);
        return false;
    }
    if (!(ctx = AvcodecWrapper::Get().Call<AVCODEC_ALLOC_CONTEXT3>(codec))) {
        ERR("Hantro avcodec_alloc_context3 failed, not memory\n");
        return false;
    }

    // set encoder param
    ctx->gop_size = m_settingParams.gopSize;
    ctx->max_b_frames = 0; // forbidden b frame
    ctx->width = config.inSize.width;
    ctx->height = config.inSize.height;
    ctx->time_base = (AVRational){1, (int)(m_settingParams.frameRate)};
    ctx->framerate = (AVRational){(int)(m_settingParams.frameRate), 1};
    ctx->pix_fmt = AV_PIX_FMT_VASTAPI;
    ctx->bit_rate = m_settingParams.bitRate;
    ctx->get_format = get_vastapi_format;
    ctx->profile = m_settingParams.profile;

    return true;
}

bool GpuEncoderHantro::CreateAndSetHwDeviceCtx(AVCodecContext *&ctx, AVCodec* codec)
{
    int err = 0;
    AVBufferRef* hwDeviceCtx;
    // create hw device ctx
    char* number = GetVastaiVideoNodeNumber();
    if (number == nullptr) {
        return false;
    }
    char* node = new char[sizeof("/dev/va_video") + sizeof(number)];
    strcpy(node, "/dev/va_video");
    strcat(node, number);
    err = AvcodecWrapper::Get().Call<AV_HWDEVICE_CTX_CREATE>(&hwDeviceCtx, AV_HWDEVICE_TYPE_VASTAPI, node, nullptr, 0);
    if (err < 0) {
        ERR("initHWAVCtx av_hwdevice_ctx_create error. Error code: %s\n", av_err2str(err));
        return false;
    }
    // set hw device ctx
    ctx->hw_device_ctx = AvcodecWrapper::Get().Call<AV_BUFFER_REF>(hwDeviceCtx);
    if (!ctx->hw_device_ctx) {
        ERR("A hardware device reference create failed.\n");
        return false;
    }
    // set hw frame ctx, RGB0 input
    if (SetHWFrameCtx(ctx, hwDeviceCtx, AV_PIX_FMT_RGB0, 1) < 0) {
        ERR("initHWAVCtx setHWFrameCtx failed.\n");
        return false;
    }
    std::string profile = "";
    if (ConvertProfileCodeToString(m_settingParams.profile, profile) != OK) {
        ERR("Get profile string according profile code failed, code: %u", m_settingParams.profile);
        return false;
    }
    std::string presetParams = "tune=1:vbvBufSize=1000:miniGopSize=1:lookaheadLength=0:intraQpOffset=-2:P2B=0";
    presetParams = presetParams + ":preset=" + profile + ":keyint=" + std::to_string(m_settingParams.gopSize);
    const char* vastParams = presetParams.c_str();
    if ((err = AvcodecWrapper::Get().Call<AV_OPT_SET>(ctx->priv_data, "vast-params", vastParams, 0)) < 0) {
        ERR("initHWAVCtx av_opt_set error. Error code: %s\n", av_err2str(err));
        return false;
    }

    if ((err = AvcodecWrapper::Get().Call<AVCODEC_OPEN2>(ctx, codec, nullptr)) < 0) {
        ERR("initHWAVCtx Cannot open video encoder codec. Error code: %s\n", av_err2str(err));
        return false;
    }
    return true;
}

int GpuEncoderHantro::SetHWFrameCtx(AVCodecContext *ctx, AVBufferRef *hw_device_ctx, AVPixelFormat pixelFormat,
                                    int flag)
{
    AVBufferRef *hw_frames_ref;
    AVHWFramesContext *frames_ctx = NULL;
    int err = 0;

    if (!(hw_frames_ref = AvcodecWrapper::Get().Call<AV_HWFRAME_CTX_ALLOC>(hw_device_ctx))) {
        ERR("Failed to create VASTAPI frame context.\n");
        return -1;
    }
    frames_ctx = (AVHWFramesContext *)(hw_frames_ref->data);

    /* set parameters */
    frames_ctx->sw_format = pixelFormat;
    frames_ctx->format    = AV_PIX_FMT_VASTAPI;
    frames_ctx->width     = ctx->width;
    frames_ctx->height    = ctx->height;
    frames_ctx->initial_pool_size = 30;
    frames_ctx->frame_buffer_flag = flag;

    if ((err = AvcodecWrapper::Get().Call<AV_HWFRAME_CTX_INIT>(hw_frames_ref)) < 0) {
        ERR("Failed to initialize VASTAPI frame context."
            "Error code: %s\n", av_err2str(err));
        AvcodecWrapper::Get().Call<AV_BUFFER_UNREF>(&hw_frames_ref);
        return err;
    }

    ctx->hw_frames_ctx = AvcodecWrapper::Get().Call<AV_BUFFER_REF>(hw_frames_ref);
    if (!ctx->hw_frames_ctx) {
        ERR("Failed to do hw_frames_ref.");
        err = AVERROR(ENOMEM);
    }

    AvcodecWrapper::Get().Call<AV_BUFFER_UNREF>(&hw_frames_ref);
    return err;
}


int32_t GpuEncoderHantro::DeInit()
{
    (void)Stop();
    if (!CheckAndLockStatus(Status::INITED)) {
        return -ERR_INVALID_STATUS;
    }

    // Release hantro fence_timeline
    if (!m_needRestart) {
        close(m_fenceTimeline);
    }

    // 销毁编码器 直接赋空
    AvcodecWrapper::Get().Call<AVCODEC_FREE_CONTEXT>(&m_avcodec);
    m_avcodec = nullptr;
    UnlockStatus(Status::INVALID);
    return OK;
}

int32_t GpuEncoderHantro::Start()
{
    if (!CheckAndLockStatus(Status::INITED)) {
        return -ERR_INVALID_STATUS;
    }
    UnlockStatus(Status::STARTED);
    INFO("Gpu encode hantro start success");
    return OK;
}

int32_t GpuEncoderHantro::Stop()
{
    if (!CheckAndLockStatus(Status::STARTED)) {
        return -ERR_INVALID_STATUS;
    }
    UnlockStatus(Status::INITED);
    return OK;
}

int32_t GpuEncoderHantro::CreateBuffer(FrameFormat format, MemType memType, GpuEncoderBufferT &buffer)
{
    std::lock_guard<std::mutex> lk(m_lock);
    if (m_status != Status::STARTED) {
        ERR("Invalid status for buffer create: %d", static_cast<int32_t>(m_status));
        return -ERR_INVALID_STATUS;
    }

    if (memType != MEM_TYPE_DEVICE) {
        ERR("Unsupport mem type for buffer create: %u", memType);
        return -ERR_UNSUPPORT_OPERATION;
    }
    if (format != FRAME_FORMAT_H264 && format != FRAME_FORMAT_HEVC) {
        ERR("Unsupport format type for buffer create: %u", format);
        return -ERR_UNSUPPORT_OPERATION;
    }

    auto newBuffer = std::make_unique<GpuEncoderBufferHantro>();
    newBuffer->format = format;
    newBuffer->memType = memType;
    newBuffer->size = m_size;
    newBuffer->gpuType = GPU_HANTRO_SG100;

    newBuffer->streamBufferSize = GetBufferSize(m_size.widthAligned, m_size.heightAligned, format);
    newBuffer->streamBuffer = std::make_unique<uint8_t[]>(newBuffer->streamBufferSize);

    buffer = newBuffer.release();
    m_buffers.emplace(buffer);
    return OK;
}

bool GpuEncoderHantro::CreateHwFrame(AVFrame *&frame)
{
    frame = AvcodecWrapper::Get().Call<AV_FRAME_ALLOC>();
    if (frame == nullptr) {
        ERR("fail to allocate hwframe");
        return false;
    }
    int32_t err = AvcodecWrapper::Get().Call<AV_HWFRAME_GET_BUFFER>(m_avcodec->hw_frames_ctx, frame, 0);
    if (err < 0) {
        ERR("fail to get buffer from avcodec context: %s", av_err2str(err));
        AvcodecWrapper::Get().Call<AV_FRAME_FREE>(&frame);
        return false;
    }

    if (frame->hw_frames_ctx == nullptr) {
        ERR("Get hwframe context fail");
        AvcodecWrapper::Get().Call<AV_FRAME_FREE>(&frame);
        return false;
    }
    return true;
}

int32_t GpuEncoderHantro::ImportBuffer(FrameFormat format, uint64_t handle, GpuEncoderBufferT &buffer)
{
    std::lock_guard<std::mutex> lk(m_lock);
    if (m_status != Status::STARTED) {
        ERR("Invalid status for buffer import: %d", static_cast<int32_t>(m_status));
        return -ERR_INVALID_STATUS;
    }
    if (format != FRAME_FORMAT_RGBA) {
        ERR("Unsupport format type for buffer import: %u", format);
        return -ERR_UNSUPPORT_OPERATION;
    }
    // hantro async buffer
    HantroCaptureBuffer *captureBuffer = reinterpret_cast<HantroCaptureBuffer*>(handle);
    int releaseFence = sw_sync_fence_create(m_fenceTimeline, "releaseFence", m_fenceValue);
    int acquireFence = -1;
    captureBuffer->acquireBufferCaller(captureBuffer->rgbBuffer, &acquireFence, &releaseFence);
    m_fenceValue++;
    auto newBuffer = std::make_unique<GpuEncoderBufferHantro>();
    newBuffer->format = format;
    newBuffer->memType = MEM_TYPE_DEVICE;
    newBuffer->size = m_size;
    newBuffer->gpuType = GPU_HANTRO_SG100;
    newBuffer->external = true;
    newBuffer->captureBuffer = captureBuffer;
    newBuffer->acquireFence = acquireFence;

    int32_t err;
    if (!CreateHwFrame(newBuffer->frame)) {
        return -ERR_INTERNAL_ERROR;
    }
    err = AvcodecWrapper::Get().Call<AV_HWFRAME_SET_ADDR_TO_SURFACEID>(newBuffer->frame, captureBuffer->data);
    if (err < 0) {
        ERR("fail to bind frame buffer handle to hwframe: %s", av_err2str(err));
        AvcodecWrapper::Get().Call<AV_FRAME_FREE>(&newBuffer->frame);
        return -1;
    }

    buffer = newBuffer.release();
    m_buffers.emplace(buffer);
    return OK;
}

int32_t GpuEncoderHantro::ReleaseBuffer(GpuEncoderBufferT &buffer)
{
    std::lock_guard<std::mutex> lk(m_lock);
    if (m_status != Status::STARTED) {
        ERR("Invalid status for buffer release: %d", static_cast<int32_t>(m_status));
        return -ERR_INVALID_STATUS;
    }
    auto &&record = m_buffers.find(buffer);
    if (record == m_buffers.end()) {
        ERR("Invalid buffer ptr for buffer release: %p", buffer);
        return -ERR_INVALID_PARAM;
    }

    auto hantroBuffer = static_cast<GpuEncoderBufferHantroT>(*record);
    if (hantroBuffer->mapped) {
        ERR("Please unmap the buffer before release.");
        return -ERR_INVALID_PARAM;
    }

    if (hantroBuffer->format == FRAME_FORMAT_RGBA) {
        AvcodecWrapper::Get().Call<AV_FRAME_FREE>(&hantroBuffer->frame);
        hantroBuffer->captureBuffer->releaseBufferCaller(hantroBuffer->captureBuffer->rgbBuffer);
        if (hantroBuffer->acquireFence > 0) {
            close(hantroBuffer->acquireFence);                                  // hantro async wait need to close fd
        }
        sw_sync_timeline_inc(m_fenceTimeline, 1);                               // hantro suggest release rgb method.
        delete hantroBuffer->captureBuffer;                                     // release async rgb buffer
    } else if (hantroBuffer->format == FRAME_FORMAT_H264 || hantroBuffer->format == FRAME_FORMAT_HEVC) {
        // release 264/hevc buffer
        hantroBuffer->streamBuffer = nullptr;
        hantroBuffer->streamBufferSize = 0;
    }

    m_buffers.erase(buffer);
    buffer = nullptr;
    delete hantroBuffer;
    return OK;
}

int32_t GpuEncoderHantro::MapBuffer(GpuEncoderBufferT &buffer, uint32_t flag)
{
    std::lock_guard<std::mutex> lk(m_lock);
    if (m_status != Status::STARTED) {
        ERR("Invalid status for buffer map: %d", static_cast<int32_t>(m_status));
        return -ERR_INVALID_STATUS;
    }
    auto &&record = m_buffers.find(buffer);
    if (record == m_buffers.end()) {
        ERR("Invalid buffer ptr for buffer map: %p", buffer);
        return -ERR_INVALID_PARAM;
    }
    if (buffer->format != FRAME_FORMAT_H264 && buffer->format != FRAME_FORMAT_HEVC) {
        ERR("Hantro map buffer only support 264/265 frame foramt : %u", buffer->format);
        return -ERR_INVALID_PARAM;
    }
    if (flag != FLAG_READ) {
        ERR("Hantro map buffer only support read flag.");
        return -ERR_INVALID_PARAM;
    }

    auto hantroBuffer = static_cast<GpuEncoderBufferHantroT>(buffer);
    if (hantroBuffer->mapped) {
        ERR("Buffer already mapped");
        return -ERR_INVALID_PARAM;
    }

    hantroBuffer->data = hantroBuffer->streamBuffer.get();
    hantroBuffer->mapped = true;
    return OK;
}

int32_t GpuEncoderHantro::RetriveBufferData(GpuEncoderBufferT &buffer, uint8_t *data,
                                            uint32_t memLen, uint32_t &dataLen)
{
    std::lock_guard<std::mutex> lk(m_lock);
    if (m_status != Status::STARTED) {
        ERR("Invalid status for buffer data retrieve: %d", static_cast<int32_t>(m_status));
        return -ERR_INVALID_STATUS;
    }
    (void) buffer;
    (void) data;
    memLen = 0;
    dataLen = 0;
    return -ERR_UNSUPPORT_OPERATION;
}

int32_t GpuEncoderHantro::UnmapBuffer(GpuEncoderBufferT &buffer)
{
    std::lock_guard<std::mutex> lk(m_lock);
    if (m_status != Status::STARTED) {
        ERR("Invalid status for buffer unmap: %d", static_cast<int32_t>(m_status));
        return -ERR_INVALID_STATUS;
    }
    auto &&record = m_buffers.find(buffer);
    if (record == m_buffers.end()) {
        ERR("Invalid buffer ptr for buffer unmap: %p", buffer);
        return -ERR_INVALID_PARAM;
    }
    if (buffer->format != FRAME_FORMAT_H264 && buffer->format != FRAME_FORMAT_HEVC) {
        ERR("Hantro unmap buffer onlut support 264/265 frame format: %u", buffer->format);
        return -ERR_INVALID_PARAM;
    }

    auto hantroBuffer = static_cast<GpuEncoderBufferHantroT>(buffer);
    if (!hantroBuffer->mapped) {
        ERR("Buffer is not mapped, cannot unmap");
        return -ERR_INVALID_PARAM;
    }

    hantroBuffer->data = nullptr;
    hantroBuffer->mapped = false;
    return OK;
}

int32_t GpuEncoderHantro::Convert(GpuEncoderBufferT &inBuffer, GpuEncoderBufferT &outBuffer)
{
    (void)inBuffer;
    (void)outBuffer;
    return -ERR_UNSUPPORT_OPERATION;
}

int32_t GpuEncoderHantro::Encode(GpuEncoderBufferT &inBuffer, GpuEncoderBufferT &outBuffer)
{
    std::lock_guard<std::mutex> lk(m_lock);
    if (m_status != Status::STARTED) {
        ERR("Invalid status for encode: %d", static_cast<int32_t>(m_status));
        return -ERR_INVALID_STATUS;
    }
    auto &&inIt = m_buffers.find(inBuffer);
    auto &&outIt = m_buffers.find(outBuffer);
    if (inIt == m_buffers.end() || outIt == m_buffers.end()) {
        ERR("Invalid buffer ptr for encode: %p | %p", inBuffer, outBuffer);
        return -ERR_INVALID_PARAM;
    }
    if (inBuffer->format != FRAME_FORMAT_RGBA ||
        (outBuffer->format != FRAME_FORMAT_H264 && outBuffer->format != FRAME_FORMAT_HEVC)) {
        ERR("Unsupport format type for encode: %u -> %u", inBuffer->format, outBuffer->format);
        return -ERR_UNSUPPORT_OPERATION;
    }
    int stats = EncodeParamSetAndEncode(inBuffer, outBuffer);
    return stats;
}

int GpuEncoderHantro::EncodeParamSetAndEncode(GpuEncoderBufferT &inBuffer, GpuEncoderBufferT &outBuffer)
{
    int stats;
    auto inHantroBuff = static_cast<GpuEncoderBufferHantroT>(inBuffer);
    auto outHantroBuff = static_cast<GpuEncoderBufferHantroT>(outBuffer);
    if (outHantroBuff->mapped) {
        ERR("Please unmap output buffer before encoder");
        return -ERR_INVALID_PARAM;
    }
    if (!inHantroBuff->asyncWaited && inHantroBuff->acquireFence > 0) {
        if (sync_wait(inHantroBuff->acquireFence, m_acquireWaitTime) < 0) {
            ERR("Hantro async wait buffer finish failed.");
            return -ERR_INTERNAL_ERROR;
        }
        inHantroBuff->asyncWaited = true;
    }
    HantroEncodeParams nowParams;
    if (m_dynamicAdjustParamFlag) {
        nowParams = m_settingParams;
        if (!DynamicAdjustParam(inHantroBuff, nowParams)) {    // 设置编码参数失败，应该引起编码失败
            ERR("Hantro try to dynamic adjust params failed.");
            return -ERR_INTERNAL_ERROR;
        }
        INFO("Hantro dynamic adjust params success.");
    }
    stats = UseFFmpegtoEncode(inHantroBuff, outHantroBuff);
    if (stats != OK) {
        ERR("Failed to encode data: %d", stats);
        return stats;
    }
    if (m_dynamicAdjustParamFlag) {
        m_settingParams = nowParams;
        m_dynamicAdjustParamFlag = false;
    }
    return stats;
}

bool GpuEncoderHantro::DynamicAdjustParam(GpuEncoderBufferHantroT rgbBuffer, HantroEncodeParams &setParams)
{
    if (m_receiveParams.bitRate != 0 && m_receiveParams.bitRate != setParams.bitRate) {
        AVFrameSideData *brSide = nullptr;
        AvcodecWrapper::Get().Call<AV_FRAME_REMOVE_SIDE_DATA>(rgbBuffer->frame, AV_FRAME_DATE_VASTAI_BITRATE_EXT1);
        brSide = AvcodecWrapper::Get().Call<AV_FRAME_NEW_SIDE_DATA>(rgbBuffer->frame, AV_FRAME_DATE_VASTAI_BITRATE_EXT1,
            sizeof(uint32_t) * 3);
        if (brSide == nullptr) {
            ERR("Hantro failed to dynamic adjust bitrate");
            return false;
        }
        uint32_t *data = (uint32_t*)brSide->data;
        data[0] = m_receiveParams.bitRate / 1000;    // kbps
        data[1] = 0;
        data[2] = 0;
        setParams.bitRate = m_receiveParams.bitRate;
        INFO("Hantro dynamic adjust bitrate param value: %u", m_receiveParams.bitRate);
    }

    if (m_receiveParams.frameRate != 0 && m_receiveParams.frameRate != setParams.frameRate) {
        AVFrameSideData *frSide = nullptr;
        AvcodecWrapper::Get().Call<AV_FRAME_REMOVE_SIDE_DATA>(rgbBuffer->frame, AV_FRAME_DATA_VASTAI_FRAMERATE);
        frSide = AvcodecWrapper::Get().Call<AV_FRAME_NEW_SIDE_DATA>(rgbBuffer->frame, AV_FRAME_DATA_VASTAI_FRAMERATE,
            sizeof(uint32_t));
        if (frSide == nullptr) {
            ERR("Hantro failed to dynamic adjust framerate.");
            return false;
        }
        uint32_t *data = (uint32_t*)frSide->data;
        data[0] = m_receiveParams.frameRate;
        setParams.frameRate = m_receiveParams.frameRate;
        INFO("Hantro dynamic adjust framerate param value: %u", m_receiveParams.frameRate);
    }

    if (m_receiveParams.keyFrame == 1) {
        rgbBuffer->frame->pict_type = AV_PICTURE_TYPE_I;
        INFO("Hantro dynamic request one keyFrame.");
    }
    return true;
}

int GpuEncoderHantro::UseFFmpegtoEncode(GpuEncoderBufferHantroT &inHantroBuff, GpuEncoderBufferHantroT &outHantroBuff)
{
    int err = AvcodecWrapper::Get().Call<AVCODEC_SEND_FRAME>(m_avcodec, inHantroBuff->frame);
    if (err < 0) {
        ERR("Error sending a frame for encoding.");
        return -ERR_INTERNAL_ERROR;
    }
    outHantroBuff->pkt = AvcodecWrapper::Get().Call<AV_PACKET_ALLOC>();
    if (err < 0) {
        ERR("Error alloc stream pkt");
        return -ERR_OUT_OF_MEM;
    }

    uint32_t writeOffset = 0;
    uint32_t currentStreamSize = 0;
    while (err >= 0) {
        err = AvcodecWrapper::Get().Call<AVCODEC_RECEIVE_PACKET>(m_avcodec, outHantroBuff->pkt);
        if (err == AVERROR(EAGAIN) || err == AVERROR_EOF) {
            AvcodecWrapper::Get().Call<AV_PACKET_FREE>(&outHantroBuff->pkt);
            return OK;
        } else if (err < 0) {
            ERR("Error during encoding: %s", av_err2str(err));
            AvcodecWrapper::Get().Call<AV_PACKET_FREE>(&outHantroBuff->pkt);
            return -ERR_INTERNAL_ERROR;
        }
        if (outHantroBuff->pkt->size == 0) {
            continue;
        }
        currentStreamSize = writeOffset + outHantroBuff->pkt->size;
        // Very unrare, when stream pkt size if larger than streamBuffer, do realloc
        if (currentStreamSize > outHantroBuff->streamBufferSize) {
            auto tmpPtr = std::make_unique<uint8_t[]>(currentStreamSize);
            std::copy_n(outHantroBuff->streamBuffer.get(), writeOffset, tmpPtr.get());
            outHantroBuff->streamBuffer = std::move(tmpPtr);
            outHantroBuff->streamBufferSize = currentStreamSize;
        }
        auto writePtr = outHantroBuff->streamBuffer.get() + writeOffset;
        outHantroBuff->dataLen = currentStreamSize;
        std::copy_n(outHantroBuff->pkt->data, outHantroBuff->pkt->size, writePtr);
        writeOffset = currentStreamSize;
        AvcodecWrapper::Get().Call<AV_PACKET_UNREF>(outHantroBuff->pkt);
    }
    AvcodecWrapper::Get().Call<AV_PACKET_FREE>(&outHantroBuff->pkt);
    return OK;
}

void GpuEncoderHantro::SetFrameRate(EncodeParamT &param, HantroEncodeParams &params)
{
    auto ptr = static_cast<EncodeParamFrameRate *>(param);
    params.frameRate = ptr->frameRate;
    return;
}

void GpuEncoderHantro::SetBitRate(EncodeParamT &param, HantroEncodeParams &params)
{
    auto ptr = static_cast<EncodeParamBitRate *>(param);
    params.bitRate = ptr->bitRate;
    return;
}

void GpuEncoderHantro::SetGopsize(EncodeParamT &param, HantroEncodeParams &params)
{
    auto ptr = static_cast<EncodeParamGopsize *>(param);
    params.gopSize = ptr->gopSize;
    return;
}

void GpuEncoderHantro::SetKeyFrame(HantroEncodeParams &params)
{
    params.keyFrame = 1;
    return;
}

void GpuEncoderHantro::SetProfile(EncodeParamT &param, HantroEncodeParams &params)
{
    auto ptr = static_cast<EncodeParamProfile *>(param);
    params.profile = ptr->profile;
    return;
}

uint32_t GpuEncoderHantro::ConvertProfileCodeToString(uint32_t profileCode, std::string &profile)
{
    profile = "";
    switch (profileCode) {
        case Vmi::GpuEncoder::ENC_PROFILE_IDC_BASELINE:
            profile = "bronze_quality";
            break;
        case Vmi::GpuEncoder::ENC_PROFILE_IDC_MAIN:
        case Vmi::GpuEncoder::ENC_PROFILE_IDC_HEVC_MAIN:
            profile = "silver_quality";
            break;
        case Vmi::GpuEncoder::ENC_PROFILE_IDC_HIGH:
            profile = "gold_quality";
            break;
        default:
            ERR("Profile number error, can't find legal profile, Code: %u", profileCode);
            return ERR_INVALID_PARAM;
    }
    return OK;
}

int32_t GpuEncoderHantro::SetEncodeParam(EncodeParamT params[], uint32_t num)
{
    bool isNeedRestart = false;
    HantroEncodeParams tmpParams = m_settingParams;
    for (uint32_t i = 0; i < num; i++) {
        switch (params[i]->id) {
            case ENCODE_PARAM_BITRATE:
                SetBitRate(params[i], tmpParams);
                break;
            case ENCODE_PARAM_FRAMERATE:
                SetFrameRate(params[i], tmpParams);
                break;
            case ENCODE_PARAM_KEYFRAME:
                SetKeyFrame(tmpParams);
                break;
            case ENCODE_PARAM_GOPSIZE:
                SetGopsize(params[i], tmpParams);
                isNeedRestart = true;
                break;
            case ENCODE_PARAM_PROFILE:
                SetProfile(params[i], tmpParams);
                isNeedRestart = true;
                break;
            default:
                ERR("Hantro set encoder param failed, unsupport param type");
                return ERR_INVALID_PARAM;
        }
    }
    std::lock_guard<std::mutex> lk(m_lock);
    m_receiveParams = tmpParams;
    if (m_status == Status::INVALID) {              // before init
        UpdateSettingParams();
        return OK;
    }
    if (isNeedRestart) {                            // change gopSize or profile
        m_needRestart = true;
        UpdateSettingParams();
        return ERR_NEED_RESET;
    }
    INFO("update encode params, framerate: %u, gopSize: %u, profile: %u, bitrate:%u, keyframe: %u",
        m_receiveParams.frameRate, m_receiveParams.gopSize, m_receiveParams.profile, m_receiveParams.bitRate,
        m_receiveParams.keyFrame);
    m_dynamicAdjustParamFlag = true;                // dynamic adjust encode params
    return OK;
}

void GpuEncoderHantro::UpdateSettingParams()
{
    // only reset or before init videoEncoder need to update settingParams.
    if (m_receiveParams.frameRate != 0 && m_receiveParams.frameRate != m_settingParams.frameRate) {
        m_settingParams.frameRate = m_receiveParams.frameRate;
    }
    if (m_receiveParams.bitRate != 0 && m_receiveParams.bitRate != m_settingParams.bitRate) {
        m_settingParams.bitRate = m_receiveParams.bitRate;
    }
    if (m_receiveParams.gopSize != 0 && m_receiveParams.gopSize != m_settingParams.gopSize) {
        m_settingParams.gopSize = m_receiveParams.gopSize;
    }
    if (m_receiveParams.profile != m_settingParams.profile) {
        m_settingParams.profile = m_receiveParams.profile;
    }
    return;
}

int32_t GpuEncoderHantro::Reset()
{
    std::unique_lock<std::mutex> lk(m_lock);
    if (m_status != Status::INITED) {
        ERR("Reset cannot be called in running status, call stop first");
        return -ERR_INVALID_STATUS;
    }
    lk.unlock();
    int32_t retCode = DeInit();
    if (retCode != OK) {
        ERR("Fail to deinit gpu encoder. Code: %d", retCode);
        return -ERR_INTERNAL_ERROR;
    };
    EncoderConfig config;
    config.inSize = m_size;
    config.capability = m_capability;
    retCode = Init(config);
    if (retCode != OK) {
        ERR("Fail to init gpu encoder. Code: %d", retCode);
        return -ERR_INTERNAL_ERROR;
    };
    return OK;
}
}
}