/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Description: H264 Parser
 */
#ifndef PARSER_H
#define PARSER_H

#include <cstdint>
#include <utility>
#include <cstddef>

namespace Vmi {
struct Sps {
    uint32_t profileIdc;
    uint32_t constraintSet0Flag;
    uint32_t constraintSet1Flag;
    uint32_t constraintSet2Flag;
    uint32_t constraintSet3Flag;
    uint32_t constraintSet4Flag;
    uint32_t constraintSet5Flag;
    uint32_t reservedZero2bits;
    uint32_t levelIdc;
    uint32_t seqParameterSetId;
    uint32_t chromaFormatIdc;
    uint32_t bitDepthLumaMinus8;
    uint32_t bitDepthChromaMinus8;
    uint32_t qpprimeYZeroTransformBypassFlag;
    uint32_t seqScalingMatrixPresentFlag;
    uint32_t log2MaxFrameNumMinus4;
    uint32_t picOrderCntType;
    uint32_t log2MaxPicOrderCntLsbMinus4;
    uint32_t deltaPicOrderAlwaysZeroFlag;
    int offsetForNonRefPic;
    int offsetForTopToBottomField;
    uint32_t numRefFramesInPicOrderCntCycle;
    uint32_t maxNumRefFrames;
    uint32_t gapsInFrameNumValueAllowedFlag;
    uint32_t picWidthInMbsMinus1;
    uint32_t picHeightInMapUnitsMinus1;
    uint32_t frameMbsOnlyFlag;
    uint32_t mbAdaptiveFrameFieldFlag;
    uint32_t direct8x8InferenceFlag;
    uint32_t frameCroppingFlag;
    uint32_t frameCropLeftOffset;
    uint32_t frameCropRightOffset;
    uint32_t frameCropTopOffset;
    uint32_t frameCropBottomOffset;
    uint32_t vuiParametersPresentFlag;
};

enum class EAvEncH264VChromaFormatIdc : uint32_t {
    FORMAT_MONO = 0,
    FORMAT_420 = 1,
    FORMAT_422 = 2,
    FORMAT_444 = 3
};

enum class NalUnitType : uint32_t {
    NAL_UNKNOWN     = 0,
    NAL_SLICE       = 1,
    NAL_SLICE_DPA   = 2,
    NAL_SLICE_DPB   = 3,
    NAL_SLICE_DPC   = 4,
    NAL_SLICE_IDR   = 5,
    NAL_SEI         = 6,
    NAL_SPS         = 7,
    NAL_PPS         = 8,
    NAL_AUD         = 9,
    NAL_FILLER      = 12,
};

enum class NalUnitType265 : uint32_t {
    TRAIL_N = 0,
    TRAIL_R,
    TSA_N,
    TSA_R,
    STSA_N,
    STSA_R,
    IDR_W_RADL = 19,
    IDR_N_LP = 20,
    CRA_NUT = 21,
    VPS_NUT = 32,
    SPS_NUT,
    PPS_NUT,
    AUD_NUT,
    EOS_NUT,
    EOB_NUT,
    FD_NUT,
    UNKNOWN = 63,
};

enum class EAvEncH264VProfile : uint32_t {
    PROFILE_UNKNOWN = 0,
    PROFILE_CAVLC444_INTRA = 44,
    PROFILE_BASE = 66,
    PROFILE_MAIN = 77,
    PROFILE_SCALABLE_BASE = 83,
    PROFILE_EXTENDED = 88,
    PROFILE_SCALABLE_HIGH = 86,
    PROFILE_HIGH = 100,
    PROFILE_HIGH10 = 110,
    PROFILE_422 = 122,
    PROFILE_444 = 144,
    PROFILE_MULTIVIEW_HIGH = 118,
    PROFILE_STEREO_HIGH = 128,
    PROFILE_HIGH444_PREDICTIVE = 244,
    PROFILE_CONSTRAINED_BASE = 256,
    PROFILE_UC_CONSTRAINED_HIGH = 257,
    PROFILE_UC_SCALABLE_CONSTRAINED_BASE = 258,
    PROFILE_UC_SCALABLE_CONSTRAINED_HIGH = 259
};

class Parser {
public:
    Parser() = default;

    ~Parser();

    bool ExtractSPS(std::pair<uint8_t *, uint32_t> &packetPair, Sps &sps);

    NalUnitType GetFrameType(std::pair<uint8_t *, uint32_t> &packetPair);

    NalUnitType265 GetFrameType265(std::pair<uint8_t *, uint32_t> &packetPair);

    uint32_t GetH264Width(Sps sps);

    uint32_t GetH264Height(Sps sps);

private:
    void InitSps(Sps &sps);

    bool SetProfile(Sps &sps);

    bool FindSps(std::pair<uint8_t *, uint32_t> &packetPair);

    bool IsStartCode(std::pair<uint8_t *, uint32_t> &packetPair);

    bool IsStartCodeAnnexb(std::pair<uint8_t *, uint32_t> &packetPair);

    uint32_t ReadBit();

    uint32_t ReadBits(uint32_t n);

    uint32_t ReadExponentialGolombCode();

    int32_t ReadSignedExponentialGolombCode();

    const uint8_t *m_h264 = nullptr;
    size_t m_len {0};
    uint32_t m_curbit {0};
};
}

#endif
