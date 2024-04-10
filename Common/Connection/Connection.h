/*
 * 版权所有 (c) 华为技术有限公司 2023-2023
 * 功能说明: 基于 TCP 协议实现的通信库示例代码
 * 服务端调用流程：VmiCreateConnection -> VmiListen -> VmiAccpet -> VmiSend/VmiRecv -> VmiCloseConnection
 * 客户端调用流程：VmiCreateConnection -> (ConfigClientAddress -> VmiBeginConnect)/(VmiConnect) -> VmiSend/VmiRecv -> VmiCloseConnection
 */
#ifndef CONNECTION_H
#define CONNECTION_H
#include <unistd.h>
#include <cstdint>
#ifdef __cplusplus
extern "C" {
#endif

constexpr int CONNECTION_SUCCESS = 0;              // 成功
constexpr int CONNECTION_FAIL_AND_CAN_RETRY = -1;  // 处理失败，但是可以重试
constexpr int CONNECTION_PARAM_CHECK_FAILED = -2;  // 参数校验失败
constexpr int CONNECTION_DISCONNECT = -3;          // 处理失败，连接已断开，无法继续
constexpr int CONNECTION_NOT_INIT_SYMBOL = -1000;  // 符号未初始化

/**
 * @功能描述：创建连接对象
 * @返回值：成功时返回新创建的通信连接描述符；错误时返回负数并设置errno
 */
int VmiCreateConnection();

/**
 * @功能描述：由服务端程序调用，指定监听的端口
 * @参数 [in] connection:通信连接描述符
 * @参数 [in] port:需监听的端口
 * @返回值：成功时返回0；错误时返回-1并设置errno
 */
int VmiListen(int connection, unsigned int port);

/**
 * @功能描述：由服务端程序调用，调用后将阻塞线程直到新的连接出现，此函数返回新的通信连接描述符
 * @参数 [in] connection:通信连接描述符
 * @返回值：返回值表示与客户端建立新连接的描述符，错误时返回负数并设置errno
 */
int VmiAccept(int connection);

/**
 * @功能描述：由客户端程序调用，配置客户端的IP和端口
 * @参数 [in] ip:使用网络字节序表示的ip地址
 * @参数 [in] port:对端的端口
 */
void VmiConfigClientAddress(unsigned int ip, unsigned int port);

/**
 * @功能描述：由客户端程序调用，用于与服务端建立连接，其开始连接的参数通过VmiConfigClientAddress进行设置
 * @参数 [in] connection:通信连接描述符
 * @参数 [in] ip:使用网络字节序表示的ip地址
 * @参数 [in] port:对端的端口
 * @返回值：成功返回0,；错误时返回负数并设置errno
 */
int VmiBeginConnect(int connection);

/**
 * @功能描述：由客户端程序调用，用于与服务端建立连接
 * @参数 [in] ip:使用网络字节序表示的ip地址
 * @参数 [in] port:对端的端口
 * @返回值：成功返回0,；若错误时返回-1并设置errno
 */
int VmiConnect(int connection, unsigned int ip, unsigned int port);

/**
 * @功能描述：发送数据给对端
 * @参数 [in] connection:通信连接描述符
 * @参数 [in] buf:指向待发送数据的指针
 * @参数 [in] len:待发送数据的字节数
 * @返回值：发送成功时返回发送的字节数；若错误时返回-1并设置errno
 */
ssize_t VmiSend(int connection, const uint8_t* buf, size_t len);

/**
 * @功能描述：接收对端发送的数据，阻塞式调用
 * @参数 [in] connection:通信连接描述符
 * @参数 [in] buf:指向用于存放接收数据的指针
 * @参数 [in] len:存放接收数据指针指向内存的字节数
 * @返回值：接收成功则返回接收到数据字节数，若错误则返回-1并设置erron
 */
ssize_t VmiRecv(int connection, uint8_t* buf, size_t len);

/**
 * @功能描述：关闭通信连接
 * @参数 [in] connection:通信连接描述符
 */
void VmiCloseConnection(int connection);

#ifdef __cplusplus
}
#endif
#endif