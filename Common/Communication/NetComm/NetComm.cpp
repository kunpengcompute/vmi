/*
 * 版权所有 (c) 华为技术有限公司 2017-2021
 * 功能说明:网络通信模块，封装包解析、分包、组包、心跳
 */
#define LOG_TAG "NetworkCommunication"

#include "NetComm.h"
#include <climits>
#include <cstring>
#include "SmartPtrMacro.h"
#include "logging.h"
#include "VmiMsgHead.h"
#include "EngineEventHandler.h"

using namespace std;

namespace Vmi {
/**
 * @功能描述：构造函数
 */
NetComm::NetComm()
{
    m_pktHandle.fill(nullptr);
}

/**
 * @功能描述：析构函数，释放对象
 */
NetComm::~NetComm()
{
    Stop();
}

/**
 * @功能描述：设置VmiSocket子类对象
 * @参数 [in] socket：VmiSocket子类对象
 */
void NetComm::SetSocket(std::unique_ptr<VmiSocket> socket)
{
    std::unique_lock<std::recursive_mutex> lockGuard(m_lock);
    m_socket = std::move(socket);
}

/**
 * @功能描述：获取心跳模块记录的loopback平均延迟，反映网络传输状况
 * @返回值：loopback平均延迟
 */
int64_t NetComm::GetLag()
{
    std::unique_lock<std::recursive_mutex> lockGuard(m_lock);
    return (m_heartbeat != nullptr) ? m_heartbeat->GetNetLoopbackLatencyAverage() : -1;
}

/**
 * @功能描述：设置Handle
 * @参数 [in] type：消息类型
 * @参数 [in] handle：对数据进行处理的PacketHandle实现类
 * @返回值：操作成功返回VMI_SUCCESS，失败返回VMI_ENGINE_HOOK_REGISTER_FAIL
 */
uint32_t NetComm::SetHandle(VMIMsgType type, std::shared_ptr<PacketHandle> handle)
{
    if (type <= VMIMsgType::INVALID || type >= VMIMsgType::END) {
        ERR("Failed to set handle, Invalid type:%u", type);
        return VMI_ENGINE_HOOK_REGISTER_FAIL;
    }

    if (handle == nullptr) {
        ERR("Failed to set handle, handle is null, type:%u", type);
        return VMI_ENGINE_HOOK_REGISTER_FAIL;
    }

    if (!InitStreamParse()) {
        ERR("Failed to set handle, Stream Parser is uninitialized, type:%u", type);
        return VMI_ENGINE_HOOK_REGISTER_FAIL;
    }
    m_streamParser->SetServiceHandle(type, handle);
    m_pktHandle[type] = handle;
    INFO("Set service handle, type:%u", type);
    return VMI_SUCCESS;
}

/**
 * @功能描述：注册处理钩子
 * @参数 [in] type：消息类型
 * @参数 [in] hook：钩子函数地址
 * @参数 [in] isSingleFragment：是否单包发送（不分片）
 * @返回值：操作成功返回VMI_SUCCESS，失败返回VMI_ENGINE_HOOK_REGISTER_FAIL
 */
uint32_t NetComm::RegisterHook(VMIMsgType type, RenderServerHandleHook hook, bool isSingleFragment)
{
    RenderServerHandleFuncHook func = hook;
    return RegisterHook(type, func, isSingleFragment);
}

uint32_t NetComm::RegisterHook(VMIMsgType type, RenderServerHandleFuncHook hook, bool isSingleFragment)
{
    if (type <= VMIMsgType::INVALID || type >= VMIMsgType::END) {
        ERR("Failed to register hook, Invalid type:%u", type);
        return VMI_ENGINE_HOOK_REGISTER_FAIL;
    }

    if (!InitStreamParse()) {
        ERR("Failed to register hook, Stream Parser is uninitialized, type:%u", type);
        return VMI_ENGINE_HOOK_REGISTER_FAIL;
    }

    std::shared_ptr<PacketHandle> handler = nullptr;
    MAKE_SHARED_NOTHROW(handler, PacketHandle, type, hook, isSingleFragment);
    if (handler == nullptr) {
        ERR("Failed to register hook, create packet handler failed, type:%u", type);
        return VMI_ENGINE_HOOK_REGISTER_FAIL;
    }
    INFO("Create packet handler:%u success", type);
    m_streamParser->SetServiceHandle(type, handler);
    m_pktHandle[type] = handler;
    INFO("Register Hook, type:%u", type);
    return VMI_SUCCESS;
}

/**
 * @功能描述：初始化StreamParse对象
 * @返回值：成功返回true，失败返回false
 */
bool NetComm::InitStreamParse()
{
    if (m_streamParser == nullptr) {
        MAKE_SHARED_NOTHROW(m_streamParser, StreamParse);
        if (m_streamParser == nullptr) {
            ERR("Failed to init stream parse, create Stream Parser failed");
            return false;
        }
    }
    return true;
}

/**
 * @功能描述：启动解析线程和心跳
 * @返回值：成功返回true，失败返回false
 */
bool NetComm::StartStreamParseAndHeartbeat()
{
    INFO("Starting Stream Parser thread & heartbeat thread...");
    MAKE_UNIQUE_NOTHROW(m_streamParseThread, StreamParseThread, *m_socket);
    if (m_streamParseThread == nullptr) {
        ERR("Failed to start stream parse and heartbeat, alloc stream parse thread failed");
        return false;
    }

    m_streamParseThread->SetStreamParse(m_streamParser);

    MAKE_SHARED_NOTHROW(m_heartbeat, Heartbeat, *m_socket,
            [this](const VmiSocket& breakSocket) -> void {
            ERR("Heartbeat timeout");
            HandleSocketBreak(breakSocket);
        });
    if (m_heartbeat == nullptr) {
        ERR("Failed to start stream parse and heartbeat, alloc heartbeat thread failed");
        return false;
    }

    if (m_streamParser != nullptr) {
        m_streamParser->SetServiceHandle(VMIMsgType::HEARTBEAT, m_heartbeat);
    }
    if (m_streamParseThread->Start() != 0) {
        ERR("Failed to start stream parse and heartbeat, start Stream Parser thread failed");
        return false;
    }
    INFO("Stream Parser thread started");

    if (!m_heartbeat->Start()) {
        ERR("Failed to start stream parse and heartbeat, start heartbeat thread failed");
        return false;
    }
    INFO("Heartbeart started");
    return true;
}

/**
 * @功能描述：启动通信，启动流解析线程和心跳
 * @返回值：成功返回true，失败返回false
 */
bool NetComm::Start()
{
    INFO("Begin to start net communication");
    std::unique_lock<std::recursive_mutex> lockGuard(m_lock);

    if (m_socket == nullptr) {
        ERR("Failed to start net communication, socket is null");
        return false;
    }
    m_socket->SetEventNotice(std::bind(&NetComm::HandleSocketBreak, this, std::placeholders::_1));

    if (!InitStreamParse()) {
        ERR("Failed to start net communication, init stream parse failed");
        Stop();
        return false;
    }

    if (!StartStreamParseAndHeartbeat()) {
        ERR("Failed to start net communication, start stream parse and heartbeat failed");
        Stop();
        return false;
    }

    m_isBroken.clear();
    INFO("Start net communication successed");
    return true;
}

/**
 * @功能描述：停止网络通信
 */
void NetComm::Stop()
{
    INFO("Begein to stop net communication");
    (void)m_isBroken.test_and_set();
    std::unique_lock<std::recursive_mutex> lockGuard(m_lock);
    if (m_socket != nullptr) {
        m_socket->CloseSocket();
        INFO("Security Audit: close client socket");
    }

    if (m_heartbeat != nullptr) {
        INFO("Begin to stop heartbeat");
        m_heartbeat->ForceStop();
        INFO("Security Audit: heartbeat stopped");
    }

    if (m_streamParseThread != nullptr) {
        INFO("Begin to stop Stream Parser thread");
        m_streamParseThread->Stop();
        INFO("Security Audit: Stream Parser thread stopped");
    }

    for (uint8_t type = VMIMsgType::HEARTBEAT; type < VMIMsgType::END; ++type) {
        if (m_pktHandle[type] != nullptr) {
            m_pktHandle[type]->ClearQueue();
            INFO("Clear stream:%u", type);
        }
    }

    INFO("Stop net communication end");
}

/**
 * @功能描述：发送type类型的数据
 * @参数 [in] type：数据的消息类型
 * @参数 [in] dataPair：数据的地址和大小pair
 * @返回值：成功返回VMI_SUCCESS，失败返回错误码
 */
uint32_t NetComm::Send(VMIMsgType type, const std::pair<uint8_t *, uint32_t> &dataPair)
{
    if (type <= VMIMsgType::INVALID || type >= VMIMsgType::END) {
        ERR("Failed to send, Invalid msg type: %u", type);
        return VMI_ENGINE_INVALID_PARAM;
    }

    uint8_t *data = dataPair.first;
    uint32_t length = dataPair.second;
    if (data == nullptr || length == 0 || length > MAX_MSG_SIZE) {
        ERR("Failed to send, Invalid paramter, data %s null, length:%u, max size:%d, type:%u",
            (data == nullptr) ? "is" : "is not", length, MAX_MSG_SIZE, type);
        return VMI_ENGINE_INVALID_PARAM;
    }

    std::unique_lock<std::recursive_mutex> lockGuard(m_lock);
    if (m_socket == nullptr) {
        ERR("Failed to send, Socket is null, type:%u", type);
        return VMI_ENGINE_UNINIT_SOCK;
    }

    std::unique_ptr<uint8_t[]> message = nullptr;
    MAKE_UNIQUE_NOTHROW(message, uint8_t[], length + sizeof(StreamMsgHead));
    if (message == nullptr) {
        ERR("Failed to send, Failed to alloc message buffer, type:%u", type);
        return VMI_ENGINE_SEND_ALLOC_FAILED;
    }
    (void)std::fill_n(message.get(), length + sizeof(StreamMsgHead), 0);

    memcpy(message.get() + sizeof(StreamMsgHead), data, length);
    int ret = m_fragments[type].FragmentSend(type, *m_socket,
        *(reinterpret_cast<StreamMsgHead *>(message.get())), length);
    if (ret == static_cast<int>(length)) {
        return VMI_SUCCESS;
    } else {
        ERR("Failed to send, Fragment send failed, errno:%d, socketfd:%d, type:%u",
            ret, m_socket->GetFd(), type);
        return VMI_ENGINE_SEND_FAIL;
    }
}

/**
 * @功能描述：发送type类型的数据
 * @参数 [in] type：数据的消息类型
 * @参数 [in] pkt：StreamMsgHead包头引用，之后是数据
 * @参数 [in] dataSize：数据大小
 * @返回值：成功返回VMI_SUCCESS，失败返回错误码
 */
uint32_t NetComm::Send(VMIMsgType type, StreamMsgHead &pkt, uint32_t dataSize)
{
    if (type <= VMIMsgType::INVALID || type >= VMIMsgType::END) {
        ERR("Failed to send, Invalid msg type:%u", type);
        return VMI_ENGINE_INVALID_PARAM;
    }

    if (dataSize == 0 || dataSize > MAX_MSG_SIZE) {
        ERR("Failed to send, Invalid paramter, dataSize:%u, type:%u", dataSize, type);
        return VMI_ENGINE_INVALID_PARAM;
    }

    std::unique_lock<std::recursive_mutex> lockGuard(m_lock);
    if (m_socket == nullptr) {
        ERR("Failed to send, Socket is null when send from net communication, type:%u", type);
        return VMI_ENGINE_UNINIT_SOCK;
    }

    int ret = m_fragments[type].FragmentSend(type, *m_socket, pkt, dataSize);
    if (static_cast<size_t>(ret) == dataSize) {
        return VMI_SUCCESS;
    }
    ERR("Failed to send, Fragment send failed, errno:%d, socketfd:%d, type:%u",
        ret, m_socket->GetFd(), type);
    return VMI_ENGINE_SEND_FAIL;
}

/**
 * @功能描述：获取libsocket接收的流量
 * @返回值：接收流量字节
 */
uint64_t NetComm::GetBytesRecv()
{
    std::unique_lock<std::recursive_mutex> lockGuard(m_lock);
    if (m_socket != nullptr) {
        return m_socket->GetBytesRecv();
    } else {
        return 0;
    }
}

/**
 * @功能描述：获取libsocket发送的流量
 * @返回值：发送流量字节
 */
uint64_t NetComm::GetBytesSend()
{
    std::unique_lock<std::recursive_mutex> lockGuard(m_lock);
    if (m_socket != nullptr) {
        return m_socket->GetBytesSend();
    } else {
        return 0;
    }
}

// 处理Socket中断事件上报，通过原子变量 m_isBroken 限制中断事件只上报一次
// 注意：该函数会被底层回调，为避免死锁，需禁止加锁。
void NetComm::HandleSocketBreak(const VmiSocket& breakSocket)
{
    bool doNotify = false;
    {
        if (!m_isBroken.test_and_set()) {
            ERR("Failed to handle socket break, Socket(%d) broken", breakSocket.GetFd());
            doNotify = true;
        }
    }
    // 通知socket异常断开
    if (doNotify) {
        std::lock_guard<std::mutex> lock(m_callbackLock);
        if (m_callback != nullptr) {
            m_callback(*this);
        } else {
            struct EngineEvent engineEvent = {VMI_ENGINE_EVENT_SOCK_DISCONN, 0, 0, 0, 0};
            EngineEventHandler::GetInstance().CreateEvent(engineEvent);
        }
    }
}

void NetComm::SetBreakCallback(NetCommBreakCallback breakCallback)
{
    std::lock_guard<std::mutex> lock(m_callbackLock);
    m_callback = breakCallback;
}
} // namespace Vmi
