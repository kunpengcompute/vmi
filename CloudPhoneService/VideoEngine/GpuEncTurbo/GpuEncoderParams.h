/*
 * 版权所有 (c) 华为技术有限公司 2023-2023
 * 功能描述：视频流编码器编码参数相关定义
 */

#ifndef GPU_ENCODER_PARAMS
#define GPU_ENCODER_PARAMS

#include <cstdint>

namespace Vmi {
namespace GpuEncoder {
/**
 * Rate control 模式，支持CBR/VBR
 */
enum EncoderRateControl : uint32_t {
    ENC_RC_CBR,  // 恒定比特率。视觉质量稳定性：不稳定；即时输出码率：恒定；输出文件大小：可控
    ENC_RC_VBR  // 动态比特率。视觉质量稳定性：稳定；即时输出码率：变化；输出文件大小：不可控
};

/**
 * 画质级别
 */
enum EncoderProfileIdc : uint32_t {
    ENC_PROFILE_IDC_BASELINE = 66,  // 基本画质。支持I/P 帧，只支持无交错（Progressive）和CAVLC
    ENC_PROFILE_IDC_MAIN = 77,      // 主流画质。提供I/P/帧，支持无交错（Progressive），也支持CAVLC 和CABAC 的支持
    ENC_PROFILE_IDC_HIGH = 100,
    ENC_PROFILE_IDC_HEVC_MAIN = 1
};

enum EncodeParamIndex : uint32_t {
    ENCODE_PARAM_NONE,
    ENCODE_PARAM_RATE_CONTROL,
    ENCODE_PARAM_BITRATE,
    ENCODE_PARAM_FRAMERATE,
    ENCODE_PARAM_GOPSIZE,
    ENCODE_PARAM_PROFILE,
    ENCODE_PARAM_KEYFRAME,
    ENCODE_PARAM_MAX
};

struct EncodeParamBase {
    explicit EncodeParamBase(EncodeParamIndex index) : id(index) {}
    EncodeParamIndex id;
};

struct EncodeParamRateControl : public EncodeParamBase {
    EncodeParamRateControl() : EncodeParamBase(ENCODE_PARAM_RATE_CONTROL) {}
    uint32_t rateControl = 0;
};

struct EncodeParamBitRate : public EncodeParamBase {
    EncodeParamBitRate() : EncodeParamBase(ENCODE_PARAM_BITRATE) {}
    uint32_t bitRate = 0;
};

struct EncodeParamFrameRate : public EncodeParamBase {
    EncodeParamFrameRate() : EncodeParamBase(ENCODE_PARAM_FRAMERATE) {}
    uint32_t frameRate;
};

struct EncodeParamGopsize : public EncodeParamBase {
    EncodeParamGopsize() : EncodeParamBase(ENCODE_PARAM_GOPSIZE) {}
    uint32_t gopSize;
};

struct EncodeParamProfile : public EncodeParamBase {
    EncodeParamProfile() : EncodeParamBase(ENCODE_PARAM_PROFILE) {}
    uint32_t profile;
};

struct EncodeParamKeyframe : public EncodeParamBase {
    EncodeParamKeyframe() : EncodeParamBase(ENCODE_PARAM_KEYFRAME) {}
    uint32_t n;
};

using EncodeParamT = EncodeParamBase *;
}
}
#endif