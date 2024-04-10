/*
 * 版权所有 (c) 华为技术有限公司 2023-2023
 * 功能描述：芯动GPU编码va接口
 */
#ifndef VA_ENC_INNO
#define VA_ENC_INNO

#include <queue>
#include <set>
#include <mutex>
#include <cstdint>
#include <string>
#include <va/va.h>
#include <va/va_enc_h264.h>

namespace Vmi {
enum GopPreset : uint32_t {
    GOP_PRESET_IP,
    GOP_PRESET_IBPBP,
    GOP_PRESET_IBBBP,
    GOP_PRESET_MAX,
};

enum EntropyMode : uint32_t {
    ENTROPY_CAVLC,
    ENTROPY_CABAC,
};

constexpr uint32_t HEVC_DEFAULT_LEVEL = 41;
constexpr uint32_t H264_DEFAULT_LEVEL = 41;
constexpr uint32_t H264_MB_WIDTH = 16;
constexpr uint32_t FRAME_P = 0;
constexpr uint32_t FRAME_B = 1;
constexpr uint32_t FRAME_I = 2;
constexpr uint32_t H264_FRAME_FORCE_I = 6;
constexpr uint32_t HEVC_FRAME_FORCE_I = 3;
constexpr uint32_t FRAME_IDR = 7;
constexpr uint32_t BITSTREAM_ALLOCATE_STEPPING = 4096;
constexpr uint32_t NAL_REF_IDC_NONE = 0;
constexpr uint32_t WIDTH_ALIGN = 32;
constexpr uint32_t HEIGHT_ALIGN = 32;
constexpr uint32_t MAX_WIDTH = 4096;
constexpr uint32_t MAX_HEIGHT = 4096;
constexpr uint32_t DEFAULT_WIDTH = 720;
constexpr uint32_t DEFAULT_HEIGHT = 1280;
constexpr uint32_t DEFAULT_FRAME_RATE = 30;
constexpr uint32_t DEFAULT_GOP_SIZE = 60;
constexpr uint32_t DEFAULT_INTRA_PERIOD = 30;
constexpr uint32_t DEFAULT_BIT_RATE = 5000000;
constexpr uint32_t DEFAULT_MIN_QP = 8;
constexpr uint32_t DEFAULT_MAX_QP = 51;
constexpr uint32_t DEFAULT_QP = 30;
constexpr uint32_t YUV_SURFACE_NUM = 8;
constexpr uint32_t STREAM_SURFACE_NUM = 8;
constexpr uint32_t HEVC_REFERENCE_FRAME_SIZE = 15;
constexpr uint32_t H264_REFERENCE_FRAME_SIZE = 16;

class VaEncInno {
public:
    VaEncInno() = default;
    ~VaEncInno() = default;
    void SetResolution(uint32_t width, uint32_t height, uint32_t widthAligned, uint32_t heightAligned);
    void SetForceIFrame(int32_t offset);
    void SetProfile(VAProfile profile);
    void SetIntraRefresh(uint32_t intraRefresh, uint32_t intraRefreshArg);
    void SetFrameRate(int32_t frameRate);
    void SetGopParam(int32_t gopSize, GopPreset gopPreset);
    void SetIntraPeriod(int32_t intraPeriod);
    void SetQp(int32_t minQp, int32_t maxQp, int32_t qp);
    void SetUseVbr(bool isUse);
    void SetRenderSequence();
    void SetEntropyMode(EntropyMode entropyMode);
    void SetBitrate(int32_t frameBitrate);
    bool Start();
    void Stop();
    int32_t EncodeFrame(int32_t yuvIndex, int32_t streamIndex);
    void SetCropOffset(uint32_t leftOffset, uint32_t rightOffset, uint32_t bottomOffset, uint32_t topOffset);
    bool GetYuvBuffer(uint32_t &slot);
    bool ReleaseYuvBuffer(uint32_t slot);
    int32_t GetYuvBufferFd(uint32_t slot);
    bool GetStreamBuffer(uint32_t &slot);
    bool ReleaseStreamBuffer(uint32_t slot);
    uint32_t RetriveStreamData(uint32_t slot, uint8_t *mem, uint32_t memLen);

    // Buffer相关
    bool MapStreamBuffer(uint32_t slot, VACodedBufferSegment **bufList);
    bool UnmapStreamBuffer(uint32_t slot);

private:
    bool CheckVaapi();
    void CheckVaProfile();
    bool GetConfigAttributes();
    void ConfigAttribRTFormat();
    void ConfigAttribRateControl();
    void ConfigAttribEncPackedHeaders();
    void ConfigAttribEncInterlaced();
    void ConfigAttribEncMaxRefFrames();
    void ConfigAttribEncMaxSlices();
    void ConfigAttribEncSliceStructure();
    void ConfigAttribEncMacroblockInfo();
    bool InitVaapi();
    bool InitEncResource();
    bool CreateCodeBuffer();
    bool SetVaapiBufferTypeHEVC();
    bool SetVaapiBufferTypeH264();
    bool SetVAEncMiscParameter();
    void DeinitVaapi();
    int32_t RenderPackedHeader();
    int32_t RenderSequence();
    int32_t RenderSequenceHEVC();
    int32_t RenderSequenceH264();
    void RenderPictureHEVC();
    void RenderPictureH264();
    int32_t RenderRateControl();
    int32_t RenderPicture(uint32_t srcIndex, uint32_t dstIndex);
    int32_t RenderRir();
    int32_t RenderSlice();
    int32_t LoadSurface(VASurfaceID surfaceID, uint8_t *yuv);
    bool GetSrcYuvAddr(uint8_t *yuv, uint8_t **yAddr, uint8_t **uAddr, uint8_t **vAddr);
    int32_t CopySrcYuv2Surface(VASurfaceID surfaceID, uint8_t *yAddr, uint8_t *uAddr, uint8_t *vAddr);
    int32_t SaveCodedData();

private:
    // VA context and config
    VADisplay m_vaDisplay {nullptr};
    VAProfile m_vaProfile {VAProfileNone};
    VAEntrypoint m_vaEntrypoint {VAEntrypointEncSlice};
    VAConfigAttrib m_vaConfigAttribSupport[VAConfigAttribTypeMax] {};
    VAConfigAttrib m_vaConfigAttribArray[VAConfigAttribTypeMax] {};
    int32_t m_vaConfigAttribNum {0};
    VAConfigID m_vaConfigId {0};
    VAContextID m_vaContextId {0};

    // VA surface queue
    VASurfaceID m_srcSurface[YUV_SURFACE_NUM] {};
    std::set<uint32_t> m_usedYuvSlot {};
    std::queue<uint32_t> m_freeYuvSlot {};
    VABufferID m_vaStreamBufferIdArray[STREAM_SURFACE_NUM] {};
    std::set<uint32_t> m_usedStreamSlot {};
    std::queue<uint32_t> m_freeStreamSlot {};

    // VA buffer ids
    VABufferID m_vaPackedHeaderParamBufferId {0};
    VABufferID m_vaPackedheaderDataBufferId {0};
    VABufferID m_vaSpsBufferId {0};
    VABufferID m_vaPpsBufferId {0};
    VABufferID m_vaSliceParamBufferId {0};
    VABufferID m_vaMiscRateControlBufferId {0};
    VABufferID m_vaMiscFrameRateBufferId {0};
    VABufferID m_vaMiscRirBufferId {0};

    // VA mapped param buffer
    VAEncPackedHeaderParameterBuffer *m_vaPackedHeaderParam {nullptr};
    uint8_t *m_vaPackedData {nullptr};
    // H264 mapped param buffer
    VAEncSequenceParameterBufferH264 *m_vaH264Sps {nullptr};
    VAEncPictureParameterBufferH264 *m_vaH264Pps {nullptr};
    VAEncSliceParameterBufferH264 *m_vaH264SliceParam {nullptr};
    // HEVC mapped param buffer
    VAEncSequenceParameterBufferHEVC *m_vaHevcSps {nullptr};
    VAEncPictureParameterBufferHEVC *m_vaHevcPps {nullptr};
    VAEncSliceParameterBufferHEVC *m_vaHevcSliceParam {nullptr};
    // Video mapped param buffer
    VAEncMiscParameterRateControl *m_vaMiscRateControl {nullptr};
    VAEncMiscParameterFrameRate *m_vaMiscFrameRate {nullptr};
    VAEncMiscParameterRIR *m_vaMiscRir {nullptr};

    // Encode params
    int32_t m_frameWidth {DEFAULT_WIDTH};
    int32_t m_frameHeight {DEFAULT_HEIGHT};
    int32_t m_frameWidthAligned {DEFAULT_WIDTH};
    int32_t m_frameHeightAligned {DEFAULT_HEIGHT};
    int32_t m_frameRate {DEFAULT_FRAME_RATE};
    uint32_t m_frameBitrate {DEFAULT_BIT_RATE};
    int32_t m_minQp {DEFAULT_MIN_QP};
    int32_t m_maxQp {DEFAULT_MAX_QP};
    int32_t m_qp {DEFAULT_QP};
    bool m_useVbr {false};
    int32_t m_gopSize {DEFAULT_GOP_SIZE};
    GopPreset m_gopPreset {GOP_PRESET_IP};
    int32_t m_intraPeriod {DEFAULT_INTRA_PERIOD};
    uint32_t m_intraRefresh {0};
    uint32_t m_intraRefreshArg {0};
    uint64_t m_streamFileSize {0};
    int32_t m_srcYuvFourcc {VA_FOURCC_IYUV};
    EntropyMode m_entropyMode {ENTROPY_CABAC};

    bool m_initFlag {false};
    int32_t m_currentFrameType {FRAME_IDR};
    uint64_t m_currentFrameNum {0};

    bool m_sequenceFlag {true};
    bool m_rateControlFlag {true};
    bool m_rirFlag {true};
    int32_t m_forceIdrOffset {-1};
    std::mutex m_dynamicParamMutex {};

    uint32_t m_cropLeftOffset {0};
    uint32_t m_cropRightOffset {0};
    uint32_t m_cropBottomOffset {0};
    uint32_t m_cropTopOffset {0};
};
}  // namespace Vmi
#endif
