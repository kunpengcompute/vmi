/*
 * 功能说明: 适配NETINT硬件视频解码器，包括解码器初始化、启动、解码、停止、销毁等
 */
#ifndef VIDEO_DECODER_NETINT_H
#define VIDEO_DECODER_NETINT_H

#include <atomic>
#include "VideoDecoder.h"
#include "ni_device_api_logan.h"
#include "ni_rsrc_api_logan.h"

namespace MediaCore {
class VideoDecoderNetint : public VideoDecoder {
public:
    VideoDecoderNetint() = default;
    ~VideoDecoderNetint() override;

    DecoderRetCode CreateDecoder(MediaStreamFormat decType) override;
    DecoderRetCode InitDecoder() override;
    DecoderRetCode SendStreamData(uint8_t *buffer, uint32_t filledLen) override;
    DecoderRetCode RetrieveFrameData(uint8_t *buffer, uint32_t maxLen, uint32_t *filledLen) override;
    DecoderRetCode SetCallbacks(std::function<void(DecodeEventIndex, uint32_t, void *)> eventCallBack) override;
    DecoderRetCode SetCopyFrameFunc(
        std::function<uint32_t(uint8_t*, uint8_t*, const PicInfoParams &, uint32_t)> copyFrame) override;
    DecoderRetCode SetDecodeParams(DecodeParamsIndex index, void *decParams) override;
    DecoderRetCode GetDecodeParams(DecodeParamsIndex index, void *decParams) override;
    DecoderRetCode Flush() override;
    DecoderRetCode StartDecoder() override;
    DecoderRetCode StopDecoder() override;
    void DestroyDecoder() override;

private:
    static constexpr uint32_t DEFAULT_WIDTH = 1280;
    static constexpr uint32_t DEFAULT_HEIGHT = 720;
    static constexpr uint32_t DEFAULT_FRAMERATE = 25;
    static constexpr uint32_t DEFAULT_BITDEPTH = 8;

    /**
     * @功能描述: 加载NETINT动态库
     * @返回值: true  成功
     *          false 失败
     */
    bool LoadNetintSharedLib() const;

    /**
     * @功能描述: 初始化解码器资源
     * @返回值: true  成功
     *          false 失败
     */
    bool InitContext();

    /**
     * @功能描述: 初始化解码器上下文参数
     * @返回值: true  成功
     *          false 失败
     */
    bool InitCtxParams();

    /**
     * @功能描述: 预处理解码前的数据包，将数据写入netint
     * @参数 [in] src 输入码流数据
     * @参数 [in] inputSize 输入数据大小
     * @返回值: sendSize 写入netint数据大小
     *          NI_LOGAN_RETCODE_FAILURE(-1) 失败
     */
    int InitPacketData(const uint8_t *src, const uint32_t inputSize);

    /**
     * @功能描述: 预处理解码后的帧数据，申请帧数据buffer
     * @返回值: true  成功
     *          false 失败
     */
    bool InitFrameData();

    /**
     * @功能描述: 将数据写入netint
     * @参数 [in] buffer 输入码流数据缓存
     * @参数 [in] filledLen 输入码流数据长度(Byte)
     * @返回值: VIDEO_DECODER_SUCCESS 成功
     *          VIDEO_DECODER_DECODE_FAIL 写入失败
     */
    DecoderRetCode DecoderWriteData(const uint8_t *buffer, const uint32_t filledLen);

    /**
     * @功能描述: 向netint读取解码后的一帧数据
     * @参数 [in] buffer 输出数据缓存
     * @参数 [in] maxLen 输出缓冲区最大长度(Byte)
     * @参数 [out] filledLen 输出数据长度(Byte)
     * @返回值: VIDEO_DECODER_SUCCESS 成功
     *          VIDEO_DECODER_DECODE_FAIL 解码一帧失败
     */
    DecoderRetCode DecoderReadData(uint8_t *buffer, const uint32_t maxLen, uint32_t *filledLen);

    /**
     * @功能描述: 处理从netint读取的解码后的数据，将数据传给上层
     * @参数 [in] buffer 输出数据缓存
     * @参数 [in] maxLen 输出缓冲区最大长度(Byte)
     * @参数 [out] filledLen 输出码据长度(Byte)
     * @返回值: VIDEO_DECODER_SUCCESS 成功
     *          VIDEO_DECODER_DECODE_FAIL 处理数据失败
     */
    DecoderRetCode DecoderHandleData(uint8_t *buffer, const uint32_t maxLen, uint32_t *filledLen);

    /**
     * @功能描述: 解码统计帧率
     */
    void DecodeFpsStat();

    /**
     * @功能描述: 销毁netint解码相关资源
     */
    void DestroyContext();

     /**
     * @功能描述: 发送码流数据到解码设备中，若数据包含码流头信息，将被保存在会话上下文中，
     *           保证执行ni_logan_device_dec_session_flush后可以继续正常解码
     * @返回值: 成功返回写入设备的数据大小；失败则返回如下错误码：
     *             NI_LOGAN_RETCODE_INVALID_PARAM
     *             NI_LOGAN_RETCODE_ERROR_NVME_CMD_FAILED
     *             NI_LOGAN_RETCODE_ERROR_INVALID_SESSION
     */
    int DeviceDecSessionWrite();

    /**
     * @功能描述: 扫描输入数据，找到下一个非VCL NAL单元（包含码流头信息）
     * @参数 [in] inBuf：码流数据和大小
     * @参数 [in] codec：编解码器类型（H.264或H.265）
     * @参数 [out] nalType：NAL单元类型
     * @返回值: 成功返回NAL单元的数据大小（包含起始码）
     *          失败则返回0
     */
    int FindNextNonVclNalu(std::pair<uint8_t*, uint32_t> inBuf, uint32_t codec, int &nalType);

    /**
     * @功能描述: 对输入码流数据查找NAL的起始码，并返回起始码所在位置
     * @参数 [in] inBuf：码流数据和大小
     * @返回值: 成功返回NAL起始码的起始位置
     *          失败则返回-1
     */
    int FindNalStartCode(std::pair<uint8_t*, uint32_t> &inBuf);

    ni_codec_t m_codec = EN_H264;
    ni_logan_encoder_params_t m_decApiParams {};
    ni_logan_session_context_t m_sessionCtx {};
    ni_logan_device_context_t *m_devCtx = nullptr;
    ni_logan_session_data_io_t m_packet {};
    ni_logan_session_data_io_t m_frame {};
    uint32_t m_writeWidth = DEFAULT_WIDTH;
    uint32_t m_writeHeight = DEFAULT_HEIGHT;
    int32_t m_stride = DEFAULT_WIDTH;
    uint32_t m_planeWidth = 0;
    uint32_t m_planeHeight = 0;
    int m_frameRate = DEFAULT_FRAMERATE;
    int m_bitDepth = DEFAULT_BITDEPTH;
    unsigned long m_load = 0;
    uint32_t m_startOfStream = 0;

    // 帧率统计相关
    int64_t m_lastTime = 0;
    uint32_t m_frameCount = 0;

    std::atomic<bool> m_stop { true };
    std::function<void(DecodeEventIndex, uint32_t, void *)> m_eventCallBack {};
    std::function<uint32_t(uint8_t*, uint8_t*, const PicInfoParams &, uint32_t)> m_copyFrame {};
};
} // namespace MediaCore

#endif // VIDEO_DECODER_NETINT_H
