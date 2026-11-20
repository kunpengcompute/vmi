/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2026. All rights reserved.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */

/**
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