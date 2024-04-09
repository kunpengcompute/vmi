/*
 * 版权所有 (c) 华为技术有限公司 2023-2023
 * 功能描述：AMD显卡视频编码器
 */

#include "GpuEncoderAmd.h"
#include <memory>
#include "logging.h"

#ifdef __cplusplus
extern "C" {
#endif
void *CreateGpuTurbo(uint32_t type)
{
    void *ret = nullptr;
    auto gpuType = static_cast<Vmi::GpuEncoder::GpuType>(type);
    switch (gpuType) {
        case Vmi::GpuEncoder::GpuType::GPU_A_W5100:
        case Vmi::GpuEncoder::GpuType::GPU_A_W6600:
        case Vmi::GpuEncoder::GpuType::GPU_A_W6800:
            ret = std::make_unique<Vmi::GpuEncoder::GpuEncoderAmd>(gpuType).release();
            break;
        default:
            WARN("Cannot create encoder, unsupport gpu type: %u", type);
            return nullptr;
    }
    return ret;
}
#ifdef __cplusplus
}
#endif

namespace {
constexpr uint32_t MAX_WIDTH = 4096;
constexpr uint32_t MAX_HEIGHT = 4096;
constexpr uint32_t WIDTH_ALIGN = 64;
constexpr uint32_t HEIGHT_ALIGN = 64;
}

namespace Vmi {
namespace GpuEncoder {

GpuEncoderAmd::GpuEncoderAmd(GpuType type)
{
    m_gpuType = type;
}

int32_t GpuEncoderAmd::Init(EncoderConfig &config)
{
    std::lock_guard<std::mutex> lockGuard(m_engineLock);
    if (m_engineStat != ENCODER_ENGINE_STATE_INVALID) {
        ERR("Failed to init gpu, unexpect engine state: %u", m_engineStat.load());
        return ERR_UNEXPECT_STATUS;
    }
    if (config.inSize.width > MAX_WIDTH || config.inSize.height > MAX_HEIGHT ||
        config.inSize.width == 0 || config.inSize.height == 0) {
        ERR("Input frame size illegal, width=%u, height=%u", config.inSize.width, config.inSize.height);
        return ERR_INVALID_PARAM;
    }
    if (config.outSize.width > MAX_WIDTH || config.outSize.height > MAX_HEIGHT ||
        config.outSize.width == 0 || config.outSize.height == 0) {
        ERR("output frame size illegal, width=%u, height=%u", config.outSize.width, config.outSize.height);
        return ERR_INVALID_PARAM;
    }
    if (config.deviceNode < DEV_DRI_RENDERD128 || config.deviceNode >= DEV_DRI_OTHER) {
        ERR("Unsupport device node: %u", config.deviceNode);
        return ERR_INVALID_DEVICE;
    }
    if (!RecordOutFrameFormat(config.capability)) {
        return ERR_UNSUPPORT_FORMAT;
    }

    config.inSize.widthAligned = AlignUp(config.inSize.width, WIDTH_ALIGN);
    config.inSize.heightAligned = AlignUp(config.inSize.height, HEIGHT_ALIGN);
    m_vaEncoderAmd.ContextInit(config);
    EncTurboCode encStatus = m_vaEncoderAmd.VaDpyInit();
    if (encStatus != ENC_TURBO_SUCCESS) {
        ERR("Gpu encoder amd init failed: %u", encStatus);
        m_vaEncoderAmd.ContextDestroy();
        m_outFormat = FRAME_FORMAT_NONE;
        return ERR_INTERNAL_ERROR;
    }
    m_inSize = config.inSize;
    m_outSize = config.outSize;
    m_engineStat = ENCODER_ENGINE_STATE_INIT;
    return OK;
}

int32_t GpuEncoderAmd::DeInit()
{
    std::lock_guard<std::mutex> lockGuard(m_engineLock);
    if (m_engineStat == ENCODER_ENGINE_STATE_INVALID) {
        ERR("Unexpect engine state: %u", m_engineStat.load());
        return ERR_UNEXPECT_STATUS;
    }
    int32_t status = OK;
    if (m_engineStat != ENCODER_ENGINE_STATE_INIT) {
        // 销毁记录中所有的buffer
        if (ReleaseAllBuffers() != OK) {
            status = ERR_INTERNAL_ERROR;
        }
        // 销毁VPP
        if (m_vaEncoderAmd.VppClose() != ENC_TURBO_SUCCESS) {
            status = ERR_INTERNAL_ERROR;
        }
        // 销毁enc
        if (m_vaEncoderAmd.EncodeClose() != ENC_TURBO_SUCCESS) {
            status = ERR_INTERNAL_ERROR;
        }
    }

    if (m_vaEncoderAmd.EncodeDestroy() != ENC_TURBO_SUCCESS) {
        status = ERR_INTERNAL_ERROR;
    }
    m_vaEncoderAmd.ContextDestroy();
    m_inSize = {0, 0, 0, 0};
    m_outSize = {0, 0, 0, 0};
    m_outFormat = FRAME_FORMAT_NONE;
    m_engineStat = ENCODER_ENGINE_STATE_INVALID;
    m_buffers = {};
    m_gpuType = GPU_NONE;
    m_rateControl = 0;
    m_bitRate = 0;
    m_frameRate = 0;
    m_gopSize = 0;
    m_profileIdc = 0;
    return status;
}

int32_t GpuEncoderAmd::ReleaseAllBuffers()
{
    int32_t status = OK;
    int32_t errorCode = OK;
    for (auto it : m_buffers) {
        switch (it->format) {
            case FRAME_FORMAT_RGBA:
                errorCode = ReleaseRgbBuffer(it);
                break;
            case FRAME_FORMAT_YUV:
                errorCode = ReleaseYuvBuffer(it);
                break;
            case FRAME_FORMAT_H264:
            case FRAME_FORMAT_HEVC:
                errorCode = ReleaseCodedBuffer(it);
                break;
            default:
                // CreateBuffer与ImportBuffer已保证不会出现除上述4中格式之外的buffer
                ERR("Unsupport frame format: %u", it->format);
                break;
        }
        if (errorCode != OK) {
            ERR("ReleaseAllBuffers release frame format:%u failed: %u", it->format, errorCode);
            status = errorCode;
        }
    }
    return status;
}

int32_t GpuEncoderAmd::Start()
{
    std::lock_guard<std::mutex> lockGuard(m_engineLock);
    if (m_engineStat == ENCODER_ENGINE_STATE_STOP) {
        m_engineStat = ENCODER_ENGINE_STATE_RUNNING;
        return OK;
    }
    if (m_engineStat != ENCODER_ENGINE_STATE_INIT) {
        ERR("Unexpect engine state: %u", m_engineStat.load());
        return ERR_UNEXPECT_STATUS;
    }

    EncTurboCode encStatus = m_vaEncoderAmd.EncodeOpen();
    if (encStatus != ENC_TURBO_SUCCESS) {
        ERR("Gpu encoder amd start encodeOpen failed: %u", encStatus);
        return ERR_INTERNAL_ERROR;
    }

    encStatus = m_vaEncoderAmd.VppOpen();
    if (encStatus != ENC_TURBO_SUCCESS) {
        ERR("Gpu encoder amd start vppOpen failed: %u", encStatus);
        encStatus = m_vaEncoderAmd.EncodeClose();
        if (encStatus != ENC_TURBO_SUCCESS) {
            ERR("Gpu encoder amd start EncodeClose failed: %u", encStatus);
        }
        return ERR_INTERNAL_ERROR;
    }
    m_engineStat = ENCODER_ENGINE_STATE_RUNNING;

    return OK;
}

int32_t GpuEncoderAmd::Stop()
{
    std::lock_guard<std::mutex> lockGuard(m_engineLock);
    if (m_engineStat != ENCODER_ENGINE_STATE_RUNNING) {
        ERR("Unexpect engine state: %u", m_engineStat.load());
        return ERR_UNEXPECT_STATUS;
    }
    m_engineStat = ENCODER_ENGINE_STATE_STOP;
    return OK;
}

int32_t GpuEncoderAmd::CreateBuffer(FrameFormat format, MemType memType, GpuEncoderBufferT &buffer)
{
    std::lock_guard<std::mutex> lockGuard(m_engineLock);
    if (m_engineStat != ENCODER_ENGINE_STATE_RUNNING) {
        ERR("Unexpect engine state: %u", m_engineStat.load());
        return ERR_UNEXPECT_STATUS;
    }
    if (memType != MEM_TYPE_DEVICE) {
        ERR("Unsupport mem type: %u", memType);
        return ERR_INVALID_PARAM;
    }

    int32_t status = ERR_INVALID_PARAM;
    switch (format) {
        case FRAME_FORMAT_YUV:
            status = CreateYuvBuffer(format, buffer);
            break;
        case FRAME_FORMAT_H264:
        case FRAME_FORMAT_HEVC:
            status = CreatecodedBuffer(format, buffer);
            break;
        default:
            ERR("Unsupport frame format: %u", format);
            break;
    }

    return status;
}

int32_t GpuEncoderAmd::CreatecodedBuffer(FrameFormat format, GpuEncoderBufferT &buffer)
{
    auto newBuffer = std::make_unique<GpuStreamBufferAmd>();
    newBuffer->format = format;
    newBuffer->memType = MEM_TYPE_DEVICE;
    newBuffer->size = m_outSize;
    newBuffer->gpuType = m_gpuType;
    EncTurboCode encStatus = m_vaEncoderAmd.CreateCodedBuffer(newBuffer->codedbufBufId);
    if (encStatus != ENC_TURBO_SUCCESS) {
        ERR("Create stream buffer CreateCodedBuffer failed: %u", encStatus);
        return ERR_INTERNAL_ERROR;
    }

    auto buffPtr = newBuffer.release();
    m_buffers.insert(buffPtr);
    buffer = buffPtr;
    return OK;
}

int32_t GpuEncoderAmd::CreateYuvBuffer(FrameFormat format, GpuEncoderBufferT &buffer)
{
    auto newBuffer = std::make_unique<GpuYuvBufferAmd>();
    newBuffer->format = format;
    newBuffer->memType = MEM_TYPE_DEVICE;
    newBuffer->size = m_outSize;
    newBuffer->gpuType = m_gpuType;
    EncTurboCode encStatus = m_vaEncoderAmd.CreateYuvSurface(newBuffer->vaYuvSurfaceId);
    if (encStatus != ENC_TURBO_SUCCESS) {
        ERR("Create yuv buffer CreateYuvSurface failed: %u", encStatus);
        return ERR_INTERNAL_ERROR;
    }
    encStatus = m_vaEncoderAmd.CreateYuvImage(newBuffer->vaOutImage);
    if (encStatus != ENC_TURBO_SUCCESS) {
        ERR("Create yuv buffer CreateYuvImage failed: %u", encStatus);
        encStatus = m_vaEncoderAmd.ReleaseYuvSurface(newBuffer->vaYuvSurfaceId);
        if (encStatus != ENC_TURBO_SUCCESS) {
            ERR("Create yuv buffer ReleaseYuvSurface failed: %u", encStatus);
        }
        return ERR_INTERNAL_ERROR;
    }
    auto buffPtr = newBuffer.release();
    m_buffers.insert(buffPtr);
    buffer = buffPtr;
    return OK;
}

int32_t GpuEncoderAmd::ImportBuffer(FrameFormat format, uint64_t handle, GpuEncoderBufferT &buffer)
{
    std::lock_guard<std::mutex> lockGuard(m_engineLock);
    if (format != FRAME_FORMAT_RGBA) {
        ERR("Import buffer failed, frame format: %u", format);
        return ERR_INVALID_PARAM;
    }
    if (handle == 0) {
        ERR("Import buffer failed, rgb handle is null");
        return ERR_INVALID_PARAM;
    }
    if (m_engineStat != ENCODER_ENGINE_STATE_RUNNING) {
        ERR("Unexpect engine state: %u", m_engineStat.load());
        return ERR_UNEXPECT_STATUS;
    }

    auto newBuffer = std::make_unique<GpuRgbBufferAmd>();
    newBuffer->format = format;
    newBuffer->memType = MEM_TYPE_DEVICE;
    newBuffer->size = m_inSize;
    newBuffer->gpuType = m_gpuType;
    newBuffer->external = true;
    EncTurboCode encStatus = m_vaEncoderAmd.ImportRgbSurface(handle, newBuffer->rgbSurfaceId);
    if (encStatus != ENC_TURBO_SUCCESS) {
        ERR("Import buffer importRgbSurface failed: %u", encStatus);
        return ERR_INTERNAL_ERROR;
    }

    auto buffPtr = newBuffer.release();
    m_buffers.insert(buffPtr);
    buffer = buffPtr;
    return OK;
}

int32_t GpuEncoderAmd::ReleaseBuffer(GpuEncoderBufferT &buffer)
{
    std::lock_guard<std::mutex> lockGuard(m_engineLock);
    if (buffer == nullptr) {
        ERR("Release buffer failed, input buffer illegal");
        return ERR_INVALID_PARAM;
    }
    if (m_engineStat != ENCODER_ENGINE_STATE_RUNNING) {
        ERR("Unexpect engine state: %u", m_engineStat.load());
        return ERR_UNEXPECT_STATUS;
    }
    // 检查buffer是否合法
    if (m_buffers.find(buffer) == m_buffers.end()) {
        ERR("Release buffer failed, input buffer illegal.");
        return ERR_INVALID_PARAM;
    }

    int32_t status = ERR_INVALID_PARAM;
    switch (buffer->format) {
        case FRAME_FORMAT_RGBA:
            status = ReleaseRgbBuffer(buffer);
            break;
        case FRAME_FORMAT_YUV:
            status = ReleaseYuvBuffer(buffer);
            break;
        case FRAME_FORMAT_H264:
        case FRAME_FORMAT_HEVC:
            status = ReleaseCodedBuffer(buffer);
            break;
        default:
            ERR("Release buffer failed, unsupport frame formt: %u", buffer->format);
            break;
    }

    return status;
}

int32_t GpuEncoderAmd::ReleaseRgbBuffer(GpuEncoderBufferT &buffer)
{
    auto rgbBuffer = static_cast<GpuRgbBufferAmdT>(buffer);
    if (rgbBuffer->mapped) {
        ERR("Release rgb buffer failed, buffer must be unmapped before release");
        return ERR_INVALID_PARAM;
    }

    EncTurboCode encStatus = m_vaEncoderAmd.ReleaseRgbSurface(rgbBuffer->rgbSurfaceId);
    if (encStatus != ENC_TURBO_SUCCESS) {
        ERR("Release rgb buffer failed: %u", encStatus);
        return ERR_INTERNAL_ERROR;
    }
    m_buffers.erase(buffer);
    buffer = nullptr;
    delete rgbBuffer;
    return OK;
}

int32_t GpuEncoderAmd::ReleaseYuvBuffer(GpuEncoderBufferT &buffer)
{
    auto yuvBuffer = static_cast<GpuYuvBufferAmdT>(buffer);
    if (yuvBuffer->mapped) {
        ERR("Release yuv buffer failed, buffer must be unmapped before release");
        return ERR_INVALID_PARAM;
    }

    int32_t status = OK;
    EncTurboCode encStatus = m_vaEncoderAmd.ReleaseYuvSurface(yuvBuffer->vaYuvSurfaceId);
    if (encStatus != ENC_TURBO_SUCCESS) {
        ERR("Release yuv buffer ReleaseYuvSurface failed: %u", encStatus);
        status = ERR_INTERNAL_ERROR;
    }
    encStatus = m_vaEncoderAmd.ReleaseYuvImage(yuvBuffer->vaOutImage);
    if (encStatus != ENC_TURBO_SUCCESS) {
        ERR("Release yuv buffer ReleaseYuvImage failed: %u", encStatus);
        status = ERR_INTERNAL_ERROR;
    }

    m_buffers.erase(buffer);
    buffer = nullptr;
    delete yuvBuffer;
    return status;
}

int32_t GpuEncoderAmd::ReleaseCodedBuffer(GpuEncoderBufferT &buffer)
{
    auto streamBuffer = static_cast<GpuStreamBufferAmdT>(buffer);
    if (streamBuffer->mapped) {
        ERR("Release Coded buffer failed, buffer must be unmapped before release");
        return ERR_INVALID_PARAM;
    }
    EncTurboCode encStatus = m_vaEncoderAmd.ReleaseCodedBuffer(streamBuffer->codedbufBufId);
    if (encStatus != ENC_TURBO_SUCCESS) {
        ERR("Release coded buffer va ReleaseCodedBuffer failed: %u", encStatus);
        return ERR_INTERNAL_ERROR;
    }

    m_buffers.erase(buffer);
    buffer = nullptr;
    delete streamBuffer;
    return OK;
}

int32_t GpuEncoderAmd::MapBuffer(GpuEncoderBufferT &buffer, uint32_t flag)
{
    std::lock_guard<std::mutex> lockGuard(m_engineLock);
    if (buffer == nullptr) {
        ERR("Map buffer failed, input buffer is null");
        return ERR_INVALID_PARAM;
    }
    if (buffer->memType != MEM_TYPE_DEVICE) {
        ERR("Unexpect buffer type: %u", buffer->memType);
        return ERR_INVALID_PARAM;
    }
    if (flag != FLAG_READ) {
        ERR("Amd map buffer only read: %u", flag);
        return ERR_INVALID_PARAM;
    }
    if (m_engineStat != ENCODER_ENGINE_STATE_RUNNING) {
        ERR("Unexpect engine state: %u", m_engineStat.load());
        return ERR_UNEXPECT_STATUS;
    }
    if (m_buffers.find(buffer) == m_buffers.end()) {
        ERR("Map buffer failed, input buffer illegal");
        return ERR_INVALID_PARAM;
    }

    int32_t status = ERR_INVALID_PARAM;
    switch (buffer->format) {
        case FRAME_FORMAT_YUV:
            status = MapYuvBuffer(buffer);
            break;
        case FRAME_FORMAT_H264:
        case FRAME_FORMAT_HEVC:
            status = MapCodedBuffer(buffer);
            break;
        default:
            ERR("Map buffer failed, unsupport frame format");
            break;
    }

    return status;
}

int32_t GpuEncoderAmd::MapYuvBuffer(GpuEncoderBufferT &buffer)
{
    auto yuvBuffer = static_cast<GpuYuvBufferAmdT>(buffer);
    if (yuvBuffer->mapped) {
        ERR("Map yuv buffer failed, buffer already mapped");
        return ERR_INVALID_PARAM;
    }
    // check convert process
    EncTurboCode encStatus = m_vaEncoderAmd.CheckConvertStatus(yuvBuffer->vaYuvSurfaceId);
    if (encStatus != ENC_TURBO_SUCCESS) {
        ERR("Rgb convert yuv is running, please wait.");
        return ERR_INTERNAL_ERROR;
    }
    // copy yuv to Image
    encStatus = m_vaEncoderAmd.CopyYuvSurfaceToImage(yuvBuffer->vaYuvSurfaceId, yuvBuffer->vaOutImage);
    if (encStatus != ENC_TURBO_SUCCESS) {
        ERR("Map yuv buffer CopyYuvSurfaceToImage failed: %u", encStatus);
        return ERR_INTERNAL_ERROR;
    }
    // Image map
    ConvertDataOutput dataOutput;
    encStatus = m_vaEncoderAmd.MapImage(yuvBuffer->vaOutImage, dataOutput);
    if (encStatus != ENC_TURBO_SUCCESS) {
        ERR("Map yub buffer MapImage failed: %u", encStatus);
        return ERR_INTERNAL_ERROR;
    }
    yuvBuffer->data = dataOutput.data;
    yuvBuffer->dataLen = dataOutput.dataLen;
    yuvBuffer->mapped = true;
    return OK;
}

int32_t GpuEncoderAmd::MapCodedBuffer(GpuEncoderBufferT &buffer)
{
    auto codeBuf = static_cast<GpuStreamBufferAmdT>(buffer);
    if (codeBuf->mapped) {
        ERR("Map coded buufer failed, buffer already mapped");
        return ERR_INVALID_PARAM;
    }

    StreamDataOutput data;
    EncTurboCode encStatus = m_vaEncoderAmd.MapStreamBuffer(codeBuf->codedbufBufId, data);
    if (encStatus != ENC_TURBO_SUCCESS) {
        ERR("Map coded buffer va MapStreamBuffer failed: %u", encStatus);
        return ERR_INTERNAL_ERROR;
    }

    codeBuf->data = data.handle;
    codeBuf->codedBufferSegment = data.codedBufferSegment;
    codeBuf->dataLen = data.dataLen;
    codeBuf->mapped = true;
    return OK;
}

int32_t GpuEncoderAmd::UnmapBuffer(GpuEncoderBufferT &buffer)
{
    std::lock_guard<std::mutex> lockGuard(m_engineLock);
    if (buffer == nullptr) {
        ERR("Unmap buffer failed, input buffer is null");
        return ERR_INVALID_PARAM;
    }
    if (buffer->memType != MEM_TYPE_DEVICE) {
        ERR("Unmap buffer failed, unsupport memType: %u", buffer->memType);
        return ERR_INVALID_PARAM;
    }
    if (m_engineStat != ENCODER_ENGINE_STATE_RUNNING) {
        ERR("Unexpect engine state: %u", m_engineStat.load());
        return ERR_UNEXPECT_STATUS;
    }

    int32_t status = ERR_INVALID_PARAM;
    switch (buffer->format) {
        case FRAME_FORMAT_YUV:
            status = UnmapYuvBuffer(buffer);
            break;
        case FRAME_FORMAT_H264:
        case FRAME_FORMAT_HEVC:
            status = UnmapCodedBuffer(buffer);
            break;
        default:
            ERR("Unmap buffer failed, unsupport frame format");
            break;
    }
    return status;
}

int32_t GpuEncoderAmd::UnmapYuvBuffer(GpuEncoderBufferT &buffer)
{
    auto yuvBuffer = static_cast<GpuYuvBufferAmdT>(buffer);
    if (!yuvBuffer->mapped) {
        ERR("Unmap yuv buffer failed, buffer must be mapped before unmap");
        return ERR_INVALID_PARAM;
    }

    EncTurboCode encStatus = m_vaEncoderAmd.UnmapImage(yuvBuffer->vaOutImage);
    if (encStatus != ENC_TURBO_SUCCESS) {
        ERR("Unmap yuv buffer UnmapImage failed: %u", encStatus);
        return ERR_INTERNAL_ERROR;
    }

    yuvBuffer->mapped = false;
    yuvBuffer->data = nullptr;
    yuvBuffer->dataLen = 0;
    return OK;
}

int32_t GpuEncoderAmd::UnmapCodedBuffer(GpuEncoderBufferT &buffer)
{
    auto codedBuffer = static_cast<GpuStreamBufferAmdT>(buffer);
    if (!codedBuffer->mapped) {
        ERR("Unmap coded buffer failed, buffer must be mapped before unmap");
        return ERR_INVALID_PARAM;
    }

    EncTurboCode encStatus = m_vaEncoderAmd.UnmapStreamBuffer(codedBuffer->codedbufBufId);
    if (encStatus != ENC_TURBO_SUCCESS) {
        ERR("Unmap coded buffer va UnmapStreamBuffer failed: %u", encStatus);
        return ERR_INTERNAL_ERROR;
    }

    codedBuffer->mapped = false;
    codedBuffer->data = nullptr;
    codedBuffer->dataLen = 0;
    return OK;
}

int32_t GpuEncoderAmd::RetriveBufferData(GpuEncoderBufferT &buffer, uint8_t* handle, uint32_t memLen, uint32_t &dataLen)
{
    std::lock_guard<std::mutex> lockGuard(m_engineLock);
    if (buffer == nullptr || handle == nullptr) {
        ERR("Retrive buffer data failed, input buffer/handle is null");
        return ERR_INVALID_PARAM;
    }
    if (buffer->format != FRAME_FORMAT_H264 && buffer->format != FRAME_FORMAT_HEVC) {
        ERR("Retrive buffer data failed, unexpect buffer format: %u", buffer->format);
        return ERR_INVALID_PARAM;
    }
    if (m_engineStat != ENCODER_ENGINE_STATE_RUNNING) {
        ERR("Retrive buffer data failed, unexpect engine status: %u", m_engineStat.load());
        return ERR_UNEXPECT_STATUS;
    }
    if (m_buffers.find(buffer) == m_buffers.end()) {
        ERR("Retrive buffer data failed, input buffer illegal.");
        return ERR_INVALID_PARAM;
    }
    auto codedBuf = static_cast<GpuStreamBufferAmdT>(buffer);
    if (!codedBuf->mapped || codedBuf->dataLen > memLen) {
        ERR("Received buffer size less than dataLen or buffer must be mapped before retrive operation");
        return ERR_INVALID_PARAM;
    }
    m_vaEncoderAmd.RetriveBufferData(codedBuf->codedBufferSegment, handle, dataLen);
    return OK;
}

int32_t GpuEncoderAmd::Convert(GpuEncoderBufferT &inBuffer, GpuEncoderBufferT &outBuffer)
{
    std::lock_guard<std::mutex> lockGuard(m_engineLock);
    if (inBuffer == nullptr || outBuffer == nullptr) {
        ERR("Convert failed, inbuffer or outBuffer is null");
        return ERR_INVALID_PARAM;
    }
    if (m_engineStat != ENCODER_ENGINE_STATE_RUNNING) {
        ERR("Unexpect engine state: %u", m_engineStat.load());
        return ERR_UNEXPECT_STATUS;
    }
    if (m_buffers.find(inBuffer) == m_buffers.end() || inBuffer->memType != MEM_TYPE_DEVICE ||
        inBuffer->format != FRAME_FORMAT_RGBA) {
        ERR("Convert failed, inBuf illegal or unsupport memType:%u, format: %u", inBuffer->memType, inBuffer->format);
        return ERR_INVALID_PARAM;
    }
    if (m_buffers.find(outBuffer) == m_buffers.end() || outBuffer->memType != MEM_TYPE_DEVICE ||
        outBuffer->format != FRAME_FORMAT_YUV) {
        ERR("Conver failed, outBuf memType:%u, format:%u", outBuffer->memType, outBuffer->format);
        return ERR_INVALID_PARAM;
    }

    auto rgbBuffer = static_cast<GpuRgbBufferAmdT>(inBuffer);
    auto yuvBuffer = static_cast<GpuYuvBufferAmdT>(outBuffer);
    EncTurboCode encStatus = m_vaEncoderAmd.RgbConvertToYuv(rgbBuffer->rgbSurfaceId, yuvBuffer->vaYuvSurfaceId);
    if (encStatus != ENC_TURBO_SUCCESS) {
        ERR("Convert RgbConvertToYuv failed: %u", encStatus);
        return ERR_INTERNAL_ERROR;
    }
    return OK;
}

int32_t GpuEncoderAmd::Encode(GpuEncoderBufferT &inBuffer, GpuEncoderBufferT &outBuffer)
{
    std::lock_guard<std::mutex> lockGuard(m_engineLock);
    if (inBuffer == nullptr || outBuffer == nullptr) {
        ERR("Encode failed, inbuffer ot outbuffer is null");
        return ERR_INVALID_PARAM;
    }
    if (m_engineStat != ENCODER_ENGINE_STATE_RUNNING) {
        ERR("Unexpect engine state: %u", m_engineStat.load());
        return ERR_UNEXPECT_STATUS;
    }
    if (m_outFormat == FRAME_FORMAT_YUV) {
        ERR("Output frame format is yuv, unsupport encode operation");
        return ERR_UNSUPPORT_OPERATION;
    }
    if (m_buffers.find(inBuffer) == m_buffers.end() || inBuffer->memType != MEM_TYPE_DEVICE ||
        inBuffer->format != FRAME_FORMAT_YUV) {
        ERR("Encode failed, inBuf memType:%u, format:%u", inBuffer->memType, inBuffer->format);
        return ERR_INVALID_PARAM;
    }
    if (m_buffers.find(outBuffer) == m_buffers.end() || outBuffer->memType != MEM_TYPE_DEVICE ||
        m_outFormat != outBuffer->format) {
        ERR("Encode failed, outbuf is illegal or unsupport memType: %u, format:%u", outBuffer->memType,
            outBuffer->format);
        return ERR_INVALID_PARAM;
    }

    auto yuvBuffer = static_cast<GpuYuvBufferAmdT>(inBuffer);
    auto codedBuf = static_cast<GpuStreamBufferAmdT>(outBuffer);
    EncTurboCode encStatus = m_vaEncoderAmd.Encode(yuvBuffer->vaYuvSurfaceId, codedBuf->codedbufBufId);
    if (encStatus != ENC_TURBO_SUCCESS) {
        ERR("Encode yuv va-Encode failed:%u", encStatus);
        return ERR_INTERNAL_ERROR;
    }
    return OK;
}

bool GpuEncoderAmd::RecordOutFrameFormat(EncoderCapability capability)
{
    switch (capability) {
        case CAP_FORMAT_CONVERT:
            m_outFormat = FRAME_FORMAT_YUV;
            break;
        case CAP_VA_ENCODE_H264:
            m_outFormat = FRAME_FORMAT_H264;
            break;
        case CAP_VA_ENCODE_HEVC:
            m_outFormat = FRAME_FORMAT_HEVC;
            break;
        default:
            ERR("Unsupport output frame format.");
            return false;
    }
    return true;
}

int32_t GpuEncoderAmd::SetEncodeParam(EncodeParamT params[], uint32_t num)
{
    bool needRestart = false;
    int32_t status = OK;
    for (uint32_t i = 0; i < num; ++i) {
        switch (params[i]->id) {
            case ENCODE_PARAM_RATE_CONTROL: {
                status = SetRateControl(params[i]);
                break;
            }
            case ENCODE_PARAM_BITRATE: {
                status = SetBitRate(params[i], needRestart);
                break;
            }
            case ENCODE_PARAM_FRAMERATE: {
                status = SetFrameRate(params[i]);
                break;
            }
            case ENCODE_PARAM_GOPSIZE: {
                status = SetGopsize(params[i]);
                break;
            }
            case ENCODE_PARAM_PROFILE: {
                status = SetProfileIdc(params[i], needRestart);
                break;
            }
            case ENCODE_PARAM_KEYFRAME: {
                auto keyFramePtr = static_cast<EncodeParamKeyframe *>(params[i]);
                m_vaEncoderAmd.SetKeyFrame(keyFramePtr->n);
                break;
            }
            default:
                ERR("Set encoder param failed, unsupport param type");
                return ERR_INVALID_PARAM;
        }
        if (status != OK) {
            return status;
        }
    }

    if (m_engineStat == ENCODER_ENGINE_STATE_RUNNING ||
        m_engineStat == ENCODER_ENGINE_STATE_STOP) {
        m_vaEncoderAmd.SetNewValue();
        if (needRestart) {
            return ERR_NEED_RESET;
        }
    }
    return status;
}

int32_t GpuEncoderAmd::SetRateControl(EncodeParamT &param)
{
    auto ratectlPtr = static_cast<EncodeParamRateControl *>(param);
    if (m_vaEncoderAmd.SetRateControl(ratectlPtr->rateControl) == ENC_TURBO_SUCCESS) {
        m_rateControl = ratectlPtr->rateControl;
        return OK;
    }
    return ERR_INVALID_PARAM;
}

int32_t GpuEncoderAmd::SetBitRate(EncodeParamT &param, bool &needRestart)
{
    auto bitRatePtr = static_cast<EncodeParamBitRate *>(param);
    if (bitRatePtr->bitRate == 0) {
        ERR("SetBitRate failed, bitRate:%u", bitRatePtr->bitRate);
        return ERR_INVALID_PARAM;
    }
    m_vaEncoderAmd.SetBitRate(bitRatePtr->bitRate);
    // 不为0，表示已经被设定过
    if (m_bitRate != 0) {
        needRestart = needRestart || (m_bitRate != bitRatePtr->bitRate);
    }
    m_bitRate = bitRatePtr->bitRate;
    return OK;
}

int32_t GpuEncoderAmd::SetFrameRate(EncodeParamT &param)
{
    auto frameRatePtr = static_cast<EncodeParamFrameRate *>(param);
    if (frameRatePtr->frameRate == 0) {
        ERR("SetFrameRate failed, frameRate: %u", frameRatePtr->frameRate);
        return ERR_INVALID_PARAM;
    }
    m_vaEncoderAmd.SetFrameRate(frameRatePtr->frameRate);
    m_frameRate = frameRatePtr->frameRate;
    return OK;
}

int32_t GpuEncoderAmd::SetGopsize(EncodeParamT &param)
{
    auto gopSizePtr = static_cast<EncodeParamGopsize *>(param);
    if (gopSizePtr->gopSize == 0) {
        ERR("SetGopSize failed, gopSize:%u", gopSizePtr->gopSize);
        return ERR_INVALID_PARAM;
    }
    m_vaEncoderAmd.SetGopSize(gopSizePtr->gopSize);
    m_gopSize = gopSizePtr->gopSize;
    return OK;
}

int32_t GpuEncoderAmd::SetProfileIdc(EncodeParamT &param, bool &needRestart)
{
    auto profilePtr = static_cast<EncodeParamProfile *>(param);
    if (m_vaEncoderAmd.SetProfileIdc(profilePtr->profile) != ENC_TURBO_SUCCESS) {
        return ERR_INVALID_PARAM;
    }
    if (m_profileIdc != 0) {
        needRestart = needRestart || (m_profileIdc != profilePtr->profile);
    }
    m_profileIdc = profilePtr->profile;
    return OK;
}

uint32_t GpuEncoderAmd::AlignUp(uint32_t val, uint32_t align) const
{
    return (val + (align - 1)) & ~(align - 1);
}

int32_t GpuEncoderAmd::Reset()
{
    std::lock_guard<std::mutex> lockGuard(m_engineLock);
    if (!CheckAndLockStatusForReset()) {
        return ERR_INVALID_STATUS;
    }
    int32_t status = OK;
    EncTurboCode encStatus = ENC_TURBO_SUCCESS;
    // 销毁
    encStatus = m_vaEncoderAmd.EncodeClose();
    if (encStatus != ENC_TURBO_SUCCESS) {
        ERR("Reset EncodeClose failed: %u", encStatus);
        status = ERR_INTERNAL_ERROR;
    }
    // 重新构建编码
    encStatus = m_vaEncoderAmd.EncodeOpen();
    if (encStatus != ENC_TURBO_SUCCESS) {
        ERR("Reset EncodeOpen failed: %u", encStatus);
        status = ERR_INTERNAL_ERROR;
    }
    // 顺利则改变状态机
    if (status == OK) {
        m_engineStat = ENCODER_ENGINE_STATE_RUNNING;
        return status;
    }
    // 销毁编码
    DeInit();
    return status;
}

bool GpuEncoderAmd::CheckAndLockStatusForReset()
{
    if (m_engineStat != ENCODER_ENGINE_STATE_RUNNING ||
        m_engineStat != ENCODER_ENGINE_STATE_STOP) {
            ERR("Unexpect engine state: %u", m_engineStat.load());
            return false;
    }
    m_engineStat = ENCODER_ENGINE_STATE_RESETTING;
    return true;
}
}
} // Vmi
