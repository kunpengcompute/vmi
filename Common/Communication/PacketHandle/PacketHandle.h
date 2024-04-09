/*
 * 版权所有 (c) 华为技术有限公司 2017-2021
 * 功能说明: 包处理器, 处理包队列和重组功能
 */
#ifndef PACKET_HANDLE_H
#define PACKET_HANDLE_H
#include <functional>
#include "CommonDefine.h"
#include "StreamParse/StreamParser.h"
#include "MsgFragment/MsgReassemble.h"

namespace Vmi {
using RenderServerHandleFuncHook = std::function<uint32_t(std::pair<uint8_t*, uint32_t> data)>;
class __attribute__ ((visibility ("default"))) PacketHandle : public ServiceHandle {
public:
    /**
     * @功能描述：PacketHandle构造函数
     * @参数 [in] type：数据包类型
     * @参数 [in] hook：钩子函数
     * @参数 [in] isSingleFragment：是否单包（不分片）
     */
    PacketHandle(VMIMsgType type, RenderServerHandleFuncHook hook, bool isSingleFragment);

    /**
     * @功能描述：PacketHandle析构函数
     */
    ~PacketHandle() override = default;

    /**
     * @功能描述：包数据处理函数，将消息存储到队列中
     * @参数 [in] packet：包的指针
     */
    void Handle(const std::pair<uint8_t*, size_t> &packet) override;

    /**
     * @功能描述：清空包队列
     */
    virtual void ClearQueue();

protected:
    VMIMsgType m_type = VMIMsgType::INVALID;
    MsgReassemble m_msgReassemble {m_type};
    RenderServerHandleFuncHook m_hook = nullptr;
};
} // namespace Vmi
#endif
