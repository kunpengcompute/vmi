/*
 * 版权所有 (c) 华为技术有限公司 2017-2021
 * 功能描述：网络通信基类
 *
 */
#define LOG_TAG "VmiSocket"

#include "VmiSocket.h"
#include "logging.h"

using namespace std;

namespace Vmi {
namespace {
    constexpr int INVALID_FD = -1;
}

/**
 * @功能描述：默认构造函数
*/
VmiSocket::VmiSocket()
    : m_fd(INVALID_FD), m_status(VmiSocketStatus::SOCKET_STATUS_INIT)
{
}

/**
 * @功能描述：构造函数
 * @参数 [in] fd：套接字句柄
 */
VmiSocket::VmiSocket(int fd)
    : m_fd(fd)
{
}

/*
 * @功能描述：通知socket断开
 */
void VmiSocket::EventNotice()
{
    if (m_eventNotice == nullptr) {
        INFO("m_eventNotice == nullptr");
        return;
    }
    m_eventNotice(*this);
}

/*
 * @功能描述：注册socket断开通知
 * @参数 [in] eventNotice：处理socket断开的回调函数
 */
void VmiSocket::SetEventNotice(EventNoticeCallback eventNotice)
{
    m_eventNotice = eventNotice;
}

/*
 * @功能描述：设置socket句柄
 * @参数 [in] fd：socket句柄
 */
void VmiSocket::SetFd(int fd)
{
    m_fd = fd;
    DBG("set fd(%d)", fd);
}

/*
 * @功能描述：获取socket句柄
 * @返回值：socket句柄
 */
int VmiSocket::GetFd() const
{
    return m_fd;
}

/*
 * @功能描述：设置状态
 * @参数 [in] status：状态，参见VmiSocketStatus定义
 */
void VmiSocket::SetStatus(VmiSocketStatus status)
{
    m_status = status;
}

/*
 * @功能描述：获取状态
 * @返回值：状态，参见VmiSocketStatus定义
 */
VmiSocketStatus VmiSocket::GetStatus() const
{
    return m_status;
}
} // namespace Vmi
