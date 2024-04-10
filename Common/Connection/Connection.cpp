/*
 * 版权所有 (c) 华为技术有限公司 2023-2023
 * 功能说明: 基于 TCP 协议实现的通信库示例代码
 * 服务端调用流程：VmiCreateConnection -> VmiListen -> VmiAccpet -> VmiSend/VmiRecv -> VmiCloseConnection
 * 客户端调用流程：VmiCreateConnection -> (ConfigClientAddress -> VmiBeginConnect)/(VmiConnect) -> VmiSend/VmiRecv -> VmiCloseConnection
 */
#define LOG_TAG "Connection"
#include "Connection.h"
#include <cerrno>
#include <unistd.h>
#include <cstring>
#include <signal.h>
#include <atomic>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include "logging.h"

using AbormalFunc = void(*)(int connection);
static std::atomic<bool> g_isInit(false);
static std::atomic<unsigned int> g_remoteIp(0);
static std::atomic<unsigned int> g_remotePort(0);

// int 形式的 ip 转换成字符串
std::string InttoIP(unsigned int ip)
{
    in_addr address;
    address.s_addr = htonl(ip);
    std::string ipStr = inet_ntoa(address);
    return ipStr;
}

#if defined (__cplusplus)
extern "C" {
#endif

/**
 * @功能描述：创建连接对象
 * @返回值：成功时返回新创建的通信连接描述符；错误时返回负数并设置errno
 */
int VmiCreateConnection()
{
    if (!g_isInit.load()) {
        // 一个对端已经关闭的socket调用两次write, 第二次将会生成SIGPIPE信号, 该信号默认结束进程.
        // 为了避免进程退出, 如下代码捕获SIGPIPE信号并忽略该信号
        if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
            WARN("Unable to catch signal SIGPIPE");
        }
        g_isInit = true;
    }
    int fd = socket(PF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        ERR("Failed to create connection, errno:(%d):%s", errno, strerror(errno));
        return CONNECTION_DISCONNECT;
    }
    return fd;
}

/**
 * @功能描述：由服务端程序调用，指定监听的端口
 * @参数 [in] connection:通信连接描述符
 * @参数 [in] port:需监听的端口
 * @返回值：成功时返回0；错误时返回-1并设置errno
 */
int VmiListen(int connection, unsigned int port)
{
    int reuse = 1;
    if (setsockopt(connection, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        ERR("Failed to listen, set connection:%d option REUSER failed, errno: (%d): %s",
            connection, errno, strerror(errno));
        return CONNECTION_FAIL_AND_CAN_RETRY;
    }
    int enable = 1;
    if (setsockopt(connection, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(enable)) < 0) {
        ERR("Failed to listen, set connection:%d option TCP_NODELAY failed, errno: (%d): %s",
            connection, errno, strerror(errno));
        return CONNECTION_FAIL_AND_CAN_RETRY;
    }
    // 绑定socket到指定的地址及端口
    struct sockaddr_in localAddr {};
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddr.sin_port = htons(port);
    if (::bind(connection, reinterpret_cast<struct sockaddr *>(&localAddr), sizeof(localAddr)) != 0) {
        ERR("Failed to listen, bind connection:%d failed, errno: (%d) %s", connection, errno, strerror(errno));
        return CONNECTION_FAIL_AND_CAN_RETRY;
    }
    // 监听TCP端口
    const int backlog = 5;
    if (listen(connection, backlog) != 0) {
        ERR("Failed to listen connection:%d, errno: (%d) %s", connection, errno, strerror(errno));
        return CONNECTION_FAIL_AND_CAN_RETRY;
    }
    INFO("Success bind and listen on connection:%d", connection);
    return CONNECTION_SUCCESS;
}

/**
 * @功能描述：由服务端程序调用，调用后将阻塞线程直到新的连接出现，此函数返回新的通信连接描述符
 * @参数 [in] connection:通信连接描述符
 * @返回值：返回值表示与客户端建立新连接的描述符，错误时返回-1并设置errno
 */
int VmiAccept(int connection)
{
    INFO("Connection:%d waitting new client", connection);
    struct sockaddr_in clientAddr = {};
    socklen_t clientLen = sizeof(clientAddr);
    int clientSockFd = accept(connection, reinterpret_cast<struct sockaddr*>(&clientAddr), &clientLen);
    if (clientSockFd < 0) {
        ERR("Failed to accpet connection:%d, errno: (%d): %s, client fd:%d",
            connection, errno, strerror(errno), clientSockFd);
        return CONNECTION_FAIL_AND_CAN_RETRY;
    }
    char ipAddress[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientAddr.sin_addr), ipAddress, INET_ADDRSTRLEN);
    INFO("Connection:%d accpet connect:%d, ip:%s, port:%d", connection, clientSockFd, ipAddress, static_cast<uint32_t>(ntohs(clientAddr.sin_port)));
    struct timeval timeout = {
        .tv_sec = 1,
        .tv_usec = 0,
    };
    // 设置发送超时时间为1s
    if (setsockopt(clientSockFd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
        WARN("Failed to set connection:%d SO_SNDTIMEO, errno: (%d): %s", clientSockFd, errno, strerror(errno));
    }
    // 设置接收超时时间为1s
    if (setsockopt(clientSockFd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        WARN("Failed to set connection:%d SO_RCVTIMEO, errno: (%d): %s", clientSockFd, errno, strerror(errno));
    }
    return clientSockFd;
}

/**
 * @功能描述：由客户端程序调用，配置客户端的IP和端口
 * @参数 [in] ip:使用网络字节序表示的ip地址
 * @参数 [in] port:对端的端口
 */
void VmiConfigClientAddress(unsigned int ip, unsigned int port)
{
    g_remoteIp = ip;
    g_remotePort = port;
}

/**
 * @功能描述：由客户端程序调用，用于与服务端建立连接，其开始连接的参数通过VmiConfigClientAddress进行设置
 * @参数 [in] connection:通信连接描述符
 * @参数 [in] ip:使用网络字节序表示的ip地址
 * @参数 [in] port:对端的端口
 * @返回值：成功返回0,；错误时返回负数并设置errno
 */
int VmiBeginConnect(int connection)
{
    return VmiConnect(connection, g_remoteIp, g_remotePort);
}

void SetClientOptions(int connection)
{
    // 使能端口复用
    constexpr int reuse = 1;
    if (setsockopt(connection, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        ERR("Failed to set connection:%d opt REUSER, errno(%d): %s", connection, errno, strerror(errno));
    }
    // close()立刻返回，但不会发送未发送完成的数据，而是通过一个REST包强制的关闭socket描述符，即强制退出
    struct linger tcplinger = {
        .l_onoff = 1,
        .l_linger = 0
    };
    if (setsockopt(connection, SOL_SOCKET, SO_LINGER, &tcplinger, sizeof(tcplinger)) < 0) {
        ERR("Failed to set connection:%d opt LINGER, errno(%d): %s", connection, errno, strerror(errno));
    }
    struct timeval timeout = {
        .tv_sec = 1,
        .tv_usec = 0
    };
    // 设置发送超时1s
    if (setsockopt(connection, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
        ERR("Failed to set connection:%d opt SO_SNDTIMEO, errno: (%d): %s", connection, errno, strerror(errno));
    }
    // 设置接收超时1s
    if (setsockopt(connection, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        ERR("Failed to set connection:%d SO_RCVTIMEO, errno: (%d): %s", connection, errno, strerror(errno));
    }
    // 关闭TCP延迟发送
    constexpr int flag = 1;
    if (setsockopt(connection, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag)) < 0) {
        ERR("Failed to set connection:%d TCP_NODELAY, errno: (%d): %s", connection, errno, strerror(errno));
    }
}



/**
 * @功能描述：由客户端程序调用，用于与服务端建立连接
 * @参数 [in] connection:通信连接描述符
 * @参数 [in] ip:使用网络字节序表示的ip地址
 * @参数 [in] port:对端的端口
 * @返回值：成功返回0,；若错误时返回-1并设置errno
 */
int VmiConnect(int connection, unsigned int ip, unsigned int port)
{
    SetClientOptions(connection);
    struct sockaddr_in remoteAddr {};
    remoteAddr.sin_family = AF_INET;
    remoteAddr.sin_addr.s_addr = htonl(ip);
    remoteAddr.sin_port = htons(port);
    int retryCount = 0;
    do {
        int ret = connect(connection, (struct sockaddr*)&(remoteAddr), sizeof(remoteAddr));
        if (ret == 0) {
            INFO("Connection:%d connected to server ip:%s, port:%u success.",
                connection, InttoIP(ip).c_str(), port);
            return CONNECTION_SUCCESS;
        }
        ERR("Failed to connect connection:%d, ip:%u, ipStr:%s, port:%u, failed time:%d, ret:%d, errno(%d):%s, try again...",
            connection, ip, InttoIP(ip).c_str(), port, retryCount, ret, errno, strerror(errno));
        retryCount++;
        const int maxRetry = 3;
        if (retryCount > maxRetry) {
            return CONNECTION_FAIL_AND_CAN_RETRY;
        }
        const int wait = 500;
        std::this_thread::sleep_for(std::chrono::milliseconds(wait));
    } while (true);
    return CONNECTION_SUCCESS;
}

/**
 * @功能描述：发送数据给对端
 * @参数 [in] connection:通信连接描述符
 * @参数 [in] buf:指向待发送数据的指针
 * @参数 [in] len:待发送数据的字节数
 * @返回值：发送成功时返回发送的字节数；若错误时返回-1并设置errno
 */
ssize_t VmiSend(int connection, const uint8_t* buf, size_t len)
{
    if (buf == nullptr || len == 0) {
        ERR("Failed to send connection:%d data, input param buf is %s null or len:%zu is zero",
            connection, buf == nullptr ? "" : "not", len);
        return CONNECTION_PARAM_CHECK_FAILED;
    }
    struct timeval curTime = {};
    struct timeval startTime = {};
    (void)gettimeofday(&startTime, nullptr);
    const long retryTimeOut = 3;
    size_t remain = len;
    while (remain > 0) {
        ssize_t ret = ::send(connection, buf, remain, MSG_NOSIGNAL);
        if (ret >= 0) {
            auto bytesSent = static_cast<size_t>(ret);
            if (bytesSent == remain) {
                return len;
            } else if (bytesSent < remain) {
                remain -= bytesSent;
                buf = buf + bytesSent;
                (void)gettimeofday(&curTime, nullptr);
                if ((curTime.tv_sec - startTime.tv_sec) > retryTimeOut) {
                    ERR("Failed to send connection:%d data, send len:%zu data, remain:%zu, bytesSent:%zu",
                        connection, len, remain, bytesSent);
                    return CONNECTION_DISCONNECT;
                }
                continue;
            } else {
                WARN("Connection:%d send data len:%zu, ret:%zu", connection, remain, bytesSent);
                remain = 0;
                continue;
            }
        } else if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR || errno == ETIMEDOUT) {
            LOG_RATE_LIMIT(ANDROID_LOG_WARN, 1, "Connection:%d send fail and try again, errno:%d, %s, ret:%zd, size:%zu",
                connection, errno, strerror(errno), ret, len);
            (void)gettimeofday(&curTime, nullptr);
            if ((curTime.tv_sec - startTime.tv_sec) > retryTimeOut) {
                ERR("Failed to send connection:%d data, send len:%zu data, remain:%zu", connection, len, remain);
                return CONNECTION_DISCONNECT;
            }
            continue;
        } else {
            ERR("Failed to send connection:%d data len:%zu, errno (%d) %s", connection, len, errno, strerror(errno));
            return CONNECTION_DISCONNECT;
        }
    }
    return CONNECTION_DISCONNECT;
}

/**
 * @功能描述：接收对端发送的数据，阻塞式调用
 * @参数 [in] connection:通信连接描述符
 * @参数 [in] buf:指向用于存放接收数据的指针
 * @参数 [in] len:存放接收数据指针指向内存的字节数
 * @返回值：接收成功则返回接收到数据字节数，若无错误只是当前socket无内容或被打断则返回0，若错误则返回-1并设置erron
 */
ssize_t VmiRecv(int connection, uint8_t* buf, size_t len)
{
    if (buf == nullptr || len == 0) {
        ERR("Failed to recv connection:%d data, input param buf is %s null or len:%zu is zero",
            connection, buf == nullptr ? "" : "not", len);
        return CONNECTION_PARAM_CHECK_FAILED;
    }
    ssize_t ret = ::recv(connection, buf, len, 0);
    if (ret > 0) {
        // 及时回复 ack，如果设置失败会导致对端的下一个包发送时间被延后40ms。
        int optVal[1] = {1};
        if (setsockopt(connection, IPPROTO_TCP, TCP_QUICKACK, optVal, sizeof(optVal)) < 0) {
            WARN("(Connection:%d set TCP_QUICKACK failed, errno(%d): %s", connection, errno, strerror(errno));
        }
        return ret;
    } else if ((ret == 0) || (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR && errno != ETIMEDOUT)) {
        ERR("Failed to recv connection:%d data, disconnected or recv fail, ret:%zd. errno(%d): %s, buf is %s null, size:%zu",
            connection, ret, errno, strerror(errno), buf == nullptr ? "" : "not", len);
        return CONNECTION_DISCONNECT;
    } else {
        LOG_RATE_LIMIT(ANDROID_LOG_WARN, 1, "(Connection:%d recv fail, can retry, ret:%zd. errno(%d): %s",
            connection, ret, errno, strerror(errno));
        return CONNECTION_FAIL_AND_CAN_RETRY;
    }
}

/**
 * @功能描述：关闭通信连接
 * @参数 [in] connection:通信连接描述符
 */
void VmiCloseConnection(int connection)
{
    INFO("Closing connection:%d", connection);
    int ret = shutdown(connection, SHUT_RDWR);
    if (ret < 0) {
        ERR("Failed to close connection:%d, shutdown failed, errno:(%d):%s", connection, errno, strerror(errno));
    }
    ret = close(connection);
    if (ret < 0) {
        ERR("Failed to close connection:%d, errno:(%d):%s", connection, errno, strerror(errno));
    }
    INFO("Close connection:%d success", connection);
}
#if defined (__cplusplus)
}
#endif