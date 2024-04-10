/*
 * 版权所有 (c) 华为技术有限公司 2023-2023
 * 功能描述: AMD显卡-VA接口层
 */

#ifndef VA_ENCODER_AMD_H
#define VA_ENCODER_AMD_H

#include "va.h"
#include "VaCommonAmd.h"
#include "VaBitstream.h"
#include "GpuEncoderBase.h"

namespace Vmi {
namespace GpuEncoder {
// 编码H.264
constexpr int REF_SURFACE_NUM = 17;
constexpr int REF_PIC_LIST_NUM = 32;
constexpr int REF_FRAMES_NUM = 16;

// 编码H.265
constexpr int MAX_TEMPORAL_SUBLAYERS = 8;
constexpr int MAX_LAYER_ID = 64;
constexpr int MAX_LONGTERM_REF_PIC = 32;
constexpr int NUM_OF_EXTRA_SLICEHEADER_BITS = 3;

struct ConvertDataOutput {
    uint8_t *data = nullptr;
    uint32_t dataLen = 0;
};

struct StreamDataOutput {
    VACodedBufferSegment *codedBufferSegment = nullptr;
    uint8_t* handle = nullptr;
    uint32_t dataLen = 0;
};

/**
 * 定义profile 类型值，mesa驱动只支持BASELINE和MAIN两种
 */
enum PROFILE_IDC_TYPE : uint32_t {
    PROFILE_IDC_HEVC_MAIN = 1, // H.265
    PROFILE_IDC_BASELINE = 66, // H.264
    PROFILE_IDC_MAIN = 77,     // H.264
    PROFILE_IDC_HIGH = 100,    // H.264
};

enum NAL_REF_TYPE : uint32_t {
    NAL_REF_IDC_NONE = 0,
    NAL_REF_IDC_LOW = 1,
    NAL_REF_IDC_MEDIUM = 2,
    NAL_REF_IDC_HIGH = 3,
};

/**
 * 定义编码中各table 类型，符合H264编解码标准定义
 */
enum NAL_TYPE : uint32_t {
    NAL_NON_IDR = 1,
    NAL_IDR = 5,
    NAL_SEI = 6,
    NAL_SPS = 7,
    NAL_PPS = 8,
    NAL_DELIMITER = 9,
};

/**
 * 定义IDR/I/P/B帧类型值，符合标准定义
 */
enum FRAME_TYPE : uint32_t {
    FRAME_TYPE_P = 0,
    FRAME_TYPE_B = 1,
    FRAME_TYPE_I = 2,
    FRAME_IDR = 7,
};

enum ENTPROY_MODE_TYPE : uint32_t {
    ENTROPY_MODE_CAVLC = 0,
    ENTROPY_MODE_CABAC = 1,
};

// H.264 table A-1.
struct H264Level {
    const char *name;
    uint8_t levelIdc;
    uint8_t constraintSet3Flag;
    uint32_t maxMbps;
    uint32_t maxFs;
    uint32_t maxDpbMbs;
    uint32_t maxBr;
    uint32_t maxCpb;
    uint16_t maxVmvR;
    uint8_t minCr;
    uint8_t maxMvsPer2mb;
};

struct VaEncoderContext {
    DeviceNode drmDeviceNode;      // 设配节点名
    VADisplay vaDpy;               // VA的指针
    int drmFd;                     // 设备节点FD
    uint32_t fourcc;               // YUV surface的数据格式, only support VA_FOURCC_NV12, YUV格式的一种表达形式
    uint32_t downloadYuvFourcc;    // 接收到用户空间的YUV格式  默认VA_FOURCC_I420【固定的】
    EncoderCapability outputFormat;      // 替代原本的VaEncoderOutput

    struct {
        VAConfigID configID;    // RGB的configID
        VAContextID contextID;  // RGB的contextID, 根据config来光剑的
        VABufferID pipelineBuf; // 这个pipelineBuf只会在 RGB to YUV环节被使用
        uint32_t width;         // RGBA格式数据宽度，此处RGBA数据w和enc里面的YUV/H264
                                // w分开定义，是因为输入输出数据的大小可以不同
        uint32_t height;        // RGBA格式数据高度
        uint32_t stride;        // RGBA格式数据宽度对齐值
        uint32_t pixelFormat;   // RGBA数据格式，比如VA_FOURCC_BGRA  Init方法中设定为VA_FOURCC_BGRX
        int reserved;
    } vpp;
    struct {
        int frameSize; // 根据输出图片的大小计算来的
        int constraintSetFlag;
        int rateControlMethod;         // 默认VA_RC_VBR，Rate control支持VBR和CBR两种
        uint32_t rtFormat;             // VA_RT_FORMAT_YUV420
        uint32_t intraPeriod;          // I frame interval.  gopSize,初始化的时候需要提供的
        uint32_t ipPeriod;             // I/P frame interval. 默认是1
        uint32_t bitRate;              // bps. 比特率
        uint32_t maxBitRate;           // bps. 最大比特率
        uint32_t frameRate;            // fps. 帧率
        uint32_t gopSize;              // 图像组，两个I帧之间的距离
        uint32_t profileIdc;           // 画质级别
        uint32_t pictureWidth;         // 输出的YUV、H264、H265的图像宽度
        uint32_t pictureHeight;        // 输出的YUV、H264、H265的图像高度
        VAProfile profile;             // base/main
        VAContextID contextID;         // vaCreateContext接收configID创建出了一个contextID
        VAConfigID configID;           // vaCreateConfig 接收profile,创建得到了一个configID
        VAEntrypoint selectEntrypoint; // 编码类型为VAEntrypointEncSlic
        // 编码
        int useSlot[REF_SURFACE_NUM];
        uint64_t currentFrameDisplay;
        uint64_t currentIDRdisplay;
        uint64_t encFrameNumber;
        uint64_t idrFrameNum;
        uint64_t currentCpbRemoval;
        uint64_t prevIdrCpbRemoval;
        uint64_t currentIdrCpbRemoval;
        uint32_t currentDpbRemovalDelta;
        uint32_t currentFrameType;
        uint32_t currentFrameNum;
        int picInitQpMinus26;
        uint32_t currentPoc;
        uint32_t numRefFrames;
        uint32_t numShortTerm;
        uint32_t pictureWidthInMbs;
        uint32_t pictureHeightInMbs;
        uint32_t pictureWidthAlignedH265;
        uint32_t pictureHeightAlignedH265;
        VABufferID seqParamBufID;
        VABufferID picParamBufID;
        VABufferID sliceParamBufID;
        VABufferID miscHrdBufID;
        VABufferID miscRateControlBufID;
        VASurfaceID refSurface[REF_SURFACE_NUM]; // 参考帧surface
        // 编码 H.264
        VAPictureH264 ReferenceFramesH264[REF_SURFACE_NUM];
        VAPictureH264 RefPicList0H264[REF_PIC_LIST_NUM];
        VAPictureH264 RefPicList1H264[REF_PIC_LIST_NUM];
        VAPictureH264 CurrentCurrPicH264;
        VAEncSequenceParameterBufferH264 seqParamH264; // sps table数据结构
        VAEncPictureParameterBufferH264 picParamH264;  // pps table数据结构
        VAEncSliceParameterBufferH264 sliceParamH264;  // slice 宏块数据结
        // 编码 H.265
        VAPictureHEVC ReferenceFramesH265[REF_SURFACE_NUM];
        VAPictureHEVC RefPicList0H265[REF_PIC_LIST_NUM];
        VAPictureHEVC RefPicList1H265[REF_PIC_LIST_NUM];
        VAPictureHEVC CurrentCurrPicH265;
        VAEncSequenceParameterBufferHEVC seqParamH265; // sps table数据结构
        VAEncPictureParameterBufferHEVC picParamH265;  // pps table数据结构
        VAEncSliceParameterBufferHEVC sliceParamH265;  // slice 数据结构
    } enc;
};

class VaEncoderAmd {
public:
    VaEncoderAmd() = default;
    ~VaEncoderAmd() = default;

    /**
     * @brief 初始化编码器输入输出帧的大小
     * @param [in] inSize: 输入视频帧大小
     * @param [in] outSize: 输出视频帧大小
     * @param [in] deviceNode: 显卡设备类型
     * @return EncTurboCode: ENC_TURBO_SUCCESS表示成功，其他错误码表示失败
     */
    EncTurboCode ContextInit(EncoderConfig &config);

    /**
     * @brief 打开显卡fd与初始化vaDpy
     * @return EncTurboCode: ENC_TURBO_SUCCESS表示成功，其他错误码表示失败
     */
    EncTurboCode VaDpyInit();

    /**
     * @brief 设置码率控制模式
     */
    EncTurboCode SetRateControl(uint32_t rateControl);

    /**
     * @brief 设置画质级别
     */
    EncTurboCode SetProfileIdc(uint32_t profileIdc);

    /**
     * @brief 设置比特率
     */
    void SetBitRate(uint32_t bitRate);

    /**
     * @brief 设置帧率
     */
    void SetFrameRate(uint32_t frameRate);

    /**
     * @brief 设置图像组大小
     */
    void SetGopSize(uint32_t gopSize);

    /**
     * @brief 设置是否需要关键帧
     */
    void SetKeyFrame(uint32_t n);

    /**
     * @brief 打开编码器，创建资源
     */
    EncTurboCode EncodeOpen();

    /**
     * @brief 创建视频处理器资源
     */
    EncTurboCode VppOpen();

    /**
     * @brief 关闭视频处理器
     */
    EncTurboCode VppClose();

    /**
     * @brief 停止编码，释放编码资源
     */
    EncTurboCode EncodeClose();

    /**
     * @brief 释放编码器驱动资源，关闭显卡设备节点
     */
    EncTurboCode EncodeDestroy();

    /**
     * @brief 释放上下文资源
     */
    void ContextDestroy();

    /**
     * @brief 把RGB的显存地址绑定到编码器上
     * @param [in] handle: RGB显存地址
     * @param [out] surfaceId: surface空间的索引
     * @return EncTurboCode: ENC_TURBO_SUCCESS表示成功，其他错误码表示失败
     */
    EncTurboCode ImportRgbSurface(uint64_t handle, VASurfaceID &surfaceId);

    /**
     * @brief 释放存储RGB数据的surface空间
     * @param [in] surfaceId: RGB显存地址
     * @param [out] surfaceId: surface空间的索引
     * @return EncTurboCode: ENC_TURBO_SUCCESS表示成功，其他错误码表示失败
     */
    EncTurboCode ReleaseRgbSurface(VASurfaceID &rgbSurfaceId);

    /**
     * @brief 创建存储YUV数据的Surface
     * @param [out] surfaceId: 存储YUV数据的surface索引
     * @return EncTurboCode: ENC_TURBO_SUCCESS表示成功，其他错误码表示失败
     */
    EncTurboCode CreateYuvSurface(VASurfaceID &surfaceId);

    /**
     * @brief 释放存储YUV数据的Surface
     * @param [out] surfaceId: 存储YUV数据的surface索引
     * @return EncTurboCode: ENC_TURBO_SUCCESS表示成功，其他错误码表示失败
     */
    EncTurboCode ReleaseYuvSurface(VASurfaceID &surfaceId);

    /**
     * @brief 创建映射YUV数据的Image
     * @param [out] yuvImage: Image的唯一索引
     * @return EncTurboCode: ENC_TURBO_SUCCESS表示成功，其他错误码表示失败
     */
    EncTurboCode CreateYuvImage(VAImage &yuvImage);

    /**
     * @brief 释放映射YUV数据的Image
     * @param [out] outImage: Image的唯一索引
     * @return EncTurboCode: ENC_TURBO_SUCCESS表示成功，其他错误码表示失败
     */
    EncTurboCode ReleaseYuvImage(VAImage &outImage);

    /**
     * @brief YUV数据拷贝到Image
     * @param [in] yuvSurfaceId: 存储YUV的surface空间索引
     * @param [in] outImage: 内核内存空间buffer
     * @return EncTurboCode: ENC_TURBO_SUCCESS表示成功，其他错误码表示失败
     */
    EncTurboCode CopyYuvSurfaceToImage(VASurfaceID yuvSurfaceId, VAImage outImage);

    /**
     * @brief Image映射到用户态内存空间
     * @param [in] outImage: 内核内存空间buffer
     * @param [out] dataOutput: 映射地址与数据大小结构体
     * @return EncTurboCode: ENC_TURBO_SUCCESS表示成功，其他错误码表示失败
     */
    EncTurboCode MapImage(VAImage outImage, ConvertDataOutput &dataOutput);

    /**
     * @brief 解除Image映射
     * @param [in] outImage: Image空间索引
     * @return EncTurboCode: ENC_TURBO_SUCCESS表示成功，其他错误码表示失败
     */
    EncTurboCode UnmapImage(VAImage outImage);

    /**
     * @brief 创建H264/265编码后存储数据的buffer
     * @param [out] bufferId: Buffer的唯一索引
     * @return EncTurboCode: ENC_TURBO_SUCCESS表示成功，其他错误码表示失败
     */
    EncTurboCode CreateCodedBuffer(VABufferID &bufferId);

    /**
     * @brief 释放H264/265编码后存储数据的buffer
     * @param [out] bufferId: Buffer的唯一索引
     * @return EncTurboCode: ENC_TURBO_SUCCESS表示成功，其他错误码表示失败
     */
    EncTurboCode ReleaseCodedBuffer(VABufferID &bufferId);

    /**
     * @brief 执行RGB转YUV格式操作
     * @param [in] rgbSurfaceId: RGB surface id
     * @param [out] yuvSurfaceId: yuv surface id
     * @return EncTurboCode: ENC_TURBO_SUCCESS表示成功，其他错误码表示失败
     */
    EncTurboCode RgbConvertToYuv(VASurfaceID &rgbSurfaceId, VASurfaceID &yuvSurfaceId);

    /**
     * @brief YUV数据编码为视频码流
     * @param [in] yuvSurfaceId: 存储Yuv的surface空间
     * @param [out] codedbufBufId: 存放编码后的数据
     * @return EncTurboCode: ENC_TURBO_SUCCESS表示成功，其他错误码表示失败
     */
    EncTurboCode Encode(VASurfaceID yuvSurfaceId, VABufferID codedbufBufId);

    /**
     * @brief 将编码后的数据Map到内存空间
     * @param [in] codedbufBufId:  存放编码后的数据
     * @param [in] codedBufferSegment: CPU内存空间
     * @param [out] handle: 内存地址
     * @param [out] dataLen: 实际数据大小
     * @return EncTurboCode: ENC_TURBO_SUCCESS表示成功，其他错误码表示失败
     */
    EncTurboCode MapStreamBuffer(VABufferID codedbufBufId, StreamDataOutput &data);
    
    /**
     * @brief 解除编码数据的映射
     * @param [in] codedbufBufId: 存放编码后的数据
     * @return EncTurboCode: ENC_TURBO_SUCCESS表示成功，其他错误码表示失败
     */
    EncTurboCode UnmapStreamBuffer(VABufferID codedbufBufId);

    /**
     * @brief 查询Rgb转Yuv数据是否已经完成
     * @param [in] yuvSurfaceId: 存放Yuv的surfaceId
     * @return EncTurboCode: ENC_TURBO_SUCCESS表示成功，其他错误码表示失败
     */
    EncTurboCode CheckConvertStatus(VASurfaceID &yuvSurfaceId);

    /**
     * @brief 将不连续的数据映射到连续的内存空间中
     * @param [in] codedBufferSegment: 链表的头节点
     * @param [in] data: 内存起始地址
     * @param [out] dataLen: 被映射数据的字节数
     * @return EncTurboCode: ENC_TURBO_SUCCESS表示成功，其他错误码表示失败
     */
    void RetriveBufferData(VACodedBufferSegment *codedBufferSegment, uint8_t *data, uint32_t &dataLen);

    /**
     * @brief 处于运行态并修改参数后，根据修改的参数更改与之关联的参数
     */
    void SetNewValue();

private:
    VaEncoderContext m_vaContext {};
    uint32_t m_frameIndex { 0 };
    uint32_t ClcYuvSize(uint32_t size) const;

    EncTurboCode DisplayDrmOpen();

    EncTurboCode EncVaQueryConfigEntrypoints();
    EncTurboCode EncVaCreateContext();
    EncTurboCode EncVaCreateSurfaces();

    EncTurboCode EncVaUninit();
    void DisplayDrmClose();

    EncTurboCode VppCreateConfig();
    EncTurboCode VppCreateContext();
    EncTurboCode VppDestroyConfig();
    EncTurboCode VppDestroyContext();

    EncTurboCode EncodeDestroyConfig();
    EncTurboCode EncodeDestroyContext();

    // 编码用
    bool m_needKeyFrame { false };
    int UtilityGetFreeSlot();
    void EncBuffersDestroy(VABufferID &bufId);
    void EncEncodeFinish();
    void EncUpdateIDRRemovalTime();
    void EncUpdateRemovalTime();
    EncTurboCode EncUpdateMiscRateCtrl();
    EncTurboCode EncUpdateMiscHrd();
    EncTurboCode EncEncodeStart(VASurfaceID &yuvSurfaceId);

    // H264编码用
    void SetProfileIdcH264();
    void EncUpdateRefPicListH264();
    uint8_t EncGuessLevelH264(int profileIdc, int64_t bitrate, int framerate, int maxDecFrameBuffering);
    void EncSeqParamInitH264(); // 初始化SPS参数
    void EncPicParamInitH264(); // 初始化PPS参数
    EncTurboCode EncUpdatePictureParamH264(VABufferID codedbufBufId);
    EncTurboCode EncUpdateSliceParamH264();
    EncTurboCode EncPictureEncodeH264(VASurfaceID &yuvSurfaceId, VABufferID &codedbufBufId);
    EncTurboCode EncUpdateSdquenceParaH264();
    void EncUpdateRefFramesH264();
    uint32_t GetListNodeNums(StreamDataOutput &data);

    // H265编码用
    std::pair<uint8_t *, uint32_t> m_stream { nullptr, 0 };
    VaBitstream m_vaBitstream;
    EncTurboCode ChangeSpsStreamH265(StreamDataOutput &data);
    void EncUpdateRefPicListH265();
    void EncSeqParamInitH265(); // 初始化SPS参数
    void EncPicParamInitH265(); // 初始化PPS参数
    EncTurboCode EncUpdatePictureParamH265(VABufferID codedbufBufId);
    EncTurboCode EncUpdateSliceParamH265();
    EncTurboCode EncPictureEncodeH265(VASurfaceID &yuvSurfaceId, VABufferID &codedbufBufId);
    EncTurboCode EncUpdateSdquenceParaH265();
    void EncUpdateRefFramesH265();
    EncTurboCode CheckEncodeEnd(VASurfaceID &yuvSurfaceId);
};
}
} // Vmi

#endif
