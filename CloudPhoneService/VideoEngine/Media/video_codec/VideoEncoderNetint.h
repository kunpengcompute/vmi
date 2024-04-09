/*
 * 版权所有 (c) 华为技术有限公司 2021-2022
 * 功能说明: 适配NETINT硬件视频编码器，包括编码器初始化、启动、编码、停止、销毁等
 */
#ifndef VIDEO_ENCODER_NETINT_H
#define VIDEO_ENCODER_NETINT_H

#include <string>
#include <unordered_map>
#include <atomic>
#include "VideoCodecApi.h"
#include "VideoEncoderCommon.h"
#include "ni_device_api_logan.h"
#include "ni_defs_logan.h"
#include "ni_rsrc_api_logan.h"

enum NiCodecType : uint32_t {
    NI_CODEC_TYPE_H264 = 0,
    NI_CODEC_TYPE_H265 = 1
};
class VideoEncoderNetint : public VideoEncoderCommon {
public:
    /**
     * @功能描述: 构造函数
     */
    explicit VideoEncoderNetint(NiCodecType codecType, EncoderFormat codecFormat);

    /**
     * @功能描述: 析构函数
     */
    ~VideoEncoderNetint() override;

    /**
     * @功能描述: 初始化编码器
     * @返回值: VIDEO_ENCODER_SUCCESS 成功
     *          VIDEO_ENCODER_INIT_FAIL 初始化编码器失败
     */
    EncoderRetCode InitEncoder();

    /**
     * @功能描述: 启动编码器
     * @返回值: VIDEO_ENCODER_SUCCESS 成功
     *          VIDEO_ENCODER_START_FAIL 启动编码器失败
     */
    EncoderRetCode StartEncoder() override;

    /**
     * @功能描述: 编码一帧数据
     * @参数 [in] inputData: 编码输入数据地址
     * @参数 [in] inputSize: 编码输入数据大小
     * @参数 [out] outputData: 编码输出数据地址
     * @参数 [out] outputSize: 编码输出数据大小
     * @返回值: VIDEO_ENCODER_SUCCESS 成功
     *          VIDEO_ENCODER_ENCODE_FAIL 编码一帧失败
     */
    EncoderRetCode EncodeOneFrame(const uint8_t *inputData, uint32_t inputSize,
        uint8_t **outputData, uint32_t *outputSize) override;

    /**
     * @功能描述: 停止编码器
     * @返回值: VIDEO_ENCODER_SUCCESS 成功
     *          VIDEO_ENCODER_STOP_FAIL 停止编码器失败
     */
    EncoderRetCode StopEncoder() override;

    /**
     * @功能描述: 销毁编码器，释放编码资源
     */
    void DestroyEncoder() override;

    /**
     * @功能描述: 重置编码器
     * @返回值: VIDEO_ENCODER_SUCCESS 成功
     *          VIDEO_ENCODER_RESET_FAIL 重置编码器失败
     */
    EncoderRetCode ResetEncoder() override;

    /**
     * @功能描述: 配置编码器参数
     * @参数 [in] config: 启动编码所需的配置参数
     * @返回值: VIDEO_ENCODER_SUCCESS 成功
     *          VIDEO_ENCODER_RESET_FAIL 配置编码器参数失败
     */
    EncoderRetCode Config(const VmiEncoderConfig& config) override;

    /**
     * @功能描述: 动态修改编码参数
     * @参数 [in] params: 可动态修改的编码参数
     * @返回值: VIDEO_ENCODER_SUCCESS 成功
     *          VIDEO_ENCODER_RESET_FAIL 修改编码器参数失败
     */
    EncoderRetCode SetParams(const VmiEncoderParams& params) override;

    /**
     * @功能描述: 强制I帧
     * @返回值: VIDEO_ENCODER_SUCCESS 成功
     *          VIDEO_ENCODER_FORCE_KEY_FRAME_FAIL 强制I帧失败
     */
    EncoderRetCode ForceKeyFrame();

private:
    /**
     * @功能描述: 加载NETINT动态库
     * @返回值: true 成功
     *          false 失败
     */
    bool LoadNetintSharedLib();

    /**
     * @功能描述: 初始化编码器资源
     * @返回值: true 成功
     *          false 失败
     */
    bool InitCodec();

    /**
     * @功能描述: 初始化编码器上下文参数
     * @返回值: true 成功
     *          false 失败
     */
    bool InitCtxParams();

    /**
     * @功能描述: 拷贝一帧数据到编码器
     * @参数 [in] src: 待编码数据地址
     * @返回值: true 成功
     *          false 失败
     */
    bool InitFrameData(const uint8_t *src);

    /**
     * @功能描述: 卸载NETINT动态库
     */
    void UnLoadNetintSharedLib();

    /**
     * @功能描述: 检查map中函数指针是否存在空指针
     */
    void CheckFuncPtr();

    ni_codec_t m_codec = EN_H264;
    ni_logan_encoder_params_t m_niEncParams = {};
    ni_logan_session_context_t m_sessionCtx = {};
    ni_logan_device_context_t *m_devCtx = nullptr;
    ni_logan_session_data_io_t m_frame = {};
    ni_logan_session_data_io_t m_packet = {};
    int m_width = DEFAULT_WIDTH;
    int m_height = DEFAULT_HEIGHT;
    int m_widthAlign = DEFAULT_WIDTH;
    int m_heightAlign = DEFAULT_HEIGHT;
    unsigned long m_load = 0;
    bool m_FunPtrError = false;
    bool m_isInited = false;
};

#endif  // VIDEO_ENCODER_NETINT_H
