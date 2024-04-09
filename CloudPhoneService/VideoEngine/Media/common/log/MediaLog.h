/*
 * 版权所有 (c) 华为技术有限公司 2021-2021
 * 功能说明: 提供媒体日志记录功能
 */
#ifndef MEDIA_LOG_H
#define MEDIA_LOG_H

#include "MediaLogDefs.h"

#ifndef LOG_TAG
#define LOG_TAG "Media"
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

/**
 * @功能描述: 设置日志回调函数，注意该接口不支持多线程调用
 * @参数 [in] logCallback: 日志回调函数
 */
void SetMediaLogCallback(MediaLogCallbackFunc logCallback);

/**
 * @功能描述: 日志打印公共实现接口，供宏函数DBG/INFO/WARN/ERR/FATAL调用
 * @参数 [in] level: 日志级别
 * @参数 [in] tag: 日志标签
 * @参数 [in] fmt: 格式化输出，与printf保持一致
 * @参数 [in] ...: 附加参数
 */
void MediaLogPrint(int level, const char *tag, const char *fmt, ...) __attribute__((format (printf, 3, 4)));

#define DBG(fmt, ...) MediaLogPrint(LOG_LEVEL_DEBUG, LOG_TAG, fmt, ##__VA_ARGS__)
#define INFO(fmt, ...) MediaLogPrint(LOG_LEVEL_INFO, LOG_TAG, fmt, ##__VA_ARGS__)
#define WARN(fmt, ...) MediaLogPrint(LOG_LEVEL_WARN, LOG_TAG, fmt, ##__VA_ARGS__)
#define ERR(fmt, ...) MediaLogPrint(LOG_LEVEL_ERROR, LOG_TAG, fmt, ##__VA_ARGS__)
#define FATAL(fmt, ...) MediaLogPrint(LOG_LEVEL_FATAL, LOG_TAG, fmt, ##__VA_ARGS__)

#endif  // MEDIA_LOG_H
