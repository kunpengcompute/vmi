/*
 * 版权所有 (c) 华为技术有限公司 2021-2022
 * 功能说明：色彩空间转换类，目前只支持RGB转YUV
 */
#include "VmiRgb2Yuv.h"
#include "Rgb2Yuv.h"
namespace Vmi {
/*
 * @功能描述：图像格式rgba转yuv
 * @参数 [in] frameData：原始图像数据
 * @参数 [in] frameStride：图像步长
 * @参数 [in] frameWidth：图像宽
 * @参数 [in] frameHeight：图像长
 * @参数 [out] yuvBuf：转化后的YUV格式图像数据
 * @返回值：成功0，失败为非零值
 */
int RGBAToYUV(uint8_t *frameData, uint32_t frameStride, uint32_t frameWidth,
              uint32_t frameHeight, uint8_t *yuvBuf)
{
    uint32_t dstUVStride = (frameWidth + 1) >> 1;
    uint32_t yLength = frameWidth * frameHeight;
    uint32_t uvLength = dstUVStride * ((frameHeight + 1u) >> 1);
    uint8_t *dstY = yuvBuf;
    uint8_t *dstU = dstY + yLength;
    uint8_t *dstV = dstU + uvLength;
#if defined(__aarch64__)
    // just support __aarch64__
    return Rgb2YuvNeon64(frameData, frameStride << 2, // 2 : frameStride(RGBA) / 4
        dstY, frameWidth, dstU, dstUVStride, dstV, dstUVStride, frameWidth, frameHeight);
#else
    // not support
    return Rgb2YuvNull(frameData, frameStride << 2, // 2 : frameStride(RGBA) / 4
        dstY, frameWidth, dstU, dstUVStride, dstV, dstUVStride, frameWidth, frameHeight);
#endif
}
}