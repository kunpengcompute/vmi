/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: write Bmp picture
 */

#ifndef BITMAP_IMAGE_H
#define BITMAP_IMAGE_H

#include <cstdio>
#include <string>

enum BMPPixelType {
    BMP_BGR = 0,
    BMP_BGRA = 1,
    BMP_RGB_565 = 2
};

struct __attribute__((packed)) TagBitMapInfoHeader {
    uint32_t biSize = 0; // 本结构所占用字节数
    uint32_t biWidth = 0; // 位图的宽度，以像素为单位
    uint32_t biHeight = 0; // 位图的高度，以像素为单位
    uint16_t biPlanes = 0; // 目标设备的级别，必须为1
    // 每个像素所需的位数，必须是1(双色),
    // 4(16色)，8(256色)或24(真彩色)之一
    uint16_t biBitCount = 0;
    // 位图压缩类型，必须是 0(不压缩),
    // 1(BI_RLE8压缩类型)或2(BI_RLE4压缩类型)之一
    uint32_t biCompression = 0;
    uint32_t biSizeImage = 0; // 位图的大小，以字节为单位
    uint32_t biXPelsPerMeter = 0; // 位图水平分辨率，每米像素数
    uint32_t biYPelsPerMeter = 0; // 位图垂直分辨率，每米像素数
    uint32_t biClrUsed = 0; // 位图实际使用的颜色表中的颜色数
    uint32_t biClrImportant = 0; // 位图显示过程中重要的颜色数
};

struct __attribute__((packed)) TagBitmapFileHeader {
    uint16_t type = 0; // 位图文件的类型，必须为BM
    uint32_t size = 0; // 位图文件的大小，以字节为单位
    uint16_t reserved1 = 0; // 位图文件保留字，必须为0
    uint16_t reserved2 = 0; // 位图文件保留字，必须为0
    uint32_t offBits = 0; // 位图数据的起始位置，以相对于位图
    struct TagBitMapInfoHeader tagBitMapInfoHeader = {}; // 位图信息头数据
};

struct WriteBMPInfo {
    uint32_t width = 0;
    uint32_t height = 0;
    BMPPixelType pixelType = BMP_BGR;
} __attribute__((packed));

bool WriteBMP(const std::string fileName, const std::string fileDir, const WriteBMPInfo &bitmapInfo,
                std::pair<const uint8_t*, size_t> bitmapData);

#endif
