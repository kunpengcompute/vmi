/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2021. All rights reserved.
 * Description: MurmurHash algorithm
 */
#ifndef MURMURHASH_H
#define MURMURHASH_H

#include <cstdint>
#include <utility>

namespace Vmi {
/**
* @brief : MurmurHash algorithm,output a 32 bit hash
           "2" means MurmurHash 2.0.
* @param [in]key: first object:the char to be calculated hash value
                  second object:the length of input char
* @param [out]hash: the hash value of the char
* @return : -1: algorithm fails, 0: algorithm success
*/
__attribute__ ((visibility ("default"))) int MurmurHash2(std::pair<const uint8_t*, uint32_t> key, uint32_t &hash);

/**
* @brief : MurmurHash algorithm,output a 64 bit hash.
           "B" means funtion B:64-bit hash for 32-bit platforms
* @param [in]key: first object:the char to be calculated hash value
                  second object:the length of input char
* @param [out]hash: the hash value of the char
* @return : -1: algorithm fails, 0: algorithm success
*/
__attribute__ ((visibility ("default"))) int MurmurHash64B(std::pair<const uint8_t*, uint32_t> key, uint64_t &hash);
}

#endif