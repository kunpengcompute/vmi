/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2021. All rights reserved.
 * Description: MurmurHash algorithm
 */
#define LOG_TAG "MurmurHash"
#include "MurmurHash.h"
#include <cstring>
#include "logging.h"

namespace Vmi {
/**
* @brief : MurmurHash algorithm,mix hash
* @param [in]data: the char to be calculated hash value
* @param [in]hash: the hash value of the char
*/
static void MixHash(uint32_t data, uint32_t& hash)
{
    // 'mixMagicConstant' and 'shiftMagicConstant' are mixing constants generated offline.
    // They're not really 'magic', they just happen to work well.
    const uint32_t mixMagicConstant = 0x5bd1e995;
    const uint32_t shiftMagicConstant = 24;
    data *= mixMagicConstant;
    data ^= data >> shiftMagicConstant;
    data *= mixMagicConstant;
    hash *= mixMagicConstant;
    hash ^= data;
}

/**
* @brief : MurmurHash algorithm,output a 32 bit hash
           "2" means MurmurHash 2.0.
* @param [in]key: first object:the char to be calculated hash value
               second object:the length of input char
* @param [out]hash: the hash value of the char
* @return : -1: algorithm fails, 0: algorithm success
*/
int MurmurHash2(std::pair<const uint8_t*, uint32_t> key, uint32_t &hash)
{
    const uint8_t* data = key.first;
    uint32_t len = key.second;
    if (data == nullptr) {
        ERR("Failed to get 32 bit hash with MurmurHash 2.0, key address is nullptr");
        return -1;
    }
    if (len == 0) {
        ERR("Failed to get 32 bit hash with MurmurHash 2.0, key length is 0");
        return -1;
    }
    const uint32_t seed = 97;
    hash = len ^ seed;

    const uint32_t mixMagicConstant = 0x5bd1e995;
    uint32_t tempData = 0;
    uint32_t tempDataLen = sizeof(tempData);
    while (len >= tempDataLen) {
        memcpy(&tempData, data, tempDataLen);
        MixHash(tempData, hash);
        len -= tempDataLen;
        data += tempDataLen;
    }

    const uint32_t byteLen = 8;
    uint32_t remainLen = len;
    while ((len--) != 0) {
        hash ^= data[len] << (len * byteLen);
    }
    if (remainLen != 0) {
        hash *= mixMagicConstant;
    }

    hash ^= hash >> 13; // left shift 13 bit
    hash *= mixMagicConstant;
    hash ^= hash >> 15; // left shift 15 bit

    return 0;
}

/**
* @brief : Final mix hash.Handle the last few bytes of the input array,
           and Do a few final mixes of the hash to ensure the last few
	       bytes are well-incorporated.
* @param [in]remainData: first object:remain data address
                        second object:remain data length
* @param [in]hash1:the first four byte hash
* @param [in]hash1:the last four byte hash
* @return : the hash value of the char
*/
static uint64_t FinalMixHash(std::pair<const uint8_t*, uint32_t> remainData, uint32_t hash1, uint32_t hash2)
{
    const uint8_t* data = remainData.first;
    uint32_t len = remainData.second;

    const uint32_t mixMagicConstant = 0x5bd1e995;
    const uint32_t byteLen = 8;
    uint32_t remainLen = len;
    while ((len--) != 0) {
        hash2 ^= data[len] << (len * byteLen);
    }
    if (remainLen != 0) {
        hash2 *= mixMagicConstant;
    }

    hash1 ^= hash2 >> 18; // left shift 18 bit
    hash1 *= mixMagicConstant;
    hash2 ^= hash1 >> 22; // left shift 22 bit
    hash2 *= mixMagicConstant;
    hash1 ^= hash2 >> 17; // left shift 17 bit
    hash1 *= mixMagicConstant;
    hash2 ^= hash1 >> 19; // left shift 19 bit
    hash2 *= mixMagicConstant;

    const uint32_t bitNum = 32;
    uint64_t hash = hash1;
    return (hash << bitNum) | hash2;
}

/**
* @brief : MurmurHash algorithm,output a 64 bit hash
           "B" means funtion B:64-bit hash for 32-bit platforms
* @param [in]key: first object:the char to be calculated hash value
                  second object:the length of input char
* @param [out]hash: the hash value of the char
* @return : -1: algorithm fails, 0: algorithm success
*/
int MurmurHash64B(std::pair<const uint8_t*, uint32_t> key, uint64_t &hash)
{
    const uint8_t* data = key.first;
    uint32_t len = key.second;
    if (len == 0) {
        ERR("Failed to get 64 bit hash with MurmurHash funtion B, key length is 0");
        return -1;
    }
    if (data == nullptr) {
        ERR("Failed to get 64 bit hash with MurmurHash funtion B, key address is nullptr");
        return -1;
    }
    const uint32_t seed = 2147483647;
    uint32_t hash1 = len ^ seed;
    uint32_t hash2 = 0;

    uint32_t tempData = 0;
    uint32_t tempDataLen = sizeof(tempData);
    while (len >= sizeof(uint64_t)) {
        memcpy(&tempData, data, tempDataLen);
        MixHash(tempData, hash1);
        len -= tempDataLen;
        data += tempDataLen;

        memcpy(&tempData, data, tempDataLen);
        MixHash(tempData, hash2);
        len -= tempDataLen;
        data += tempDataLen;
    }

    if (len >= tempDataLen) {
        memcpy(&tempData, data, tempDataLen);
        MixHash(tempData, hash1);
        len -= tempDataLen;
        data += tempDataLen;
    }

    std::pair<const uint8_t*, uint32_t> remainData(data, len);
    hash = FinalMixHash(remainData, hash1, hash2);
    return 0;
}
}