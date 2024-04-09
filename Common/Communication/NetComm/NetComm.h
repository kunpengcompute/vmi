/*
 * 版权所有 (c) 华为技术有限公司 2017-2021
 * 功能说明:网络通信模块，封装包解析、分包、组包、心跳
 */
#ifndef NET_COMM_H
#define NET_COMM_H

#include <mutex>
#include <array>
#include <atomic>
#include "CommonDefine.h"
#include "VmiMsgHead.h"
#include "Socket/VmiSocket.h"
#include "Heartbeat/Heartbeat.h"
#include "StreamParse/StreamParser.h"
#include "PacketHandle/PacketHandle.h"
#include "MsgFragment/MsgFragment.h"

namespace Vmi {

enum VmiNetComRetCode : int32_t {
    VMI_ENGINE_INVALID_PARAM = 1,
    VMI_ENGINE_UNINIT_SOCK = 2,
    VMI_ENGINE_SEND_ALLOC_FAILED = 3,
    VMI_ENGINE_SEND_MEMSET_FAILED = 4,
    VMI_ENGINE_SEND_MEMCPY_FAILED = 5,
    VMI_ENGINE_SEND_FAIL = 6,
    VMI_ENGINE_HOOK_REGISTER_FAIL = 7
};

class __attribute__ ((visibility ("default"))) NetComm {
public:
    using NetCommBreakCallback = std::function<void(NetComm&)>;
    NetComm();
    virtual ~NetComm();
    void SetSocket(std::unique_ptr<VmiSocket> socket);

    /**
     * @功能描述：启动通信，启动解析线程和心跳
     * @返回值：成功返回true，失败返回false
     */
    bool Start();
    /**
     * @功能描述：停止网络通信（加锁）
     */
    void Stop();

    /**
     * @功能描述：发送type类型的数据
     * @参数 [in] type：数据的消息类型
     * @参数 [in] dataPair：数据的地址和大小pair
     * @返回值：成功返回VMI_SUCCESS，失败返回错误码
     */
    uint32_t Send(VMIMsgType type, const std::pair<uint8_t *, uint32_t>& dataPair);

    /**
     * @功能描述：发送type类型的数据
     * @参数 [in] type：数据的消息类型
     * @参数 [in] pkt：StreamMsgHead包头引用，之后是数据
     * @参数 [in] dataSize：数据大小
     * @返回值：成功返回VMI_SUCCESS，失败返回错误码
    */
    uint32_t Send(VMIMsgType type, StreamMsgHead& pkt, uint32_t dataSize);

    /**
     * @功能描述：设置Handle
     * @参数 [in] type：消息类型
     * @参数 [in] handle：对数据进行处理的PacketHandle实现类
     * @返回值：操作成功返回VMI_SUCCESS，失败返回VMI_ENGINE_HOOK_REGISTER_FAIL
     */
    uint32_t SetHandle(VMIMsgType type, std::shared_ptr<PacketHandle> serviceHandle);
    /**
     * @功能描述：注册处理钩子
     * @参数 [in] type：消息类型
     * @参数 [in] hook：钩子函数地址
     * @参数 [in] isSingleFragment：是否单包发送（不分片）
     * @返回值：操作成功返回VMI_SUCCESS，失败返回VMI_ENGINE_HOOK_REGISTER_FAIL
     */
    uint32_t RegisterHook(VMIMsgType type, RenderServerHandleHook hook, bool isSingleFragment);

    uint32_t RegisterHook(VMIMsgType type, RenderServerHandleFuncHook hook, bool isSingleFragment);

    /**
     * @功能描述：获取libsocket接收流量
     * @返回值：接收流量字节
     */
    uint64_t GetBytesRecv();

    /**
     * @功能描述：获取libsocket发送流量
     * @返回值：发送流量字节
     */
    uint64_t GetBytesSend();

    /**
     * @功能描述：获取心跳模块记录的loopback平均延迟，反映网络传输状况
     * @返回值：loopback平均延迟
     */
    int64_t GetLag();

    void HandleSocketBreak(const VmiSocket& breakSocket);

    void SetBreakCallback(NetCommBreakCallback breakCallback);
private:
    bool StartStreamParseAndHeartbeat();
    bool InitStreamParse();

    std::recursive_mutex m_lock = {};
    std::unique_ptr<VmiSocket> m_socket = nullptr;
    std::array<std::shared_ptr<PacketHandle>, VMIMsgType::END> m_pktHandle = {};
    std::shared_ptr<Heartbeat> m_heartbeat = nullptr;
    std::shared_ptr<StreamParse> m_streamParser = nullptr;
    std::unique_ptr<StreamParseThread> m_streamParseThread = nullptr;
    std::array<MsgFragment, VMIMsgType::END> m_fragments = {};
    std::atomic_flag m_isBroken = ATOMIC_FLAG_INIT;
    std::mutex m_callbackLock;
    NetCommBreakCallback m_callback = nullptr;
};
} // namespace Vmi
#endif
