/*
 * 版权所有 (c) 华为技术有限公司 2021-2021
 * 功能说明: 提供日志记录功能
 */
#ifndef VIDEO_ENCODER_LOG_H
#define VIDEO_ENCODER_LOG_H

#include <android/log.h>

#ifndef LOG_TAG
#define LOG_TAG "Native"
#endif
#ifdef DBG
#undef DBG
#endif
#ifdef INFO
#undef INFO
#endif
#ifdef WARN
#undef WARN
#endif
#ifdef ERR
#undef ERR
#endif
#ifdef FATAL
#undef FATAL
#endif

class VideoEncoderLog {
public:
    /**
     * @功能描述: 获取VideoEncoderLog单例对象
     * @返回值: 返回值VideoEncoderLog单例对象引用
     */
    static VideoEncoderLog& GetInstance();

    /**
     * @功能描述: 获取日志级别
     * @返回值: 日志级别
     */
    int GetLogLevel() const;

private:
    VideoEncoderLog();
    ~VideoEncoderLog() = default;
    VideoEncoderLog(const VideoEncoderLog&) = delete;
    VideoEncoderLog& operator=(const VideoEncoderLog&) = delete;
    VideoEncoderLog(VideoEncoderLog &&) = delete;
    VideoEncoderLog& operator=(VideoEncoderLog &&) = delete;

    /**
     * @功能描述: 获取日志级别，通过读属性"ro.vmi.loglevel"获取
     */
    void GetLogLevelProp();

    int m_logLevel = ANDROID_LOG_INFO;
};

/**
 * @功能描述: 日志打印公共实现接口，供宏函数DBG/INFO/WARN/ERR/FATAL调用
 * @参数 [in] level: 日志级别
 * @参数 [in] vmiTag: 日志标签
 * @参数 [in] fmt: 格式化输出，与printf保持一致
 * @参数 [in] ...: 附加参数
 */
void VmiLogPrint(int level, const char *vmiTag, const char *fmt, ...) __attribute__((format (printf, 3, 4)));

#define DBG(fmt, ...) VmiLogPrint(ANDROID_LOG_DEBUG, LOG_TAG, fmt, ##__VA_ARGS__)
#define INFO(fmt, ...) VmiLogPrint(ANDROID_LOG_INFO, LOG_TAG, fmt, ##__VA_ARGS__)
#define WARN(fmt, ...) VmiLogPrint(ANDROID_LOG_WARN, LOG_TAG, fmt, ##__VA_ARGS__)
#define ERR(fmt, ...) VmiLogPrint(ANDROID_LOG_ERROR, LOG_TAG, fmt, ##__VA_ARGS__)
#define FATAL(fmt, ...) VmiLogPrint(ANDROID_LOG_FATAL, LOG_TAG, fmt, ##__VA_ARGS__)                                                                                      \

#endif  // VIDEO_ENCODER_LOG_H
