/**
 * 版权所有 (c) 华为技术有限公司 2023-2023
 * 服务端网络管理模块类，负责依据网络类型，启动相关监听服务或者连接对应的服务端
 * 1. 依据部署类型，传入对应的网络类型（视频流场景、指令流场景、弹性GPU场景），启动对应的监听服务或连接对应端口
 * 2. 管理网络连接，监听到网络连接断开时，回调离线消息，新收到链接时，回到上线消息
 */
#include "NetworkCommManager.h"
#include <functional>
#include <unistd.h>
#include "VmiMsgHead.h"
#include "logging.h"
#include "SmartPtrMacro.h"
#include "MainLooper.h"
#include "SystemProperty.h"
#include "Communication/Connection/ClientConnection.h"

namespace Vmi {
namespace {
    const unsigned int g_port = 8888;
}

BreakNetCommRunnable::BreakNetCommRunnable(NetComm& comm) : m_comm(comm) {}

int BreakNetCommRunnable::Run()
{
    NetworkCommManager::GetInstance().BreakNetCommSync(m_comm);
    return 0;
}

int ConnectRenderServerRunnable::Run()
{
    NetworkCommManager::GetInstance().ConnectRenaderServer();
    return 0;
}

NetworkCommManager& NetworkCommManager::GetInstance()
{
    static NetworkCommManager instance;
    return instance;
}

NetworkCommManager::~NetworkCommManager()
{
    UnInitialize();
}

bool NetworkCommManager::Initialize(NetworkType type)
{
    if (!VerifyType(type)) {
        ERR("Failed to init network manager, type:%d is invalid", static_cast<int>(type));
        return false;
    }
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_isInit) {
        WARN("Network manager repeat initialization");
        return true;
    }
    if (m_callback == nullptr) {
        WARN("Failed to init network manger, new connection callback not set");
        return false;
    }
    m_type = type;
    if (!GetElasticGpuRenderInfo()) {
        ERR("Failed to init network manager, get elastic gpu info failed");
        return false;
    }
    MAKE_UNIQUE_NOTHROW(m_serverListen, ServerConnection, g_port, GetVersionCheckType());
    if (m_serverListen == nullptr) {
        ERR("Failed to init network manager, new server connection failed");
        return false;
    }
    m_serverListen->RegisterNewClientCallback(std::bind(&NetworkCommManager::AcceptNewNetComm,
        this, std::placeholders::_1, std::placeholders::_2));
    if (!m_serverListen->Start()) {
        ERR("Failed to init network manager, start server failed");
        return false;
    }
    // 如果是保持连接，则投递开始连接的任务
    if (m_type == NetworkType::ELASTIC_GPU && m_isKeep) {
        PostConnectRenderServerRunnable();
    }
    m_isInit = true;
    return true;
}

bool NetworkCommManager::ConnectRenderServerImpl()
{
    if (m_clientComm != nullptr) {
        WARN("Failed to connect render server, already connected");
        return true;
    }
    ClientConnection clientConnection(VersionCheck::EngineType::INSTRUCTION_ENGINE_TYPE, true);
    std::unique_ptr<NetComm> netComm = clientConnection.Connect(m_renderIp, m_renderPort);
    if (netComm == nullptr) {
        ERR("Failed to connect server, connect failed");
        return false;
    }
    // 设置网络模块异常断开的回调函数
    netComm->SetBreakCallback(std::bind(&NetworkCommManager::BreakNetComm, this, std::placeholders::_1));
    // 创建NetworkComm对象
    std::shared_ptr<NetworkComm> networkComm = nullptr;
    MAKE_SHARED_NOTHROW(networkComm, NetworkCommmElasticGpu, std::move(netComm));
    if (networkComm == nullptr) {
        ERR("Failed to connect server, new network comm failed");
        netComm->Stop();
        return false;
    }
    // 调Start()函数，开启收发数据、心跳
    if (!networkComm->Start()) {
        m_callback(false, networkComm);
        ERR("Failed to connect server, start failed");
        return false;
    }
    // 先通知其他模块，此连接已经上线（同步操作）
    if (m_callback != nullptr) {
        m_callback(true, networkComm);
    }
    m_clientComm = networkComm;
    return true;
}

void NetworkCommManager::UnInitialize()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_isInit) {
        return;
    }
    m_isInit = false;
    m_callback = nullptr;
    m_clientComm = nullptr;
    m_serverComm = nullptr;
    m_serverListen = nullptr;
}

uint32_t NetworkCommManager::GetSendDataReservedByte()
{
    return sizeof(StreamMsgHead);
}

void NetworkCommManager::RegisterNetworkChangeStatus(NetworkChangeCallback callback)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_callback != nullptr) {
        ERR("Failed to register network change status callback, duplicate setting");
        return;
    }
    m_callback = callback;
}

bool NetworkCommManager::VerifyType(NetworkType type)
{
    if (type == NetworkType::VIDEO_SERVER || type == NetworkType::INSTRUCTION_SERVER ||
        type == NetworkType::ELASTIC_GPU) {
        return true;
    }
    return false;
}

void NetworkCommManager::BreakNetCommSync(NetComm& comm)
{
    // 加锁找到对应的资源
    std::lock_guard<std::mutex> lock(m_mutex);
    bool isServer = false;
    std::shared_ptr<NetworkComm> breakNetcomm = nullptr;
    if (m_serverComm != nullptr && m_serverComm->IsSameNetComm(&comm)) {
        isServer = true;
        breakNetcomm = m_serverComm;
        m_serverComm = nullptr;
    } else if (m_clientComm != nullptr && m_clientComm->IsSameNetComm(&comm)) {
        isServer = false;
        breakNetcomm = m_clientComm;
        m_clientComm = nullptr;
    } else {
        ERR("Failed to deal break network comm, not found");
        return;
    }
    comm.Stop();
    INFO("begin notify netcomm break is server:%d", static_cast<int>(isServer));
    if (m_callback != nullptr) {
        m_callback(false, breakNetcomm);
    }
    ElasticGpuNetCommBreak(isServer);
}

void NetworkCommManager::ElasticGpuNetCommBreak(bool isServer)
{
    if (m_type != NetworkType::ELASTIC_GPU) {
        return;
    }
    if (!m_isKeep) {
        // 如果是随用随断，无论是视频流连接断开还是指令流连接断开，另外一个连接都需要同步断开
        std::shared_ptr<NetworkComm> simulBreakNetcomm = nullptr;
        if (isServer) {
            simulBreakNetcomm = m_clientComm;
            m_clientComm = nullptr;
        } else {
            simulBreakNetcomm = m_serverComm;
            m_serverComm = nullptr;
        }
        if (m_callback != nullptr && simulBreakNetcomm != nullptr) {
            m_callback(false, simulBreakNetcomm);
        }
    } else {
        // 若保持连接的情况下，指令流连接断开了，则视频流连接也需要断开，并尝试重新连接指令流
        if (!isServer) {
            std::shared_ptr<NetworkComm> simulBreakNetcomm = nullptr;
            if (m_serverComm != nullptr) {
                simulBreakNetcomm = m_serverComm;
                m_serverComm = nullptr;
            }
            if (m_callback != nullptr && simulBreakNetcomm != nullptr) {
                m_callback(false, simulBreakNetcomm);
            }
            PostConnectRenderServerRunnable();
        }
    }
}

void NetworkCommManager::ConnectRenaderServer()
{
    bool isConnectSuccess = false;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!ConnectRenderServerImpl()) {
            isConnectSuccess = false;
        } else {
            isConnectSuccess = true;
        }
    }
    if (!isConnectSuccess) {
        // 未连接成功的情况下，将等待1秒，继续投递任务尝试连接
        constexpr uint32_t sleepTime = 1000000; // 1s
        usleep(sleepTime);
        PostConnectRenderServerRunnable();
    }
}

void NetworkCommManager::PostConnectRenderServerRunnable()
{
    // 投递连接渲染服务器的任务
    std::unique_ptr<ConnectRenderServerRunnable> runnable = nullptr;
    MAKE_UNIQUE_NOTHROW(runnable, ConnectRenderServerRunnable);
    if (runnable == nullptr) {
        ERR("Failed to post connect render server runnable, new failed");
        return;
    }
    MainLooper::GetInstance().SendMsg(std::move(runnable));
}

void NetworkCommManager::BreakNetComm(NetComm& comm)
{
    // 投递异步任务
    std::unique_ptr<BreakNetCommRunnable> runnable = nullptr;
    MAKE_UNIQUE_NOTHROW(runnable, BreakNetCommRunnable, comm);
    if (runnable == nullptr) {
        ERR("Failed to call break netcomm, new break netcomm runnable failed");
        return;
    }
    MainLooper::GetInstance().SendMsg(std::move(runnable));
}

void NetworkCommManager::AcceptNewNetComm(const ServerConnection& connection, std::unique_ptr<NetComm> comm)
{
    if (comm == nullptr) {
        ERR("Failed to accept new connection, port:%d, netcomm is nullptr", connection.GetPort());
        return;
    }
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_serverComm != nullptr) {
        ERR("Previously connected but not disconnected, rejecting newly received links");
        comm->Stop();
        return;
    }
    if (m_type == NetworkType::ELASTIC_GPU && m_clientComm == nullptr && m_isKeep) {
        ERR("Render server is disconnect, waitting for connect render server");
        comm->Stop();
        return;
    }
    // 设置网络模块异常断开的回调函数
    comm->SetBreakCallback(std::bind(&NetworkCommManager::BreakNetComm, this, std::placeholders::_1));
    // 创建NetworkComm对象
    std::shared_ptr<NetworkComm> networkComm = nullptr;
    if (m_type == NetworkType::VIDEO_SERVER || m_type == NetworkType::ELASTIC_GPU) {
        MAKE_SHARED_NOTHROW(networkComm, NetworkCommmVideo, std::move(comm));
    } else if (m_type == NetworkType::INSTRUCTION_SERVER) {
        MAKE_SHARED_NOTHROW(networkComm, NetworkCommmInstruction, std::move(comm));
    } else {
        ERR("Failed to accept connection, type:%d is error", static_cast<int>(m_type));
    }
    if (networkComm == nullptr) {
        ERR("Failed to accpet new connection, port:%d, new network comm failed", connection.GetPort());
        comm->Stop();
        return;
    }
    networkComm->RegisterActiveDisconnectCallback(
        std::bind(&NetworkCommManager::BreakNetComm, this, std::placeholders::_1));
    // 调Start()函数，开启收发数据、心跳
    if (!networkComm->Start()) {
        m_callback(false, networkComm);
        ERR("Failed to accpet new connection, port:%d, start failed", connection.GetPort());
        return;
    }
    // 先通知其他模块，此连接已经上线（同步操作）
    if (m_callback != nullptr) {
        m_callback(true, networkComm);
    }
    if (m_type == NetworkType::ELASTIC_GPU && m_clientComm == nullptr && !m_isKeep) {
        if (!ConnectRenderServerImpl()) {
            ERR("Failed to connect render server");
            m_callback(false, networkComm);
            return;
        }
    }
    m_serverComm = networkComm;
    return;
}

VersionCheck::EngineType NetworkCommManager::GetVersionCheckType()
{
    if (m_type == NetworkType::VIDEO_SERVER || m_type == NetworkType::ELASTIC_GPU) {
        return VersionCheck::EngineType::VIDEO_ENGINE_TYPE;
    } else if (m_type == NetworkType::INSTRUCTION_SERVER) {
        return VersionCheck::EngineType::INSTRUCTION_ENGINE_TYPE;
    } else {
        WARN("Failed to get version type, type:%d", static_cast<int>(m_type));
        return VersionCheck::EngineType::UNKNOWN;
    }
}

bool NetworkCommManager::GetElasticGpuRenderInfo()
{
    if (m_type != NetworkType::ELASTIC_GPU) {
        return true;
    }
    int32_t isKeep = GetPropertyWithDefault("vmi.render.is.keep", -1);
    if (isKeep != 0 && isKeep != 1) {
        ERR("Failed to get elastic gpu render info, vmi.render.is.keep:%d must be 0/1", isKeep);
        return false;
    }
    m_isKeep = (isKeep == 1);
    std::string renderIp = GetPropertyWithDefault("vmi.render.ip", "0");
    if (renderIp == "0") {
        ERR("Failed to get elastic gpu render info, vmi.render.ip:%s must be set", renderIp.c_str());
        return false;
    }
    int renderPort = GetPropertyWithDefault("vmi.render.port", -1);
    if (renderPort == -1) {
        ERR("Failed to get elastic gpu render info, vmi.render.port:%d must be set", renderPort);
        return false;
    }
    m_renderIp = renderIp;
    m_renderPort = renderPort;
    return true;
}
}