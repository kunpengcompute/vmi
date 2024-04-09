/*
 * 版权所有 (c) 华为技术有限公司 2023-2023
 * 功能描述：CPU视频流编码器
 */

#ifndef GPU_TURBO
#define GPU_TURBO

#include <set>
#include "GpuEncoderBase.h"

namespace Vmi {
namespace GpuEncoder {

class CpuEncoder : public GpuEncoderBase {
public:
    explicit CpuEncoder(GpuType type);

    /**
     * @brief 初始化编码引擎
     * @param [in] config: 初始化参数
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
     * @param [in] type: Buffer类型
     * @param [out] buffer: 创建的buffer
     * @return int32_t: OK表示成功，其他错误码表示失败
     */
    int32_t CreateBuffer(FrameFormat format, MemType type, GpuEncoderBufferT &buffer) override;

    /**
     * @brief 从外部导入RGB数据并存储在VA创建的Buffer
     * @param [in] format: 视频帧类型
     * @param [in] handle: 视频帧数据指针
     * @param [out] buffer: 含有RGB数据的Buffer
     * @return int32_t: OK表示成功，其他错误码表示失败
     */
    int32_t ImportBuffer(FrameFormat format, uint64_t handle, GpuEncoderBufferT &buffer) override;

    /**
     * @brief 释放Buffer
     * @param [in] buffer: 需要被释放的Buffer
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
     * @brief 解除buffer与内存空间的映射关系
     * @param [in] buffer: 需要解除映射的buffer
     * @return int32_t: OK表示成功，其他错误码表示失败
     */
    int32_t UnmapBuffer(GpuEncoderBufferT &buffer) override;

    /**
     * @brief 部分buffer无法直接映射，可以利用该接口完成buffer映射. 一般是采用264/265的编码后的不连续存储的数据
     * @param [in] buffer: 存储视频帧数据的buffer
     * @param [in] data: 内存buffer的起始地址
     * @param [in] memLen: 内存buffer的大小
     * @param [out] dataLen: 实际映射数据大小
     * @return int32_t: OK表示成功，其他错误码表示失败
     */
    int32_t RetriveBufferData(GpuEncoderBufferT &buffer, uint8_t* data, uint32_t memLen, uint32_t& dataLen) override;

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
    uint32_t AlignUp(uint32_t val, uint32_t align);
    int32_t ReleaseAllBuffers();
    
    GpuType m_gpuType = GPU_NONE;
    FrameSize m_inSize{0, 0, 0, 0};
    FrameSize m_outSize{0, 0, 0, 0};
    FrameFormat m_outFormat = FRAME_FORMAT_NONE;
    std::set<GpuEncoderBufferT> m_buffers{};
};
}
} // Vmi
#endif