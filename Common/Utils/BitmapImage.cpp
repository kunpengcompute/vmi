/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2021. All rights reserved.
 * Description: write Bmp picture
 */
#define LOG_TAG "BitmapImage"
#include "BitmapImage.h"
#include <cstdlib>
#include <climits>
#include <arpa/inet.h>
#include "Log/logging.h"

namespace {
    const uint8_t PIXEL_BYTES[3] = {3, 4, 2}; // 3 bmp format:3 means BGR; 4 means BGRA; 2 means RGB_565
    const uint16_t PIXEL_BITS[3] = {24, 32, 16}; // 3 bmp format:BGR is 24 bit; BGRA is 32 bit; RGB_565 is 16bit
}

/**
 * @brief : set bmp header
 * @param [in]width: the width of pciture
 * @param [in]height: the height of pciture
 * @param [in]pixelType: include BMP_BGR,BMP_BGRA
 * @param [out]bitmapFileHeader: the bitmap file header
 */
static void SetBMPHeader(const uint32_t width, const uint32_t height, const BMPPixelType pixelType,
    TagBitmapFileHeader& bitmapFileHeader)
{
    const uint32_t bmpHeaderLen = sizeof(TagBitmapFileHeader);
    TagBitmapFileHeader tagBitmapFileHeader;
    tagBitmapFileHeader.type = ntohs(0x424d); // 0x424d is "BMP"
    tagBitmapFileHeader.size = width * height * PIXEL_BYTES[pixelType] + bmpHeaderLen;
    tagBitmapFileHeader.offBits = bmpHeaderLen;

    TagBitMapInfoHeader tagBitMapInfoHeader;
    tagBitMapInfoHeader.biSize = sizeof(TagBitMapInfoHeader);
    tagBitMapInfoHeader.biWidth = width;
    tagBitMapInfoHeader.biHeight = height;
    tagBitMapInfoHeader.biPlanes = 1;
    tagBitMapInfoHeader.biBitCount = PIXEL_BITS[pixelType];

    tagBitmapFileHeader.tagBitMapInfoHeader = tagBitMapInfoHeader;
    bitmapFileHeader = tagBitmapFileHeader;
}

static bool GetValidFilePath(const std::string fileName, const std::string fileDir, std::string &outfilePath)
{
    char realPath[PATH_MAX] = {0};
    if (realpath(fileDir.c_str(), realPath) == nullptr) {
        ERR("Failed to get valid file path, input file dir:%s", fileDir.c_str());
        return false;
    }

    outfilePath = std::string(realPath) + "/" + fileName + ".bmp";
    return true;
}

/**
* @brief : save BMP picture
* @param [in]fileName: save file name
* @param [in]fileDir: save path dir
* @param [in]bitmapInfo: writebmp info (width, height, pixelType)
* @param [in]bmpData: first object:the data of pciture
                    second object:the data length of pciture
* @return : false: Write BMP data fail.
            true:  Write BMP data success
*/
bool WriteBMP(const std::string fileName, const std::string fileDir, const WriteBMPInfo &bitmapInfo,
              std::pair<const uint8_t*, size_t> bitmapData)
{
    const uint8_t* bitmap = bitmapData.first;
    if (bitmap == nullptr) {
        ERR("Failed to write bmp, bitmap is null");
        return false;
    }

    if (bitmapInfo.pixelType != BMP_BGRA) {
        ERR("Failed to write bmp, pixel type:%d is error.", bitmapInfo.pixelType);
        return false;
    }

    const uint32_t maxWidth = 23170; // maxWidth * maxHeight smaller than (0x7FFFFFFF - 54) / 4
    const uint32_t maxHeight = 23170;
    if (bitmapInfo.width == 0 || bitmapInfo.height == 0 ||
        bitmapInfo.width > maxWidth || bitmapInfo.height > maxHeight ||
        bitmapInfo.width * bitmapInfo.height * PIXEL_BYTES[bitmapInfo.pixelType] !=
        static_cast<uint32_t>(bitmapData.second)) {
        ERR("Failed to write bmp, bitmap width or height is error, width = %u, height = %u, bitmap size:%zu",
            bitmapInfo.width, bitmapInfo.height, bitmapData.second);
        return false;
    }

    TagBitmapFileHeader tagBitmapFileHeader = {};
    SetBMPHeader(bitmapInfo.width, bitmapInfo.height, bitmapInfo.pixelType, tagBitmapFileHeader);

    std::string filePath = "";
    if (!GetValidFilePath(fileName, fileDir, filePath)) {
        return false;
    }
    FILE* file = fopen(filePath.c_str(), "wb");
    if (file == nullptr) {
        ERR("Failed to write bmp, open file '%s' failed", filePath.c_str());
        return false;
    }

    const uint32_t bmpHeaderLen = sizeof(TagBitmapFileHeader);
    int writeByte = fwrite(&tagBitmapFileHeader, sizeof(unsigned char), bmpHeaderLen, file);
    if (writeByte != bmpHeaderLen) {
        ERR("Failed to write bmp, write byte:%d not equal to bitmap file header length:%u", writeByte, bmpHeaderLen);
        (void)fclose(file);
        return false;
    }

    writeByte = fwrite(bitmap, sizeof(unsigned char), bitmapData.second, file);
    (void) fclose(file);
    if (writeByte != bitmapData.second) {
        ERR("Failed to write bmp, write byte:%d not equal to bitmap data size:%zu", writeByte, bitmapData.second);
        return false;
    }

    return true;
}

