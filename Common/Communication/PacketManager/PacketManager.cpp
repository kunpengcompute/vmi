/*
 * 版权所有 (c) 华为技术有限公司 2017-2021
 * 功能说明: 数据包管理器，创建数据包队列，用于保存重组后的包，等待进一步处理
 */
#define LOG_TAG "PacketManager"

#include "PacketManager.h"
#include "VmiMsgHead.h"
#include "logging.h"
#include "SmartPtrMacro.h"

using namespace std;
namespace Vmi {
/*
 * @功能描述：构造函数，初始化包队列数组为空
 */
PacketManager::PacketManager()
{
}

/*
 * @功能描述：析构函数，销毁数据包队列
 */
PacketManager::~PacketManager()
{
    for (auto &queue : m_packetQueues) {
        queue = nullptr;
    }
}

PacketManager& PacketManager::GetInstance()
{
    static PacketManager instance;
    return instance;
}

/*
 * @功能描述：验证数据包队列类型和对应的数据包队列是否存在
 * @参数 [in] type：数据包队列类型
 * @返回值：类型合法且数据包队列存在返回true，否则返回false
 */
bool PacketManager::Verify(VMIMsgType type)
{
    if (type <= VMIMsgType::INVALID || type >= VMIMsgType::END) {
        ERR("Failed to verify, Invalid msg type: %u", type);
        return false;
    }
    if (m_packetQueues[type] == nullptr) {
        ERR("Failed to verify, packet queue:%u is null", type);
        return false;
    }
    return true;
}

/*
 * @功能描述：创建指定类型的数据包队列，不加锁，不支持在多线程情况下创建和删除队列
 * @参数 [in] type：数据包类型
 * @参数 [in] isBlock：是否阻塞
 * @返回值：成功返回true，失败返回false
 */
bool PacketManager::CreateQueue(VMIMsgType type, bool isBlock)
{
    if (type <= VMIMsgType::INVALID || type >= VMIMsgType::END) {
        ERR("Failed to create queue, invalid msg type: %u", type);
        return false;
    }

    std::unique_ptr<PacketQueue> queue = nullptr;
    MAKE_UNIQUE_NOTHROW(queue, PacketQueue, isBlock);
    if (queue == nullptr) {
        ERR("Failed to create queue, type:%u", type);
        return false;
    }

    INFO("packet queue created, type:%u, block:%d", type, isBlock);
    m_packetQueues[type] = std::move(queue);
    return true;
}

/*
 * @功能描述：把数据包加入指定类型的队列
 * @参数 [in] type：数据包类型
 * @参数 [in] packet：数据包的地址和大小
 * @返回值：成功返回true，失败返回false
 */
bool PacketManager::PutPkt(VMIMsgType type, const std::pair<uint8_t *, uint32_t> &packet)
{
    if (!Verify(type)) {
        ERR("Failet to put packet, verify failed");
        free(packet.first); // msg类型非法或队列为空，释放数据包
        return false;
    }

    m_packetQueues[type]->PutPkt(packet);
    return true;
}

/*
 * @功能描述：从指定数据包队列中取出包
 * @参数 [in] type：数据包队列类型
 * @返回值：验证参数非法返回空pair，有数据包返回其地址和大小，没有数据包，阻塞模式下等待，非阻塞模式返回空pair
 */
std::pair<uint8_t *, uint32_t> PacketManager::GetNextPkt(VMIMsgType type)
{
    if (!Verify(type)) {
        ERR("Failed to get next packet, verify failed");
        return std::pair<uint8_t *, uint32_t>(nullptr, 0);
    }
    return m_packetQueues[type]->GetNextPkt();
}

/*
 * @功能描述：清空所有数据包队列
 */
void PacketManager::ClearPkt()
{
    for (auto &queue : m_packetQueues) {
        if (queue != nullptr) {
            queue->Clear();
        }
    }
}
} // namespace Vmi
