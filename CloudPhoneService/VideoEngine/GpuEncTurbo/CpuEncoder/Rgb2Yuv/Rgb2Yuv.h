/*
 * 版权所有 (c) 华为技术有限公司 2021-2022
 * 功能说明： RGB转YUV函数 ARM64
 */
#ifndef RGB_2_YUV_H
#define RGB_2_YUV_H

#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__aarch64__)
int Rgb2YuvNeon64(uint8_t *src_abgr, uint32_t src_stride_abgr, uint8_t *dst_y, uint32_t dst_stride_y, uint8_t *dst_u,
    uint32_t dst_stride_u, uint8_t *dst_v, uint32_t dst_stride_v, uint32_t width, uint32_t height);
#else
inline int Rgb2YuvNull(
    uint8_t*, uint32_t, uint8_t*, uint32_t, uint8_t*, uint32_t, uint8_t*, uint32_t, uint32_t, uint32_t)
{
    return 0;
}
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif