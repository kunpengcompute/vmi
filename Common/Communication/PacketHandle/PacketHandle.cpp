/*
 * 版权所有 (c) 华为技术有限公司 2017-2021
 * 功能说明: 包处理器, 处理包队列和重组功能
 */
#define LOG_TAG "PacketHandle"

#include "PacketHandle.h"
#include "VmiMsgHead.h"
#include "logging.h"

namespace Vmi {
/**
 * @功能描述：PacketHandle构造函数
 * @参数 [in] type：数据包类型
 * @参数 [in] hook：钩子函数
 * @参数 [in] isSingleFragment：是否单包（不分片）
 */
PacketHandle::PacketHandle(VMIMsgType type, RenderServerHandleFuncHook hook, bool isSingleFragment)
    : m_type(type), m_msgReassemble(type), m_hook(hook)
{
    m_msgReassemble.SetSingleSend(isSingleFragment);
}

/**
 * @功能描述：包处理函数，将包存储到队列中
 * @参数 [in] packet：包的指针
 */
void PacketHandle::Handle(const std::pair<uint8_t*, size_t> &packet)
{
    uint8_t *pktData = packet.first;
    size_t pktSize = packet.second;
    if (pktData == nullptr) {
        ERR("Failed to handle, packet is null");
        return;
    }

    if (pktSize < sizeof(StreamMsgHead) || pktSize > MAX_MSG_SIZE) {
        ERR("Failed to handle, packet size:%zu is invalid, min(head) size:%zu, max size:%d",
            pktSize, sizeof(StreamMsgHead), MAX_MSG_SIZE);
        free(pktData);
        pktData = nullptr;
        return;
    }

    StreamMsgHead *msg = reinterpret_cast<StreamMsgHead *>(packet.first);
    std::pair<uint8_t *, uint32_t> packetPair = m_msgReassemble.Reassemble(*msg);
    uint8_t *reassemblePacket = packetPair.first;
    uint32_t dataLen = packetPair.second;
    if (reassemblePacket == nullptr || dataLen == 0) {
        return;
    }

    if (m_hook == nullptr) {
        ERR("Failed to handle, not found hook to handle packet(type:%u), free it", m_type);
        free(reassemblePacket);
        reassemblePacket = nullptr;
        return;
    }
    std::pair<uint8_t*, uint32_t> pairData = {reassemblePacket, dataLen};
    uint32_t ret = m_hook(pairData);
    if (ret != 0) {
        ERR("Failed to handle, call hook error, errno:%u", ret);
    }
}

/**
 * @功能描述：清空包队列
 */
void PacketHandle::ClearQueue()
{
    m_msgReassemble.Clear();
}
} // namespace Vmi
