/*
 * 版权所有 (c) 华为技术有限公司 2022-2022
 * 功能描述：Inno GPU编码模块对外接口
 */

#include "GpuEncoderInno.h"
#include <string>
#include <chrono>
#include <memory>
#include <new>
#include <algorithm>
#include <unistd.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include "logging.h"

#ifdef __cplusplus
extern "C" {
#endif
void *CreateGpuTurbo(uint32_t type)
{
    void *ret = nullptr;
    auto gpuType = static_cast<Vmi::GpuEncoder::GpuType>(type);
    switch (gpuType) {
        case Vmi::GpuEncoder::GpuType::GPU_INNO_G1:
        case Vmi::GpuEncoder::GpuType::GPU_INNO_G1P:
            ret = std::make_unique<Vmi::GpuEncoder::GpuEncoderInno>().release();
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

namespace {
const std::string INNO_IENC_LIB_NAME = "libienc.so";
#ifdef __LP64__
    const std::string INNO_IENC_LIB_PATH = "/system/lib64/" + INNO_IENC_LIB_NAME;
#else
    const std::string INNO_IENC_LIB_PATH = "/system/lib/" + INNO_IENC_LIB_NAME;
#endif

// 内部使用，需要保证输入宽高小于4096
uint32_t GetBufferSize(uint32_t width, uint32_t height, uint32_t type)
{
    switch (type) {
        case Vmi::GpuEncoder::FRAME_FORMAT_YUV:
        case Vmi::GpuEncoder::FRAME_FORMAT_NV12: {
            uint32_t pixelsNum = width * height;
            auto uvSize = (pixelsNum + 3) >> 2;
            return pixelsNum + uvSize + uvSize;
        }
        case Vmi::GpuEncoder::FRAME_FORMAT_RGBA:
        case Vmi::GpuEncoder::FRAME_FORMAT_BGRA: {
            return width * height * 4;
        }
        case Vmi::GpuEncoder::FRAME_FORMAT_H264:
        case Vmi::GpuEncoder::FRAME_FORMAT_HEVC: {
            return width * height * 3;
        }
        default: {
            return 0;
        }
    }
}

inline uint32_t AlignUp(uint32_t val, uint32_t align)
{
    return (val + (align - 1)) & ~(align - 1);
}

constexpr uint32_t MAX_WIDTH = 4096;
constexpr uint32_t MAX_HEIGHT = 4096;
constexpr uint32_t WIDTH_ALIGN = 32;
constexpr uint32_t HEIGHT_ALIGN = 32;
constexpr uint32_t MAX_BUFFER_SIZE = 128 * 1024 * 1024;

}

namespace Vmi {
namespace GpuEncoder {
void GpuEncoderInno::GpuBufferFence::Lock()
{
    std::lock_guard<std::mutex> lk(m_lock);
    m_ready = false;
}

void GpuEncoderInno::GpuBufferFence::Unlock()
{
    {
        std::lock_guard<std::mutex> lk(m_lock);
        m_ready = true;
    }
    m_control.notify_one();
}

void GpuEncoderInno::GpuBufferFence::Wait()
{
    std::unique_lock<std::mutex> lk(m_lock);
    m_control.wait(lk, [this]() { return m_ready; });
}

bool GpuEncoderInno::CheckAndLockStatus(Status status)
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

void GpuEncoderInno::UnlockStatus(Status status)
{
    std::lock_guard<std::mutex> lk(m_lock);
    m_status = status;
}

bool GpuEncoderInno::LoadInnoLib()
{
    std::lock_guard<std::mutex> lock(m_lock);
    m_iencLibHandle = dlopen(INNO_IENC_LIB_PATH.c_str(), RTLD_LAZY);
    if (m_iencLibHandle == nullptr) {
        ERR("Cannot open libienc.so, errno: %d, reson: %s", errno, dlerror());
        return false;
    }

    m_iencOpenEncoder = reinterpret_cast<IencOpenEncoder>(dlsym(m_iencLibHandle, "ienc_open_encoder"));
    if (m_iencOpenEncoder == nullptr) {
        ERR("Cannot find ienc_open_encoder: %s", dlerror());
        return false;
    }

    m_iencCloseEncoder = reinterpret_cast<IencCloseEncoder>(dlsym(m_iencLibHandle, "ienc_close_encoder"));
    if (m_iencCloseEncoder == nullptr) {
        ERR("Cannot find ienc_close_encoder: %s", dlerror());
        return false;
    }

    m_iencEncodeOneFrame = reinterpret_cast<IencEncodeOneFrame>(dlsym(m_iencLibHandle, "ienc_encode_one_frame"));
    if (m_iencEncodeOneFrame == nullptr) {
        ERR("Cannot find ienc_encode_one_frame: %s", dlerror());
        return false;
    }

    m_iencGetFrame = reinterpret_cast<IencGetFrame>(dlsym(m_iencLibHandle, "ienc_get_frame"));
    if (m_iencGetFrame == nullptr) {
        ERR("Cannot find ienc_get_frame: %s", dlerror());
        return false;
    }

    m_iencReleaseFrame = reinterpret_cast<IencReleaseFrame>(dlsym(m_iencLibHandle, "ienc_release_frame"));
    if (m_iencReleaseFrame == nullptr) {
        ERR("Cannot find ienc_release_frame: %s", dlerror());
        return false;
    }

    return true;
}

void GpuEncoderInno::UnLoadInnoLib() 
{
    std::lock_guard<std::mutex> lock(m_lock);
    m_iencOpenEncoder = nullptr;
    m_iencCloseEncoder = nullptr;
    m_iencEncodeOneFrame = nullptr;
    m_iencGetFrame = nullptr;
    m_iencReleaseFrame = nullptr;
    if (m_iencLibHandle) {
        dlclose(m_iencLibHandle);
        m_iencLibHandle = nullptr;
    }
}

int32_t GpuEncoderInno::Init(EncoderConfig &config)
{
    if (!CheckAndLockStatus(Status::INVALID)) {
        ERR("Init: status check failed");
        return -ERR_INVALID_STATUS;
    }

    if (config.inSize.width > MAX_WIDTH || config.inSize.height > MAX_HEIGHT ||
        config.inSize.width == 0 || config.inSize.height == 0) {
        ERR("Invalid frame size, current[%u x %u], max[%u x %u]",
            config.inSize.width, MAX_WIDTH, config.inSize.height, MAX_HEIGHT);
        UnlockStatus(m_originalStatus);
        return -ERR_INVALID_PARAM;
    }

    if (!LoadInnoLib()) {
        ERR("Fail to init rgb2yuv module");
        UnLoadInnoLib();
        UnlockStatus(m_originalStatus);
        return -ERR_INVALID_DEVICE;
    }

    m_size = config.inSize;
    m_size.widthAligned = AlignUp(m_size.width, WIDTH_ALIGN);
    m_size.heightAligned = AlignUp(m_size.height, HEIGHT_ALIGN);

    // m_iencAttr会由ApplyParamsToAttr提前初始化，SetEncodeParam一定要在Init之前完成
    if (m_iencAttr.enc_attr.pic_width == 0) {
        m_iencAttr.enc_attr.pic_width = m_size.width;
    }
    if (m_iencAttr.enc_attr.pic_height == 0) {
        m_iencAttr.enc_attr.pic_height = m_size.height;
    }

    m_iencEncoder = m_iencOpenEncoder(&m_iencAttr);
    if (!m_iencEncoder) {
        ERR("ienc_open_encoder failed");
        UnLoadInnoLib();
        UnlockStatus(m_originalStatus);
        return -ERR_INTERNAL_ERROR;
    }

    UnlockStatus(Status::INITED);
    return OK;
}

int32_t GpuEncoderInno::ResetImgSize(uint32_t width, uint32_t height)   
{
    return OK;
}

int32_t GpuEncoderInno::DeInit()
{
    (void)Stop();
    if (!CheckAndLockStatus(Status::INITED)) {
        return -ERR_INVALID_STATUS;
    }
    if (m_iencEncoder) {
        m_iencCloseEncoder(m_iencEncoder);
        m_iencEncoder = nullptr;
        INFO("ienc encoder closed");
    }
    UnLoadInnoLib();
    UnlockStatus(Status::INVALID);
    return OK;
}

int32_t GpuEncoderInno::Start()
{
    std::unique_lock<std::mutex> lk(m_lock);
    if (m_status != Status::INITED) {
        return -ERR_INVALID_STATUS;
    }
    m_status = Status::STARTED;
    INFO("Gpu encode inno start success");
    return OK;
}

int32_t GpuEncoderInno::Stop()
{
    std::unique_lock<std::mutex> lk(m_lock);
    if (m_status != Status::STARTED) {
        return -ERR_INVALID_STATUS;
    }
    m_status = Status::INITED;
    ReleaseAllBuffer();
    return OK;
}

int32_t GpuEncoderInno::CreateBuffer(FrameFormat format, MemType memType, GpuEncoderBufferT &buffer)
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
    if (format != FRAME_FORMAT_NV12 && format != FRAME_FORMAT_H264 && format != FRAME_FORMAT_HEVC) {
        ERR("Unsupport format type for buffer create: %u", format);
        return -ERR_UNSUPPORT_OPERATION;
    }

    auto newBuffer = std::make_unique<GpuEncoderBufferInno>();
    newBuffer->format = format;
    newBuffer->memType = memType;
    newBuffer->size = m_size;
    newBuffer->gpuType = GPU_INNO_G1P;
    uint32_t bufferSize = GetBufferSize(m_size.widthAligned, m_size.heightAligned, format);
    if (bufferSize == 0) {
        ERR("Invalid buffer size for buffer create, size=%u", bufferSize);
        return -ERR_INVALID_PARAM;
    }
    if (bufferSize > MAX_BUFFER_SIZE) {
        ERR("Buffer size exceeds limit for buffer create, size=%u max=%u", bufferSize, MAX_BUFFER_SIZE);
        return -ERR_INVALID_PARAM;
    }
    newBuffer->data = new (std::nothrow) uint8_t[bufferSize];
    if (newBuffer->data == nullptr) {
        ERR("Failed to allocate data buffer, size=%u", bufferSize);
        return -ERR_OUT_OF_MEM;
    }
    newBuffer->dataLen = bufferSize;
    buffer = newBuffer.release();
    m_buffers.emplace(buffer);
    return OK;
}

int32_t GpuEncoderInno::ImportBuffer(FrameFormat format, uint64_t handle, GpuEncoderBufferT &buffer)
{
    std::lock_guard<std::mutex> lk(m_lock);
    if (m_status != Status::STARTED) {
        ERR("Invalid status for buffer import: %d", static_cast<int32_t>(m_status));
        return -ERR_INVALID_STATUS;
    }
    if (format != FRAME_FORMAT_BGRA) {
        ERR("Unsupport format type for buffer import: %u", format);
        return -ERR_UNSUPPORT_OPERATION;
    }

    int32_t fd = static_cast<int32_t>(handle);
    
    auto newBuffer = std::make_unique<GpuEncoderBufferInno>();
    newBuffer->format = format;
    newBuffer->memType = MEM_TYPE_DEVICE;
    newBuffer->size = m_size;
    newBuffer->gpuType = GPU_INNO_G1P;
    newBuffer->external = true;
    newBuffer->fd = fd;

    buffer = newBuffer.release();
    m_buffers.emplace(buffer);
    return OK;
}

int32_t GpuEncoderInno::ReleaseBuffer(GpuEncoderBufferT &buffer)
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
    auto innoBuffer = static_cast<GpuEncoderBufferInnoT>(*record);

    if (innoBuffer->data) {
        delete[] innoBuffer->data;
        innoBuffer->data = nullptr;
    }

    m_buffers.erase(buffer);
    delete innoBuffer;
    buffer = nullptr;
    return OK;
}


void GpuEncoderInno::ReleaseAllBuffer()
{
    for (auto it : m_buffers) {
        auto innoBuffer = static_cast<GpuEncoderBufferInnoT>(it);
        if (innoBuffer->data) {
            delete[] innoBuffer->data;
            innoBuffer->data = nullptr;
        }
        delete innoBuffer;
    }
    m_buffers.clear();
}

int32_t GpuEncoderInno::MapBuffer(GpuEncoderBufferT &buffer, uint32_t flag)
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

    auto innoBuffer = static_cast<GpuEncoderBufferInnoT>(*record);
    if (innoBuffer->data == nullptr) {
        ERR("Invalid buffer data for buffer map: %p", buffer);
        return -ERR_INVALID_PARAM;
    }

    ienc_stream_t stream;
    int32_t result_fd = -1;
    int ret = m_iencGetFrame(m_iencEncoder, &result_fd, &stream, -1);

    if (ret != IENC_ERR_NONE) {
        ERR("ienc_get_frame failed, error: %d", ret);
        return -ERR_INTERNAL_ERROR;
    }

    if (result_fd != innoBuffer->fd) {
        ERR("ienc_get_frame returned wrong fd: expected=%d, got=%d", 
            innoBuffer->fd, result_fd);
        m_iencReleaseFrame(m_iencEncoder, &stream);
        return -ERR_INTERNAL_ERROR;
    }

    uint32_t dataLen = 0;
    for (uint32_t i = 0; i < stream.pack_count; i++) {
        dataLen += stream.pack[i].len;
    }
    
    uint32_t offset = 0;
    for (uint32_t i = 0; i < stream.pack_count; i++) {
        uint8_t* addr = stream.pack[i].addr + stream.pack[i].offset;
        std::copy_n(addr, stream.pack[i].len, innoBuffer->data + offset);
        offset += stream.pack[i].len;
    }
    
    innoBuffer->dataLen = dataLen;
    
    m_iencReleaseFrame(m_iencEncoder, &stream);
    
    return OK;
}

int32_t GpuEncoderInno::RetriveBufferData(GpuEncoderBufferT &buffer, uint8_t *data, uint32_t memLen, uint32_t &dataLen)
{
    (void) buffer;
    (void) data;
    (void) memLen;
    (void) dataLen;
    return OK;
}

int32_t GpuEncoderInno::UnmapBuffer(GpuEncoderBufferT &buffer)
{
    return OK;
}

int32_t GpuEncoderInno::Convert(GpuEncoderBufferT &inBuffer, GpuEncoderBufferT &outBuffer)
{
    (void) inBuffer;
    (void) outBuffer;
    return OK;
}

int32_t GpuEncoderInno::Encode(GpuEncoderBufferT &inBuffer, GpuEncoderBufferT &outBuffer)
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

    // Inno G1P支持BGRA和NV12格式输入，输出必须是H264或HEVC
    if ((inBuffer->format != FRAME_FORMAT_BGRA && inBuffer->format != FRAME_FORMAT_NV12) ||
        (outBuffer->format != FRAME_FORMAT_H264 && outBuffer->format != FRAME_FORMAT_HEVC)) {
        ERR("Unsupport format type for encode: %u -> %u", inBuffer->format, outBuffer->format);
        return -ERR_UNSUPPORT_OPERATION;
    }

    auto inInnoBuff = static_cast<GpuEncoderBufferInnoT>(inBuffer);

    ienc_frame_t ienc_frame = {};
    ienc_frame.fd = inInnoBuff->fd;
    int ret = m_iencEncodeOneFrame(m_iencEncoder, &ienc_frame);

    if (ret != IENC_ERR_NONE) {
        ERR("ienc_encode_one_frame failed, error: %d", ret);
        return -ERR_INTERNAL_ERROR;
    }

    auto outInnoBuff = static_cast<GpuEncoderBufferInnoT>(outBuffer);
    outInnoBuff->fd = inInnoBuff->fd;
    return OK;
}

void GpuEncoderInno::SetFrameRate(EncodeParamT &param, InnoEncodeParams &params)
{
    auto ptr = static_cast<EncodeParamFrameRate *>(param);
    params.frameRate = ptr->frameRate;
}
void GpuEncoderInno::SetBitRate(EncodeParamT &param, InnoEncodeParams &params)
{
    auto ptr = static_cast<EncodeParamBitRate *>(param);
    params.bitRate = ptr->bitRate;
}
void GpuEncoderInno::SetGopsize(EncodeParamT &param, InnoEncodeParams &params)
{
    auto ptr = static_cast<EncodeParamGopsize *>(param);
    params.gopSize = ptr->gopSize;
}
void GpuEncoderInno::SetKeyFrame(InnoEncodeParams &params)
{
    params.keyFrame = 1;
}
void GpuEncoderInno::SetProfile(EncodeParamT &param, InnoEncodeParams &params)
{
    auto ptr = static_cast<EncodeParamProfile *>(param);
    params.profile = ptr->profile;
}
void GpuEncoderInno::SetRcmode(EncodeParamT &param, InnoEncodeParams &params)
{
    auto ptr = static_cast<EncodeParamRateControl *>(param);
    params.rcMode = ptr->rateControl;
}
void GpuEncoderInno::SetStreamWidth(EncodeParamT &param, InnoEncodeParams &params)
{
    auto ptr = static_cast<EncodeParamStreamWidth *>(param);
    params.streamWidth = ptr->streamWidth;
}
void GpuEncoderInno::SetStreamHeight(EncodeParamT &param, InnoEncodeParams &params)
{
    auto ptr = static_cast<EncodeParamStreamHeight *>(param);
    params.streamHeight = ptr->streamHeight;
}
void GpuEncoderInno::SetCrfLevel(EncodeParamT &param, InnoEncodeParams &params)
{
    auto ptr = static_cast<EncodeParamCrf *>(param);
    params.crf = ptr->crf;
    return;
}
void GpuEncoderInno::SetMaxCrfRate(EncodeParamT &param, InnoEncodeParams &params)
{
    auto ptr = static_cast<EncodeParamMaxCrfRate *>(param);
    params.maxCrfRate = ptr->maxCrfRate;
    return;
}

// TODO 
int32_t GpuEncoderInno::SetEncodeParam(EncodeParamT params[], uint32_t num)
{
    InnoEncodeParams tmpParams = m_settingParams;
    for (uint32_t i = 0; i < num; i++) {
        switch (params[i]->id) {
            case ENCODE_PARAM_RATE_CONTROL:
                SetRcmode(params[i], tmpParams);
                // isNeedRestart = true;
                break;
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
                break;
            case ENCODE_PARAM_PROFILE:
                SetProfile(params[i], tmpParams);
                break;
            case ENCODE_PARAM_STREAM_WIDTH:
                SetStreamWidth(params[i], tmpParams);
                break;
            case ENCODE_PARAM_STREAM_HEIGHT:
                SetStreamHeight(params[i], tmpParams);
                break;
            case ENCODE_PARAM_CRF:
                SetCrfLevel(params[i], tmpParams);
                break;
            case ENCODE_PARAM_CRF_MAXRATE:
                SetMaxCrfRate(params[i], tmpParams);
                break;
            default:
                ERR("Inno set encoder param failed, unsupport param type");
                return -ERR_INVALID_PARAM;
        }
    }
    std::lock_guard<std::mutex> lk(m_lock);
    m_receiveParams = tmpParams;
    if (m_status == Status::INVALID) {              // before init
        UpdateSettingParams();
        return OK;
    }

    INFO("update encode params, framerate: %u, gopSize: %u, profile: %u, bitrate:%u, keyframe: %u",
        m_receiveParams.frameRate, m_receiveParams.gopSize, m_receiveParams.profile, m_receiveParams.bitRate,
        m_receiveParams.keyFrame);
    INFO("update encode params, crf: %u, maxcrfrate: %u, rcMode: %u", m_receiveParams.crf, m_receiveParams.maxCrfRate,
        m_receiveParams.rcMode);
    return ERR_NEED_RESET;
}

void GpuEncoderInno::UpdateSettingParams()
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
    if (m_receiveParams.crf != UINT32_MAX && m_receiveParams.crf != m_settingParams.crf) {
        m_settingParams.crf = m_receiveParams.crf;
    }
    if (m_receiveParams.maxCrfRate != UINT32_MAX && m_receiveParams.maxCrfRate != m_settingParams.maxCrfRate) {
        m_settingParams.maxCrfRate = m_receiveParams.maxCrfRate;
    }
    if (m_receiveParams.rcMode != UINT32_MAX && m_receiveParams.rcMode != m_settingParams.rcMode) {
        m_settingParams.rcMode = m_receiveParams.rcMode;
    }
    if (m_receiveParams.streamWidth != 0 && m_receiveParams.streamWidth != m_settingParams.streamWidth) {
        m_settingParams.streamWidth = m_receiveParams.streamWidth;
    }
    if (m_receiveParams.streamHeight != 0 && m_receiveParams.streamHeight != m_settingParams.streamHeight) {
        m_settingParams.streamHeight = m_receiveParams.streamHeight;
    }
    ApplyParamsToAttr();
    return;
}

ienc_profile_e GpuEncoderInno::ConvertProfile(uint32_t profileCode)
{
    switch (profileCode) {
        case ENC_PROFILE_IDC_BASELINE:
            return IENC_AVC_BASELINE_PROFILE;
        case ENC_PROFILE_IDC_MAIN:
            return IENC_AVC_MAIN_PROFILE;
        case ENC_PROFILE_IDC_HIGH:
            return IENC_AVC_HIGH_PROFILE;
        case ENC_PROFILE_IDC_HEVC_MAIN:
            return IENC_HEVC_MAIN_PROFILE;
        default:
            return IENC_AVC_BASELINE_PROFILE;
    }
}

ienc_rc_mode_e GpuEncoderInno::ConvertRcMode(uint32_t rcMode)
{
    switch (rcMode) {
        case ENC_RC_VBR:
            return IENC_RC_MODE_VBR;
        case ENC_RC_CBR:
            return IENC_RC_MODE_CBR;
        default:
            return IENC_RC_MODE_CBR;
    }
}

void GpuEncoderInno::ApplyParamsToAttr()
{
    m_iencAttr.enc_attr.src_format = IENC_FORMAT_PVRIC_ARGB_8X8;
    m_iencAttr.enc_attr.profile = ConvertProfile(m_settingParams.profile);
    if (m_settingParams.streamWidth != 0) {
        m_iencAttr.enc_attr.pic_width = m_settingParams.streamWidth;
    } else if (m_size.width != 0) {
        m_iencAttr.enc_attr.pic_width = m_size.width;
    }
    if (m_settingParams.streamHeight != 0) {
        m_iencAttr.enc_attr.pic_height = m_settingParams.streamHeight;
    } else if (m_size.height != 0) {
        m_iencAttr.enc_attr.pic_height = m_size.height;
    }
    m_iencAttr.enc_attr.b_frame_num = 0;
    m_iencAttr.enc_attr.csc_mode = IENC_CSC_MODE_BT601;
    m_iencAttr.enc_attr.csc_range = IENC_CSC_RANGE_LIMIT;
    uint32_t qp = (m_settingParams.crf != UINT32_MAX) ? m_settingParams.crf : 25;
    m_iencAttr.rc_attr.rc_mode = ConvertRcMode(m_settingParams.rcMode);
    switch (m_iencAttr.rc_attr.rc_mode) {
        case IENC_RC_MODE_CBR:
            m_iencAttr.rc_attr.cbr_attr.intra_period = m_settingParams.gopSize;
            m_iencAttr.rc_attr.cbr_attr.intra_idr_period = m_settingParams.gopSize;
            m_iencAttr.rc_attr.cbr_attr.src_frame_rate = m_settingParams.frameRate;
            m_iencAttr.rc_attr.cbr_attr.bit_rate = m_settingParams.bitRate;
            m_iencAttr.rc_attr.cbr_attr.init_qp = qp;
            break;
        case IENC_RC_MODE_VBR:
            m_iencAttr.rc_attr.vbr_attr.intra_period = m_settingParams.gopSize;
            m_iencAttr.rc_attr.vbr_attr.intra_idr_period = m_settingParams.gopSize;
            m_iencAttr.rc_attr.vbr_attr.src_frame_rate = m_settingParams.frameRate;
            m_iencAttr.rc_attr.vbr_attr.max_bit_rate = m_settingParams.bitRate;
            m_iencAttr.rc_attr.vbr_attr.init_qp = qp;
            m_iencAttr.rc_attr.vbr_attr.min_qp = 10;
            m_iencAttr.rc_attr.vbr_attr.max_qp = 51;
            break;
        case IENC_RC_MODE_CQP:
            m_iencAttr.rc_attr.cqp_attr.intra_period = m_settingParams.gopSize;
            m_iencAttr.rc_attr.cqp_attr.intra_idr_period = m_settingParams.gopSize;
            m_iencAttr.rc_attr.cqp_attr.src_frame_rate = m_settingParams.frameRate;
            m_iencAttr.rc_attr.cqp_attr.qp = qp;
            break;
    }
}

int32_t GpuEncoderInno::Reset()
{
    if (!CheckAndLockStatus(Status::INITED)) {
        ERR("Reset cannot be called in running status, call stop first");
        return -ERR_INVALID_STATUS;
    }

    if (m_iencEncoder) {
        m_iencCloseEncoder(m_iencEncoder);
        m_iencEncoder = nullptr;
    }

    // 使运行期通过 SetEncodeParam 设置的参数在 reset 后生效
    UpdateSettingParams();

    m_iencEncoder = m_iencOpenEncoder(&m_iencAttr);

    if (!m_iencEncoder) {
        ERR("ienc_open_encoder failed after reset");
        UnlockStatus(Status::INVALID);
        return -ERR_INTERNAL_ERROR;
    }

    UnlockStatus(Status::INITED);
    return OK;
}
}
}
