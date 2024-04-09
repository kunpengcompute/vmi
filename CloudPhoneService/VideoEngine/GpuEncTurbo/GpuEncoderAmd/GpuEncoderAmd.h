/*
 * 版权所有 (c) 华为技术有限公司 2023-2023
 * 功能描述：AMD显卡视频编码器
 */

#ifndef GPU_ENCODER_AMD_H
#define GPU_ENCODER_AMD_H

#include <set>
#include <mutex>
#include <atomic>
#include <cstdint>
#include "va.h"
#include "VaEncoderAmd.h"
#include "../GpuEncoderBase.h"

namespace Vmi {
namespace GpuEncoder {

enum GpuEncoderAmdStat : uint32_t {
    ENCODER_ENGINE_STATE_INVALID,
    ENCODER_ENGINE_STATE_INIT,
    ENCODER_ENGINE_STATE_RUNNING,
    ENCODER_ENGINE_STATE_STOP,
    ENCODER_ENGINE_STATE_RESETTING
};

/**
 * @brief 存储RGB的buffer
 */
struct GpuRgbBufferAmd : public GpuEncoderBuffer {
    bool mapped = false;
    bool external = false;
    VASurfaceID rgbSurfaceId = 0;  // RGB
};

struct GpuYuvBufferAmd : public GpuEncoderBuffer {
    bool mapped = false;
    bool external = false;
    VASurfaceID vaYuvSurfaceId = 0; // YUV
    VAImage vaOutImage = {};     // For YUV Map
};

struct GpuStreamBufferAmd : public GpuEncoderBuffer {
    bool mapped = false;
    bool external = false;
    VABufferID codedbufBufId = VA_INVALID_ID;                  // H264/265
    VACodedBufferSegment* codedBufferSegment = nullptr;  // For H264/265 Map
};

using GpuRgbBufferAmdT = GpuRgbBufferAmd *;
using GpuYuvBufferAmdT = GpuYuvBufferAmd *;
using GpuStreamBufferAmdT = GpuStreamBufferAmd *;

class GpuEncoderAmd : public GpuEncoderBase {
public:
    explicit GpuEncoderAmd(GpuType type);

    /**
     * @brief 初始化编码引擎
     * @param [in] config: 编码器转码/编码功能,显卡节点,输入图像和输出图像大小
     * @return int32_t: OK表示成功，其他错误码表示失败
     */
    int32_t Init(EncoderConfig &config) override;

    /**
     * @brief 销毁编码器
     * @return int32_t: OK表示成功，其他错误码表示失败
     */
    int32_t DeInit() override;

    /**
     * @brief 开启编码器
     * @return int32_t: OK表示成功，其他错误码表示失败
     */
    int32_t Start() override;

    /**
     * @brief 停止编码器
     * @return int32_t: OK表示成功，其他错误码表示失败
     */
    int32_t Stop() override;

    /**
     * @brief 创建一个Buffer
     * @param [in] format: 视频帧类型
     * @param [in] memType: Buffer类型
     * @param [out] buffer: buffer
     * @return int32_t: OK表示成功，其他错误码表示失败
     */
    int32_t CreateBuffer(FrameFormat format, MemType memType, GpuEncoderBufferT &buffer) override;

    /**
     * @brief 导入一个外部的buffer
     * @param [in] format: 视频帧类型
     * @param [in] handle: 视频帧数据指针
     * @param [out] buffer: buffer
     * @return int32_t: OK表示成功，其他错误码表示失败
     */
    int32_t ImportBuffer(FrameFormat format, uint64_t handle, GpuEncoderBufferT &buffer) override;

    /**
     * @brief 释放Buffer
     * @param [in] buffer: buffer
     * @return int32_t: OK表示成功，其他错误码表示失败
     */
    int32_t ReleaseBuffer(GpuEncoderBufferT &buffer) override;

    /**
     * @brief 将Device类型buffer中的数据映射到内存
     * @param [out] buffer: buffer
     * @param [in] flag: 数据的读写权限
     * @return int32_t: OK表示成功，其他错误码表示失败
     */
    int32_t MapBuffer(GpuEncoderBufferT &buffer, uint32_t flag) override;

    /**
     * @brief 解除buffer映射
     * @param [in] buffer: 需要解除映射的buffer
     * @return int32_t: OK表示成功，其他错误码表示失败
     */
    int32_t UnmapBuffer(GpuEncoderBufferT &buffer) override;

    /**
     * @brief 部分buffer无法直接映射，可以利用该接口完成buffer映射. 一般是采用264/265的编码后的不连续存储的数据
     * @param [in] buffer: 存储视频帧数据的buffer
     * @param [out] handle: 内存buffer的起始地址
     * @param [in] memLen: 内存buffer的大小
     * @param [out] dataLen: 实际映射数据大小
     * @return int32_t: OK表示成功，其他错误码表示失败
     */
    int32_t RetriveBufferData(GpuEncoderBufferT &buffer, uint8_t* handle,
                                          uint32_t memLen, uint32_t& dataLen) override;

    /**
     * @brief 执行RGB格式转YUV格式
     * @param [in] inBuffer: 存储RGB的buffer
     * @param [out] outBuffer: 存储YUV的buffer
     * @return int32_t: OK表示成功，其他错误码表示失败
     */
    int32_t Convert(GpuEncoderBufferT &inBuffer, GpuEncoderBufferT &outBuffer) override;

    /**
     * @brief 编码
     * @param [in] inBuffer: 存储YUV的buffer
     * @param [out] outBuffer: 存储H264/265编码后视频帧的buffer
     * @return int32_t: OK表示成功，其他错误码表示失败
     */
    int32_t Encode(GpuEncoderBufferT &inBuffer, GpuEncoderBufferT &outBuffer) override;

    /**
     * @brief 设置编码器的参数
     * @param [in] params: 要设置的参数列表
     * @param [in] num: 参数列表的大小
     * @return int32_t: OK表示成功，其他错误码表示失败
     */
    int32_t SetEncodeParam(EncodeParamT params[], uint32_t num) override;

    /**
     * @brief 重置编码器，使部分编码参数生效
     * @return int32_t: OK表示成功，其他错误码表示失败
     */
    int32_t Reset() override;
private:
    uint32_t AlignUp(uint32_t val, uint32_t align) const;
    int32_t CreateYuvBuffer(FrameFormat format, GpuEncoderBufferT &buffer);
    int32_t CreatecodedBuffer(FrameFormat format, GpuEncoderBufferT &buffer);
    int32_t ReleaseRgbBuffer(GpuEncoderBufferT &buffer);
    int32_t ReleaseYuvBuffer(GpuEncoderBufferT &buffer);
    int32_t ReleaseCodedBuffer(GpuEncoderBufferT &buffer);
    int32_t MapYuvBuffer(GpuEncoderBufferT &buffer);
    int32_t MapCodedBuffer(GpuEncoderBufferT &buffer);
    int32_t UnmapYuvBuffer(GpuEncoderBufferT &buffer);
    int32_t UnmapCodedBuffer(GpuEncoderBufferT &buffer);
    int32_t ReleaseAllBuffers();
    bool RecordOutFrameFormat(EncoderCapability capability);
    bool CheckAndLockStatusForReset();
    int32_t SetRateControl(EncodeParamT &param);
    int32_t SetBitRate(EncodeParamT &param, bool &needRestart);
    int32_t SetFrameRate(EncodeParamT &param);
    int32_t SetGopsize(EncodeParamT &param);
    int32_t SetProfileIdc(EncodeParamT &param, bool &needRestart);

    VaEncoderAmd m_vaEncoderAmd{};
    std::atomic<GpuEncoderAmdStat> m_engineStat{ENCODER_ENGINE_STATE_INVALID};
    std::mutex m_engineLock{};
    GpuType m_gpuType = GPU_NONE;
    FrameSize m_inSize{0, 0, 0, 0};
    FrameSize m_outSize{0, 0, 0, 0};
    FrameFormat m_outFormat = FRAME_FORMAT_NONE;
    uint32_t m_rateControl = 0;
    uint32_t m_bitRate = 0;
    uint32_t m_frameRate = 0;
    uint32_t m_gopSize = 0;
    uint32_t m_profileIdc = 0;
    std::set<GpuEncoderBufferT> m_buffers{};
};
}
} // Vmi

#endif
