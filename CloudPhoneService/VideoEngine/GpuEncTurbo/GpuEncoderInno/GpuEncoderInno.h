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
#include "InnoFbcApi.h"

namespace Vmi {
namespace GpuEncoder {
struct InnoEncodeParams {
    uint32_t frameRate = 0;
    uint32_t gopSize = 0;
    uint32_t bitRate = 0;
    uint32_t keyFrame = 0;
    uint32_t profile = 0;
    uint32_t streamWidth = 0;
    uint32_t streamHeight = 0;
    uint32_t crf = UINT32_MAX;
    uint32_t maxCrfRate = UINT32_MAX;
    uint32_t rcMode = UINT32_MAX;
};

class GpuEncoderInno : public GpuEncoderBase {
public:
    int32_t Init(EncoderConfig &config) override;
    int32_t ResetImgSize(uint32_t width, uint32_t height) override;

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

    // // va编码相关
    // struct InnoEncodeParam {
    //     uint32_t profile = VAProfileH264ConstrainedBaseline; // 默认baseline profile
    //     uint32_t vbrMode = false; // 默认使用固定码率模式
    //     uint32_t bitrate = 5000000; // 5000000：默认使用5Mbps码率
    //     uint32_t frameRate = 30; // 30：默认30帧
    //     uint32_t gopSize = 30; // 30：默认I帧间隔30帧
    //     uint32_t entropy = ENTROPY_CAVLC; // baseline仅支持CAVLC编码
    // };
    // InnoEncodeParam m_encodeParam {};
    // void InitEncodeParam();

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
        int32_t fd = -1;           // DMA-BUF fd
        bool external = false;     // 是否是外部导入
    };

    using GpuEncoderBufferInnoT = GpuEncoderBufferInno *;
    std::set<GpuEncoderBufferT> m_buffers {};

    bool CheckAndLockStatus(Status status);
    void UnlockStatus(Status status);
    void ReleaseAllBuffer();

    bool LoadInnoLib();
    void UnLoadInnoLib();

    typedef void* ienc_encoder_t;

    void* m_iencLibHandle {nullptr};

    using IencOpenEncoder = ienc_encoder_t(*)(const ienc_attr_t*);
    using IencCloseEncoder = void(*)(ienc_encoder_t);
    using IencEncodeOneFrame = int(*)(ienc_encoder_t, ienc_frame_t*);
    using IencGetFrame = int(*)(ienc_encoder_t, int32_t*, ienc_stream_t*, int32_t);
    using IencReleaseFrame = void(*)(ienc_encoder_t, const ienc_stream_t*);

    IencOpenEncoder m_iencOpenEncoder {nullptr};
    IencCloseEncoder m_iencCloseEncoder {nullptr};
    IencEncodeOneFrame m_iencEncodeOneFrame {nullptr};
    IencGetFrame m_iencGetFrame {nullptr};
    IencReleaseFrame m_iencReleaseFrame {nullptr};

    void SetFrameRate(EncodeParamT &param, InnoEncodeParams &params);
    void SetBitRate(EncodeParamT &param, InnoEncodeParams &params);
    void SetGopsize(EncodeParamT &param, InnoEncodeParams &params);
    void SetKeyFrame(InnoEncodeParams &params);
    void SetProfile(EncodeParamT &param, InnoEncodeParams &params);
    void SetRcmode(EncodeParamT &param, InnoEncodeParams &params);
    void SetStreamWidth(EncodeParamT &param, InnoEncodeParams &params);
    void SetStreamHeight(EncodeParamT &param, InnoEncodeParams &params);
    void SetCrfLevel(EncodeParamT &param, InnoEncodeParams &params);
    void SetMaxCrfRate(EncodeParamT &param, InnoEncodeParams &params);
    void UpdateSettingParams();
    ienc_profile_e ConvertProfile(uint32_t profileCode);
    ienc_rc_mode_e ConvertRcMode(uint32_t rcMode);
    void ApplyParamsToAttr();

    // 类参数
    GpuEncoder::FrameSize m_size {};
    Status m_status { Status::INVALID };
    Status m_originalStatus { Status::INVALID };
    std::mutex m_lock {};

    ienc_encoder_t m_iencEncoder {nullptr};
    ienc_attr_t m_iencAttr {};
    
    // bool m_needRestart = false;
    // bool m_needSetWidthOrHeight = false;
    InnoEncodeParams m_settingParams {30, 30, 5000000, 0, Vmi::GpuEncoder::ENC_PROFILE_IDC_MAIN,
        0, 0, 21, 10000000, 0};
    InnoEncodeParams m_receiveParams;
};
}
}
#endif