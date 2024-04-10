/**
 * 版权所有 (c) 华为技术有限公司 2023-2023
 * 连接服务端辅助类
 */
#define LOG_TAG "ClientConnection"
#include "ClientConnection.h"
#include "Connection.h"
#include "logging.h"
#include "SmartPtrMacro.h"
#include "ConnectionSocket.h"
#include <arpa/inet.h>

namespace Vmi {
ClientConnection::ClientConnection(VersionCheck::EngineType type) : m_engineType(type) {}

ClientConnection::ClientConnection(VersionCheck::EngineType type, bool isServer) : m_engineType(type),
    m_isServer(isServer) {}

/**
 * 调用此函数时，已经将ip和端口传给libConnection.so
 * 返回的NetComm对象没有调Start，即还没有开始收发数据
 * 拿到此接口返回的对象后，应该先注册感兴趣的消息回调函数，之后再调Start
 */
std::unique_ptr<NetComm> ClientConnection::Connect()
{
    int connection = VmiCreateConnection();
    if (connection < 0) {
        ERR("Failed to connect client, create connection failed");
        VmiCloseConnection(connection);
        return nullptr;
    }
    int ret = VmiBeginConnect(connection);
    if (ret != CONNECTION_SUCCESS) {
        ERR("Failed to connect client, begin connect failed");
        VmiCloseConnection(connection);
        return nullptr;
    }
    return ConnectComm(connection);
}

std::unique_ptr<NetComm> ClientConnection::Connect(const std::string& ip, unsigned int port)
{
    struct in_addr addr = {};
    int ret = inet_aton(ip.c_str(), &addr);
    if (ret == 0) {
        ERR("Failed to connect client, please check ip and port");
        return 0;
    }
    unsigned int ipInfo = ntohl(addr.s_addr);
    return Connect(ipInfo, port);
}

std::unique_ptr<NetComm> ClientConnection::Connect(unsigned int ip, unsigned int port)
{
    int connection = VmiCreateConnection();
    if (connection < 0) {
        ERR("Failed to connect client, create connection failed");
        VmiCloseConnection(connection);
        return nullptr;
    }
    int ret = VmiConnect(connection, ip, port);
    if (ret != CONNECTION_SUCCESS) {
        ERR("Failed to connect client, connect failed");
        VmiCloseConnection(connection);
        return nullptr;
    }
    return ConnectComm(connection);
}

std::unique_ptr<NetComm> ClientConnection::ConnectComm(int connection)
{
    // 版本号检验
    VersionCheck check(connection, m_engineType);
    if (m_isServer) {
        constexpr uint32_t waitTime = 5000;
        if (!check.CheckVersionServer(waitTime)) {
            ERR("Failed to connect client, connection fd:%d, check server version in %u ms failed",
                connection, waitTime);
            VmiCloseConnection(connection);
            return nullptr;
        }
    } else {
        if (!check.CheckVersionServer()) {
            ERR("Failed to connect client, connection fd:%d, check server version failed", connection);
            VmiCloseConnection(connection);
            return nullptr;
        }
    }

    // 创建NetComm对象
    std::unique_ptr<NetComm> netComm = nullptr;
    MAKE_UNIQUE_NOTHROW(netComm, NetComm);
    if (netComm == nullptr) {
        ERR("Failed to connect client, connection fd:%d, create netcomm failed", connection);
        VmiCloseConnection(connection);
        return nullptr;
    }
    // 创建VmiSocket对象
    std::unique_ptr<ConnectionSocket> socket = nullptr;
    MAKE_UNIQUE_NOTHROW(socket, ConnectionSocket, connection);
    if (socket == nullptr) {
        ERR("Failed to connect client, conection fd:%d, create socket failed", connection);
        return nullptr;
    }
    // 设置SOCKET_STATUS_RUNNING状态
    socket->SetStatus(VmiSocketStatus::SOCKET_STATUS_RUNNING);
    // 创建NetComm对象
    netComm->SetSocket(std::move(socket));
    return netComm;
}
}