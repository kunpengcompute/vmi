/*
 * 版权所有 (c) 华为技术有限公司 2017-2022
 * 功能说明：视频流引擎客户端网络通信功能接口。
 */
#ifndef NET_CONTROLLER_H
#define NET_CONTROLLER_H
#include <mutex>
#include "logging.h"
#include "NetComm/NetComm.h"

namespace Vmi {
enum class JNIState : uint32_t {
    INVALID = 0,
    INIT,
    START,
    CONNECTED,
    STOPPED,
    CONNECTION_FAILURE
};

class NetController {
public:
    /**
     * @功能描述：NetController的单例函数
     * @返回值：返回NetController单例对象引用
     */
    static NetController& GetInstance();

    /**
     * @功能描述：初始化，创建通信组件
     * @返回值：成功返回true，失败返回false
     */
    bool Init();

    /**
     * @功能描述：客户端SDK启动函数，创建各种消息队列和服务，创建心跳、数据解析线程等，
     *           创建socket连接连接服务端
     * @返回值：SDK启动结果，VMI_SUCCESS代表SDK启动成功，其他代表SDK启动失败
     */
    int Start();

    bool OnNewConnect(int connection);

    /**
     * @功能描述：客户端SDK停止函数，销毁各种消息队列和服务，回收心跳、数据解析线程等，
     *           断开socket连接
     */
    void Stop();

    /**
     * @功能描述：获取网络平均延迟
     * @返回值：网络平均延迟，单位是微秒，0表示无穷大
     */
    int64_t GetLag();

    // 调用 NetComm 接口发送报文
    uint32_t NetCommSend(VMIMsgType type, const std::pair<uint8_t *, uint32_t> &dataPair);

    // 设置 NetComm 的 Handle
    uint32_t SetNetCommHandle(VMIMsgType type, std::shared_ptr<PacketHandle> handle);

private:
    /**
     * @功能描述：NetController构造函数
     */
    NetController() = default;

    /**
     * @功能描述：NetController析构函数
     */
    ~NetController();

    /**
     * @功能描述：NetController复制拷贝构造函数
     * @参数 [in] control：需要复制拷贝的NetController对象
     */
    NetController(const NetController& control) = delete;

    /**
     * @功能描述：NetController拷贝赋值函数
     * @参数 [in] control：需要复制拷贝的NetController对象
     */
    NetController& operator=(const NetController& control) = delete;

    /**
     * @功能描述：NetController移动构造函数
     * @参数 [in] control：需要移动构造的NetController对象
     */
    NetController(const NetController&& control) = delete;

    /**
     * @功能描述：NetController移动赋值函数
     * @参数 [in] control：需要移动赋值的NetController对象
     */
    NetController& operator=(const NetController&& control) = delete;

    /**
     * @功能描述：连接服务端socket
     * @返回值：true代表启动SDK成功，false代表启动SDK失败
     */
    bool StartConnect();

    /**
     * @功能描述：断开服务端socket
     * @返回值：true代表停止SDK成功，false代表停止SDK失败
     */
    void StopConnect();

    /**
     * @功能描述：获取SDK当前状态
     * @返回值：参见JNIState枚举定义
     */
    JNIState GetState();

    /**
     * @功能描述：设置SDK状态
     * @参数 [in] state：参见JNIState枚举定义
     */
    void SetState(JNIState state);

    std::recursive_mutex m_lock;
    enum JNIState m_state = JNIState::INVALID;

    NetComm m_netComm;
};
}
#endif
