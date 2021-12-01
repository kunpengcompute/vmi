/*
 * 功能说明: 媒体日志通用类型定义
 */
#ifndef MEDIA_LOG_DEFS_H
#define MEDIA_LOG_DEFS_H

enum MediaLogLevel {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL
};

using MediaLogCallbackFunc = void (*)(int level, const char *tag, const char *fmt);

#endif  // MEDIA_LOG_DEFS_H
