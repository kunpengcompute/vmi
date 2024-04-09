/*
 * 版权所有 (c) 华为技术有限公司 2023-2024
 * 功能说明: Agent程序的主逻辑
 */
#ifndef VMI_AGENT_H
#define VMI_AGENT_H
#include <cstdint>
#include "VmiApi.h"
#include "Utils/Looper.h"

namespace Vmi {
class RecvDataRunnable : public Runnable {
public:
    RecvDataRunnable(VmiDataType dataType, VmiCmd cmd, std::pair<uint8_t*, uint32_t> data);
    int Run() override;
private:
    VmiDataType m_dataType;
    VmiCmd m_cmd;
    std::pair<uint8_t*, uint32_t> m_data;
};

union CmdHelp {
    uint32_t cmd;
    struct { // 数据默认是小端序，低位放在低地址
        uint16_t cmdId;
        uint8_t cmdType;
        uint8_t dataType;
    } info;
};

class VmiAgent {
public:
    VmiAgent() = default;
    ~VmiAgent();
    bool Initialize();
};
}
#endif
