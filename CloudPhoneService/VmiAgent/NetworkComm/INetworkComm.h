/*
 * 版权所有 (c) 华为技术有限公司 2024-2024
 */
#ifndef INETWORK_COMM_H
#define INETWORK_COMM_H
#include <cstdint>
#include <memory>
#include <functional>
#include "VmiDef.h"
namespace Vmi {
using RecvDataCallback = std::function<uint32_t(std::pair<uint8_t*, uint32_t> data)>;

class INetworkComm {
public:
    INetworkComm() = default;
    virtual ~INetworkComm() = default;
    /**
     * @功能描述：判断此数据类型是否经过此网络传输
     * @参数 [in] type：数据类型
     * @返回值：若返回true，则表示此数据类型由这网络传输，若返回false，则表示不由此网络传输
     */
    virtual bool IsConveyDataType(VmiDataType type);

    /**
     * @功能描述：向网络模块发送数据，数据包的前面还预留的网络模块GetSendDataReservedByte()函数返回的字节数
     * @参数 [in] type：数据类型
     * @参数 [in] data：数据起始地址，内存排布：|ReserveByte|数据包|
     * @参数 [in] size: 数据包大小（不包含预留字节）
     * @返回值：返回已发送数据大小
     */
    virtual int SendWithReservedByte(VmiDataType type, uint8_t* data, uint32_t size);

    /**
     * @功能描述：注册接收网络数据的回调处理函数
     * @参数 [in] type：数据类型
     * @参数 [in] callback：接收网络数据回调函数指针
     * @参数 [in] isSingleFragment: 是否单包发送，既不分片，true是不分片，false是分片发送
     */
    virtual void RegisterRecvDataCallback(VmiDataType type, RecvDataCallback callback, bool isSingleFragment);

    /**
     * @功能描述：主动断开连接
     */
    virtual void ActiveDisconnect();
};
}
#endif
