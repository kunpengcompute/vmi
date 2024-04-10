/*
 * 版权所有 (c) 华为技术有限公司 2024-2024
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

void INetworkComm::RegisterRecvDataCallback(VmiDataType type, RecvDataCallback callback, bool isSingleFragment)
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