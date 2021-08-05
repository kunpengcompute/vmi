/*
 * 版权所有 (c) 华为技术有限公司 2021-2021
 * 功能说明: 提供视频编解码器对外接口
 */
#ifndef VIDEO_CODEC_API_H
#define VIDEO_CODEC_API_H

#include "VideoCodecDefs.h"
#include "MediaLogDefs.h"

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
     * @参数 [in] encParams: 编码参数结构体
     * @返回值: VIDEO_ENCODER_SUCCESS 成功
     *          VIDEO_ENCODER_INIT_FAIL 初始化编码器失败
     */
    virtual EncoderRetCode InitEncoder(const EncoderParams &encParams) = 0;

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
};

extern "C" {

/**
 * @功能描述: 注册日志回调函数
 * @参数 [in] logCallback: 日志处理函数指针
 * @返回值: true 成功
 *          false 注册函数失败
 */
bool RegisterMediaLogCallback(const MediaLogCallbackFunc logCallback);

/**
 * @功能描述: 创建编码器实例
 * @参数 [in] encType: 编码器类型，取值范围见EncoderType
 * @参数 [out] encoder: 编码器实例
 * @返回值: VIDEO_ENCODER_SUCCESS 成功
 *          VIDEO_ENCODER_CREATE_FAIL 创建编码器实例失败
 */
EncoderRetCode CreateVideoEncoder(uint32_t encType, VideoEncoder** encoder);

/**
 * @功能描述: 销毁编码器实例
 * @参数 [in] encType: 编码器类型，取值范围见EncoderType
 * @参数 [in] encoder: 编码器实例
 * @返回值: VIDEO_ENCODER_SUCCESS 成功
 *          VIDEO_ENCODER_DESTROY_FAIL 销毁编码器实例失败
 */
EncoderRetCode DestroyVideoEncoder(uint32_t encType, VideoEncoder* encoder);
}

#endif  // VIDEO_CODEC_API_H
