/**
 * 版权所有 (c) 华为技术有限公司 2023-2023
 * 服务端网络管理模块类，负责依据网络类型，启动相关监听服务或者连接对应的服务端
 * 1. 依据部署类型，传入对应的网络类型（视频流场景、指令流场景、弹性GPU场景），启动对应的监听服务或连接对应端口
 * 2. 管理网络连接，监听到网络连接断开时，回调离线消息，新收到链接时，回到上线消息
 */
#ifndef NETWORK_COMM_MANAGER_H
#define NETWORK_COMM_MANAGER_H
#include <mutex>
#include <memory>
#include <map>
#include <string>
#include <vector>
#include "NetworkExport.h"
#include "NetworkComm.h"
#include "Communication/Connection/ServerConnection.h"
#include "Communication/NetComm/NetComm.h"
#include "Utils/Looper.h"
namespace Vmi {
class BreakNetCommRunnable : public Runnable {
public:
    explicit BreakNetCommRunnable(NetComm& comm);

    int Run() override;
private:
    NetComm& m_comm;
};

class ConnectRenderServerRunnable : public Runnable {
public:
    int Run() override;
};

class NetworkCommManager {
public:
    static NetworkCommManager& GetInstance();

    bool Initialize(NetworkType type);

    void UnInitialize();

    uint32_t GetSendDataReservedByte();

    void RegisterNetworkChangeStatus(NetworkChangeCallback callback);

    void BreakNetCommSync(NetComm& comm);

    void ConnectRenaderServer();
private:
    NetworkCommManager() = default;
    ~NetworkCommManager();
    NetworkCommManager(const NetworkCommManager&) = delete;
    NetworkCommManager(NetworkCommManager &&) = delete;
    NetworkCommManager& operator=(const NetworkCommManager&) = delete;
    NetworkCommManager& operator=(NetworkCommManager &&) = delete;
    bool VerifyType(NetworkType type);
    VersionCheck::EngineType GetVersionCheckType();
    void BreakNetComm(NetComm& comm);
    void AcceptNewNetComm(const ServerConnection& connection, std::unique_ptr<NetComm> comm);
    bool ConnectRenderServerImpl();
    bool GetElasticGpuRenderInfo();
    void PostConnectRenderServerRunnable();
    void ElasticGpuNetCommBreak(bool isServer);
private:
    std::unique_ptr<ServerConnection> m_serverListen = nullptr;
    std::shared_ptr<NetworkComm> m_serverComm = nullptr;
    std::shared_ptr<NetworkComm> m_clientComm = nullptr;
    NetworkChangeCallback m_callback = nullptr;
    NetworkType m_type = NetworkType::ERROR;
    std::string m_renderIp = "";
    int m_renderPort = 0;
    std::mutex m_mutex = {};
    bool m_isInit = false;
    bool m_isKeep = false;
};
}

#endif