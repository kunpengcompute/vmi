/**
 * 版权所有 (c) 华为技术有限公司 2023-2023
 * 抽象VmiSocket实现类，适配了Demo部分的网络库
 */
#define LOG_TAG "ConnectionSocket"
#include "ConnectionSocket.h"
#include <mutex>
#include "Connection.h"
#include "Communication.h"
#include "logging.h"
#include "VmiMsgHead.h"

namespace Vmi {
ConnectionSocket::ConnectionSocket(int fd) : VmiSocket(fd) {}

ConnectionSocket::~ConnectionSocket()
{
    CloseSocket();
}

ssize_t ConnectionSocket::Send(const std::pair<uint8_t*, size_t> &sendBuf)
{
    if (GetStatus() != VmiSocketStatus::SOCKET_STATUS_RUNNING) {
        ERR("Failed to send data, fd:%d is disconnect", m_fd);
        return SOCKET_SEND_FAIL_DISCONN;
    }
    uint8_t* buf = sendBuf.first;
    size_t size = sendBuf.second;
    if (buf == nullptr || size == 0 || size > MAX_MSG_SIZE) {
        ERR("Failed to send data, invalid send buffer, buf %s nullptr, size:%zu, max size:%d",
            (buf == nullptr) ? "is" : "is not", size, MAX_MSG_SIZE);
        return SOCKET_SEND_FAIL_RETRY;
    }
    std::lock_guard<std::mutex> lock(m_socketMutex);
    StreamMsgHead *msgHead = reinterpret_cast<StreamMsgHead *>(buf);
    msgHead->SetMsgSeq(++m_curSendMsgSeq);
    if (m_msgHeadVerifyFeature) {
        msgHead->SetMurmurHash(std::make_pair(buf, static_cast<uint32_t>(size)));
    }
    ssize_t ret = VmiSend(m_fd, buf, size);
    if (ret < 0) {
        if (ret == CONNECTION_DISCONNECT && GetStatus() != VmiSocketStatus::SOCKET_STATUS_DISCONN) {
            ERR("Failed to send data, size:%zu fd:%d, connection disconnect", size, m_fd);
            SetStatus(VmiSocketStatus::SOCKET_STATUS_DISCONN);
            EventNotice();
            return SOCKET_SEND_FAIL_DISCONN;
        } else {
            ERR("Failed to send data, size:%zu fd:%d, need retry", size, m_fd);
            return SOCKET_SEND_FAIL_RETRY;
        }
    }
    m_totalSendByte += ret;
    return ret;
}

ssize_t ConnectionSocket::Recv(const std::pair<uint8_t*, size_t> &recvBuf)
{
    if (GetStatus() != VmiSocketStatus::SOCKET_STATUS_RUNNING) {
        ERR("Failed to recv data, fd:%d is disconnect", m_fd);
        return SOCKET_SEND_FAIL_DISCONN;
    }
    uint8_t *buf = recvBuf.first;
    size_t size = recvBuf.second;
    if (buf == nullptr || size == 0) {
        ERR("Failed to recv data, invalid receive buffer, buf %s nullptr, size:%zu",
            (buf == nullptr) ? "is" : "is not", size);
        return SOCKET_RECV_FAIL_RETRY;
    }
    int conn = m_fd;
    ssize_t ret = VmiRecv(m_fd, buf, size);
    if (ret <= 0) {
        if (ret == CONNECTION_DISCONNECT) {
            ERR("Failed to recv data, errno: %zd, size: %zu. set socket(%d) status disconn", ret, size, conn);
            if (GetStatus() != VmiSocketStatus::SOCKET_STATUS_DISCONN) {
                SetStatus(VmiSocketStatus::SOCKET_STATUS_DISCONN);
                EventNotice();
            }
            return SOCKET_RECV_FAIL_DISCONN;
        } else if (ret == CONNECTION_FAIL_AND_CAN_RETRY) {
            LOG_RATE_LIMIT(ANDROID_LOG_DEBUG, 1, "(TCP socket fd %d) recv fail retry, errno: %zd.",
                conn, ret);
        }
        return SOCKET_RECV_FAIL_RETRY;
    }
    m_totalRecvByte += ret;
    return ret;
}

void ConnectionSocket::CloseSocket()
{
    SetStatus(VmiSocketStatus::SOCKET_STATUS_EXIT);
    SetEventNotice(nullptr);
    int fd = GetFd();
    const int invalidFd = -1;
    if (fd != invalidFd) {
        VmiCloseConnection(fd);
        INFO("Connection:%d has been destoryed", fd);
        SetFd(invalidFd);
    }
}

uint64_t ConnectionSocket::GetBytesRecv() const
{
    return m_totalRecvByte;
}

uint64_t ConnectionSocket::GetBytesSend() const
{
    return m_totalSendByte;
}
}