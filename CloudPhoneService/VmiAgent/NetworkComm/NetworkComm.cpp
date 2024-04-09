/**
 * 版权所有 (c) 华为技术有限公司 2023-2024
 * INetworkComm的实现类，及隐藏内部的NetComm
 * 1. 实现了INetworkComm接口
 * 2. 是Common中NetComm的适配类，自身只是一个接口转发，对内部的NetComm隐藏
 * 3. 子类NetworkCommmVideo/NetworkCommmElasticGpu/NetworkCommmInstruction通过重写
 *    IsConveyDataType接口去控制此网络连接需承载哪些数据消息
 * 注：NetworkComm中的m_netComm外部已保证此智能指针不为空，为了效率考虑，每个接口不判断是否为空
 */
#include "NetworkComm.h"
#include "logging.h"
namespace Vmi {

NetworkComm::NetworkComm(std::unique_ptr<NetComm> comm) : m_netComm(std::move(comm)) {}

bool NetworkComm::Start()
{
    return m_netComm->Start();
}

int NetworkComm::SendWithReservedByte(VmiDataType type, uint8_t* data, uint32_t size)
{
    if (data == nullptr) {
        ERR("Failed to send data, type:%u, size:%u", static_cast<uint32_t>(type), size);
        return -1;
    }
    StreamMsgHead* head = reinterpret_cast<StreamMsgHead *>(data);
    return m_netComm->Send(CovertDataTypeToMsgType(type), *head, size);
}

void NetworkComm::RegisterRecvDataCallback(VmiDataType type, RecvDataCallback callback, bool isSingleFragment)
{
    uint32_t ret = m_netComm->RegisterHook(CovertDataTypeToMsgType(type), callback, isSingleFragment);
    if (ret != 0) {
        ERR("Failed to register type:%u recv callback", static_cast<uint32_t>(type));
    }
}

void NetworkComm::ActiveDisconnect()
{
    if (m_disconnectCallback == nullptr) {
        ERR("Failed to active disconnect, callback not set");
        return;
    }
    m_disconnectCallback(*m_netComm.get());
}

bool NetworkComm::IsSameNetComm(NetComm* res)
{
    return res == m_netComm.get();
}

void NetworkComm::RegisterActiveDisconnectCallback(NetComm::NetCommBreakCallback cb)
{
    m_disconnectCallback = cb;
}

VMIMsgType NetworkComm::CovertDataTypeToMsgType(VmiDataType type)
{
    switch (type) {
        case DATA_AUDIO:
            return VMIMsgType::AUDIO;
        case DATA_MIC:
            return VMIMsgType::MIC;
        case DATA_VIDEO:
            return VMIMsgType::VIDEO_RR2;
        case DATA_TOUCH:
            return VMIMsgType::TOUCH_INPUT;
        default:
            return VMIMsgType::INVALID;
    }
}

// 视频流情况下建立的网络连接对象
// 连接需承载音频、触控、按键、视频流的数据
bool NetworkCommmVideo::IsConveyDataType(VmiDataType type)
{
    if (type == DATA_AUDIO || type == DATA_TOUCH || type == DATA_MIC || type == DATA_VIDEO) {
        return true;
    }
    return false;
}

// 弹性GPU场景与渲染服务器建立的网络连接对象
// 连接需承载GL消息、RENDER_VIDEO(从渲染服务器回传的视频数据)的数据
bool NetworkCommmElasticGpu::IsConveyDataType(VmiDataType type)
{
    (void) type;
    return false;
}

// 指令流情况下建立的网络连接对象
// 连接需承载音频、触控、按键、GL消息的数据
bool NetworkCommmInstruction::IsConveyDataType(VmiDataType type)
{
    (void) type;
    return false;
}
}