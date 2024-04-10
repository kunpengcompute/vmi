/*
 * 版权所有 (c) 华为技术有限公司 2022-2022
 * 功能说明：va编码引擎，创建SPS包
 */

#ifndef VA_BITSTREAM_H
#define VA_BITSTREAM_H

#include <utility>
#include <cstdint>

namespace Vmi {
constexpr int MAX_SREAM_LENGTH = 100;

struct BitStream {
    uint32_t buffer[MAX_SREAM_LENGTH];
    uint32_t bitOffset = 0;
};
class VaBitstream {
public:
    VaBitstream() = default;
    ~VaBitstream() = default;
    void FillBitstreamForSPS(VAEncSequenceParameterBufferHEVC &seqParam, uint32_t width, uint32_t height);
    std::pair<uint8_t *, uint32_t> GetBitstreamForSPS();
private:
    BitStream m_bs;
    uint32_t Swap32(uint32_t x);
    void PutUe(uint32_t val);
    void PutUi(uint32_t val, uint32_t sizeInBits);
    void RbspTrailingBits(int bit);
    void BitstreamEnd();
};
}  // namespace Vmi

#endif