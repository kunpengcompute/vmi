/**
 * 版权所有 (c) 华为技术有限公司 2023-2023
 * 服务端网络模块对外暴露接口，通过回调函数暴露网络连接的上线/离线消息，保证和业务处理解耦
 */
#include "NetworkExport.h"
#include "NetworkCommManager.h"

bool NetworkInitialize(NetworkType type)
{
    return Vmi::NetworkCommManager::GetInstance().Initialize(type);
}

uint32_t NetworkGetSendDataReservedByte()
{
    return Vmi::NetworkCommManager::GetInstance().GetSendDataReservedByte();
}

void RegisterNetworkChangeStatusCallback(NetworkChangeCallback callback)
{
    return Vmi::NetworkCommManager::GetInstance().RegisterNetworkChangeStatus(callback);
}