/**
 * 版权所有 (c) 华为技术有限公司 2023-2023
 * 监听端口辅助类，主要功能如下：
 * 1. 开启某个端口的监听
 * 2. 收到新连接后，将回调上层新的连接
 */
#define LOG_TAG "ServerConnection"
#include "ServerConnection.h"
#include <functional>
#include "logging.h"
#include "SmartPtrMacro.h"
#include "Connection.h"
#include "ConnectionSocket.h"
namespace Vmi {
ServerConnection::ServerConnection(unsigned int port, VersionCheck::EngineType type) : m_port(port),
    m_engineType(type) {}

ServerConnection::~ServerConnection()
{
    Stop();
}

bool ServerConnection::Start()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_newConnectionCallback == nullptr) {
        ERR("Failed to start server connection, callback not set");
        return false;
    }
    m_listenConnection = VmiCreateConnection();
    if (m_listenConnection < 0) {
        ERR("Failed to start server connection, create connection failed");
        return false;
    }
    int ret = VmiListen(m_listenConnection, m_port);
    if (ret != CONNECTION_SUCCESS) {
        ERR("Failed to start server connection, listen connection:%d, port:%d failed", m_listenConnection, m_port);
        return false;
    }
    m_thread = std::thread(std::bind(&ServerConnection::Run, this));
    m_threadStatus = true;
    return true;
}

void ServerConnection::Stop()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_listenConnection != -1) {
        VmiCloseConnection(m_listenConnection);
        m_listenConnection = -1;
    }
    m_threadStatus = false;
    if (m_thread.joinable()) {
        m_thread.join();
    }
}

void ServerConnection::RegisterNewClientCallback(NewConnectCb cb)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_newConnectionCallback = cb;
}

void ServerConnection::Run()
{
    while (m_threadStatus) {
        int newAccpetConnection = VmiAccept(m_listenConnection);
        if (newAccpetConnection < 0) {
            WARN("Server connection failed to accept new client connection, listen connection:%d", m_listenConnection);
            continue;
        }
        AcceptNewClient(newAccpetConnection);
    }
}

void ServerConnection::AcceptNewClient(int connection)
{
    // 版本号检验
    VersionCheck check(connection, m_engineType);
    if (!check.CheckVersionServer()) {
        ERR("Failed to accpet new client, connection fd:%d, version check failed", connection);
        VmiCloseConnection(connection);
        return;
    }
    // 创建NetComm对象
    std::unique_ptr<NetComm> netComm = nullptr;
    MAKE_UNIQUE_NOTHROW(netComm, NetComm);
    if (netComm == nullptr) {
        ERR("Failed to accpet new client, connection fd:%d, create netcomm failed", connection);
        VmiCloseConnection(connection);
        return;
    }
    // 创建VmiSocket对象
    std::unique_ptr<ConnectionSocket> socket = nullptr;
    MAKE_UNIQUE_NOTHROW(socket, ConnectionSocket, connection);
    if (socket == nullptr) {
        ERR("Failed to accpet new client, connection fd:%d, create socket failed", connection);
        return;
    }
    // 设置SOCKET_STATUS_RUNNING状态
    socket->SetStatus(VmiSocketStatus::SOCKET_STATUS_RUNNING);
    // 创建NetComm对象
    netComm->SetSocket(std::move(socket));
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_newConnectionCallback != nullptr) {
        m_newConnectionCallback(*this, std::move(netComm));
    }
}

bool ServerConnection::operator<(const ServerConnection& res)
{
    return m_port < res.GetPort();
}

unsigned int ServerConnection::GetPort() const
{
    return m_port;
}
}