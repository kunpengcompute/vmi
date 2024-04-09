/*
 * 版权所有 (c) 华为技术有限公司 2017-2022
 * 功能说明：视频流引擎客户端网络通信功能接口实现。
 */
#define LOG_TAG "NetController"
#include "NetController.h"
#include "CommonDefine.h"
#include "SmartPtrMacro.h"
#include "VersionCheck.h"
#include "VersionCheck.h"
#include "EngineEventHandler.h"
#include "Connection.h"
#include "Connection/ConnectionSocket.h"

namespace Vmi {

/**
 * @功能描述：NetController的单例函数
 * @返回值：返回NetController单例对象引用
 */
NetController& NetController::GetInstance()
{
    static NetController instance;
    return instance;
}

/**
 * @功能描述：NetController析构函数
 */
NetController::~NetController()
{
    SetState(JNIState::INVALID);
    m_netComm.Stop();
}

/**
 * @功能描述：初始化，创建通信组件
 * @返回值：成功返回true，失败返回false
 */
bool NetController::Init()
{
    std::lock_guard<std::recursive_mutex> lockGuard(m_lock);
    if (GetState() != JNIState::INVALID) {
        return true;
    }

    SetState(JNIState::INIT);
    return true;
}

/**
 * @功能描述：获取SDK当前状态
 * @返回值：参见JNIState枚举定义
 */
JNIState NetController::GetState()
{
    std::lock_guard<std::recursive_mutex> lockGuard(m_lock);
    return m_state;
}

/**
 * @功能描述：设置SDK状态
 * @参数 [in] state：参见JNIState枚举定义
 */
void NetController::SetState(JNIState state)
{
    INFO("State_set to %u", static_cast<uint32_t>(state));
    std::lock_guard<std::recursive_mutex> lockGuard(m_lock);
    m_state = state;
}

bool NetController::StartConnect()
{
    int connection = VmiCreateConnection();
    if (connection < 0) {
        ERR("Failed to start client, create connection failed");
        return false;
    }

    int ret = VmiBeginConnect(connection);
    if (ret != CONNECTION_SUCCESS) {
        ERR("Failed to start client, connect failed");
        VmiCloseConnection(connection);
        return false;
    }

    return OnNewConnect(connection);
}

bool NetController::OnNewConnect(int connection)
{
    INFO("Get new connection:%d", connection);
    std::lock_guard<std::recursive_mutex> lockGuard(m_lock);
    VersionCheck check(connection, VersionCheck::EngineType::VIDEO_ENGINE_TYPE);
    if (!check.CheckVersionClient()) {
        ERR("Failed to check version");
        VmiCloseConnection(connection);
        SetState(JNIState::CONNECTION_FAILURE);
        return false;
    }
    std::unique_ptr<ConnectionSocket> socket = nullptr;
    MAKE_UNIQUE_NOTHROW(socket, ConnectionSocket, connection);
    if (socket == nullptr) {
        ERR("error: No memory to create LibSocket");
        VmiCloseConnection(connection);
        SetState(JNIState::CONNECTION_FAILURE);
        return false;
    }

    socket->SetStatus(VmiSocketStatus::SOCKET_STATUS_RUNNING);
    m_netComm.SetSocket(std::move(socket));
    if (!m_netComm.Start()) {
        ERR("error: Failed to start video Net COMM");
        SetState(JNIState::CONNECTION_FAILURE);
        return false;
    }

    SetState(JNIState::CONNECTED);
    return true;
}

 /**
  * @功能描述：断开vmiagent socket
  */
void NetController::StopConnect()
{
    INFO("Begin NetController StopConnect");
    m_netComm.Stop();
}

/**
 * @功能描述：客户端SDK启动函数，创建各种消息队列和服务，创建心跳、数据解析线程等，
 *           创建socket连接服务端
 * @返回值：SDK启动结果，VMI_SUCCESS代表SDK启动成功，其他代表SDK启动失败
 */
int NetController::Start()
{
    std::lock_guard<std::recursive_mutex> lockGuard(m_lock);
    JNIState state = GetState();
    if (state == JNIState::INVALID) {
        WARN("NetController start exec order error, current state:%u", static_cast<uint32_t>(state));
        return VMI_CLIENT_EXEC_ORDER_FAIL;
    }
    if ((state == JNIState::START) || (state == JNIState::CONNECTED)) {
        INFO("Sdk already start!");
        return VMI_CLIENT_ALREADY_STARTED;
    }

    if (!StartConnect()) {
        StopConnect();
        ERR("Error: NetController start failed!");
        return VMI_CLIENT_START_FAIL;
    }
    SetState(JNIState::START);
    INFO("NetController start success");
    return VMI_SUCCESS;
}

/**
 * @功能描述：客户端SDK停止函数，销毁各种消息队列和服务，回收心跳、数据解析线程等，
 *           断开socket
 */
void NetController::Stop()
{
    INFO("Begin stop");
    std::lock_guard<std::recursive_mutex> lockGuard(m_lock);
    JNIState state = GetState();
    if ((state == JNIState::INVALID) || (state == JNIState::INIT)) {
        WARN("NetController stop exec order error, current state:%u", static_cast<uint32_t>(state));
        return;
    }
    if (GetState() == JNIState::STOPPED) {
        INFO("NetController already stopped");
        return;
    }
    StopConnect();
    SetState(JNIState::STOPPED);
}

/**
 * @功能描述：获取网络平均延迟
 * @返回值：网络平均延迟，单位是微秒，0表示无穷大
 */
int64_t NetController::GetLag()
{
    std::lock_guard<std::recursive_mutex> lockGuard(m_lock);
    return m_netComm.GetLag();
}

// 调用 NetComm 接口发送报文
uint32_t NetController::NetCommSend(VMIMsgType type, const std::pair<uint8_t *, uint32_t> &dataPair)
{
    return m_netComm.Send(type, dataPair);
}

// 设置 NetComm 的 Handle
uint32_t NetController::SetNetCommHandle(VMIMsgType type, std::shared_ptr<PacketHandle> handle)
{
    return m_netComm.SetHandle(type, handle);
}
}
