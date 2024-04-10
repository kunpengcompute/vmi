/*
 * 版权所有 (c) 华为技术有限公司 2017-2021
 * 功能描述：网络通信基类
 *
 */

#ifndef VMI_SOCKET_H
#define VMI_SOCKET_H

#include <memory>
#include <mutex>
#include <functional>

namespace Vmi {
enum class VmiSocketStatus {
    SOCKET_STATUS_INIT = 0, // 初始态
    SOCKET_STATUS_RUNNING = 1, // 运行态
    SOCKET_STATUS_EXIT = -1, // 退出态
    SOCKET_STATUS_DISCONN = -2, // socket连接断开
};

class __attribute__ ((visibility ("default"))) VmiSocket : public std::enable_shared_from_this<VmiSocket> {
public:
    /**
     * @功能描述：默认构造函数
     */
    VmiSocket();

    /**
     * @功能描述：构造函数
     * @参数 [in] fd：套接字句柄
     */
    explicit VmiSocket(int fd);

    /*
     * @功能描述：虚析构函数
     */
    virtual ~VmiSocket() = default;

    /*
     * @功能描述：发送数据
     * @参数 [in] sendBuf：待发送数据buffer
     * @返回值：大于等于0表示实际发送的字节数，小于0表示失败
     */
    virtual ssize_t Send(const std::pair<uint8_t*, size_t> &sendBuf) = 0;

    /*
     * @功能描述：接收数据
     * @参数 [in] recvBuf：接收buffer
     * @返回值：大于等于0表示实际接收到的字节数，小于0表示失败
     */
    virtual ssize_t Recv(const std::pair<uint8_t*, size_t> &recvBuf) = 0;

    /*
     * @功能描述：通知socket断开
     */
    virtual void EventNotice();

    /*
     * @功能描述：注册socket断开通知
     * @参数 [in] eventNotice：处理断开的回调函数
     */
    using EventNoticeCallback = std::function<void (const VmiSocket& callback)>;
    virtual void SetEventNotice(EventNoticeCallback eventNotice);

    /*
     * @功能描述：设置socket句柄
     * @参数 [in] fd：socket句柄
     */
    void SetFd(int fd);

    /*
     * @功能描述：获取socket句柄
     * @返回值：socket句柄
     */
    int GetFd() const;

    /*
     * @功能描述：设置状态
     * @参数 [in] status：状态，参见VmiSocketStatus定义
     */
    void SetStatus(VmiSocketStatus status);

    /*
     * @功能描述：获取状态
     * @返回值：状态，参见VmiSocketStatus定义
     */
    VmiSocketStatus GetStatus() const;

    /*
     * @功能描述：关闭socket抽象方法
     */
    virtual void CloseSocket() = 0;

    /**
     * @功能描述：接收字节
     */
    virtual uint64_t GetBytesRecv() const = 0;

    /**
     * @功能描述：发送字节
     */
    virtual uint64_t GetBytesSend() const = 0;

protected:
    int m_fd = -1;
    VmiSocketStatus m_status = VmiSocketStatus::SOCKET_STATUS_INIT;
    EventNoticeCallback m_eventNotice = nullptr;
    std::mutex m_socketMutex = {};
};

class VmiServerService {
public:
    /*
     * @功能描述：构造函数
     */
    VmiServerService() = default;

    /*
     * @功能描述：虚析构函数
     */
    virtual ~VmiServerService() = default;

    /*
     * @功能描述：通知新连接接入
     * @参数 [in] socket：新连接VmiSocket智能指针
     */
    virtual void Callback(std::shared_ptr<VmiSocket> socket) = 0;
};
} // namespace Vmi
#endif // VMI_SOCKET_H
