/**
 * 版权所有 (c) 华为技术有限公司 2023-2024
 * 服务端网络模块对外暴露接口，通过回调函数暴露网络连接的上线/离线消息，保证和业务处理解耦
 */
#ifndef NETWORKEXPORT_H
#define NETWORKEXPORT_H
#include <cstdint>
#include "INetworkComm.h"
#ifdef __cplusplus
extern "C" {
#endif

enum class NetworkType {
    ERROR = 0,
    VIDEO_SERVER,
    INSTRUCTION_SERVER,
    ELASTIC_GPU,
};

using NetworkChangeCallback = void(*)(bool isOnline, std::shared_ptr<Vmi::INetworkComm> netcomm);

bool NetworkInitialize(NetworkType type);

// 获取发送的网络数据预先预留的大小
uint32_t NetworkGetSendDataReservedByte();

// 回调网络状态变化，此函数要在网络模块初始化前设置
void RegisterNetworkChangeStatusCallback(NetworkChangeCallback callback);

#ifdef __cplusplus
}
#endif
#endif