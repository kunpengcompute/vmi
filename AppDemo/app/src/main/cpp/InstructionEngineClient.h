/*
 * 版权所有 (c) 华为技术有限公司 2017-2021
 * 功能说明:指令流引擎接口C++函数
 */
#ifndef INSTRUCTION_ENGINE_CLIENT_H
#define INSTRUCTION_ENGINE_CLIENT_H
#include "InstructionEngine.h"
typedef struct FrameInfo {
    uint64_t tid;           // 渲染线程id
    uint32_t fps;           // 渲染线程帧率
    uint64_t frameCount;    // 渲染线程帧数
} FrameInfo;

typedef struct StatusInfo {
    uint64_t bytesRecv;     // 接收字节数
    uint64_t bytesSend;     // 发送字节数
    uint64_t lag;           // 网路延迟(ms)
    FrameInfo info[4];      // 渲染线程统计信息
} StatusInfo;

/**
 * @功能描述：指令引擎初始化函数
 * @参数 [in] event：传入的指令流事件回调函数指针
 * @返回值：VMI_SUCCESS，初始化成功，VMI_INSTRUCTION_CLIENT_INITIALIZE_FAIL，初始化失败。
 */
uint32_t Initialize(OnVmiInstructionEngineEvent event);

/**
 * @功能描述：指令引擎启动函数
 * @参数 [in] surface：java层传入的surface，在jni层转成ANativeWindow指针。
 * @返回值：VMI_SUCCESS，初始化成功，VMI_INSTRUCTION_CLIENT_INVALID_PARAM，参数异常，
 *         VMI_INSTRUCTION_CLIENT_ALREADY_STARTED，代表重复启动，VMI_INSTRUCTION_CLIENT_START_FAIL,
 *         代表启动失败
 */
uint32_t Start(uint64_t surface, uint32_t width, uint32_t height, uint32_t densityDpi);

/**
 * @功能描述：指令引擎停止函数
 */
void Stop();

/**
 * @功能描述：获取指令流引擎渲染线程统计信息
 * @返回值：返回指令流引擎渲染线程统计信息，请参考InstructionEngine.h中StatusInfo结构体定义
 */
StatusInfo GetStat();

/**
 * @功能描述：指令引擎socket接收函数
 * @参数 [in] type：接收数据类型，请参考InstructionEngine.h中VMIMsgType枚举定义
 * @参数 [in] data：接收数据buffer地址
 * @参数 [in] length：接收数据buffer长度
 * @返回值：大于0，代表接收的字节数。0代表失败
 */
uint32_t RecvData(VMIMsgType type, uint8_t* data, uint32_t length);


/**
 * @功能描述：指令引擎socket发送函数
 * @参数 [in] type：发送数据类型，请参考InstructionEngine.h中VMIMsgType枚举定义
 * @参数 [in] data：发送数据buffer地址
 * @参数 [in] length：发送数据buffer长度
 * @返回值：VMI_SUCCESS, 代表发送成功，VMI_INSTRUCTION_ENGINE_INVALID_PARAM代表type类型不支持。
 *         VMI_INSTRUCTION_ENGINE_UNINIT_SOCK，代表socket未开建立。 VMI_INSTRUCTION_ENGINE_SEND_ALLOC_FAILED代表底层发送buffer生成失败。
 * 		   VMI_INSTRUCTION_ENGINE_SEND_MEMCPY_FAILED，代表底层发送buffer拷贝失败，VMI_INSTRUCTION_ENGINE_SEND_FAIL代表底层发送失败。
 */
uint32_t SendData(VMIMsgType type, uint8_t* data, uint32_t length);
#endif