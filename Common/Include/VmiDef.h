/**
 * 版权所有 (c) 华为技术有限公司 2023-2023
 * 视频流云手机对外接口：参数定义
 */

#ifndef VMI_DEF_H
#define VMI_DEF_H

#include <cstdint>

enum VmiErrCode : int32_t {
    OK = 0,
    ERR_VERSION_MISMATCH = 0x1,                      // 版本不兼容
    ERR_ILLEGAL_INPUT,                               // 输入参数不合法
    ERR_INVALID_CMD,                                 // 不支持的命令
    ERR_INVALID_STATUS,                              // 当前组件状态不支持该操作
    ERR_INVALID_MODULE,                              // 不支持的组件类型
    ERR_NOT_ENOUGH_RESOURCE,                         // 资源不足，无法完成操作
    ERR_INTERNAL,                                    // 组件内部错误
    ERR_REPEAT_INITIALIZATION,                       // 重复初始化
    ERR_MODULE_NOT_INIT,                             // 模块未初始化
    ERR_MODULE_NOT_START,                            // 模块未启动
    ERR_MODULE_REPEAT_START,                         // 模块重复启动
    ERR_MODULE_REPEAT_STOP,                          // 模块重复停止
    ERR_COMMON_MAX = 0x100,
    ERR_VIDEO_UNSUPPORT_OPERATION = 0x101,
    ERR_VIDEO_PARAMS_SETTING = 0x102,
    ERR_VIDEO_MAX = 0x200,
    ERR_AUDIO_MAX = 0x300,
    ERR_MIC_MAX = 0x400,
    ERR_TOUCH_MAX = 0x500
};

/**
 * 定义支持的数据类型
 */
enum VmiDataType : uint8_t {
    DATA_VIDEO = 0,                                  // 码流出流组件
    DATA_AUDIO,                                      // 音频播放组件
    DATA_TOUCH,                                      // 触控和按键组件
    DATA_MIC,                                        // 麦克风组件
    DATA_TYPE_MAX
};

/**
 * 定义各个模块的版本号
 */
enum VmiVideoVerson : uint16_t {
    CUR_VIDEO_VERSION = 6,
};

enum VmiAudioVersion : uint16_t {
    CUR_AUDIO_VERSION = 6,
};

enum VmiTouchVersion : uint16_t {
    CUR_TOUCH_VERSION = 6,
};

enum VmiMicVerson : uint16_t {
    CUR_MIC_VERSION = 6,
};

#define MAKE_VERSION(dataType, version) ((dataType) << 24 | (version))

enum VmiVersion : uint32_t {
    VIDEO_CUR_VERSION = MAKE_VERSION(DATA_VIDEO, CUR_VIDEO_VERSION),
    AUDIO_CUR_VERSION = MAKE_VERSION(DATA_AUDIO, CUR_AUDIO_VERSION),
    TOUCH_CUR_VERSION = MAKE_VERSION(DATA_TOUCH, CUR_TOUCH_VERSION),
    MIC_CUR_VERSION = MAKE_VERSION(DATA_MIC, CUR_MIC_VERSION),
};

/**
 * 定义支持的模块状态
 */
enum VmiModuleStatus : uint32_t {
    MODULE_NOT_INIT = 0,                             // 模块未初始化
    MODULE_INITED,                                   // 模块已初始化
    MODULE_STARTED,                                  // 模块已启动
    MODULE_STOPED,                                   // 模块已停止
};

/**
 * cmd定义
 */
enum VmiCmdType : uint8_t {
    CMD_SET_PARAM = 0,                               // 设置参数类型命令
    CMD_GET_PARAM,                                   // 获取参数类型命令
    CMD_TRANS_DATA,                                  // 传输数据类型命令
};

enum VmiVideoCmdId : uint16_t {
    SET_ENCODER_PARAM = 0,                           // 客户端设置设置编码参数到服务端
    RETURN_VIDEO_DATA,                               // 服务端返回视频数据到客户端
    GET_ENCODER_PARAM,                               // 获取视频模块的编码参数
};

enum VmiAudioCmdId : uint16_t {
    SET_AUDIOPLAY_PARAM = 0,                         // 客户端设置音频播放参数到服务端
    RETURN_AUDIO_PLAY_DATA,                          // 服务端发送音频播放数据到客户端
    SET_CLIENT_VOLUME,                               // 服务端发送扬声器大小到客户端
    GET_AUDIOPLAY_PARAM,                             // 客户端从服务端获取音频播放参数
};

enum VmiMicCmdId : uint16_t {
    SEND_MIC_DATA = 0,                               // 客户端发送麦克风数据到服务端
    RETURN_OPEN_CLIENT_MIC,                          // 服务端通知客户端麦克风启动
    RETURN_CLOSE_CLIENT_MIC,                         // 服务端客户端麦克风关闭
};

enum VmiTouchCmdId : uint16_t {
    SEND_TOUCH_EVENT = 0,                            // 客户端发送触控数据到服务端
    SEND_KEY_EVENT,                                  // 客户端发送按键数据到服务端
};

#define MAKE_CMD(dataType, cmdType, cmdId)  ((dataType) << 24 | (cmdType) << 16 | (cmdId))

enum VmiCmd : uint32_t {
    // 视频模块的cmd定义
    VIDEO_SET_ENCODER_PARAM = MAKE_CMD(DATA_VIDEO, CMD_SET_PARAM, SET_ENCODER_PARAM),      // 参数数据格式参见：EncodeParams
    VIDEO_RETURN_VIDEO_DATA = MAKE_CMD(DATA_VIDEO, CMD_TRANS_DATA, RETURN_VIDEO_DATA),     // 传输数据格式参见：VideoData
    VIDEO_GET_ENCODER_PARAM = MAKE_CMD(DATA_VIDEO, CMD_GET_PARAM, GET_ENCODER_PARAM),      // 参数数据格式参见：EncodeParams
    // 音频模块的cmd定义
    AUDIO_SET_AUDIOPLAY_PARAM = MAKE_CMD(DATA_AUDIO, CMD_SET_PARAM, SET_AUDIOPLAY_PARAM),  // 参数数据格式参见：AudioPlayParams
    AUDIO_GET_AUDIOPLAY_PARAM = MAKE_CMD(DATA_AUDIO, CMD_GET_PARAM, GET_AUDIOPLAY_PARAM),
    AUDIO_RETURN_AUDIO_PLAY_DATA = MAKE_CMD(DATA_AUDIO, CMD_TRANS_DATA, RETURN_AUDIO_PLAY_DATA),  // 传输数据格式参见：AudioData
    AUDIO_SET_CLIENT_VOLUME = MAKE_CMD(DATA_AUDIO, CMD_SET_PARAM, SET_CLIENT_VOLUME),
    // 麦克风模块的cmd定义
    MIC_SEND_MIC_DATA = MAKE_CMD(DATA_MIC, CMD_TRANS_DATA, SEND_MIC_DATA),
    MIC_RETURN_OPEN_CLIENT_MIC = MAKE_CMD(DATA_MIC, CMD_TRANS_DATA, RETURN_OPEN_CLIENT_MIC),
    MIC_RETURN_CLOSE_CLIENT_MIC = MAKE_CMD(DATA_MIC, CMD_TRANS_DATA, RETURN_CLOSE_CLIENT_MIC),
    // 触控模块的cmd定义
    TOUCH_SEND_TOUCH_EVENT = MAKE_CMD(DATA_TOUCH, CMD_TRANS_DATA, SEND_TOUCH_EVENT),    // 传输数据格式参见：VmiTouchInputData
    TOUCH_SEND_KEY_EVENT = MAKE_CMD(DATA_TOUCH, CMD_TRANS_DATA, SEND_KEY_EVENT),        // 传输数据格式参见：VmiKeyInputData
};

/**
 * 通用回调函数，用于发送数据或指令。
*/
using DataCallback = int(*)(VmiDataType module, VmiCmd cmd, uint8_t *data, uint32_t size);

/**
 * 通用配置基类结构体，包含版本号信息。
 * 各组件具体的配置结构体需要将该结构体作为第一个成员
*/
struct VmiConfig {
    uint32_t version;                           // 设置为各个组件的当前的版本号，参见：VmiVersion结构体
} __attribute__((packed));

struct DataTypeConfig {
    bool shouldInit = false;
    uint32_t sendDataOffset = 0;                // 通过回调函数发送数据时，在数据头预留的空间，单位为Byte
} __attribute__((packed));

// 启动视频流引擎配置项，包含要启动的模块以及回调函数发送数据时，在数据头预留的空间
struct VmiConfigEngine {
    DataCallback dataCallback = nullptr;        // 用于发送服务端数据的回调
    DataTypeConfig dataTypeConfig[DATA_TYPE_MAX];
} __attribute__((packed));

/**
 * 视频相关定义
*/
enum VideoFrameType {
    H264,
    YUV,                                        // YV12
    RGB,                                        // RGBA8888，暂不支持
    H265,
    FRAME_TYPE_MAX
};

// 编码模式
enum RCMode : uint32_t  {
    ABR,                                        // 平均码率，暂不支持
    CRF,                                        // 画质优先，暂不支持
    CBR,                                        // 恒定码率
    CAPPED_CRF,                                 // 画质优先，但限制码率，暂不支持
    RC_MODE_MAX
};

// 编码器类型
enum EncoderType : uint32_t {
    CPU,                                        // 软编
    VPU,                                        // 编码卡硬件加速
    GPU,                                        // GPU硬件加速，暂不支持
    ENCODE_TYPE_MAX
};

// 编码级别
enum ProfileType : uint32_t {
    BASELINE,                                   // H264支持
    MAIN,                                       // H264、H265支持
    HIGH                                        // H264支持
};

struct FrameSize {
    uint32_t width = 720;                       // 720: 默认720P
    uint32_t height = 1280;                     // 1280: 默认720P
    uint32_t widthAligned = 720;                // 720: 默认不做对齐(输出码流时不生效)
    uint32_t heightAligned = 1280;              // 1280: 默认不做对齐(输出码流时不生效)
} __attribute__((packed));

struct VideoLatency {
    uint64_t startCaptureTimestamp;             // 抓图开始时间点
    uint64_t endCaptureTimestamp;               // 抓图完成时间点
    uint64_t startEncodeTimestamp;              // 编码开始时间点
    uint64_t endEncodeTimestamp;                // 编码完成时间点
} __attribute__((packed));

struct ExtDataVideo {
    uint64_t timestamp;                          // 帧渲染完成的时间戳
    uint8_t orientation;                        // 当前帧的旋转方向
    FrameSize size;                             // 当前图像大小
    VideoLatency latencyInfo;                   // 抓图和编码时间点
    uint32_t frameRate;                         // 当前屏幕刷新率
} __attribute__((packed));

struct VideoData {
    ExtDataVideo extData;                       // 码流包额外信息
    uint32_t size;
    uint8_t data[0];
} __attribute__((packed));

struct EncodeParams {
    uint32_t bitrate = 3000000;                 // 码率
    uint32_t gopSize = 30;                      // I帧间隔
    ProfileType profile = BASELINE;                 // 编码复杂度
    RCMode rcMode = CBR;                        // 流控模式
    uint32_t forceKeyFrame = 0;                 // 在设置后第N帧强制生成I帧，0表示不生效(目前仅支持0与1的设置)
    bool interpolation = false;                 // 补帧开关
} __attribute__((packed));

// 启动码流服务的初始配置，包含编码器的初始参数
struct VmiConfigVideo : public VmiConfig {
    EncoderType encoderType = CPU;              // 编码器类型
    VideoFrameType videoFrameType = H264;       // 视频输出格式
    FrameSize resolution;                       // 抓图分辨率
    uint32_t density = 320;                     // 屏幕密度
    bool renderOptimize = true;                 // 出流优化，默认开启，暂不支持关闭
    EncodeParams encodeParams;                  // 初始编码参数
} __attribute__((packed));

/**
 * 音频相关定义
*/
enum AudioType : uint32_t {
    OPUS,
    PCM
};

struct ExtDataAudio {
    uint32_t audioType = OPUS;                  // 编码模式：OPUS/PCM
    uint8_t channels = 2;                       // 声道数 1：MONO/2:STEREO(默认)
    uint8_t audioBitDepth = 16;                 // 采样深度 8bit/16bit(默认)
    uint32_t sampleRate = 48000;                // 采样率 48000Hz(默认)
    uint32_t sampleInterval = 10;               // 采样间隔 5ms/10ms(默认)/20ms
    int64_t timestamp;                          // 音频数据时间戳 毫秒，UTC时间
} __attribute__((packed));

struct AudioData {
    ExtDataAudio extData;                       // 音频包额外信息
    uint32_t size;
    uint8_t data[0];
} __attribute__((packed));

struct AudioVolume {
    uint32_t leftVolume = 100;                  // 左声道音量
    uint32_t rightVolume = 100;                 // 右声道音量
} __attribute__((packed));

struct AudioPlayParams {
    uint32_t sampleInterval = 10;               // ms
    uint32_t bitrate = 192000;                  // bps，只有在OPUS格式有效
} __attribute__((packed));

struct VmiConfigAudio : public VmiConfig  {
    AudioType audioType = OPUS;
    AudioPlayParams params;                     // 默认的音频播放参数
} __attribute__((packed));

struct VmiConfigMic : public VmiConfig {
    AudioType audioType = OPUS;
} __attribute__((packed));

/**
 * 触控相关定义
*/
// 触控事件
struct VmiTouchEvent {
    uint8_t pointerID;
    uint8_t action;
    uint16_t x;
    uint16_t y;
    uint16_t pressure;
} __attribute__((packed));

// 触控输入数据
struct VmiTouchInputData {
    uint8_t orientation;                        // 客户端当前旋转方向
    uint16_t uiWidth;                           // 客户端UI分辨率
    uint16_t uiHeight;                          // 客户端UI分辨率
    VmiTouchEvent touchEvent;
} __attribute__((packed));

// 键盘输入数据
struct VmiKeyInputData {
    uint16_t keyCode;
    uint16_t action;
} __attribute__((packed));

struct VmiConfigTouch : public VmiConfig {} __attribute__((packed));
#endif
