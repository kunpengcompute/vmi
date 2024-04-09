/**
 * 版权所有 (c) 华为技术有限公司 2023-2023
 * 连接服务端辅助类
 */
#ifndef CLIENT_CONNECTION_H
#define CLIENT_CONNECTION_H
#include "VersionCheck.h"
#include "NetComm/NetComm.h"
namespace Vmi {
class ClientConnection {
public:
    explicit ClientConnection(VersionCheck::EngineType type);
    ClientConnection(VersionCheck::EngineType type, bool isServer);
    ~ClientConnection() = default;
    std::unique_ptr<NetComm> Connect();

    std::unique_ptr<NetComm> Connect(unsigned int ip, unsigned int port);

    std::unique_ptr<NetComm> Connect(const std::string& ip, unsigned int port);
private:
    std::unique_ptr<NetComm> ConnectComm(int connection);
    VersionCheck::EngineType m_engineType = VersionCheck::EngineType::UNKNOWN;
    bool m_isServer = false;
};
}
#endif