/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: H264 Parser
 */
#define LOG_TAG "Parser"
#include "Parser.h"
#include <cstdlib>
#include <cstring>

#include "logging.h"

using namespace Vmi;
namespace {
    const uint32_t BITS_RESERVED_ZERO = 2;
    const uint32_t BITS_PER_BYTE = 8;
    const uint32_t BITS_UINT32_MAX = 32;
    const uint32_t BYTES_OFFSET_START_CODE = 4;
    const uint32_t BYTES_OFFSET_START_CODE_ANNEXB = 5;
    const uint32_t BYTES_START_CODE = 3;
    const uint32_t BYTES_START_CODE_ANNEXB = 4;
    const uint32_t BYTES_START_CODE_265 = 4;
    const uint32_t H264_MACRO_BLOCK_SIZE = 16;
    const uint32_t H264_FACTOR = 2;
}

Parser::~Parser()
{
    m_len = 0;
    m_curbit = 0;
    m_h264 = nullptr;
}

uint32_t Parser::ReadBit()
{
    if (m_curbit > m_len * BITS_PER_BYTE) {
        return UINT32_MAX;
    }
    if (m_h264 == nullptr) {
        return UINT32_MAX;
    }
    uint32_t index = m_curbit / BITS_PER_BYTE;
    uint32_t offset = m_curbit % BITS_PER_BYTE + 1;
    m_curbit++;
    return (m_h264[index] >> (BITS_PER_BYTE - offset)) & 0x01;
}

uint32_t Parser::ReadBits(uint32_t n)
{
    uint32_t r = 0;
    for (uint32_t i = 0; i < n; i++) {
        r |= (ReadBit() << (n - i - 1));
    }
    return r;
}

uint32_t Parser::ReadExponentialGolombCode()
{
    uint32_t i = 0;
    while ((ReadBit() == 0) && (i < BITS_UINT32_MAX)) {
        i++;
    }
    uint32_t r = ReadBits(i);
    r += (1 << i) - 1;
    return r;
}

int32_t Parser::ReadSignedExponentialGolombCode()
{
    uint32_t r = ReadExponentialGolombCode();
    int32_t ret = 0;
    if ((r & 0x01) != 0) {
        r = (r + 1) >> 1;
        ret = static_cast<int32_t>(r);
    } else {
        r = r >> 1;
        ret = -(static_cast<int32_t>(r));
    }
    return ret;
}

bool Parser::IsStartCode(std::pair<uint8_t *, uint32_t> &packetPair)
{
    uint8_t *buf = packetPair.first;
    if (buf[0] == 0 && buf[1] == 0 && buf[2] == 1) { // 0: 0x00, 1: 0x00, 2: 0x01
        return true;
    }
    return false;
}

bool Parser::IsStartCodeAnnexb(std::pair<uint8_t *, uint32_t> &packetPair)
{
    uint8_t *buf = packetPair.first;
    if (buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 1) { // 0: 0x00, 1: 0x00, 2: 0x00, 3: 0x01
        return true;
    }
    return false;
}

bool Parser::FindSps(std::pair<uint8_t *, uint32_t> &packetPair)
{
    if (m_h264 == nullptr) {
        return false;
    }
    if (IsStartCodeAnnexb(packetPair)) {
        if ((packetPair.first[BYTES_START_CODE_ANNEXB] & 0x1f) == static_cast<int>(NalUnitType::NAL_SPS)) {
            m_h264 += BYTES_OFFSET_START_CODE_ANNEXB;
            m_len -= BYTES_OFFSET_START_CODE_ANNEXB;
            return true;
        }
    } else if (IsStartCode(packetPair)) {
        if ((packetPair.first[BYTES_START_CODE] & 0x1f) == static_cast<int>(NalUnitType::NAL_SPS)) {
            m_h264 += BYTES_OFFSET_START_CODE;
            m_len -= BYTES_OFFSET_START_CODE;
            return true;
        }
    }
    return false;
}

void Parser::InitSps(Sps &sps)
{
    sps.frameCropLeftOffset = 0;
    sps.frameCropRightOffset = 0;
    sps.frameCropTopOffset = 0;
    sps.frameCropBottomOffset = 0;
    sps.profileIdc = ReadBits(BITS_PER_BYTE);
    sps.constraintSet0Flag = ReadBit();
    sps.constraintSet1Flag = ReadBit();
    sps.constraintSet2Flag = ReadBit();
    sps.constraintSet3Flag = ReadBit();
    sps.constraintSet4Flag = ReadBit();
    sps.constraintSet5Flag = ReadBit();
    sps.reservedZero2bits  = ReadBits(BITS_RESERVED_ZERO);
    sps.levelIdc = ReadBits(BITS_PER_BYTE);
    sps.seqParameterSetId = ReadExponentialGolombCode();
}

bool Parser::SetProfile(Sps &sps)
{
    EAvEncH264VProfile profileIdc = static_cast<EAvEncH264VProfile>(sps.profileIdc);
    if (profileIdc == EAvEncH264VProfile::PROFILE_HIGH ||
        profileIdc == EAvEncH264VProfile::PROFILE_HIGH10 ||
        profileIdc == EAvEncH264VProfile::PROFILE_422 ||
        profileIdc == EAvEncH264VProfile::PROFILE_HIGH444_PREDICTIVE ||
        profileIdc == EAvEncH264VProfile::PROFILE_CAVLC444_INTRA ||
        profileIdc == EAvEncH264VProfile::PROFILE_SCALABLE_BASE ||
        profileIdc == EAvEncH264VProfile::PROFILE_SCALABLE_HIGH ||
        profileIdc == EAvEncH264VProfile::PROFILE_MULTIVIEW_HIGH) {
        sps.chromaFormatIdc = ReadExponentialGolombCode();
        if (sps.chromaFormatIdc == static_cast<uint32_t>(EAvEncH264VChromaFormatIdc::FORMAT_444)) {
            uint32_t residualColourTransformFlag = ReadBit();
            if (residualColourTransformFlag == UINT32_MAX) {
                return false;
            }
        }
        sps.bitDepthLumaMinus8 = ReadExponentialGolombCode();
        sps.bitDepthChromaMinus8 = ReadExponentialGolombCode();
        sps.qpprimeYZeroTransformBypassFlag = ReadBit();
        sps.seqScalingMatrixPresentFlag = ReadBit();
    }
    return true;
}

bool Parser::ExtractSPS(std::pair<uint8_t *, uint32_t> &packetPair, Sps &sps)
{
    m_h264 = packetPair.first;
    m_len = packetPair.second;
    m_curbit = 0;
    if (!FindSps(packetPair)) {
        return false;
    }
    InitSps(sps);
    (void)SetProfile(sps);
    sps.log2MaxFrameNumMinus4 = ReadExponentialGolombCode();
    sps.picOrderCntType = ReadExponentialGolombCode();
    if (sps.picOrderCntType == 0) {
        sps.log2MaxPicOrderCntLsbMinus4 = ReadExponentialGolombCode();
    } else if (sps.picOrderCntType == 1) {
        sps.deltaPicOrderAlwaysZeroFlag = ReadBit();
        sps.offsetForNonRefPic = ReadSignedExponentialGolombCode();
        sps.offsetForTopToBottomField = ReadSignedExponentialGolombCode();
        sps.numRefFramesInPicOrderCntCycle = ReadExponentialGolombCode();
        for (uint32_t i = 0; i < sps.numRefFramesInPicOrderCntCycle; i++) {
            (void)ReadSignedExponentialGolombCode();
        }
    }
    sps.maxNumRefFrames = ReadExponentialGolombCode();
    sps.gapsInFrameNumValueAllowedFlag = ReadBit();
    sps.picWidthInMbsMinus1 = ReadExponentialGolombCode();
    sps.picHeightInMapUnitsMinus1 = ReadExponentialGolombCode();
    sps.frameMbsOnlyFlag = ReadBit();
    if (sps.frameMbsOnlyFlag == 0) {
        sps.mbAdaptiveFrameFieldFlag = ReadBit();
    }
    sps.direct8x8InferenceFlag = ReadBit();
    sps.frameCroppingFlag = ReadBit();
    if (sps.frameCroppingFlag != 0) {
        sps.frameCropLeftOffset = ReadExponentialGolombCode();
        sps.frameCropRightOffset = ReadExponentialGolombCode();
        sps.frameCropTopOffset = ReadExponentialGolombCode();
        sps.frameCropBottomOffset = ReadExponentialGolombCode();
    }
    sps.vuiParametersPresentFlag = ReadBit();
    return true;
}

NalUnitType Parser::GetFrameType(std::pair<uint8_t *, uint32_t> &packetPair)
{
    uint8_t *buf = packetPair.first;
    uint32_t len = packetPair.second;
    int startCode = 0;
    if (len > BYTES_START_CODE_ANNEXB && IsStartCodeAnnexb(packetPair)) {
        startCode = BYTES_START_CODE_ANNEXB;
    } else if (len > BYTES_START_CODE && IsStartCode(packetPair)) {
        startCode = BYTES_START_CODE;
    } else {
        ERR("Data format error, %02x %02x", buf[0], buf[1]);
        return NalUnitType::NAL_UNKNOWN;
    }
    NalUnitType type = static_cast<NalUnitType>(buf[startCode] & 0x1f);
    switch (type) {
        case NalUnitType::NAL_SLICE_IDR:
        case NalUnitType::NAL_SEI:
        case NalUnitType::NAL_SPS:
        case NalUnitType::NAL_PPS:
        case NalUnitType::NAL_SLICE:
        case NalUnitType::NAL_SLICE_DPA:
        case NalUnitType::NAL_SLICE_DPB:
        case NalUnitType::NAL_SLICE_DPC:
        case NalUnitType::NAL_AUD:
        case NalUnitType::NAL_FILLER:
            return type;
        default:
            return NalUnitType::NAL_UNKNOWN;
    }
}

NalUnitType265 Parser::GetFrameType265(std::pair<uint8_t *, uint32_t> &packetPair)
{
    uint8_t *buf = packetPair.first;
    uint32_t len = packetPair.second;
    int startCode = 0;
    if (len > BYTES_START_CODE_265 && IsStartCodeAnnexb(packetPair)) {
        startCode = BYTES_START_CODE_265;
    } else {
        ERR("Data format error, %02x %02x", buf[0], buf[1]);
        return NalUnitType265::UNKNOWN;
    }
    // H265 NALU头固定16bit，构成如下
    // | 0(1bit) | nal_unit_type(6bit) | nuh_layer_id(6bit) | nul_temporal_id_plus1(3bit) |
    uint8_t nalUnitType = (buf[startCode] >> 1) & 0x3f; // 0x3f：0011 1111，6bit，用于提取nal_unit_type的掩码
    NalUnitType265 type = static_cast<NalUnitType265>(nalUnitType);
    switch (type) {
        case NalUnitType265::TRAIL_N:
        case NalUnitType265::TRAIL_R:
        case NalUnitType265::TSA_N:
        case NalUnitType265::TSA_R:
        case NalUnitType265::STSA_N:
        case NalUnitType265::STSA_R:
        case NalUnitType265::IDR_W_RADL:
        case NalUnitType265::IDR_N_LP:
        case NalUnitType265::CRA_NUT:
        case NalUnitType265::VPS_NUT:
        case NalUnitType265::SPS_NUT:
        case NalUnitType265::PPS_NUT:
        case NalUnitType265::AUD_NUT:
        case NalUnitType265::EOS_NUT:
        case NalUnitType265::EOB_NUT:
        case NalUnitType265::FD_NUT:
            return type;
        default:
            return NalUnitType265::UNKNOWN;
    }
}

uint32_t Parser::GetH264Width(Sps sps)
{
    uint32_t param1 = (sps.picWidthInMbsMinus1 + 1) * H264_MACRO_BLOCK_SIZE;
    uint32_t param2 = sps.frameCropRightOffset * H264_FACTOR;
    uint32_t param3 = sps.frameCropLeftOffset * H264_FACTOR;
    return param1 - param2 - param3;
}

uint32_t Parser::GetH264Height(Sps sps)
{
    uint32_t param1 = H264_FACTOR - sps.frameMbsOnlyFlag;
    uint32_t param2 = (sps.picHeightInMapUnitsMinus1 + 1) * H264_MACRO_BLOCK_SIZE;
    uint32_t param3 = sps.frameCropBottomOffset * H264_FACTOR;
    uint32_t param4 = sps.frameCropTopOffset * H264_FACTOR;
    return (param1 * param2) - param3 - param4;
}
