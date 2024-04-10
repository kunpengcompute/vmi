/*
 * 版权所有 (c) 华为技术有限公司 2022-2022
 * 功能说明：va编码引擎common
 */
#ifndef VA_COMMON_H
#define VA_COMMON_H

#include <cstdint>

namespace Vmi {
constexpr int BIY_PER_BYTE = 8; // 8 bits per byte
constexpr int LCU_SIZE = 64;

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
    ENC_TURBO_ERROR_ENGINE_STATUS = 0x00000007         // 安全函数操作错误返回
};

/**
 * Rate control 模式，支持CBR/VBR
 */
enum EncRateControl : uint32_t {
    ENC_RC_CBR,  // 恒定比特率。视觉质量稳定性：不稳定；即时输出码率：恒定；输出文件大小：可控
    ENC_RC_VBR  // 动态比特率。视觉质量稳定性：稳定；即时输出码率：变化；输出文件大小：不可控
};

/**
 * 画质级别
 */
enum EncProfileIdc : uint32_t {
    ENC_PROFILE_IDC_BASELINE = 66,  // 基本画质。支持I/P 帧，只支持无交错（Progressive）和CAVLC
    ENC_PROFILE_IDC_MAIN = 77,      // 主流画质。提供I/P/帧，支持无交错（Progressive），也支持CAVLC 和CABAC 的支持
    ENC_PROFILE_IDC_HIGH = 100,
    ENC_PROFILE_IDC_HEVC_MAIN = 1
};

/**
 * 输出数据类型
 */
enum OutputDataType : uint32_t {
    OUTPUT_YUV,   // 输出YUV数据
    OUTPUT_H264,  // 输出H264数据
    OUTPUT_H265   // 输出H265数据
};

/**
 * GPU驱动路径
 */
enum DRMDevicePath : uint32_t {
    DEV_DRI_RENDERD128 = 128,  // "/dev/dri/renderD128"
    DEV_DRI_RENDERD129 = 129,  // "/dev/dri/renderD129"
    DEV_DRI_RENDERD130 = 130,  // "/dev/dri/renderD130"
    DEV_DRI_RENDERD131 = 131,  // "/dev/dri/renderD131"
    DEV_DRI_OTHER              // NULL
};

struct VaFrameInput {
    bool isNewFrame;
    uint8_t *srcBuffer;
};

/**
 * Input soure type and file path
 */
struct VaEncodeInput {
    OutputDataType outputDataType;
    int reserved;
};

/**
 * Input config value
 */
struct VaEncodeConfig {
    uint32_t widthInput;              // 输入数据宽度
    uint32_t heightInput;             // 输入数据高度
    uint32_t strideInput;             // 输入数据对齐宽度
    uint32_t widthOutput;             // 输出数据宽度
    uint32_t heightOutput;            // 输入数据高度
    EncRateControl rateControl;  // 码率控制模式
    uint32_t bitRate;                 // 比特率
    uint32_t frameRate;               // 帧率
    uint32_t gopSize;                 // 图像组
    EncProfileIdc profileIdc;
    DRMDevicePath drmDevicePath;
};

/**
 * 输出数据返回结构体
 */
struct VaCodeBuffer {
    void *codedMem;    // 数据指针
    int codedMemSize;  // 数据大小
};
}  // namespace Vmi
#endif
