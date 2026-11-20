/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2026. All rights reserved.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */
#include "INetworkComm.h"

namespace Vmi {
// INetworkComm定义的虚函数接口需二次开发者继承INetworkComm重写
bool INetworkComm::IsConveyDataType(VmiDataType type)
{
    (void) type;
    return false;
}

int INetworkComm::SendWithReservedByte(VmiDataType type, uint8_t* data, uint32_t size)
{
    (void) type;
    (void) data;
    (void) size;
    return 0;
}

int INetworkComm::SendWithReservedByte(VMIMsgType type, uint8_t* data, uint32_t size)
{
    (void) type;
    (void) data;
    (void) size;
    return 0;
}

void INetworkComm::RegisterRecvDataCallback(VmiDataType type, RecvDataCallback callback, bool isSingleFragment)
{
    (void) type;
    (void) callback;
    (void) isSingleFragment;
    return;
}

void INetworkComm::RegisterRecvDataCallback(VMIMsgType type, RecvDataCallback callback, bool isSingleFragment)
{
    (void) type;
    (void) callback;
    (void) isSingleFragment;
    return;
}

void INetworkComm::ActiveDisconnect()
{
    return;
}
}