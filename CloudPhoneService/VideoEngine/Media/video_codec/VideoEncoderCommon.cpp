/*
 * 功能说明: 编码参数公共部分代码
 */
#define LOG_TAG "VideoEncoderCommon"
#include "VideoEncoderCommon.h"
#include <string>
#include <unordered_map>
#include <atomic>
#include "MediaLog.h"
#include "Property.h"

/**
    * @功能描述: 设置编码参数
    * @返回值: VIDEO_ENCODER_SUCCESS 成功
    *          VIDEO_ENCODER_SET_ENCODE_PARAMS_FAIL 设置编码参数失败
    */
EncoderRetCode VideoEncoderCommon::SetEncodeParams()
{
    if (EncodeParamsChange()) {
        if (EncodeParamsChangeNoReset()) {
            m_bitChangeFlag = true;
        }
        m_encParams = m_tmpEncParams;
        m_paramsChangeFlag = true;
        INFO("Handle encoder config change: [bitrate, gopsize, profile] = [%u,%u,%s]",
            m_encParams.bitrate, m_encParams.gopsize, m_encParams.profile.c_str());
    } else {
        INFO("Using encoder config: [bitrate, gopsize, profile] = [%u,%u,%s]",
            m_encParams.bitrate, m_encParams.gopsize, m_encParams.profile.c_str());
    }
    return VIDEO_ENCODER_SUCCESS;
}

/**
    * @功能描述: 判断编码参数是否改变
    */
bool VideoEncoderCommon::EncodeParamsChange()
{
    return (m_tmpEncParams.bitrate != m_encParams.bitrate) || (m_tmpEncParams.gopsize != m_encParams.gopsize) ||
        (m_tmpEncParams.profile != m_encParams.profile) || (m_tmpEncParams.width != m_encParams.width) ||
        (m_tmpEncParams.height != m_encParams.height) || (m_tmpEncParams.framerate != m_encParams.framerate);
}

/**
    * @功能描述: 判断编码参数修改是否需要重启编码器
    */
bool VideoEncoderCommon::EncodeParamsChangeNoReset()
{
    return (m_tmpEncParams.bitrate != m_encParams.bitrate) && (m_tmpEncParams.gopsize == m_encParams.gopsize) &&
        (m_tmpEncParams.profile == m_encParams.profile) && (m_tmpEncParams.width == m_encParams.width) &&
        (m_tmpEncParams.height == m_encParams.height) && (m_tmpEncParams.framerate == m_encParams.framerate);
}

/**
    * @功能描述: 获取ro编码参数
    * @返回值: true 成功
    *          false 失败
    */
bool VideoEncoderCommon::GetRoEncParam()
{
    if (!VerifyEncodeRoParams(m_videoParams.width, m_videoParams.height, m_videoParams.framerate)) {
        ERR("encoder params is not supported");
        return false;
    }

    m_tmpEncParams.width = m_videoParams.width;
    m_tmpEncParams.height = m_videoParams.height;
    m_tmpEncParams.framerate = m_videoParams.framerate;
    return true;
}

/**
    * @功能描述: 获取Persist编码参数
    * @返回值: true 成功
    *          false 失败
    */
bool VideoEncoderCommon::GetPersistEncParam()
{
    std::string bitrate = std::to_string(m_videoParams.bitrate);
    std::string gopsize = std::to_string(m_videoParams.gopsize);
    std::string profile = m_videoParams.profile;

    if (!VerifyEncodeParams(bitrate, gopsize, profile)) {
        ERR("Verify encode params faied, bitrate: %s, gopSize: %s, profile: %s", bitrate.c_str(), gopsize.c_str(),
            profile.c_str());
        return false;
    }
    m_tmpEncParams.bitrate = StrToInt(bitrate);
    m_tmpEncParams.gopsize = StrToInt(gopsize);
    m_tmpEncParams.profile = profile;
    return true;
}

/**
    * @功能描述: 校验编码参数合法性
    * @参数 [in] width 屏幕宽,height 屏幕高,framerate 帧率
    * @返回值: true 成功
    *          false 失败
    */
bool VideoEncoderCommon::VerifyEncodeRoParams(int32_t width, int32_t height, int32_t framerate)
{
    bool isEncodeParamsTrue = true;

    if (width > height) {
        if ((width > LANDSCAPE_WIDTH_MAX) || (height > LANDSCAPE_HEIGHT_MAX) ||
            (width < LANDSCAPE_WIDTH_MIN) || (height < LANDSCAPE_HEIGHT_MIN)) {
            ERR("Invalid value[%dx%d] for [width,height], check failed!", width, height);
            isEncodeParamsTrue = false;
        }
    } else {
        if ((width > PORTRAIT_WIDTH_MAX) || (height > PORTRAIT_HEIGHT_MAX) ||
            (width < PORTRAIT_WIDTH_MIN) || (height < PORTRAIT_HEIGHT_MIN)) {
            ERR("Invalid value[%dx%d] for[width,height], check failed!", width, height);
            isEncodeParamsTrue = false;
        }
    }

    if ((framerate != FRAMERATE_MIN) && (framerate != FRAMERATE_MAX)) {
        ERR("Invalid value[%d] for [framerate], check failed!", framerate);
        isEncodeParamsTrue = false;
    }
    return isEncodeParamsTrue;
}

/**
    * @功能描述: 校验编码参数合法性
    * @参数 [in] bitrate 码率,gopsize gop间隔,profile 配置
    * @返回值: true 成功
    *          false 失败
    */
bool VideoEncoderCommon::VerifyEncodeParams(std::string &bitrate, std::string &gopsize, std::string &profile)
{
    bool isEncodeParamsTrue = true;
    if ((StrToInt(bitrate) < BITRATE_MIN) || (StrToInt(bitrate) > BITRATE_MAX)) {
        WARN("Invalid property value[%s] for property[bitrate], use last correct encode bitrate[%u]",
            bitrate.c_str(), m_encParams.bitrate);
        isEncodeParamsTrue = false;
    }

    if ((StrToInt(gopsize) < GOPSIZE_MIN) || (StrToInt(gopsize) > GOPSIZE_MAX)) {
    WARN("Invalid property value[%s] for property[gopsize], use last correct encode gopsize[%u]",
        gopsize.c_str(), m_encParams.gopsize);
    isEncodeParamsTrue = false;
    }

    if ((m_codecFormat == ENCODER_TYPE_OPENH264) || (m_codecFormat == ENCODER_TYPE_NETINTH264) ||
        (m_codecFormat == ENCODER_TYPE_VASTAIH264) || (m_codecFormat == ENCODER_TYPE_QUADRAH264)) {
        if (profile != ENCODE_PROFILE_BASELINE &&
            profile != ENCODE_PROFILE_MAIN &&
            profile != ENCODE_PROFILE_HIGH) {
            WARN("Invalid property value[%s] for property[profile], use last correct encode profile[%s]",
                profile.c_str(), m_encParams.profile.c_str());
            isEncodeParamsTrue = false;
            }
    } else if ((m_codecFormat == ENCODER_TYPE_NETINTH265) || (m_codecFormat == ENCODER_TYPE_VASTAIH265) ||
               (m_codecFormat == ENCODER_TYPE_QUADRAH265)) {
                if (profile != ENCODE_PROFILE_MAIN) {
                    WARN("Invalid property value[%s] for property[profile], use last correct encode profile[%s]",
                        profile.c_str(), m_encParams.profile.c_str());
                    isEncodeParamsTrue = false;
                }
        }

    return isEncodeParamsTrue;
}

/**
* @功能描述: 单帧编码参数校验
*/
EncoderRetCode VideoEncoderCommon::EncodeParamsCheck()
{
    if (!m_videoParamsChangeFlag) {
        return VIDEO_ENCODER_SUCCESS;
    }
    if ((!GetRoEncParam()) || (!GetPersistEncParam())) {
        ERR("init encoder failed: GetEncParam failed");
        return VIDEO_ENCODER_INIT_FAIL;
    }
    SetEncodeParams();
    m_videoParamsChangeFlag = false;    // 重置视频流模式下的修改标志位
    return VIDEO_ENCODER_SUCCESS;
}

EncoderRetCode VideoEncoderCommon::GeneralConfig(const VmiEncoderConfig& config)
{
    m_videoParams.width = config.width;
    m_videoParams.height = config.height;
    m_videoParams.framerate = config.frameRate;
    m_videoParams.bitrate = config.encodeParams.bitRate;
    m_videoParams.gopsize = config.encodeParams.gopSize;
    if (config.encodeParams.profile == ProfileType::BASELINE_PROFILE) {
        m_videoParams.profile = ENCODE_PROFILE_BASELINE;
    } else if (config.encodeParams.profile == ProfileType::MAIN_PROFILE) {
        m_videoParams.profile = ENCODE_PROFILE_MAIN;
    } else if (config.encodeParams.profile == ProfileType::HIGH_PROFILE) {
        m_videoParams.profile = ENCODE_PROFILE_HIGH;
    }
    m_nextKeyFrameDistance = config.encodeParams.keyFrame;
    m_paramsChangeFlag = false;
    INFO("VideoCodec Config width: %u, height: %u, framerate: %u bitRate: %u, gopSize: %u, profile: %u,"
        "keyFrame: %u", config.width, config.height, config.frameRate, config.encodeParams.bitRate,
        config.encodeParams.gopSize, config.encodeParams.profile, config.encodeParams.keyFrame);
    return VIDEO_ENCODER_SUCCESS;
}

EncoderRetCode VideoEncoderCommon::GeneralSetParams(const VmiEncoderParams& params)
{
    std::string tmpProfile = ENCODE_PROFILE_MAIN;
    if (params.profile == ProfileType::BASELINE_PROFILE) {
        tmpProfile = ENCODE_PROFILE_BASELINE;
    } else if (params.profile == ProfileType::MAIN_PROFILE) {
        tmpProfile = ENCODE_PROFILE_MAIN;
    } else if (params.profile == ProfileType::HIGH_PROFILE) {
        tmpProfile = ENCODE_PROFILE_HIGH;
    }
    if (tmpProfile != m_videoParams.profile) {
        m_videoParams.profile = tmpProfile;
        m_videoParamsChangeFlag = true;
    }
    if (params.bitRate != m_videoParams.bitrate) {
        m_videoParams.bitrate = params.bitRate;
        m_videoParamsChangeFlag = true;
    }
    if (params.gopSize != m_videoParams.gopsize) {
        m_videoParams.gopsize = params.gopSize;
        m_videoParamsChangeFlag = true;
    }
    m_nextKeyFrameDistance = params.keyFrame;
    INFO("VideoCodec SetParams bitRate: %u, gopSize: %u, profile: %u, keyFrame: %u", params.bitRate,
        params.gopSize, params.profile, params.keyFrame);
    return VIDEO_ENCODER_SUCCESS;
}

bool VideoEncoderCommon::VerifyParamsBeforeEncode()
{
    if (EncodeParamsCheck() != VIDEO_ENCODER_SUCCESS) {
        ERR("init encoder failed: GetEncParam failed");
        return false;
    }

    if (m_paramsChangeFlag) {
        if (ResetEncoder() != VIDEO_ENCODER_SUCCESS) {
            ERR("reset encoder failed while encoding");
            return false;
        }
        m_paramsChangeFlag = false;
    }
    return true;
}