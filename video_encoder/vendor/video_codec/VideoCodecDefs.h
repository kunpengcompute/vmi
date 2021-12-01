/*
 * 功能说明: 视频编码器通用类型定义
 */
#ifndef VIDEO_CODEC_DEFS_H
#define VIDEO_CODEC_DEFS_H

#include <cstdint>

// 编码器返回码
enum EncoderRetCode : uint32_t {
    VIDEO_ENCODER_SUCCESS                = 0x00,
    VIDEO_ENCODER_CREATE_FAIL            = 0x01,  // 创建编码器失败
    VIDEO_ENCODER_INIT_FAIL              = 0x02,  // 初始化编码器失败
    VIDEO_ENCODER_START_FAIL             = 0x03,  // 启动编码器失败
    VIDEO_ENCODER_ENCODE_FAIL            = 0x04,  // 编码失败
    VIDEO_ENCODER_STOP_FAIL              = 0x05,  // 停止编码器失败
    VIDEO_ENCODER_DESTROY_FAIL           = 0x06,  // 销毁编码器失败
    VIDEO_ENCODER_REGISTER_FAIL          = 0x07,  // 注册函数失败
    VIDEO_ENCODER_RESET_FAIL             = 0x08,  // 重置编码器失败
    VIDEO_ENCODER_FORCE_KEY_FRAME_FAIL   = 0x09,  // 强制I帧失败
    VIDEO_ENCODER_SET_ENCODE_PARAMS_FAIL = 0x0A   // 设置编码参数失败
};

// 编码档位
enum EncodeProfiles : uint32_t {
    ENCODE_PROFILE_BASELINE,
    ENCODE_PROFILE_MAIN,
    ENCODE_PROFILE_HIGH
};

// 编码参数
struct EncodeParams {
    uint32_t frameRate = 0;  // 编码输入帧率
    uint32_t bitrate = 0;    // 编码输出码率
    uint32_t gopSize = 0;    // 关键帧间隔
    uint32_t profile = 0;    // 编码档位
    uint32_t width = 0;      // 编码输入/输出宽度
    uint32_t height = 0;     // 编码输入/输出高度

    bool operator==(const EncodeParams &rhs) const
    {
        return (frameRate == rhs.frameRate) && (bitrate == rhs.bitrate) && (gopSize == rhs.gopSize) &&
            (profile == rhs.profile) && (width == rhs.width) && (height == rhs.height);
    }
};

// 编码器类型
enum EncoderType : uint32_t {
    ENCODER_TYPE_OPENH264 = 0x01,    // 开源OpenH264编码器
    ENCODER_TYPE_NETINTH264 = 0x02,  // NETINT h.264硬件编码器
    ENCODER_TYPE_NETINTH265 = 0x03   // NETINT h.265硬件编码器
};

#endif  // VIDEO_CODEC_DEFS_H
