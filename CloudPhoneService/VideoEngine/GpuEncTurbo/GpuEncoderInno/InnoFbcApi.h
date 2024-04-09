/*
 * 版权所有 (c) 华为技术有限公司 2023-2023
 * 功能描述：接入芯动FBC库头文件
 */
#ifndef INNO_FBC_API_H
#define INNO_FBC_API_H

#include <cstdint>

namespace Vmi {
namespace GpuEncoder {
enum IfbcResult : uint32_t {
    RESULT_SUCCESS = 0,
    RESULT_COMMON = 1,
    RESULT_INVALID_PARAM = 2,
    RESULT_OUT_OF_MEMORY = 3,
    RESULT_CREATE_IMAGE_FD = 4,
};

enum IfbcPixelFormat : uint32_t {
    FORMAT_XRGB8888 = 0x00,
    FORMAT_ARGB8888 = 0x01,

    FORMAT_NV12 = 0x10,
    FORMAT_NV21 = 0x11,
    FORMAT_YUV420 = 0x20,
    FORMAT_YVU420 = 0x21,

    FORMAT_TEX_XRGB8888 = 0x30,
    FORMAT_TEX_ARGB8888 = 0x31,

    FORMAT_2D_TEX_XRGB8888 = 0x40,
    FORMAT_2D_TEX_ARGB8888 = 0x41,

    FORMAT_MEM_XRGB8888 = 0x50,
    FORMAT_MEM_ARGB8888 = 0x51,

    FORMAT_ANATIVE_XRGB8888 = 0x60,
    FORMAT_ANATIVE_ARGB8888 = 0x61,

    FORMAT_INVALID = 0xFF,
};

struct IfbcFrame {
    int x;
    int y;
    int width;
    int height;
    int stride;
    int slot;
    IfbcPixelFormat pixelFormat;
    uint64_t fd;
};

using InnoConvertHandle = void *;
using EglDisplpay = void *;
using EglContext = void *;
struct EglInfo {
    EglDisplpay eglDisplay;
    EglContext eglContext;
};

using EglInfoT = EglInfo *;
using IfbcFrameT = IfbcFrame *;
}
}

#endif
