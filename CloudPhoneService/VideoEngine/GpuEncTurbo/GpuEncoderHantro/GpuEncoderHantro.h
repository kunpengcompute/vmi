/*
 * 版权所有 (c) 华为技术有限公司 2023-2023
 * 功能描述：瀚博GPU编码模块对外接口
 */

#ifndef GPU_ENCODER_HANTRO
#define GPU_ENCODER_HANTRO

#include <functional>
#include <mutex>
#include <set>
#include <map>
#include <queue>
#include <memory>
#include <cstdint>
extern "C" {
    #include "libavcodec/avcodec.h"
    #include "libavutil/opt.h"
    #include "libavutil/imgutils.h"
}
#include "HantroCaptureWrapperBase.h"
#include "../GpuEncoderBase.h"

namespace Vmi {
namespace GpuEncoder {
struct HantroEncodeParams {
    uint32_t frameRate = 0;
    uint32_t gopSize = 0;
    uint32_t bitRate = 0;
    uint32_t keyFrame = 0;
    uint32_t profile = 0;
};
class GpuEncoderHantro : public GpuEncoderBase {
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

    // Buffer相关
    struct GpuEncoderBufferHantro : public GpuEncoderBuffer {
        bool external = false;
        bool mapped = false;
        bool asyncWaited = false;
        int acquireFence = -1;
        HantroCaptureBuffer* captureBuffer = nullptr;
        AVFrame *frame = nullptr;
        AVPacket *pkt = nullptr;
        std::unique_ptr<uint8_t[]> streamBuffer = nullptr;
        uint32_t streamBufferSize = 0;
    };
    using GpuEncoderBufferHantroT = GpuEncoderBufferHantro *;

    bool CheckAndLockStatus(Status status);
    void UnlockStatus(Status status);

    // Init
    bool AllocContext(AVCodecContext *&ctx, AVCodec *&codec, EncoderConfig &config);
    bool CreateAndSetHwDeviceCtx(AVCodecContext *&ctx, AVCodec *codec);
    int SetHWFrameCtx(AVCodecContext *ctx, AVBufferRef *hw_device_ctx, AVPixelFormat pixelFormat, int flag);

    // Encode
    int EncodeParamSetAndEncode(GpuEncoderBufferT &inBuffer, GpuEncoderBufferT &outBuffer);
    int UseFFmpegtoEncode(GpuEncoderBufferHantroT &inHantroBuff, GpuEncoderBufferHantroT &outHantroBuff);
    void UpdateSettingParams();
    bool DynamicAdjustParam(GpuEncoderBufferHantroT rgbBuffer, HantroEncodeParams &setParams);

    // AVcodec
    AVCodecContext* m_avcodec;
    bool CreateHwFrame(AVFrame *&frame);

    // params legal-check in gputurbo caller
    void SetFrameRate(EncodeParamT &param, HantroEncodeParams &params);
    void SetBitRate(EncodeParamT &param, HantroEncodeParams &params);
    void SetGopsize(EncodeParamT &param, HantroEncodeParams &params);
    void SetKeyFrame(HantroEncodeParams &params);
    void SetProfile(EncodeParamT &param, HantroEncodeParams &params);
    uint32_t ConvertProfileCodeToString(uint32_t profileCode, std::string &profile); // convert profileCode to string

    std::set<GpuEncoderBufferT> m_buffers {};
    // 类参数
    GpuEncoder::FrameSize m_size {};
    GpuEncoder::EncoderCapability m_capability {};
    Status m_status { Status::INVALID };
    Status m_originalStatus { Status::INVALID };
    std::mutex m_lock {};
    // Async
    int m_fenceTimeline = -1;
    unsigned int m_fenceValue = 1;
    const int m_acquireWaitTime = 35;    // ms

    // 编码参数
    bool m_needRestart = false;
    bool m_dynamicAdjustParamFlag = false;
    HantroEncodeParams m_settingParams {30, 30, 5000000, 0, Vmi::GpuEncoder::ENC_PROFILE_IDC_BASELINE};
    HantroEncodeParams m_receiveParams;
};
}
}
#endif