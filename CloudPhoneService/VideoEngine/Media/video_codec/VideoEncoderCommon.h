/*
 * 功能说明: 编码参数公共部分代码
 */
#ifndef VIDEO_ENCODER_COMMON_H
#define VIDEO_ENCODER_COMMON_H

#include <string>
#include <atomic>
#include "VideoCodecApi.h"

namespace {
    constexpr uint32_t DEFAULT_WIDTH = 720;
    constexpr uint32_t DEFAULT_HEIGHT = 1280;
    constexpr uint32_t DEFAULT_FRAMERATE = 30;
    constexpr uint32_t DEFAULT_GOPSIZE = 30;
    constexpr uint32_t BITRATE_DEFAULT_264 = 5000000;
    constexpr uint32_t BITRATE_DEFAULT_265 = 3000000;
    constexpr uint32_t DEFAULT_RCMODE = 2;
    constexpr uint32_t CBR_RCMODE = 2;
    constexpr uint32_t CAPPED_CRF_RCMODE = 3;
    constexpr uint32_t DEFAULT_CRF = 34;
    constexpr uint32_t DEFAULT_MAX_CRFRATE = 20000000;
    constexpr int32_t DEFAULT_VBV_BUFFER_SIZE = 1000;
    const std::string ENCODE_PROFILE_BASELINE = "baseline";
    const std::string ENCODE_PROFILE_MAIN = "main";
    const std::string ENCODE_PROFILE_HIGH = "high";
    enum ProfileType : uint32_t {
        BASELINE_PROFILE = 0,
        MAIN_PROFILE = 1,
        HIGH_PROFILE = 2,
    };
}

class VideoEncoderCommon : public VideoEncoder {
public:
    /**
     * @功能描述: 默认构造函数
     */
    VideoEncoderCommon() = default;

    /**
     * @功能描述: 默认析构函数
     */
    ~VideoEncoderCommon() override = default;
    
    /**
     * @功能描述: 设置编码参数
     * @返回值: VIDEO_ENCODER_SUCCESS 成功
     *          VIDEO_ENCODER_SET_ENCODE_PARAMS_FAIL 设置编码参数失败
     */
    EncoderRetCode SetEncodeParams();

    /**
     * @功能描述: 判断编码参数改变时是否需要重启编码器
     */
    bool EncodeParamsChangeNoReset();

    /**
     * @功能描述: 单帧编码参数校验
     */
    EncoderRetCode EncodeParamsCheck();

    /**
     * @功能描述: 配置编码器参数
     * @参数 [in] config: 启动编码所需的配置参数
     * @返回值: VIDEO_ENCODER_SUCCESS 成功
     *          VIDEO_ENCODER_RESET_FAIL 配置编码器参数失败
     */
    EncoderRetCode GeneralConfig(const VmiEncoderConfig& config);

    /**
     * @功能描述: 动态修改编码参数
     * @参数 [in] params: 可动态修改的编码参数
     * @返回值: VIDEO_ENCODER_SUCCESS 成功
     *          VIDEO_ENCODER_RESET_FAIL 修改编码器参数失败
     */
    EncoderRetCode GeneralSetParams(const VmiEncoderParams& params);

    /**
     * @功能描述: 编码前对编码参数进行检查
     * @返回值: true:成功, false:失败
     */
    virtual bool VerifyParamsBeforeEncode();

protected:
    // 编码模式
    enum RCMode : uint32_t  {
        ABR,                                        // 平均码率，暂不支持
        CRF,                                        // 画质优先，暂不支持
        CBR,                                        // 恒定码率
        CAPPED_CRF,                                 // 画质优先，但限制码率，暂不支持
        RC_MODE_MAX
    };
    // 编码参数
    struct EncodeParams {
        uint32_t framerate = 0;
        uint32_t bitrate = 0;    // 编码输出码率
        uint32_t gopsize = 0;    // 关键帧间隔
        std::string profile = "";    // 编码档位
        uint32_t width = 0;      // 编码输入/输出宽度
        uint32_t height = 0;     // 编码输入/输出高度
        uint32_t rcMode = 0;                        // 流控模式
        uint32_t crf = 0;                          // crf码控级别
        uint32_t maxCrfRate = 0;             // crf码率峰值
        int32_t vbvBufferSize = 0;               // crf码率缓冲区大小
        bool operator==(const EncodeParams &rhs) const
        {
            return (framerate == rhs.framerate) && (bitrate == rhs.bitrate) && (gopsize == rhs.gopsize) &&
                (profile == rhs.profile) && (width == rhs.width) && (height == rhs.height) &&
                (rcMode == rhs.rcMode) && (crf == rhs.crf) && (maxCrfRate == rhs.maxCrfRate) &&
                (vbvBufferSize == rhs.vbvBufferSize);
        }
    };

    EncodeParams m_encParams = {DEFAULT_FRAMERATE, BITRATE_DEFAULT_264, DEFAULT_GOPSIZE, ENCODE_PROFILE_BASELINE,
        DEFAULT_WIDTH,DEFAULT_HEIGHT, DEFAULT_RCMODE, DEFAULT_CRF, DEFAULT_MAX_CRFRATE, DEFAULT_VBV_BUFFER_SIZE};
    EncodeParams m_videoParams = {DEFAULT_FRAMERATE, BITRATE_DEFAULT_264, DEFAULT_GOPSIZE, ENCODE_PROFILE_BASELINE,
        DEFAULT_WIDTH,DEFAULT_HEIGHT, DEFAULT_RCMODE, DEFAULT_CRF, DEFAULT_MAX_CRFRATE, DEFAULT_VBV_BUFFER_SIZE};
    std::atomic<bool> m_videoParamsChangeFlag { false };
    std::atomic<uint32_t> m_nextKeyFrameDistance { 0 };

    std::atomic<bool> m_paramsChangeFlag = { false };
    std::atomic<bool> m_paramsChangeNoResetFlag = { false };
};

#endif  // VIDEO_ENCODER_COMMON_H
