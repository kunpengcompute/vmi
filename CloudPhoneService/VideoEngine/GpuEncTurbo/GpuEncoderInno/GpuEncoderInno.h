/*
 * 版权所有 (c) 华为技术有限公司 2023-2023
 * 功能描述：芯动GPU编码模块对外接口
 */

#ifndef GPU_ENCODER_INNO
#define GPU_ENCODER_INNO

#include <functional>
#include <mutex>
#include <set>
#include <map>
#include <queue>
#include <thread>
#include <cstdint>
#include <future>
#include <condition_variable>
#include "../GpuEncoderBase.h"
#include "VaEncInno.h"
#include "InnoFbcApi.h"

namespace Vmi {
namespace GpuEncoder {
class GpuEncoderInno : public GpuEncoderBase {
public:
    int32_t Init(EncoderConfig &config) override;

    int32_t DeInit() override;

    int32_t Start() override;

    int32_t Stop() override;

    int32_t CreateBuffer(FrameFormat format, MemType memType, GpuEncoderBufferT &buffer) override;

    int32_t ImportBuffer(FrameFormat format, uint64_t handle, GpuEncoderBufferT &buffer) override;

    int32_t ReleaseBuffer(GpuEncoderBufferT &buffer) override;

    int32_t MapBuffer(GpuEncoderBufferT &buffer, uint32_t flag) override;

    int32_t UnmapBuffer(GpuEncoderBufferT &buffer) override;

    int32_t RetriveBufferData(GpuEncoderBufferT &buffer, uint8_t* data, uint32_t memLen, uint32_t& dataLen) override;

    int32_t Convert(GpuEncoderBufferT &inBuffer, GpuEncoderBufferT &outBuffer) override;

    int32_t Encode(GpuEncoderBufferT &inBuffer, GpuEncoderBufferT &outBuffer) override;

    int32_t SetEncodeParam(EncodeParamT params[], uint32_t num) override;

    int32_t Reset() override;
private:
    enum class Status {
        INVALID,
        BUSY,
        INITED,
        STARTED,
    };

    // va编码相关
    struct InnoEncodeParam {
        uint32_t profile = VAProfileH264ConstrainedBaseline; // 默认baseline profile
        uint32_t vbrMode = false; // 默认使用固定码率模式
        uint32_t bitrate = 5000000; // 5000000：默认使用5Mbps码率
        uint32_t frameRate = 30; // 30：默认30帧
        uint32_t gopSize = 30; // 30：默认I帧间隔30帧
        uint32_t entropy = ENTROPY_CAVLC; // baseline仅支持CAVLC编码
    };
    InnoEncodeParam m_encodeParam {};
    std::unique_ptr<VaEncInno> m_encoder {nullptr};
    void InitEncodeParam();

    class GpuBufferFence {
    public:
        GpuBufferFence() = default;
        ~GpuBufferFence() = default;
        void Lock();
        void Unlock();
        void Wait();
    private:
        std::mutex m_lock {};
        std::condition_variable m_control {};
        bool m_ready {true};
    };

    // Buffer相关
    struct GpuEncoderBufferInno : public GpuEncoderBuffer {
        bool mapped = false;
        bool external = false;
        uint32_t slot = 0;
        int32_t fd = 0;
        VACodedBufferSegment *bufList = nullptr;
        bool needWaitFence = false;
        GpuBufferFence fence;
    };
    using GpuEncoderBufferInnoT = GpuEncoderBufferInno *;
    std::set<GpuEncoderBufferT> m_buffers {};

    bool CheckAndLockStatus(Status status);
    void UnlockStatus(Status status);
    void ReleaseAllBuffer();
    uint32_t MapStreamBuffer(GpuEncoderBufferInnoT &buffer);

    // RGB转YUV相关
    struct InnoYuvLib {
        void *lib = nullptr;
        std::function<InnoConvertHandle(EglInfoT, uint32_t *)> init = {};
        std::function<void(InnoConvertHandle)> deinit = {};
        std::function<int(InnoConvertHandle, const IfbcFrameT, IfbcFrameT)> convert = {};
    };
    InnoYuvLib m_innoYuvLib {};
    InnoConvertHandle m_convertHandle {nullptr};
    bool LoadInnoLib();
    bool UnLoadInnoLib();

    IfbcFrame m_inFrame;
    IfbcFrame m_outFrame;
    std::packaged_task<bool()> m_convertTask {};
    bool DoConvert(GpuEncoderBufferInnoT inBuffer, GpuEncoderBufferInnoT outBuffer);
    void ConvertThreadFunc();
    bool m_hasNewFrame { false };
    std::mutex m_convertLock {};
    std::condition_variable m_convertCtl {};
    std::thread m_convertThread; // 转换操作在OpenGL中完成，需要保证在同一个线程

    // 类参数
    GpuEncoder::FrameSize m_size {};
    Status m_status { Status::INVALID };
    Status m_originalStatus { Status::INVALID };
    std::mutex m_lock {};
};
}
}
#endif