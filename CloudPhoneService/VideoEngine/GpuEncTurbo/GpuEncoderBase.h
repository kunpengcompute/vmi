/*
 * 版权所有 (c) 华为技术有限公司 2023-2023
 * 功能描述：视频流编码器基类
 */

#ifndef GPU_ENCODER_BASE
#define GPU_ENCODER_BASE

#include "GpuEncoderParams.h"

struct ModuleInfo {
    // 硬件对应ID
    uint32_t moduleId = 0;
    // 硬件具备的能力列表，例如支持H264和H265编码，该值为(1 << CAP_VA_ENCODE_H264) | (1 << CAP_VA_ENCODE_HEVC)
    uint32_t capabilities = 0;
};

namespace Vmi {
namespace GpuEncoder {
/**
 * @brief 显卡设备节点
 */
enum DeviceNode : uint32_t {
    DEV_DRI_RENDERD128 = 128,
    DEV_DRI_RENDERD129 = 129,
    DEV_DRI_RENDERD130 = 130,
    DEV_DRI_RENDERD131 = 131,
    DEV_DRI_OTHER = 132
};

/**
 * @brief 视频帧类型
 */
enum FrameFormat : uint32_t {
    FRAME_FORMAT_NONE,
    FRAME_FORMAT_RGBA,
    FRAME_FORMAT_BGRA,
    FRAME_FORMAT_YUV,
    FRAME_FORMAT_NV12,
    FRAME_FORMAT_H264,
    FRAME_FORMAT_HEVC,
    FRAME_FORMAT_COUNT
};

/**
 * @brief Buffer类型
 */
enum MemType : uint32_t {
    MEM_TYPE_NONE,
    MEM_TYPE_HOST,
    MEM_TYPE_DEVICE,
    MEM_TYPE_COUNT
};

/**
 * @brief 错误码
 */
enum GpuEncoderErrorCode : uint32_t {
    OK,
    ERR_INVALID_STATUS,
    ERR_INVALID_DEVICE,
    ERR_INVALID_PARAM,
    ERR_OUT_OF_MEM,
    ERR_UNEXPECT_STATUS,
    ERR_UNSUPPORT_FORMAT,
    ERR_UNSUPPORT_OPERATION,
    ERR_INTERNAL_ERROR,
    ERR_NEED_RESET, // 部分编码参数设置后需要reset生效
    ERR_UNKNOW
};

/**
* @brief 视频帧大小
*/
struct FrameSize {
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t widthAligned = 0;
    uint32_t heightAligned = 0;
    bool operator==(const FrameSize &other)
    {
        return this->width == other.width &&
               this->height == other.height &&
               this->widthAligned == other.widthAligned &&
               this->heightAligned == other.heightAligned;
    }
    bool operator!=(const FrameSize &other)
    {
        return this->width != other.width ||
               this->height != other.height ||
               this->widthAligned != other.widthAligned ||
               this->heightAligned != other.heightAligned;
    }
};

/**
* @brief 显卡型号
*/
enum GpuType : uint32_t {
    GPU_NONE = 0,
    GPU_INNO_G1 = 0x1,
    GPU_INNO_G1P,
    GPU_INNO_MAX = 0x100,
    GPU_HANTRO_SG100 = 0x101,
    GPU_HANTRO_MAX = 0x200,
    GPU_A_W5100 = 0x201,
    GPU_A_W5500,
    GPU_A_W6600,
    GPU_A_W6800,
    GPU_A_MAX = 0x400,
    GPU_SOFT = 0x401,
    GPU_MAX = 0x10000,

    VPU_NETINT_T408 = 0x10001,
    VPU_NETINT_T432,
    VPU_NETINT_QUADRA_T2A,
    VPU_NETINT_MAX = 0x10200,
};

enum MapFlag : uint32_t {
    FLAG_READ = 1,
    FLAG_WRITE = 1 << 1,
};

struct GpuEncoderBuffer {
    FrameFormat format = FRAME_FORMAT_NONE;
    MemType memType = MEM_TYPE_NONE;
    FrameSize size = {};
    GpuType gpuType = GPU_NONE;
    uint8_t *data  = nullptr;
    uint32_t dataLen = 0;
};

enum EncoderCapability : uint32_t {
    CAP_NONE,
    CAP_FORMAT_CONVERT,
    CAP_VA_ENCODE_H264,
    CAP_VA_ENCODE_HEVC,
    CAP_MAX,
};

struct EncoderConfig {
    EncoderCapability capability = CAP_NONE;
    DeviceNode deviceNode = DEV_DRI_OTHER;
    FrameSize inSize = {};
    FrameSize outSize = {};
};

using GpuEncoderBufferT = GpuEncoderBuffer *;

class GpuEncoderBase {
public:
    GpuEncoderBase() = default;
    virtual ~GpuEncoderBase() = default;

    /**
     * @brief 初始化编码引擎
     * @param [in] config: 初始化参数
     * @return int32_t: OK表示成功，其他错误码表示失败
     */
    virtual int32_t Init(EncoderConfig &config) = 0;

    /**
     * @brief 销毁编码器
     * @return int32_t: OK表示成功，其他错误码表示失败
     */
    virtual int32_t DeInit() = 0;

    /**
     * @brief 开启编码器
     * @return int32_t: OK表示成功，其他错误码表示失败
     */
    virtual int32_t Start() = 0;

    /**
     * @brief 停止编码器
     * @return int32_t: OK表示成功，其他错误码表示失败
     */
    virtual int32_t Stop() = 0;

    /**
     * @brief 创建一个Buffer
     * @param [in] format: 视频帧类型
     * @param [in] type: Buffer类型
     * @param [out] buffer: 创建的buffer
     * @return int32_t: OK表示成功，其他错误码表示失败
     */
    virtual int32_t CreateBuffer(FrameFormat format, MemType type, GpuEncoderBufferT &buffer) = 0;

    /**
     * @brief 从外部导入RGB数据并存储在VA创建的Buffer
     * @param [in] format: 视频帧类型
     * @param [in] handle: 视频帧数据指针
     * @param [out] buffer: 含有RGB数据的Buffer
     * @return int32_t: OK表示成功，其他错误码表示失败
     */
    virtual int32_t ImportBuffer(FrameFormat format, uint64_t handle, GpuEncoderBufferT &buffer) = 0;

    /**
     * @brief 释放Buffer
     * @param [in] buffer: 需要被释放的Buffer
     * @return int32_t: OK表示成功，其他错误码表示失败
     */
    virtual int32_t ReleaseBuffer(GpuEncoderBufferT &buffer) = 0;

    /**
     * @brief 将Device类型buffer中的数据映射到内存
     * @param [out] buffer: buffer
     * @param [in] flag: 数据的读写权限
     * @return int32_t: OK表示成功，其他错误码表示失败
     */
    virtual int32_t MapBuffer(GpuEncoderBufferT &buffer, uint32_t flag) = 0;

    /**
     * @brief 解除buffer与内存空间的映射关系
     * @param [in] buffer: 需要解除映射的buffer
     * @return int32_t: OK表示成功，其他错误码表示失败
     */
    virtual int32_t UnmapBuffer(GpuEncoderBufferT &buffer) = 0;

    /**
     * @brief 部分buffer无法直接映射，可以利用该接口完成buffer映射. 一般是采用264/265的编码后的不连续存储的数据
     * @param [in] buffer: 存储视频帧数据的buffer
     * @param [in] data: 内存buffer的起始地址
     * @param [in] memLen: 内存buffer的大小
     * @param [out] dataLen: 实际映射数据大小
     * @return int32_t: OK表示成功，其他错误码表示失败
     */
    virtual int32_t RetriveBufferData(GpuEncoderBufferT &buffer, uint8_t* data, uint32_t memLen, uint32_t& dataLen) = 0;

    /**
     * @brief 执行RGB格式转YUV格式
     * @param [in] inBuffer: 存储RGB的buffer
     * @param [out] outBuffer: 存储YUV的buffer
     * @return int32_t: OK表示成功，其他错误码表示失败
     */
    virtual int32_t Convert(GpuEncoderBufferT &inBuffer, GpuEncoderBufferT &outBuffer) = 0;

    /**
     * @brief 编码
     * @param [in] inBuffer: 存储YUV的buffer
     * @param [out] outBuffer: 存储H264/265编码后视频帧的buffer
     * @return int32_t: OK表示成功，其他错误码表示失败
     */
    virtual int32_t Encode(GpuEncoderBufferT &inBuffer, GpuEncoderBufferT &outBuffer) = 0;

    /**
     * @brief 设置编码器的参数
     * @param [in] params: 要设置的参数列表
     * @param [in] num: 参数列表的大小
     * @return int32_t: OK表示成功，其他错误码表示失败
     */
    virtual int32_t SetEncodeParam(EncodeParamT params[], uint32_t num) = 0;

    /**
     * @brief 重置编码器，使部分编码参数生效
     * @return int32_t: OK表示成功，其他错误码表示失败
     */
    virtual int32_t Reset() = 0;
};
}
}

#endif