/*
 * 版权所有 (c) 华为技术有限公司 2017-2021
 * 功能说明: 数据包管理器，创建数据包队列，用于保存重组后的包，等待进一步处理
 */
#ifndef PACKET_MANAGER_H
#define PACKET_MANAGER_H

#include <cstdint>
#include <mutex>
#include <array>
#include "VmiMsgHead.h"
#include "PacketQueue.h"

namespace Vmi {
class __attribute__ ((visibility ("default"))) PacketManager {
public:
    /*
     * @功能描述：创建指定类型的数据包队列，不加锁，不支持在多线程情况下创建和删除队列
     * @参数 [in] type：数据包类型
     * @参数 [in] isBlock：是否阻塞
     * @返回值：成功返回true，失败返回false
     */
    bool CreateQueue(VMIMsgType type, bool isBlock);
    /*
     * @功能描述：把数据包加入指定类型的队列
     * @参数 [in] type：数据包类型
     * @参数 [in] packet：数据包的地址和大小
     * @返回值：成功返回true，失败返回false
     */
    bool PutPkt(VMIMsgType type, const std::pair<uint8_t *, uint32_t> &packet);
    /*
     * @功能描述：从指定数据包队列中取出包
     * @参数 [in] type：数据包队列类型
     * @返回值：验证参数非法返回空pair，有数据包返回其地址和大小，没有数据包，阻塞模式下等待，非阻塞模式返回空pair
     */
    std::pair<uint8_t *, uint32_t> GetNextPkt(VMIMsgType type);
    /*
     * @功能描述：清空所有数据包队列
     */
    void ClearPkt();

    static PacketManager& GetInstance();

private:
    /*
     * @功能描述：构造函数，初始化包队列数组为空
     */
    PacketManager();
    /*
     * @功能描述：析构函数，销毁数据包队列
     */
    ~PacketManager();
    PacketManager(const PacketManager &) = delete;
    PacketManager(PacketManager &&) = delete;
    PacketManager& operator=(const PacketManager &) = delete;
    PacketManager& operator=(PacketManager &&) = delete;
    /*
     * @功能描述：验证数据包队列类型和对应的数据包队列是否存在
     * @参数 [in] type：数据包队列类型
     * @返回值：类型合法且数据包队列存在返回true，否则返回false
     */
    bool Verify(VMIMsgType type);

    std::array<std::unique_ptr<PacketQueue>, VMIMsgType::END> m_packetQueues = { nullptr };
};
} // namespace Vmi
#endif
