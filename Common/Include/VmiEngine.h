/*
 * 版权所有 (c) 华为技术有限公司 2017-2023
 * 功能说明: 指令流引擎定义的对外通知事件以及通信消息类型
 */

#ifndef VMI_ENGINE_H
#define VMI_ENGINE_H

#include <cstdint>

#if defined (__cplusplus)
extern "C" {
#endif

// VMIMsgType消息类型定义与VmiStream.cpp中的std::map类型TYPE_NAME相对应
// 如果增加VMIMsgType类型需要同时添加对应的TYPE_NAME条目
enum VMIMsgType : uint8_t {
    INVALID = 0,
    HEARTBEAT,
    GL,
    AUDIO,
    TOUCH_INPUT,
    NAVBAR_INPUT,
    VIDEO_RR, // video stream for remote render (from vmiAgent)
    SENSOR,
    MIC,
    VIDEO_RR2 = 16, // video stream for remote render (to videoAgent)
    END
};

enum VmiEngineRetCode : int32_t {
    VMI_CLIENT_RECV_ORDER_FAIL = -5,
    VMI_CLIENT_RECV_MEMCPY_FAILED = -4,
    VMI_CLIENT_RECV_LENGTH_NO_MATCH = -3,
    VMI_CLIENT_RECV_DATA_NULL = -2,
    VMI_CLIENT_RECV_INVALID_PAPAM = -1,
    VMI_SUCCESS = 0,
    VMI_CLIENT_INVALID_PARAM = 0x0A050001,
    VMI_CLIENT_START_FAIL = 0x0A050002,
    VMI_CLIENT_ALREADY_STARTED = 0x0A050003,
    VMI_CLIENT_STOP_FAIL = 0x0A050004,
    VMI_CLIENT_SEND_HOOK_REGISTER_FAIL = 0x0A050005,
    VMI_CLIENT_SEND_FAIL = 0x0A050006,
    VMI_CLIENT_SEND_AGAIN = 0x0A050007,
    VMI_CLIENT_INITIALIZE_FAIL = 0x0A050008,
    VMI_CLIENT_EXEC_ORDER_FAIL = 0x0A050009
};

enum VmiEngineEvent {
    VMI_ENGINE_EVENT_SOCK_DISCONN   = -2,       // 连接断开
    VMI_ENGINE_EVENT_PKG_BROKEN     = -3,       // 数据包损坏
    VMI_ENGINE_EVENT_VERSION_ERROR  = -4,       // 服务端和客户端的版本不匹配
    VMI_ENGINE_EVENT_READY = -5,                // 引擎渲染第一帧画面成功
    VMI_ENGINE_EVENT_ORIENTATION_CHANGED = -6,  // 服务端方向转屏事件
    VMI_ENGINE_EVENT_CACHE_OVERFLOW = -7,       // 缓存溢出
    VMI_ENGINE_EVENT_BAD_LATENCY = -8,          // 网络时延过大
    VMI_ENGINE_EVENT_NEW_CONNECTION = -9,       // 收到新的网络连接
    VMI_ENGINE_EVENT_TRAFFIC_ABNORMAL = -10,    // 网络流量异常
    VMI_ENGINE_EVENT_APP_TRAFFIC_ABNORMAL = -11, // app发往Sipc的流量异常
    VMI_ENGINE_EVENT_GET_VERSION_TIMEOUT = -12,  // 获取版本号超时
    VMI_ENGINE_EVENT_VERSION_MISMATCH = -13,     // 版本号校验失败
    VMI_ENGINE_EVENT_ENGINE_MISMATCH = -14,      // 引擎匹配失败
};

typedef struct EngineEvent {
    int event = 0;
    int para1 = 0;
    int para2 = 0;
    int para3 = 0;
    int para4 = 0;
    char additionInfo[4096] = {0};
} EngineEvent;

// 参数值event是引擎的回调函数
using OnVmiEngineEvent = void (*)(EngineEvent event);

#if defined (__cplusplus)
}
#endif
#endif
