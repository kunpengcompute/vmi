/*
 * 功能说明: 编码参数公共部分代码
 */
#define LOG_TAG "VideoEncoderCommon"
#include "VideoEncoderCommon.h"
#include <string>
#include "logging.h"

/**
    * @功能描述: 设置编码参数
    * @返回值: VIDEO_ENCODER_SUCCESS 成功
    *          VIDEO_ENCODER_SET_ENCODE_PARAMS_FAIL 设置编码参数失败
    */
EncoderRetCode VideoEncoderCommon::SetEncodeParams()
{
    if (EncodeParamsChangeNoReset()) {
        m_paramsChangeNoResetFlag = true;
    }
    m_encParams = m_videoParams;
    m_paramsChangeFlag = true;
    return VIDEO_ENCODER_SUCCESS;
}

/**
    * @功能描述: 判断编码参数修改是否需要重启编码器
    */
bool VideoEncoderCommon::EncodeParamsChangeNoReset()
{
    return ((m_videoParams.bitrate != m_encParams.bitrate) || (m_videoParams.maxCrfRate != m_encParams.maxCrfRate) ||
        (m_videoParams.crf != m_encParams.crf) || (m_videoParams.vbvBufferSize != m_encParams.vbvBufferSize)) &&
        (m_videoParams.gopsize == m_encParams.gopsize) && (m_videoParams.framerate == m_encParams.framerate) &&
        (m_videoParams.profile == m_encParams.profile) && (m_videoParams.width == m_encParams.width) &&
        (m_videoParams.height == m_encParams.height) && (m_videoParams.rcMode == m_encParams.rcMode);
}

/**
* @功能描述: 单帧编码参数校验
*/
EncoderRetCode VideoEncoderCommon::EncodeParamsCheck()
{
    if (!m_videoParamsChangeFlag) {
        return VIDEO_ENCODER_SUCCESS;
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
    m_videoParams.rcMode = config.encodeParams.rcMode;
    m_videoParams.crf = config.encodeParams.crf;
    m_videoParams.maxCrfRate = config.encodeParams.maxCrfRate;
    m_videoParams.vbvBufferSize = config.encodeParams.vbvBufferSize;
    if (config.encodeParams.profile == ProfileType::BASELINE_PROFILE) {
        m_videoParams.profile = ENCODE_PROFILE_BASELINE;
    } else if (config.encodeParams.profile == ProfileType::MAIN_PROFILE) {
        m_videoParams.profile = ENCODE_PROFILE_MAIN;
    } else if (config.encodeParams.profile == ProfileType::HIGH_PROFILE) {
        m_videoParams.profile = ENCODE_PROFILE_HIGH;
    }
    m_nextKeyFrameDistance = config.encodeParams.keyFrame;
    m_paramsChangeFlag = false;
    INFO("VideoCodec Config width: %u, height: %u, framerate: %u bitRate: %u, gopSize: %u, profile: %u, keyFrame: %u,"
        "rcMode: %u, crf: %u, maxCrfRate: %u, vbvBufferSize: %d", 
        config.width, config.height, config.frameRate, config.encodeParams.bitRate, config.encodeParams.gopSize,
        config.encodeParams.profile, config.encodeParams.keyFrame,config.encodeParams.rcMode, 
        config.encodeParams.crf, config.encodeParams.maxCrfRate, config.encodeParams.vbvBufferSize);
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
    if (tmpProfile != m_videoParams.profile || params.bitRate != m_videoParams.bitrate ||
        params.gopSize != m_videoParams.gopsize || params.rcMode != m_videoParams.rcMode ||
        params.crf != m_videoParams.crf || params.maxCrfRate != m_videoParams.maxCrfRate ||
        params.vbvBufferSize != m_videoParams.vbvBufferSize) {
        m_videoParamsChangeFlag = true;
    }
    m_videoParams.profile = tmpProfile;
    m_videoParams.bitrate = params.bitRate;
    m_videoParams.gopsize = params.gopSize;
    m_videoParams.rcMode = params.rcMode;
    m_videoParams.crf = params.crf;
    m_videoParams.maxCrfRate = params.maxCrfRate;
    m_videoParams.vbvBufferSize = params.vbvBufferSize;
    m_nextKeyFrameDistance = params.keyFrame;
    INFO("VideoCodec SetParams bitRate: %u, gopSize: %u, profile: %u, keyFrame: %u,"
        "rcMode: %u, crf: %u, maxCrfRate: %u, vbvBufferSize: %d",
        params.bitRate, params.gopSize, params.profile, params.keyFrame,
        params.rcMode, params.crf, params.maxCrfRate, params.vbvBufferSize);
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