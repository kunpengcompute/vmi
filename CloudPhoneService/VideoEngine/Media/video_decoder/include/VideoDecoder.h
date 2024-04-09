/*
 * 功能说明: 视频解码器功能接口
 */

#ifndef VIDEO_DECODER_H
#define VIDEO_DECODER_H

#include <functional>

enum DecoderRetCode : uint32_t {
    VIDEO_DECODER_SUCCESS,                // 成功
    VIDEO_DECODER_CREATE_FAIL,            // 创建解码器失败
    VIDEO_DECODER_INIT_FAIL,              // 初始化解码器失败
    VIDEO_DECODER_START_FAIL,             // 启动解码器失败
    VIDEO_DECODER_DECODE_FAIL,            // 解码失败
    VIDEO_DECODER_STOP_FAIL,              // 停止解码器失败
    VIDEO_DECODER_DESTROY_FAIL,           // 销毁解码器失败
    VIDEO_DECODER_RESET_FAIL,             // 重置解码器失败
    VIDEO_DECODER_GET_DECODE_PARAMS_FAIL, // 获取解码参数失败
    VIDEO_DECODER_SET_DECODE_PARAMS_FAIL, // 设置解码参数失败
    VIDEO_DECODER_SET_FUNC_FAIL,          // 设置回调函数失败
    VIDEO_DECODER_WRITE_OVERFLOW,         // 输入buffer输送过快
    VIDEO_DECODER_READ_UNDERFLOW,         // 输出buffer获取过快
    VIDEO_DECODER_BAD_PIC_SIZE,           // 解码后实际分辨率与配置分辨率不符
    VIDEO_DECODER_EOS
};

enum DecoderPort : uint32_t {
    IN_PORT,
    OUT_PORT
};

// 解码输入格式
enum MediaStreamFormat : uint32_t {
    STREAM_FORMAT_AVC,
    STREAM_FORMAT_HEVC,
    STREAM_FORMAT_NONE
};

// 解码输出格式
enum MediaPixelFormat : uint32_t {
    PIXEL_FORMAT_RGBA_8888,
    PIXEL_FORMAT_YUV_420P,
    PIXEL_FORMAT_FLEX_YUV_420P,
    PIXEL_FORMAT_NV12,
    PIXEL_FORMAT_NV21,
    PIXEL_FORMAT_NONE
};

// 解码事件
enum DecodeEventIndex : uint32_t {
    INDEX_PIC_INFO_CHANGE,
    INDEX_EVENT_NONE
};

// 解码参数
enum DecodeParamsIndex : uint32_t {
    INDEX_PIC_INFO,
    INDEX_PORT_FORMAT_INFO,
    INDEX_ALIGN_INFO,
    INDEX_PARAM_NONE
};

struct AlignInfoParams {
    uint32_t widthAlign = 0;
    uint32_t heightAlign = 0;
};

struct PicInfoParams {
    uint32_t width = 0;
    uint32_t height = 0;
    int32_t stride = 0;
    uint32_t scanLines = 0;
    uint32_t cropWidth = 0;
    uint32_t cropHeight = 0;
};

struct PortFormatParams {
    DecoderPort port {};
    int32_t format = 0;
};

class VideoDecoder {
public:
    VideoDecoder() = default;
    virtual ~VideoDecoder() = default;

    /**
     * @功能描述: 创建解码器
     * @参数 [in] MediaStreamFormat decType, 解码器需要接受的码流类型
     * @返回值: VIDEO_DECODER_SUCCESS 成功
     *          VIDEO_DECODER_CREATE_FAIL 创建解码器失败
     */
    virtual DecoderRetCode CreateDecoder(MediaStreamFormat decType) = 0;

    /**
     * @功能描述: 初始化解码器
     * @返回值: VIDEO_DECODER_SUCCESS 成功
     *          VIDEO_DECODER_INIT_FAIL 初始化函数失败
     */
    virtual DecoderRetCode InitDecoder() = 0;

    /**
     * @功能描述: 设置解码器参数
     * @参数 [in] DecodeParamsIndex index, 需要设置的参数所对应的index
     * @参数 [in] decParams, 设置参数的具体数据
     * @返回值: VIDEO_DECODER_SUCCESS 成功
     *          VIDEO_DECODER_SET_DECODE_PARAMS_FAIL 设置参数函数失败
     */
    virtual DecoderRetCode SetDecodeParams(DecodeParamsIndex index, void *decParams) = 0;

    /**
     * @功能描述: 获取解码器参数
     * @参数 [in] DecodeParamsIndex index, 需要获取的参数所对应的index
     * @参数 [in] decParams, 获取参数的具体数据
     * @返回值: VIDEO_DECODER_SUCCESS 成功
     *          VIDEO_DECODER_GET_DECODE_PARAMS_FAIL 获取参数函数失败
     */
    virtual DecoderRetCode GetDecodeParams(DecodeParamsIndex index, void *decParams) = 0;

    /**
     * @功能描述: 设置对应解码器在发生事件时,通知OMX组件的回调函数
     * @参数 [in] eventCallBack, 对应参数意义如下:
     *            _1: 发生事件所对应的编号 index
     *            _2: 用于存放对应事件回调的数据 data1
     *            _3: 当uint32_t大小不足以存放对应事件回调的数据时,使用该指针传输数据指针 data2
     * @返回值: VIDEO_DECODER_SUCCESS 成功
     *          VIDEO_DECODER_SET_FUNC_FAIL 设置函数失败
     */
    virtual DecoderRetCode SetCallbacks(std::function<void(DecodeEventIndex, uint32_t, void *)> eventCallBack) = 0;

    /**
     * @功能描述: 设置将解码好的一帧数据拷贝到输出buffer的钩子函数(用于屏蔽输出接口格式差异)
     * @参数 [in] copyFrame,对应参数意义如下:
     *              _1: 解码完成数据地址 src
     *              _2: OMX组件提供的输出buffer地址 dst
     *              _3: 输出帧分辨率信息
     *              _4: 输出缓冲区的最大长度
     *              返回值: 成功拷贝数据的长度(Byte)
     * @返回值: VIDEO_DECODER_SUCCESS 成功
     *          VIDEO_DECODER_SET_FUNC_FAIL 设置函数失败
     */
    virtual DecoderRetCode SetCopyFrameFunc(
        std::function<uint32_t(uint8_t*, uint8_t*, const PicInfoParams &, uint32_t)> copyFrame) = 0;

    /**
     * @功能描述: 同步接口,提供一份码流数据供解码器解码
     * @参数 [in] buffer 输入码流数据缓存
     * @参数 [in] filledLen 输入码流数据长度(Byte)
     * @返回值: VIDEO_DECODER_SUCCESS 成功
     *          VIDEO_DECODER_DECODE_FAIL 发送失败
     *          VIDEO_DECODER_WRITE_OVERFLOW 输入buffer速度太快
     */
    virtual DecoderRetCode SendStreamData(uint8_t *buffer, uint32_t filledLen) = 0;

    /**
     * @功能描述: 同步接口,获取一帧解码输出, 拷贝数据时需要使用SetCopyFrameFunc提供的CopyFrame函数
     * @参数 [in] buffer 输出码流数据缓存
     * @参数 [in] maxLen 输出缓冲区最大长度(Byte)
     * @参数 [out] filledLen 输出码流数据长度(Byte)
     * @返回值: VIDEO_DECODER_SUCCESS 成功
     *          VIDEO_DECODER_DECODE_FAIL 解码一帧失败
     *          VIDEO_DECODER_READ_UNDERFLOW 请求输出速度太快
     */
    virtual DecoderRetCode RetrieveFrameData(uint8_t *buffer, uint32_t maxLen, uint32_t *filledLen) = 0;

    /**
     * @功能描述: 放弃当前所有的解码buffer
     * @返回值: VIDEO_DECODER_SUCCESS 成功
     *          VIDEO_DECODER_RESET_FAIL FLUSH失败
     */
    virtual DecoderRetCode Flush() = 0;

    /**
     * @功能描述: 启动解码器,成功后可以开始解码流程
     * @返回值: VIDEO_DECODER_SUCCESS 成功
     *          VIDEO_DECODER_START_FAIL 启动解码器失败
     */
    virtual DecoderRetCode StartDecoder() = 0;

    /**
     * @功能描述: 终止解码流程
     * @返回值: VIDEO_DECODER_SUCCESS 成功
     *          VIDEO_DECODER_STOP_FAIL 终止解码流程失败
     */
    virtual DecoderRetCode StopDecoder() = 0;

    /**
     * @功能描述: 销毁解码器,通知解码器释放资源
     */
    virtual void DestroyDecoder() = 0;
};

extern "C" {
/**
 * @功能描述: 获取硬件解码器对象
 * @参数 [out] decoder 硬件解码器对象指针的指针
 * @返回值: VIDEO_DECODER_SUCCESS 成功
 *          VIDEO_DECODER_CREATE_FAIL 失败
 */
DecoderRetCode CreateVideoDecoder(VideoDecoder** decoder);

/**
 * @功能描述: 销毁硬件解码器对象
 * @参数 [in] decoder 硬件解码器对象指针
 * @返回值: VIDEO_DECODER_SUCCESS 成功
 *          VIDEO_DECODER_DESTROY_FAIL失败
 */
DecoderRetCode DestroyVideoDecoder(VideoDecoder* decoder);
}

#endif
