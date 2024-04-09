/*
 * 版权所有 (c) 华为技术有限公司 2022-2022
 * 功能说明：视频流引擎客户端的对外C接口
 */
#ifndef VIDEO_ENGINE_CLIENT_H
#define VIDEO_ENGINE_CLIENT_H

#include "VmiEngine.h"

#if defined (__cplusplus)
extern "C" {
#endif

namespace Vmi {
struct StatisticsInfo {
    uint64_t lag;     // 网路延迟(ms)
    uint32_t recvFps; // 接收帧率
    uint32_t decFps;  // 解码帧率
};
};

/**
 * @功能描述：初始化视频流引擎客户端，包括网络通信组件等
 * @参数 [in] eventHandleFunc：传入的视频流引擎客户端事件回调函数指针
 * @返回值：VMI_SUCCESS，初始化成功，其他返回值参考错误码定义
 */
uint32_t Initialize(OnVmiEngineEvent eventHandleFunc);

/**
 * @功能描述：启动视频流引擎客户端，包括启动视频解码线程和网络连接等，调用者需在调用之前保证网络通道可用
 * @参数 [in] surface：Activity传下来的Surface控件对象，并通过Jni转换成ANativeWindow指针，再转成uint64_t值
 *                    若传入的surface值为0，则引擎只接收数据，不进行视频解码
 * @参数 [in] width：设备屏幕的宽度，取值大于0
 * @参数 [in] height：设备屏幕的高度，取值大于0
 * @参数 [in] densityDpi：设备屏幕的像素密度，取值大于0
 * @返回值：VMI_SUCCESS，初始化成功，其他返回值参考错误码定义
 */
uint32_t Start(uint64_t surface, uint32_t width, uint32_t height, uint32_t densityDpi);

/**
 * @功能描述：停止视频流引擎客户端，包括停止视频解码线程和网络连接等；该函数是阻塞式的，要等所有线程退出，资源被释放后才会返回
 *           禁止在OnVmiEngineEvent回调函数中调用视频流引擎客户端的Stop函数
 */
void Stop();

/**
 * @功能描述：获取视频流引擎客户端的统计数据，包括网络时延、接收帧率、解码帧率等
 * @参数 [out] info：统计信息，包括网络时延、接收帧率、解码帧率等
 */
void GetStatistics(Vmi::StatisticsInfo &info);

/**
 * @功能描述：视频流引擎客户端接收数据
 * @参数 [in] type：接收数据类型，请参考VmiEngine.h中VMIMsgType枚举定义
 * @参数 [in] data：代表接收type类型数据buffer地址
 * @参数 [in] length：代表接收type类型数据buffer的长度
 * @返回值：大于0，代表接收的字节数，其他返回值参考错误码定义
 */
int32_t RecvData(VMIMsgType type, uint8_t* data, uint32_t length);

/**
 * @功能描述：视频流引擎客户端发送数据
 * @参数 [in] type：发送数据类型，请参考VmiEngine.h中VMIMsgType枚举定义
 * @参数 [in] data：代表发送type类型数据buffer地址
 * @参数 [in] length：代表发送type类型数据buffer长度
 * @返回值：VMI_SUCCESS, 代表发送成功；其他返回值参考错误码定义
 */
uint32_t SendData(VMIMsgType type, uint8_t* data, uint32_t length);

#if defined (__cplusplus)
}
#endif
#endif
