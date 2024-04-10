/*
 * 版权所有 (c) 华为技术有限公司 2021-2022
 * 功能说明：色彩空间转换类，目前只支持RGB转YUV
 */

#ifndef VMI_RGB2YUV_H
#define VMI_RGB2YUV_H

#include <cstdint>

namespace Vmi {
int RGBAToYUV(uint8_t *frameData, uint32_t frameStride, uint32_t frameWidth, uint32_t frameHeight, uint8_t *yuvBuf);
}
#endif
