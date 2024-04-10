/*
 * 版权所有 (c) 华为技术有限公司 2023-2023
 * 功能描述：CPU视频流编码器
 */

#include <memory>
#include "logging.h"
#include "Rgb2Yuv/VmiRgb2Yuv.h"
#include "CpuEncoder.h"

namespace {
constexpr uint32_t MAX_WIDTH = 4096;
constexpr uint32_t MAX_HEIGHT = 4096;
constexpr uint32_t WIDTH_ALIGN = 64;
constexpr uint32_t HEIGHT_ALIGN = 64;
}

namespace Vmi {
namespace GpuEncoder {

CpuEncoder::CpuEncoder(GpuType type)
{
    m_gpuType = type;
}

int32_t CpuEncoder::Init(EncoderConfig &config)
{
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
    config.inSize.widthAligned = AlignUp(config.inSize.width, WIDTH_ALIGN);
    config.inSize.heightAligned = AlignUp(config.inSize.height, HEIGHT_ALIGN);
    m_inSize = config.inSize;
    m_outSize = config.outSize;
    return OK;
}

int32_t CpuEncoder::DeInit()
{
    int32_t status = ReleaseAllBuffers();
    if (status != OK) {
        return status;
    }
    m_inSize = {0, 0, 0, 0};
    m_outSize = {0, 0, 0, 0};
    m_outFormat = FRAME_FORMAT_NONE;
    m_buffers = {};
    m_gpuType = GPU_NONE;
    return status;
}

int32_t CpuEncoder::ReleaseAllBuffers()
{
    int32_t status = OK;
    int32_t errorCode = OK;
    for (auto it : m_buffers) {
        errorCode = ReleaseBuffer(it);
        if (errorCode != OK) {
            ERR("ReleaseAllBuffers release frame format:%u failed: %u", it->format, errorCode);
            status = errorCode;
        }
    }
    return status;
}

int32_t CpuEncoder::Start()
{
    return OK;
}

int32_t CpuEncoder::Stop()
{
    return OK;
}

int32_t CpuEncoder::CreateBuffer(FrameFormat format, MemType type, GpuEncoderBufferT &buffer)
{
    (void)type;
    if (format != FRAME_FORMAT_YUV) {
        ERR("Failed to create buffer, unsupport mem type: %u", format);
        return ERR_INVALID_PARAM;
    }
    auto newBuffer = std::make_unique<GpuEncoderBuffer>();
    newBuffer->format = format;
    newBuffer->memType = MEM_TYPE_HOST;
    newBuffer->size = m_outSize;
    uint32_t pixelsNum = m_outSize.width * m_outSize.height;
    auto uvSize = (pixelsNum + 3) >> 2; // 3: (2^2 - 1)用于除4的对齐, 2：右移2用于除4，uv的数据大小为y的1/4
    newBuffer->dataLen = pixelsNum + uvSize + uvSize;
    newBuffer->data = std::make_unique<uint8_t[]>(newBuffer->dataLen).release();
    newBuffer->gpuType = m_gpuType;
    auto buffPtr = newBuffer.release();
    m_buffers.insert(buffPtr);
    buffer = buffPtr;
    return OK;
}

int32_t CpuEncoder::ImportBuffer(FrameFormat format, uint64_t handle, GpuEncoderBufferT &buffer)
{
    if (format != FRAME_FORMAT_RGBA) {
        ERR("Unsupport mem type: %u", format);
        return ERR_INVALID_PARAM;
    }
    if (handle == 0) {
        ERR("Import buffer failed, rgb handle is null");
        return ERR_INVALID_PARAM;
    }
    auto newbuffer = std::make_unique<GpuEncoderBuffer>();
    newbuffer->format = format;
    newbuffer->gpuType = m_gpuType;
    newbuffer->data = reinterpret_cast<uint8_t*>(handle);
    newbuffer->size = m_inSize;
    newbuffer->memType = MEM_TYPE_DEVICE;
    auto buffPtr = newbuffer.release();
    m_buffers.insert(buffPtr);
    buffer = buffPtr;
    return OK;
}

int32_t CpuEncoder::ReleaseBuffer(GpuEncoderBufferT &buffer)
{
    if (buffer == nullptr) {
        ERR("Release buffer failed, input buffer illegal");
        return ERR_INVALID_PARAM;
    }
    if (m_buffers.find(buffer) == m_buffers.end()) {
        ERR("Release buffer failed, input buffer illegal.");
        return ERR_INVALID_PARAM;
    }
    m_buffers.erase(buffer);
    if (buffer->memType == MEM_TYPE_HOST) {
        delete buffer->data;
    }
    delete buffer;
    buffer = nullptr;
    return OK;
}

int32_t CpuEncoder::MapBuffer(GpuEncoderBufferT &buffer, uint32_t flag)
{
    (void)buffer;
    (void)flag;
    return ERR_UNSUPPORT_OPERATION;
}

int32_t CpuEncoder::UnmapBuffer(GpuEncoderBufferT &buffer)
{
    (void)buffer;
    return ERR_UNSUPPORT_OPERATION;
}

int32_t CpuEncoder::RetriveBufferData(GpuEncoderBufferT &buffer, uint8_t* data, uint32_t memLen, uint32_t& dataLen)
{
    (void)buffer;
    (void)data;
    (void)memLen;
    (void)dataLen;
    return ERR_UNSUPPORT_OPERATION;
}

int32_t CpuEncoder::Convert(GpuEncoderBufferT &inBuffer, GpuEncoderBufferT &outBuffer)
{
    if (inBuffer == nullptr || outBuffer == nullptr) {
        ERR("Convert failed, inbuffer or outbuffer is null");
        return ERR_INVALID_PARAM;
    }
    if (RGBAToYUV(inBuffer->data, outBuffer->size.widthAligned, outBuffer->size.width,
        outBuffer->size.height, outBuffer->data) != 0) {
        ERR("Convert RgbConvertToYuv failed");
        return ERR_INTERNAL_ERROR;
    }
    return OK;
}

int32_t CpuEncoder::Encode(GpuEncoderBufferT &inBuffer, GpuEncoderBufferT &outBuffer)
{
    (void)inBuffer;
    (void)outBuffer;
    return ERR_UNSUPPORT_OPERATION;
}

int32_t CpuEncoder::SetEncodeParam(EncodeParamT params[], uint32_t num)
{
    (void)params;
    (void)num;
    return ERR_UNSUPPORT_OPERATION;
}

uint32_t CpuEncoder::AlignUp(uint32_t val, uint32_t align)
{
    return (val + (align - 1)) & ~(align - 1);
}

int32_t CpuEncoder::Reset()
{
    return OK;
}
}
} // Vmi