/**
 * 版权所有 (c) 华为技术有限公司 2023-2024
 * INetworkComm的实现类，及隐藏内部的NetComm
 * 1. 实现了INetworkComm接口
 * 2. 是Common中NetComm的适配类，自身只是一个接口转发，对内部的NetComm隐藏
 * 3. 子类NetworkCommmVideo/NetworkCommmElasticGpu/NetworkCommmInstruction通过重写
 *    IsConveyDataType接口去控制此网络连接需承载哪些数据消息
 * 注：NetworkComm中的m_netComm外部已保证此智能指针不为空，为了效率考虑，每个接口不判断是否为空
 */
#ifndef NETWORKCOMM_H
#define NETWORKCOMM_H
#include <memory>
#include "INetworkComm.h"
#include "VmiDef.h"
#include "Include/VmiEngine.h"
#include "Communication/NetComm/NetComm.h"
namespace Vmi {
class NetworkComm : public INetworkComm {
public:
    explicit NetworkComm(std::unique_ptr<NetComm> comm);

    bool Start();

    int SendWithReservedByte(VmiDataType type, uint8_t* data, uint32_t size) override;

    void RegisterRecvDataCallback(VmiDataType type, RecvDataCallback callback, bool isSingleFragment) override;

    void ActiveDisconnect() override;

    bool IsSameNetComm(NetComm* res);

    void RegisterActiveDisconnectCallback(NetComm::NetCommBreakCallback cb);
private:
    VMIMsgType CovertDataTypeToMsgType(VmiDataType type);
    std::unique_ptr<NetComm> m_netComm;
    NetComm::NetCommBreakCallback m_disconnectCallback;
};

// 视频流场景，建立的网络连接对象
// 需承载音频、触控、按键、视频流的数据
class NetworkCommmVideo : public NetworkComm {
public:
    explicit NetworkCommmVideo(std::unique_ptr<NetComm> comm) : NetworkComm(std::move(comm)) {}
    bool IsConveyDataType(VmiDataType type) override;
};

// 弹性GPU场景，与渲染服务器建立的网络连接对象
// 需承载GL消息、RENDER_VIDEO(从渲染服务器回传的视频数据)的数据
class NetworkCommmElasticGpu : public NetworkComm {
public:
    explicit NetworkCommmElasticGpu(std::unique_ptr<NetComm> comm) : NetworkComm(std::move(comm)) {}
    bool IsConveyDataType(VmiDataType type) override;
};

// 指令流场景，建立的网络连接对象
// 需承载音频、触控、按键、GL消息的数据
class NetworkCommmInstruction : public NetworkComm {
public:
    explicit NetworkCommmInstruction(std::unique_ptr<NetComm> comm) : NetworkComm(std::move(comm)) {}
    bool IsConveyDataType(VmiDataType type) override;
};
}
#endif