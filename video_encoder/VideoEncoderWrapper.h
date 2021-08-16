/*
 * 版权所有 (c) 华为技术有限公司 2021-2021
 * 功能说明: 视频编码器对外提供接口，包括编码器创建、初始化、启动、编码、停止、销毁等
 */
#ifndef VIDEO_ENCODER_WRAPPER_H
#define VIDEO_ENCODER_WRAPPER_H

#include <cstdint>

// 编码器返回码
enum VmiEncoderRetCode : uint32_t {
    VMI_ENCODER_SUCCESS       = 0x00,
    VMI_ENCODER_CREATE_FAIL   = 0x01,  // 创建编码器失败
    VMI_ENCODER_INIT_FAIL     = 0x02,  // 初始化编码器失败
    VMI_ENCODER_START_FAIL    = 0x03,  // 启动编码器失败
    VMI_ENCODER_ENCODE_FAIL   = 0x04,  // 编码失败
    VMI_ENCODER_STOP_FAIL     = 0x05,  // 停止编码器失败
    VMI_ENCODER_DESTROY_FAIL  = 0x06,  // 销毁编码器失败
    VMI_ENCODER_REGISTER_FAIL = 0x07   // 注册函数失败
};

// 编码参数
struct VmiEncodeParams {
    uint32_t width = 0;      // 编码输入/输出宽度
    uint32_t height = 0;     // 编码输入/输出高度
    uint32_t frameRate = 0;  // 编码输入帧率
    uint32_t bitrate = 0;    // 编码输出码率
};

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @功能描述: 创建编码器
 * @参数 [out] encHandle: 编码器对象句柄
 * @返回值: VMI_ENCODER_SUCCESS 成功
 *          VMI_ENCODER_CREATE_FAIL 创建编码器失败
 */
VmiEncoderRetCode VencCreateEncoder(uint32_t *encHandle);

/**
 * @功能描述: 初始化编码器
 * @参数 [in] encHandle: 编码器对象句柄
 * @参数 [in] encParams: 编码参数结构体
 * @返回值: VMI_ENCODER_SUCCESS 成功
 *          VMI_ENCODER_INIT_FAIL 初始化编码器失败
 */
VmiEncoderRetCode VencInitEncoder(uint32_t encHandle, const VmiEncodeParams encParams);

/**
 * @功能描述: 启动编码器
 * @参数 [in] encHandle: 编码器对象句柄
 * @返回值: VMI_ENCODER_SUCCESS 成功
 *          VMI_ENCODER_START_FAIL 启动编码器失败
 */
VmiEncoderRetCode VencStartEncoder(uint32_t encHandle);

/**
 * @功能描述: 编码器编码一帧数据
 * @参数 [in] encHandle: 编码器对象句柄
 * @参数 [in] inputData: 编码输入数据地址
 * @参数 [in] inputSize: 编码输入数据大小
 * @参数 [out] outputData: 编码输出数据地址
 * @参数 [out] outputSize: 编码输出数据大小
 * @返回值: VMI_ENCODER_SUCCESS 成功
 *          VMI_ENCODER_ENCODE_FAIL 编码一帧失败
 */
VmiEncoderRetCode VencEncodeOneFrame(uint32_t encHandle, const uint8_t *inputData, uint32_t inputSize,
    uint8_t **outputData, uint32_t * outputSize);

/**
 * @功能描述: 停止编码器
 * @参数 [in] encHandle: 编码器对象句柄
 * @返回值: VMI_ENCODER_SUCCESS 成功
 *          VMI_ENCODER_STOP_FAIL 停止编码器失败
 */
VmiEncoderRetCode VencStopEncoder(uint32_t encHandle);

/**
 * @功能描述: 销毁编码器
 * @参数 [in] encHandle: 编码器对象句柄
 * @返回值: VMI_ENCODER_SUCCESS 成功
 *          VMI_ENCODER_DESTROY_FAIL 销毁编码器失败
 */
VmiEncoderRetCode VencDestroyEncoder(uint32_t encHandle);

#ifdef __cplusplus
}
#endif

#endif  // VIDEO_ENCODER_WRAPPER_H
