/*
 * 版权所有 (c) 华为技术有限公司 2021-2022
 * 功能说明: 提供视频编解码器对外接口
 */
#ifndef VIDEO_CODEC_API_H
#define VIDEO_CODEC_API_H
#include <cstdint>

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

enum EncoderFormat : uint32_t {
    ENCODER_TYPE_OPENH264 = 0,    // 开源OpenH264编码器
    ENCODER_TYPE_NETINTH264 = 1,  // NETINT h.264硬件编码器
    ENCODER_TYPE_NETINTH265 = 2,   // NETINT h.265硬件编码器
    ENCODER_TYPE_VASTAIH264 = 3,  // VASTAI h.264硬件编码器
    ENCODER_TYPE_VASTAIH265 = 4,  // VASTAI h.265硬件编码器
    ENCODER_TYPE_QUADRAH264 = 5,  // QUATRA h.264硬件编码器
    ENCODER_TYPE_QUADRAH265 = 6,  // QUATRA h.265硬件编码器
    ENCODER_TYPE_T432H264 = 7,  // T432 h.264硬件编码器
    ENCODER_TYPE_T432H265 = 8,  // T432 h.265硬件编码器
};

struct VmiEncoderParams {
    uint32_t bitRate = 3000000;
    uint32_t gopSize = 30;
    uint32_t profile = 1;
    uint32_t keyFrame = 0;
};

struct VmiEncoderConfig {
    uint32_t width = 720;
    uint32_t height = 1280;
    uint32_t frameRate = 30;
    VmiEncoderParams encodeParams;
};

class VideoEncoder {
public:
    /**
     * @功能描述: 默认构造函数
     */
    VideoEncoder() = default;

    /**
     * @功能描述: 默认析构函数
     */
    virtual ~VideoEncoder() = default;

    /**
     * @功能描述: 初始化编码器
     * @返回值: VIDEO_ENCODER_SUCCESS 成功
     *          VIDEO_ENCODER_INIT_FAIL 初始化编码器失败
     */
    virtual EncoderRetCode InitEncoder() = 0;

    /**
     * @功能描述: 启动编码器
     * @返回值: VIDEO_ENCODER_SUCCESS 成功
     *          VIDEO_ENCODER_START_FAIL 启动编码器失败
     */
    virtual EncoderRetCode StartEncoder() = 0;

    /**
     * @功能描述: 编码一帧数据
     * @参数 [in] inputData: 编码输入数据地址
     * @参数 [in] inputSize: 编码输入数据大小
     * @参数 [out] outputData: 编码输出数据地址
     * @参数 [out] outputSize: 编码输出数据大小
     * @返回值: VIDEO_ENCODER_SUCCESS 成功
     *          VIDEO_ENCODER_ENCODE_FAIL 编码一帧失败
     */
    virtual EncoderRetCode EncodeOneFrame(const uint8_t *inputData, uint32_t inputSize,
        uint8_t **outputData, uint32_t *outputSize) = 0;

    /**
     * @功能描述: 停止编码器
     * @返回值: VIDEO_ENCODER_SUCCESS 成功
     *          VIDEO_ENCODER_STOP_FAIL 停止编码器失败
     */
    virtual EncoderRetCode StopEncoder() = 0;

    /**
     * @功能描述: 销毁编码器，释放编码资源
     */
    virtual void DestroyEncoder() = 0;

    /**
     * @功能描述: 重置编码器
     * @返回值: VIDEO_ENCODER_SUCCESS 成功
     *          VIDEO_ENCODER_RESET_FAIL 重置编码器失败
     */
    virtual EncoderRetCode ResetEncoder() = 0;

    /**
     * @功能描述: 配置编码器参数
     * @参数 [in] 启动编码所需的配置参数
     * @返回值: VIDEO_ENCODER_SUCCESS 成功
     *          VIDEO_ENCODER_RESET_FAIL 配置编码器参数失败
     */
    virtual EncoderRetCode Config(const VmiEncoderConfig& config) = 0;

    /**
     * @功能描述: 动态修改编码参数
     * @参数 [in] 可动态修改的编码参数
     * @返回值: VIDEO_ENCODER_SUCCESS 成功
     *          VIDEO_ENCODER_RESET_FAIL 修改编码器参数失败
     */
    virtual EncoderRetCode SetParams(const VmiEncoderParams& params) = 0;
};

extern "C" {
/**
 * @功能描述: 创建编码器实例
 * @参数 [out] encoder: 编码器实例
 * @参数 [in] encoderFormat: 编码器类型
 * @返回值: VIDEO_ENCODER_SUCCESS 成功
 *          VIDEO_ENCODER_CREATE_FAIL 创建编码器实例失败
 */
EncoderRetCode CreateVideoEncoder(VideoEncoder** encoder, EncoderFormat encoderFormat);

/**
 * @功能描述: 销毁编码器实例
 * @参数 [in] encoder: 编码器实例
 * @返回值: VIDEO_ENCODER_SUCCESS 成功
 *          VIDEO_ENCODER_DESTROY_FAIL 销毁编码器实例失败
 */
EncoderRetCode DestroyVideoEncoder(VideoEncoder* encoder);
}

#endif  // VIDEO_CODEC_API_H
