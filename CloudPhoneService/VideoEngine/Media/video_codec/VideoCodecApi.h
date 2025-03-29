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
    uint32_t rcMode = 2;                        // 流控模式
    uint32_t crf = 34;                          // crf码控级别
    uint32_t maxCrfRate = 20000000;             // crf码率峰值
    int32_t vbvBufferSize = 1000;               // crf码率缓冲区大小
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
     * @返回值: VIDEO_ENCODER_SUCCESS 初始化编码器成功
     *          VIDEO_ENCODER_INIT_FAIL 初始化编码器失败
     */
    virtual EncoderRetCode InitEncoder() = 0;

    /**
     * @功能描述: 启动编码器
     * @返回值: VIDEO_ENCODER_SUCCESS 启动编码器成功
     *          VIDEO_ENCODER_START_FAIL 启动编码器失败
     */
    virtual EncoderRetCode StartEncoder() = 0;

    /**
     * @功能描述: 编码一帧数据
     * @参数 [in] inputData: 待编码数据缓冲区首地址
     * @参数 [in] inputSize: 待编码数据长度，单位Byte
     * @参数 [out] outputData: 编码后的数据
     * @参数 [out] outputSize: 编码后的数据长度，单位Byte
     * @返回值: VIDEO_ENCODER_SUCCESS 编码一帧成功
     *          VIDEO_ENCODER_ENCODE_FAIL 编码一帧失败
     */
    virtual EncoderRetCode EncodeOneFrame(const uint8_t *inputData, uint32_t inputSize,
        uint8_t **outputData, uint32_t *outputSize) = 0;

    /**
     * @功能描述: 停止编码器
     * @返回值: VIDEO_ENCODER_SUCCESS 停止编码器成功
     *          VIDEO_ENCODER_STOP_FAIL 停止编码器失败
     */
    virtual EncoderRetCode StopEncoder() = 0;

    /**
     * @功能描述: 销毁编码器，释放编码资源
     */
    virtual void DestroyEncoder() = 0;

    /**
     * @功能描述: 重置编码器
     * @返回值: VIDEO_ENCODER_SUCCESS 重置编码器成功
     *          VIDEO_ENCODER_RESET_FAIL 重置编码器失败
     */
    virtual EncoderRetCode ResetEncoder() = 0;

    /**
     * @功能描述: 配置编码器参数
     * @参数 [in] 启动编码所需的配置参数
     * @返回值: VIDEO_ENCODER_SUCCESS 成功
     */
    virtual EncoderRetCode Config(const VmiEncoderConfig& config) = 0;

    /**
     * @功能描述: 动态修改编码参数
     * @参数 [in] 可动态修改的编码参数
     * @返回值: VIDEO_ENCODER_SUCCESS 成功
     */
    virtual EncoderRetCode SetParams(const VmiEncoderParams& params) = 0;
};

extern "C" {
/**
 * @功能描述: 创建编码器实例
 * @参数 [in] fd: 编码器标记符
 * @参数 [in] encoderFormat: 编码器类型
 * @返回值: VIDEO_ENCODER_SUCCESS 成功
 *          VIDEO_ENCODER_CREATE_FAIL 创建编码器实例失败
 */
EncoderRetCode CreateVideoEncoder(int32_t* fd, EncoderFormat encoderFormat);

/**
 * @功能描述: 销毁编码器实例
 * @参数 [in] fd: 编码器标记符
 * @返回值: VIDEO_ENCODER_SUCCESS 成功
 *          VIDEO_ENCODER_DESTROY_FAIL 销毁编码器实例失败
 */
EncoderRetCode DestroyVideoEncoder(int32_t fd);

/**
 * @功能描述: 实现帧缩放，只支持分辨率等比例往下缩小
 * @参数 [in] uint32_t width: 设置范围[240，分辨率宽度]
 * @参数 [in] uint32_t height: 设置范围[240，分辨率高度]
 * @返回值: VIDEO_ENCODER_SUCCESS 成功
 *          VIDEO_ENCODER_SET_ENCODE_PARAMS_FAIL 失败
 */
EncoderRetCode FrameScaling(uint32_t width, uint32_t height);

/**
* @功能描述: 初始化编码器
* @参数 [in] fd: 编码器标记符
* @返回值: VIDEO_ENCODER_SUCCESS 初始化编码器成功
*          VIDEO_ENCODER_INIT_FAIL 初始化编码器失败
*/
EncoderRetCode InitEncoder(int32_t fd);

/**
* @功能描述: 启动编码器
* @参数 [in] fd: 编码器标记符
* @返回值: VIDEO_ENCODER_SUCCESS 初始化编码器成功
*          VIDEO_ENCODER_INIT_FAIL 初始化编码器失败
*/
EncoderRetCode StartEncoder(int32_t fd);

/**
* @功能描述: 编码一帧数据
* @参数 [in] fd: 编码器标记符
* @参数 [in] inputData: 待编码数据缓冲区首地址
* @参数 [in] inputSize: 待编码数据长度，单位Byte
* @参数 [out] outputData: 编码后的数据
* @参数 [out] outputSize: 编码后的数据长度，单位Byte
* @返回值: VIDEO_ENCODER_SUCCESS 编码一帧成功
*          VIDEO_ENCODER_ENCODE_FAIL 编码一帧失败
*/
EncoderRetCode EncodeOneFrame(int32_t fd, const uint8_t *inputData, uint32_t inputSize,
        uint8_t **outputData, uint32_t *outputSize);

/**
* @功能描述: 停止编码器
* @参数 [in] fd: 编码器标记符
* @返回值: VIDEO_ENCODER_SUCCESS 停止编码器成功
*          VIDEO_ENCODER_STOP_FAIL 停止编码器失败
*/
EncoderRetCode StopEncoder(int32_t fd);

/**
 * @功能描述: 销毁编码器，释放编码资源
 * @参数 [in] fd: 编码器标记符
 * @返回值: VIDEO_ENCODER_SUCCESS 成功
 *          VIDEO_ENCODER_DESTROY_FAIL 销毁编码器失败
 */
EncoderRetCode DestoryEncoder(int32_t fd);

/**
* @功能描述: 重置编码器
* @参数 [in] fd: 编码器标记符
* @返回值: VIDEO_ENCODER_SUCCESS 重置编码器成功
*          VIDEO_ENCODER_RESET_FAIL 重置编码器失败
*/
EncoderRetCode ResetEncoder(int32_t fd);

/**
* @功能描述: 配置编码器参数
* @参数 [in] fd: 编码器标记符
* @参数 [in] 启动编码所需的配置参数
* @返回值: VIDEO_ENCODER_SUCCESS 成功
*/
EncoderRetCode Config(int32_t fd, const VmiEncoderConfig& config);

/**
* @功能描述: 动态修改编码参数
* @参数 [in] fd: 编码器标记符
* @参数 [in] 可动态修改的编码参数
* @返回值: VIDEO_ENCODER_SUCCESS 成功
*/
EncoderRetCode SetParams(int32_t fd, const VmiEncoderParams& params);

}

#endif  // VIDEO_CODEC_API_H
