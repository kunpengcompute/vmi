/*
 * 版权所有 (c) 华为技术有限公司 2022-2022
 * 功能描述：Inno GPU编码模块对外接口
 */

#include "GpuEncoderInno.h"
#include <string>
#include <chrono>
#include <memory>
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
const std::string INNO_IFBC_LIB_NAME = "libifbc.so";
#ifdef __LP64__
    const std::string INNO_IFBC_LIB_PATH = "/system/lib64/" + INNO_IFBC_LIB_NAME;
#else
    const std::string INNO_IFBC_LIB_PATH = "/system/lib/" + INNO_IFBC_LIB_NAME;
#endif

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
        } default: {
            return 0;
        }
    }
}

inline uint32_t AlignUp(uint32_t val, uint32_t align)
{
    return (val + (align - 1)) & ~(align - 1);
}

inline VAProfile GetProfile(uint32_t profile)
{
    switch (profile) {
        case Vmi::GpuEncoder::ENC_PROFILE_IDC_BASELINE:
            return VAProfileH264ConstrainedBaseline;
        case Vmi::GpuEncoder::ENC_PROFILE_IDC_MAIN:
            return VAProfileH264Main;
        case Vmi::GpuEncoder::ENC_PROFILE_IDC_HIGH:
            return VAProfileH264High;
        case Vmi::GpuEncoder::ENC_PROFILE_IDC_HEVC_MAIN:
            return VAProfileHEVCMain;
        default:
            // 默认使用H264 baseline
            return VAProfileH264ConstrainedBaseline;
    }
}
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
    m_innoYuvLib.lib = dlopen(INNO_IFBC_LIB_PATH.c_str(), RTLD_LAZY);
    if (m_innoYuvLib.lib == nullptr) {
        ERR("Fail to load Inno rgb2yuv lib, errno: %d, reson: %s", errno, dlerror());
        return false;
    }
    using InitFunc = InnoConvertHandle(*)(EglInfoT, uint32_t *);
    m_innoYuvLib.init = reinterpret_cast<InitFunc>(dlsym(m_innoYuvLib.lib, "ifbc_convert_init"));
    using DeinitFunc = void(*)(InnoConvertHandle);
    m_innoYuvLib.deinit = reinterpret_cast<DeinitFunc>(dlsym(m_innoYuvLib.lib, "ifbc_convert_deinit"));
    using ConvertFunc = int(*)(InnoConvertHandle, const IfbcFrameT, IfbcFrameT);
    m_innoYuvLib.convert = reinterpret_cast<ConvertFunc>(dlsym(m_innoYuvLib.lib, "ifbc_convert"));
    if (m_innoYuvLib.init == nullptr || m_innoYuvLib.deinit == nullptr || m_innoYuvLib.convert == nullptr) {
        ERR("Fail to load Inno rgb2yuv functions, errno: %d, reson: %s", errno, dlerror());
        (void)dlclose(m_innoYuvLib.lib);
        m_innoYuvLib = {};
        return false;
    }
    return true;
}

void GpuEncoderInno::InitEncodeParam()
{
    m_encoder->SetResolution(m_size.width, m_size.height, m_size.widthAligned, m_size.heightAligned);
    m_encoder->SetProfile(static_cast<VAProfile>(m_encodeParam.profile));
    m_encoder->SetUseVbr(m_encodeParam.vbrMode); // true表示使用动态码率，false表示固定码率
    m_encoder->SetBitrate(m_encodeParam.bitrate);
    m_encoder->SetFrameRate(m_encodeParam.frameRate);
    m_encoder->SetIntraPeriod(m_encodeParam.gopSize);
    m_encoder->SetGopParam(m_encodeParam.gopSize, GOP_PRESET_IP);
    m_encoder->SetEntropyMode(ENTROPY_CABAC);
    m_encoder->SetRenderSequence(); // 在编码时生成sps信息
}

int32_t GpuEncoderInno::Init(EncoderConfig &config)
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

    if (!LoadInnoLib()) {
        ERR("Fail to init rgb2yuv module");
        UnlockStatus(m_originalStatus);
        return -ERR_INVALID_DEVICE;
    }

    m_size = config.inSize;
    m_size.widthAligned = AlignUp(m_size.width, WIDTH_ALIGN);
    m_size.heightAligned = AlignUp(m_size.height, HEIGHT_ALIGN);

    m_encoder = std::make_unique<VaEncInno>();
    InitEncodeParam();
    if (!m_encoder->Start()) {
        ERR("Fail to start gpu encoder");
        UnLoadInnoLib();
        UnlockStatus(m_originalStatus);
        return -ERR_INTERNAL_ERROR;
    }

    UnlockStatus(Status::INITED);
    INFO("Gpu encode inno init success");
    return OK;
}

bool GpuEncoderInno::UnLoadInnoLib()
{
    if (dlclose(m_innoYuvLib.lib) != 0) {
        return false;
    }
    m_innoYuvLib = {};
    return true;
}

void GpuEncoderInno::ReleaseAllBuffer()
{
    for (auto it : m_buffers) {
        auto innoBuffer = static_cast<GpuEncoderBufferInnoT>(it);
        if (innoBuffer->mapped) {
            munmap(innoBuffer->data, innoBuffer->dataLen);
            innoBuffer->mapped = false;
        }
        if (!innoBuffer->external) {
            if (innoBuffer->format == FRAME_FORMAT_NV12) {
                m_encoder->ReleaseYuvBuffer(innoBuffer->slot);
            } else if (innoBuffer->format == FRAME_FORMAT_H264 || innoBuffer->format == FRAME_FORMAT_HEVC) {
                m_encoder->ReleaseStreamBuffer(innoBuffer->slot);
            }
        }
        delete innoBuffer;
    }
    m_buffers.clear();
}

int32_t GpuEncoderInno::DeInit()
{
    (void)Stop();
    if (!CheckAndLockStatus(Status::INITED)) {
        return -ERR_INVALID_STATUS;
    }
    m_encoder->Stop();
    m_encoder = nullptr;
    if (!UnLoadInnoLib()) {
        WARN("Fail to unload rgb2yuv module");
    }
    UnlockStatus(Status::INVALID);
    return OK;
}

int32_t GpuEncoderInno::Start()
{
    std::unique_lock<std::mutex> lk(m_lock);
    if (m_status != Status::INITED) {
        return -ERR_INVALID_STATUS;
    }
    std::unique_lock<std::mutex> lkConvert(m_convertLock);
    m_status = Status::STARTED;
    m_convertThread = std::thread(&GpuEncoderInno::ConvertThreadFunc, this);
    INFO("Gpu encode inno start success");
    return OK;
}

int32_t GpuEncoderInno::Stop()
{
    std::unique_lock<std::mutex> lk(m_lock);
    if (m_status != Status::STARTED) {
        return -ERR_INVALID_STATUS;
    }
    std::unique_lock<std::mutex> lkConvert(m_convertLock);
    m_status = Status::INITED;
    lkConvert.unlock();
    m_convertCtl.notify_all();
    if (m_convertThread.joinable()) {
        m_convertThread.join();
    }
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

    if (format == FRAME_FORMAT_NV12) {
        if (!m_encoder->GetYuvBuffer(newBuffer->slot)) {
            ERR("fail to get yuv buffer");
            return -ERR_OUT_OF_MEM;
        }
        newBuffer->fd = m_encoder->GetYuvBufferFd(newBuffer->slot);
        if (newBuffer->fd < 0) {
            ERR("fail to get yuv buffer fd");
            m_encoder->ReleaseYuvBuffer(newBuffer->slot);
            return -ERR_INTERNAL_ERROR;
        }
    } else if (format == FRAME_FORMAT_H264 || format == FRAME_FORMAT_HEVC) {
        if (!m_encoder->GetStreamBuffer(newBuffer->slot)) {
            ERR("fail to get stream buffer");
            return -ERR_OUT_OF_MEM;
        }
    }
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

    auto newBuffer = std::make_unique<GpuEncoderBufferInno>();
    newBuffer->format = format;
    newBuffer->memType = MEM_TYPE_DEVICE;
    newBuffer->size = m_size;
    newBuffer->gpuType = GPU_INNO_G1P;
    newBuffer->external = true;
    newBuffer->fd = handle;

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
    if (innoBuffer->mapped) {
        munmap(innoBuffer->data, innoBuffer->dataLen);
        innoBuffer->mapped = false;
    }
    if (innoBuffer->format == FRAME_FORMAT_NV12) {
        m_encoder->ReleaseYuvBuffer(innoBuffer->slot);
    } else if (innoBuffer->format == FRAME_FORMAT_H264 || innoBuffer->format == FRAME_FORMAT_HEVC) {
        m_encoder->ReleaseStreamBuffer(innoBuffer->slot);
    }
    m_buffers.erase(buffer);
    buffer = nullptr;
    delete innoBuffer;
    return OK;
}

uint32_t GpuEncoderInno::MapStreamBuffer(GpuEncoderBufferInnoT &buffer)
{
    if (!m_encoder->MapStreamBuffer(buffer->slot, &buffer->bufList)) {
        ERR("Map Stream buff failed");
    }
    VACodedBufferSegment *bufList = buffer->bufList;
    uint32_t streamSize = 0;
    uint32_t bufNum = 0;
    while (bufList != nullptr) {
        if (bufList->buf == nullptr) {
            WARN("Stream data is null");
            break;
        }
        streamSize += bufList->size;
        ++bufNum;
        bufList = reinterpret_cast<VACodedBufferSegment *>(bufList->next);
    }
    buffer->dataLen = streamSize;
    if (bufNum == 1) {
        // 链表大小为1，可以直接返回数据指针和长度
        buffer->data = static_cast<uint8_t *>(buffer->bufList->buf);
    } else {
        // 链表大小大于1，无法直接返回指针，需要调用RetriveBufferData接口获取数据
        buffer->data = nullptr;
    }
    return bufNum;
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

    auto innoBuffer = static_cast<GpuEncoderBufferInnoT>(buffer);
    if (innoBuffer->mapped) {
        ERR("Buffer already mapped");
        return -ERR_INVALID_PARAM;
    }

    // 存放码流的buffer映射后是一个链表，若链表长度大于1，无法直接返回连续内存，只返回长度
    if (buffer->format == FRAME_FORMAT_H264 ||
        buffer->format == FRAME_FORMAT_HEVC) {
        auto bufNum = MapStreamBuffer(innoBuffer);
        if (bufNum == 0) {
            ERR("Map get zero stream buffer");
            return -ERR_INTERNAL_ERROR;
        }
    } else {
        innoBuffer->dataLen = GetBufferSize(innoBuffer->size.widthAligned, innoBuffer->size.heightAligned,
            innoBuffer->format);
        if (innoBuffer->dataLen != 0 && innoBuffer->fd > 0) {
            auto dataPtr = mmap(nullptr, innoBuffer->dataLen, flag, MAP_SHARED, innoBuffer->fd, 0);
            innoBuffer->data = reinterpret_cast<uint8_t *>(dataPtr);
        } else {
            ERR("data length or fd is invalid");
            return -ERR_INTERNAL_ERROR;
        }
    }
    innoBuffer->mapped = true;
    return OK;
}

int32_t GpuEncoderInno::RetriveBufferData(GpuEncoderBufferT &buffer, uint8_t *data, uint32_t memLen, uint32_t &dataLen)
{
    std::lock_guard<std::mutex> lk(m_lock);
    if (m_status != Status::STARTED) {
        ERR("Invalid status for buffer data retrieve: %d", static_cast<int32_t>(m_status));
        return -ERR_INVALID_STATUS;
    }
    auto &&record = m_buffers.find(buffer);
    if (record == m_buffers.end()) {
        ERR("Invalid buffer ptr for buffer data retrieve: %p", buffer);
        return -ERR_INVALID_PARAM;
    }
    // RetriveBufferData仅用于获取码流数据，因为码流数据无法直接map
    if (buffer->format != FRAME_FORMAT_H264 && buffer->format != FRAME_FORMAT_HEVC) {
        ERR("Unsupport format type for buffer data retrieve: %u", buffer->format);
        return -ERR_UNSUPPORT_OPERATION;
    }
    auto innoBuffer = static_cast<GpuEncoderBufferInnoT>(buffer);

    if (!innoBuffer->mapped || innoBuffer->bufList == nullptr) {
        ERR("Buffer is not mapped for buffer data retrieve");
        return -ERR_INVALID_PARAM;
    }

    dataLen = 0;
    VACodedBufferSegment *bufList = innoBuffer->bufList;
    while (bufList != nullptr) {
        if (bufList->buf == nullptr) {
            WARN("Stream data is null, skip data copy");
            break;
        }
        dataLen += bufList->size;
        if (dataLen > memLen) {
            WARN("Stream size exceed given buffer size [%u/%u], skip data copy", dataLen, memLen);
            dataLen = 0;
            break;
        }
        std::copy_n(static_cast<uint8_t *>(bufList->buf), bufList->size, data);
        data += bufList->size;
        bufList = reinterpret_cast<VACodedBufferSegment *>(bufList->next);
    }
    if (dataLen == 0) {
        return -ERR_INTERNAL_ERROR;
    }
    return OK;
}

int32_t GpuEncoderInno::UnmapBuffer(GpuEncoderBufferT &buffer)
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
    auto innoBuffer = static_cast<GpuEncoderBufferInnoT>(buffer);
    if (!innoBuffer->mapped) {
        ERR("Buffer is not mapped, cannot unmap");
        return -ERR_INVALID_PARAM;
    }

    if (buffer->format == FRAME_FORMAT_H264 ||
        buffer->format == FRAME_FORMAT_HEVC) {
        m_encoder->UnmapStreamBuffer(innoBuffer->slot);
    } else {
        munmap(innoBuffer->data, innoBuffer->dataLen);
    }
    innoBuffer->mapped = false;
    return OK;
}

void GpuEncoderInno::ConvertThreadFunc()
{
    /* XD使用OpenGL进行格式转换，要求convertHandle必须在同一个线程里创建，使用和销毁
       因此抽取独立阻塞式线程完成该操作 */
    m_convertHandle = m_innoYuvLib.init(nullptr, nullptr);
    if (m_convertHandle == nullptr) {
        ERR("Get null convert handle!, abort");
        return;
    }
    while (m_status == Status::STARTED) {
        std::unique_lock<std::mutex> lk(m_convertLock);
        m_convertCtl.wait(lk, [this]() {
            return m_hasNewFrame || m_status != Status::STARTED;
        });
        if (m_status != Status::STARTED) {
            break;
        }
        m_convertTask();
        m_hasNewFrame = false;
    }
    m_innoYuvLib.deinit(m_convertHandle);
}

bool GpuEncoderInno::DoConvert(GpuEncoderBufferInnoT inBuffer, GpuEncoderBufferInnoT outBuffer)
{
    std::unique_lock<std::mutex> lk(m_convertLock);
    m_inFrame = { 0, 0, static_cast<int>(inBuffer->size.width),
                          static_cast<int>(inBuffer->size.height),
                          static_cast<int>(inBuffer->size.widthAligned),
                          0, FORMAT_ARGB8888, static_cast<uint64_t>(inBuffer->fd) };
    m_outFrame = { 0, 0, static_cast<int>(outBuffer->size.width),
                           static_cast<int>(outBuffer->size.height),
                           static_cast<int>(outBuffer->size.widthAligned),
                           0, FORMAT_NV12, static_cast<uint64_t>(outBuffer->fd) };
    m_hasNewFrame = true;
    m_convertTask = std::packaged_task<bool()>([this] {
        int ret = m_innoYuvLib.convert(m_convertHandle, &m_inFrame, &m_outFrame);
        if (ret != 0) {
            ERR("Transfer color format fail, err: %d", ret);
            return false;
        }
        return true;
    });
    lk.unlock();
    auto f = m_convertTask.get_future();
    m_convertCtl.notify_all();
    return f.get();
}

int32_t GpuEncoderInno::Convert(GpuEncoderBufferT &inBuffer, GpuEncoderBufferT &outBuffer)
{
    std::lock_guard<std::mutex> lk(m_lock);
    if (m_status != Status::STARTED) {
        ERR("Invalid status for convert: %d", static_cast<int32_t>(m_status));
        return -ERR_INVALID_STATUS;
    }
    auto &&inIt = m_buffers.find(inBuffer);
    auto &&outIt = m_buffers.find(outBuffer);
    if (inIt == m_buffers.end() || outIt == m_buffers.end()) {
        ERR("Invalid buffer ptr for convert: %p | %p", inBuffer, outBuffer);
        return -ERR_INVALID_PARAM;
    }
    if (inBuffer->format != FRAME_FORMAT_BGRA || outBuffer->format != FRAME_FORMAT_NV12) {
        ERR("Unsupport format type for convert: %u -> %u", inBuffer->format, outBuffer->format);
        return -ERR_UNSUPPORT_OPERATION;
    }

    auto inInnoBuff = static_cast<GpuEncoderBufferInnoT>(inBuffer);
    auto outInnoBuff = static_cast<GpuEncoderBufferInnoT>(outBuffer);

    if (!DoConvert(inInnoBuff, outInnoBuff)) {
        ERR("Transfer color format fail");
        return -ERR_INTERNAL_ERROR;
    }
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

    if (inBuffer->format != FRAME_FORMAT_NV12 ||
        (outBuffer->format != FRAME_FORMAT_H264 && outBuffer->format != FRAME_FORMAT_HEVC)) {
        ERR("Unsupport format type for encode: %u -> %u", inBuffer->format, outBuffer->format);
        return -ERR_UNSUPPORT_OPERATION;
    }

    auto inInnoBuff = static_cast<GpuEncoderBufferInnoT>(inBuffer);
    auto outInnoBuff = static_cast<GpuEncoderBufferInnoT>(outBuffer);

    int ret = m_encoder->EncodeFrame(inInnoBuff->slot, outInnoBuff->slot);
    if (ret != 0) {
        ERR("Encode one frame fail, err: %d", ret);
        return -ERR_INTERNAL_ERROR;
    }
    return OK;
}

int32_t GpuEncoderInno::SetEncodeParam(EncodeParamT params[], uint32_t num)
{
    if (num >= ENCODE_PARAM_MAX) {
        ERR("Params num overflow, given: %u, max: %u", num, ENCODE_PARAM_MAX);
    }
    bool needRestart = false;
    for (uint32_t i = 0; i < num; ++i) {
        switch (params[i]->id) {
            case ENCODE_PARAM_BITRATE: {
                auto param = static_cast<EncodeParamBitRate *>(params[i]);
                m_encodeParam.bitrate = param->bitRate;
                needRestart = true;
                break;
            } case ENCODE_PARAM_GOPSIZE: {
                auto param = static_cast<EncodeParamGopsize *>(params[i]);
                m_encodeParam.gopSize = param->gopSize;
                needRestart = true;
                break;
            } case ENCODE_PARAM_PROFILE: {
                auto param = static_cast<EncodeParamProfile *>(params[i]);
                m_encodeParam.profile = GetProfile(param->profile);
                if (m_encodeParam.profile == VAProfileH264ConstrainedBaseline) {
                    m_encodeParam.entropy = ENTROPY_CAVLC; // h264 baseline仅支持CAVLC编码
                } else {
                    m_encodeParam.entropy = ENTROPY_CABAC; // 非h264 baseline使用更优的CABAC编码
                }
                needRestart = true;
                break;
            } case ENCODE_PARAM_FRAMERATE: {
                auto param = static_cast<EncodeParamFrameRate *>(params[i]);
                m_encodeParam.frameRate = param->frameRate;
                break;
            } case ENCODE_PARAM_KEYFRAME: {
                if (m_encoder != nullptr) {
                    m_encoder->SetForceIFrame(0);
                }
                break;
            } default:
                ERR("Params index overflow, given: %u, max: %u", params[i]->id, ENCODE_PARAM_MAX);
                return -ERR_INVALID_PARAM;
        }
    }
    if (needRestart && m_status >= Status::INITED) {
        return ERR_NEED_RESET;
    }
    return OK;
}

int32_t GpuEncoderInno::Reset()
{
    if (!CheckAndLockStatus(Status::INITED)) {
        ERR("Reset cannot be called in running status, call stop first");
        return -ERR_INVALID_STATUS;
    }
    m_encoder->Stop();
    m_encoder = std::make_unique<VaEncInno>();
    InitEncodeParam();
    if (!m_encoder->Start()) {
        ERR("Fail to start gpu encoder");
        m_encoder = nullptr;
        UnlockStatus(Status::INVALID);
        return -ERR_INTERNAL_ERROR;
    }
    UnlockStatus(Status::INITED);
    return OK;
}
}
}