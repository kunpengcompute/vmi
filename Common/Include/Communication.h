/*
 * 版权所有 (c) 华为技术有限公司 2021-2022
 * 功能描述：通信库对外协议头文件，第三方通信库需实现如下C接口
 *
 */
#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <cstddef>
#include <cstdint>
#include <sys/types.h>

#if defined (__cplusplus)
extern "C" {
#endif

enum VmiSocketErrorCode {
    SOCKET_RECV_FAIL_RETRY = -1,   // 接收函数返回失败，可再调接收函数重试
    SOCKET_RECV_FAIL_DISCONN = -2, // 接收函数返回失败，SOCKET断链，不可用
    SOCKET_SEND_FAIL_RETRY = -3,   // 发送函数返回失败，可再调发送函数重试
    SOCKET_SEND_FAIL_DISCONN = -4, // 发送函数返回失败，SOCKET断链，不可用
};

// 收到新连接的回调函数
using OnNewConnectionCallback = void (*)(int connection);

/**
 * @功能描述：注册新连接建立后的通知回调函数，二次开发者可基于任意通信协议实现通信库，并在通信库建立好可靠传输通信通道后，
 *           通过回调函数通知引擎已经建立好连接，后续引擎将基于该连接进行数据通信。该函数是引擎要求二次开发者在通信模块
 *           实现的外部符号，引擎会调用该函数注册新连接建立后的通知回调函数
 * @参数 [in] cb：新连接建立后的通知回调函数
 * @返回值：成功返回0，失败返回错误码
 */
int RegConnectionCB(OnNewConnectionCallback cb);

/**
 * @功能描述：关闭通信连接，销毁通信资源。该接口是引擎要求二次开发者在通信模块实现的外部符号，
 *           引擎会调用该接口关闭并销毁指定的通信连接。在通信连接中断后，SendFully和Recv调用应返回错误码
 * @参数 [in] connection： OnNewConnectionCallback返回的连接句柄
 * @返回值：成功返回0，失败返回错误码
 */
int CloseConnection(int connection);

/**
 * @功能描述：发送数据给对端，该函数是引擎要求二次开发者在通信模块实现的外部符号，引擎会调用该函数发送数据
 *           通信库需保证SendFully把调用者传入的数据全部发送完或者发生错误才能返回，且需支持多线程互斥调用
 * @参数 [in] connection：OnNewConnectionCallback返回的连接句柄
 * @参数 [in] buf：指向待发送数据的内存指针
 * @参数 [in] len：待发送数据长度，不超过64KB
 * @返回值：成功返回len，表示已将所有数据发送（不存在只发送部分数据的情况）；有错误返回对应的错误码 VmiSocketErrorCode；
 */
ssize_t SendFully(int connection, uint8_t *buf, size_t len);

/**
 * @功能描述：接收对端发送的数据。引擎要求二次开发者实现通信模块，用于数据的发送和接收
 *           该函数是引擎要求二次开发者在通信模块实现的外部符号，引擎会调用该函数接收数据。需实现为阻塞式调用
 * @参数 [in] connection：OnNewConnectionCallback返回的连接句柄
 * @参数 [in] buf：指向用于存放接收数据的内存指针
 * @参数 [in] len：用于存放接收数据的内存长度，不超过64KB
 * @返回值：成功返回实际收到的数据大小，有错误返回对应的错误码 VmiSocketErrorCode
 */
ssize_t Recv(int connection, uint8_t *buf, size_t len);

/**
 * @功能描述：校验服务端和客户端版本是否匹配
 * @参数 [in] serverVersion：服务端版本号信息
 * @参数 [in] clientVersion：客户端版本号信息
 * @返回值：校验成功返回true，校验失败返回false
 */
bool CheckVersionMatch(const char* serverVersion, const char* clientVersion);

#if defined (__cplusplus)
}
#endif
#endif
