/**
 * 版权所有 (c) 华为技术有限公司 2023-2023
 * 监听端口辅助类，主要功能如下：
 * 1. 开启某个端口的监听
 * 2. 收到新连接后，将回调上层新的连接
 */
#ifndef SERVER_CONNECTION_H
#define SERVER_CONNECTION_H
#include <memory>
#include <mutex>
#include <functional>
#include <thread>
#include "VersionCheck.h"
#include "NetComm/NetComm.h"

namespace Vmi {

class ServerConnection {
public:
    using NewConnectCb = std::function<void(const ServerConnection& connection, std::unique_ptr<NetComm> netcomm)>;

    ServerConnection(unsigned int port, VersionCheck::EngineType type);

    ~ServerConnection();

    bool Start();

    void Stop();

    void RegisterNewClientCallback(NewConnectCb cb);

    bool operator<(const ServerConnection& res);

    unsigned int GetPort() const;
private:
    void Run();

    void AcceptNewClient(int connection);
private:
    std::mutex m_mutex;
    int m_listenConnection = -1;
    NewConnectCb m_newConnectionCallback = nullptr;
    unsigned int m_port = 0;
    VersionCheck::EngineType m_engineType = VersionCheck::EngineType::UNKNOWN;
    std::thread m_thread = {};
    volatile bool m_threadStatus = false;
};
}

#endif