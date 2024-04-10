/*
 * 版权所有 (c) 华为技术有限公司 2022-2022
 * 功能说明：va编码引擎，创建SPS包
 */

#include <cmath>
#include <string>
#include <va/va.h>
#include "logging.h"
#include "VaCommon.h"
#include "VaBitstream.h"

using namespace Vmi;

std::pair<uint8_t *, uint32_t> VaBitstream::GetBitstreamForSPS()
{
    uint8_t *output = reinterpret_cast<uint8_t *>(m_bs.buffer);
    return std::pair<uint8_t *, uint32_t>(output, (m_bs.bitOffset + BIY_PER_BYTE - 1) / BIY_PER_BYTE);
}

void VaBitstream::FillBitstreamForSPS(VAEncSequenceParameterBufferHEVC &seqParam, uint32_t width, uint32_t height)
{
    (void)std::fill_n(m_bs.buffer, MAX_SREAM_LENGTH, 0);
    m_bs.bitOffset = 0;
    uint32_t newWidthsamples = (width + LCU_SIZE - 1) / LCU_SIZE * LCU_SIZE;
    uint32_t newHeightsamples = (height + LCU_SIZE - 1) / LCU_SIZE * LCU_SIZE;
    PutUe(0);  // sps_seq_parameter_set_id
    PutUe(seqParam.seq_fields.bits.chroma_format_idc);
    PutUe(newWidthsamples);
    PutUe(newHeightsamples);
    if (width != newWidthsamples || height != newHeightsamples) {
        PutUi(1, 1);  // conformance_window_flag
        PutUe(0);
        PutUe((newWidthsamples - width) >> 1);
        PutUe(0);
        PutUe((newHeightsamples - height) >> 1);
    } else {
        PutUi(0, 1);  // conformance_window_flag
    }
    PutUe(seqParam.seq_fields.bits.bit_depth_luma_minus8);
    PutUe(seqParam.seq_fields.bits.bit_depth_chroma_minus8);
    int log2MaxPicOrderCntLsbMinus4 = ceil(log(seqParam.intra_period) / log(2.0)) + 1;  // 2 change log2
    log2MaxPicOrderCntLsbMinus4 = log2MaxPicOrderCntLsbMinus4 < 4 ? 0 : log2MaxPicOrderCntLsbMinus4 - 4;  // minus 4
    PutUe(log2MaxPicOrderCntLsbMinus4);  // log2_max_pic_order_cnt_lsb_minus4
    PutUi(0, 1);                         // sps_sub_layer_ordering_info_present_flag
    PutUe(1);
    PutUe(0);  // sps_max_num_reorder_pics
    PutUe(0);  // sps_max_latency_increase_plus1
    PutUe(seqParam.log2_min_luma_coding_block_size_minus3);
    PutUe(seqParam.log2_diff_max_min_luma_coding_block_size);
    PutUe(0);  // log2_min_luma_transform_block_size_minus2
    PutUe(seqParam.log2_diff_max_min_transform_block_size);
    PutUe(seqParam.max_transform_hierarchy_depth_inter);
    PutUe(seqParam.max_transform_hierarchy_depth_intra);
    PutUi(0, 1);  // scaling_list_enabled_flag
    PutUi(1, 1);  // amp_enabled_flag
    PutUi(seqParam.seq_fields.bits.sample_adaptive_offset_enabled_flag, 1);
    PutUi(0, 1);  // pcm_enabled_flag
    PutUe(1);     // num_short_term_ref_pic_sets
    PutUe(1);     // num_negative_pics
    PutUe(0);     // num_positive_pics
    PutUe(0);     // delta_poc_s0_minus1
    PutUi(1, 1);  // used_by_curr_pic_s0_flag
    PutUi(0, 1);  // long_term_ref_pics_present_flag
    PutUi(seqParam.seq_fields.bits.sps_temporal_mvp_enabled_flag, 1);
    PutUi(seqParam.seq_fields.bits.strong_intra_smoothing_enabled_flag, 1);
    PutUi(seqParam.vui_parameters_present_flag, 1);
    PutUi(0, 1);  // sps_extension_present_flag
    RbspTrailingBits(0);
    BitstreamEnd();
}

void VaBitstream::BitstreamEnd()
{
    uint32_t pos = (m_bs.bitOffset >> 5);  // 5 for clc
    uint32_t bitOffset = (m_bs.bitOffset & 0x1f);
    uint32_t bitleft = 32 - bitOffset;  // 32 int_32
    if (bitOffset != 0) {
        m_bs.buffer[pos] = Swap32((m_bs.buffer[pos] << bitleft));
    }
}

void VaBitstream::RbspTrailingBits(int bit)
{
    PutUi(1, 1);
    uint32_t bitOffset = (m_bs.bitOffset & 0x7);  // 0x7 last 3 bit
    uint32_t bitleft = BIY_PER_BYTE - bitOffset;
    int newVal;
    if (bitOffset == 0) {
        return;
    }
    if (bit != 0) {
        newVal = (1 << bitleft) - 1;
    } else {
        newVal = 0;
    }
    PutUi(newVal, bitleft);
}

void VaBitstream::PutUi(uint32_t val, uint32_t sizeInBits)
{
    uint32_t pos = (m_bs.bitOffset >> 5);          // 5 for clc
    uint32_t bitOffset = (m_bs.bitOffset & 0x1f);  // 0x1f last 5 bit
    uint32_t bitleft = 32 - bitOffset;           // 32 int_32
    if (sizeInBits == 0) {
        return;
    }
    m_bs.bitOffset += sizeInBits;
    if (bitleft > sizeInBits) {
        m_bs.buffer[pos] = (m_bs.buffer[pos] << sizeInBits) | val;
    } else {
        sizeInBits -= bitleft;
        m_bs.buffer[pos] = (m_bs.buffer[pos] << bitleft) | (val >> sizeInBits);
        m_bs.buffer[pos] = Swap32(m_bs.buffer[pos]);
        m_bs.buffer[pos + 1] = val;
    }
}

void VaBitstream::PutUe(uint32_t val)
{
    int sizeInBits = 0;
    uint32_t tmpVal = ++val;
    while (tmpVal > 0) {
        tmpVal >>= 1;
        sizeInBits++;
    }
    PutUi(0, sizeInBits - 1);  // leading zero
    PutUi(val, sizeInBits);
}

uint32_t VaBitstream::Swap32(uint32_t x)
{
    // 0xff 24 0xff00 8 0xff0000 8 24 0xff for clc 1234 to 4321
    return (((x & 0xff) << 24) | ((x & 0xff00) << 8) | ((x & 0xff0000) >> 8) | ((x >> 24) & 0xff));
}