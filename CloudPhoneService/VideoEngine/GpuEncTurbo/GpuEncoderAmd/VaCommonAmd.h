/*
 * 版权所有 (c) 华为技术有限公司 2023-2023
 * 功能说明：AMD VA引擎通用属性
 */

#ifndef VA_COMMON_AMD_H
#define VA_COMMON_AMD_H

#include "../GpuEncoderBase.h"

namespace Vmi {
namespace GpuEncoder {
constexpr int BIY_PER_BYTE = 8; // 8 bits per byte
constexpr int LCU_SIZE = 64;

struct DevicePair {
    DeviceNode deviceNode;
    const char* devicePath;
};

/**
 * @brief A卡映射路径
 */
constexpr DevicePair AMD_DEVICE_TYPE_MAP[] = {
    {DEV_DRI_RENDERD128, "/dev/dri/renderD128"},
    {DEV_DRI_RENDERD129, "/dev/dri/renderD129"},
    {DEV_DRI_RENDERD130, "/dev/dri/renderD130"},
    {DEV_DRI_RENDERD131, "/dev/dri/renderD131"}
};

/**
 * 返回值定义
 */
enum EncTurboCode : uint32_t {
    ENC_TURBO_SUCCESS = 0x00000000,
    ENC_TURBO_ERROR_INVALID_PARAMETER = 0x00000001,    // 参数传输错误
    ENC_TURBO_ERROR_OPEN_DISPLAY_DEVICE = 0x00000002,  // 驱动设备节点打开错误
    ENC_TURBO_ERROR_CONTEXT_HANDLE = 0x00000003,       // 上下文指针错误
    ENC_TURBO_ERROR_CONFIG = 0x00000004,               // va 配置错误
    ENC_TURBO_ERROR_VA_STATUS = 0x00000005,            // va 返回值错误
    ENC_TURBO_ERROR_VA_SYNC_STATUS = 0x00000006,       // va 编码错误
    ENC_TURBO_ERROR_ENGINE_STATUS = 0x00000007,         // 安全函数操作错误返回
    ENC_TURBO_ERROR_INTERNAL_STATUS = 0x00000008       // 内部错误
};

/**
 * 输出数据返回结构体
 */
struct VaCodeBuffer {
    void *codedMem;    // 数据指针
    int codedMemSize;  // 数据大小
};
}
}

#endif