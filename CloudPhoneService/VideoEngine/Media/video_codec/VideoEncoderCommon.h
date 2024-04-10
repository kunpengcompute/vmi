/*
 * 功能说明: 编码参数公共部分代码
 */
#ifndef VIDEO_ENCODER_COMMON_H
#define VIDEO_ENCODER_COMMON_H

#include <string>
#include <unordered_map>
#include <atomic>
#include "VideoCodecApi.h"

namespace {
    constexpr int32_t PORTRAIT_WIDTH_MAX = 4320;
    constexpr int32_t PORTRAIT_WIDTH_MIN = 144;
    constexpr int32_t PORTRAIT_HEIGHT_MAX = 7680;
    constexpr int32_t PORTRAIT_HEIGHT_MIN = 176;
    constexpr int32_t LANDSCAPE_WIDTH_MAX = 7680;
    constexpr int32_t LANDSCAPE_WIDTH_MIN = 176;
    constexpr int32_t LANDSCAPE_HEIGHT_MAX = 4320;
    constexpr int32_t LANDSCAPE_HEIGHT_MIN = 144;
    constexpr uint32_t DEFAULT_WIDTH = 720;
    constexpr uint32_t DEFAULT_HEIGHT = 1280;
    constexpr uint32_t FRAMERATE_MIN = 30;
    constexpr uint32_t FRAMERATE_MAX = 60;
    constexpr uint32_t GOPSIZE_MIN = 30;
    constexpr uint32_t GOPSIZE_MAX = 3000;
    constexpr uint32_t BITRATE_MIN = 1000000;
    constexpr uint32_t BITRATE_MAX = 10000000;
    constexpr uint32_t BITRATE_DEFAULT_264 = 5000000;
    constexpr uint32_t BITRATE_DEFAULT_265 = 3000000;
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
     * @功能描述: 判断编码参数是否改变
     */
    bool EncodeParamsChange();

    /**
     * @功能描述: 判断编码参数改变时是否需要重启编码器
     */
    bool EncodeParamsChangeNoReset();

        /**
     * @功能描述: 获取ro编码参数
     * @返回值: true 成功
     *          false 失败
     */
    bool GetRoEncParam();

    /**
     * @功能描述: 获取Persist编码参数
     * @返回值: true 成功
     *          false 失败
     */
    bool GetPersistEncParam();

    /**
     * @功能描述: 校验编码参数合法性
     * @参数 [in] width 屏幕宽,height 屏幕高,framerate 帧率
     * @返回值: true 成功
     *          false 失败
     */
    bool VerifyEncodeRoParams(int32_t width, int32_t height, int32_t framerate);

    /**
     * @功能描述: 校验编码参数合法性
     * @参数 [in] bitrate 码率,gopsize gop间隔,profile 配置
     * @返回值: true 成功
     *          false 失败
     */
    bool VerifyEncodeParams(std::string &bitrate, std::string &gopsize, std::string &profile);

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
    // 编码参数
    struct EncodeParams {
        uint32_t framerate = 0;
        uint32_t bitrate = 0;    // 编码输出码率
        uint32_t gopsize = 0;    // 关键帧间隔
        std::string profile = "";    // 编码档位
        uint32_t width = 0;      // 编码输入/输出宽度
        uint32_t height = 0;     // 编码输入/输出高度

        bool operator==(const EncodeParams &rhs) const
        {
            return (framerate == rhs.framerate) && (bitrate == rhs.bitrate) && (gopsize == rhs.gopsize) &&
                (profile == rhs.profile) && (width == rhs.width) && (height == rhs.height);
        }
    };

    EncoderFormat m_codecFormat { ENCODER_TYPE_OPENH264 };
    EncodeParams m_encParams = {static_cast<uint32_t>(FRAMERATE_MIN), static_cast<uint32_t>(BITRATE_DEFAULT_264),
        static_cast<uint32_t>(GOPSIZE_MIN), ENCODE_PROFILE_BASELINE, static_cast<uint32_t>(DEFAULT_WIDTH),
        static_cast<uint32_t>(DEFAULT_HEIGHT)};
    EncodeParams m_tmpEncParams = {static_cast<uint32_t>(FRAMERATE_MIN), static_cast<uint32_t>(BITRATE_DEFAULT_264),
        static_cast<uint32_t>(GOPSIZE_MIN), ENCODE_PROFILE_BASELINE, static_cast<uint32_t>(DEFAULT_WIDTH),
        static_cast<uint32_t>(DEFAULT_HEIGHT)};
    EncodeParams m_videoParams = {static_cast<uint32_t>(FRAMERATE_MIN), static_cast<uint32_t>(BITRATE_DEFAULT_264),
        static_cast<uint32_t>(GOPSIZE_MIN), ENCODE_PROFILE_BASELINE, static_cast<uint32_t>(DEFAULT_WIDTH),
        static_cast<uint32_t>(DEFAULT_HEIGHT)};
    std::atomic<bool> m_videoParamsChangeFlag { false };
    std::atomic<uint32_t> m_nextKeyFrameDistance { 0 };

    std::atomic<bool> m_paramsChangeFlag = { false };
    std::atomic<bool> m_bitChangeFlag = { false };
};

#endif  // VIDEO_ENCODER_COMMON_H
