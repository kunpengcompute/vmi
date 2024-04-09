/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2021. All rights reserved.
 * Description:  logging framework
 */

#ifndef LOGGING_H
#define LOGGING_H

#ifdef __cplusplus
#include <cstdint>
#include <chrono>
#include <string>
#else
#include <stdint.h>
#endif

#ifdef __ANDROID__
#include "android/log.h"
#else

#ifdef __cplusplus
#include <cstdio>
#else
#include <stdio.h>
#endif

#include <sys/time.h>
#include <syslog.h>

enum AndroidLogPriority {
    ANDROID_LOG_UNKNOWN = 0, ANDROID_LOG_DEFAULT, ANDROID_LOG_VERBOSE,
    ANDROID_LOG_DEBUG, ANDROID_LOG_INFO, ANDROID_LOG_WARN,
    ANDROID_LOG_ERROR, ANDROID_LOG_FATAL, ANDROID_LOG_SILENT
};

using LogCallbackFunction=void (*)(const int level, const char* tag, const char* log);

#endif

#ifdef DEBUG_PRINTOUT  // defined in Android.mk
#define VMI_DEBUG_LEVEL ANDROID_LOG_DEBUG
#else
#define VMI_DEBUG_LEVEL ANDROID_LOG_INFO
#endif

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief: VmiLogPrint, do not use this function directly,use ERR, INFO, DBG, MEMDUMP instead.
 * @param [in] fileName: code file anem
 * @param [in] lineNumber: code line number
 * @param [in] functionName: code function name
 * @param [in] level: print level
 * @param [in] vmiTag: android log tag
 * @param fmt : as fmt in printf
 * @param ... : variable number of arguments
 */
void VmiLogPrint(int level, const char* vmiTag, const char* fmt, ...) __attribute__((format (printf, 3, 4)));


#ifdef __cplusplus
}
#endif

#ifndef LOG_TAG
#define LOG_TAG "Native"
#endif
#ifdef ERR
#undef ERR
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
#ifdef FATAL
#undef FATAL
#endif

#define DBG(fmt, ...) VmiLogPrint(ANDROID_LOG_DEBUG, LOG_TAG, fmt, ##__VA_ARGS__)
#define INFO(fmt, ...) VmiLogPrint(ANDROID_LOG_INFO, LOG_TAG, fmt, ##__VA_ARGS__)
#define WARN(fmt, ...) VmiLogPrint(ANDROID_LOG_WARN, LOG_TAG, fmt, ##__VA_ARGS__)
#define ERR(fmt, ...) VmiLogPrint(ANDROID_LOG_ERROR, LOG_TAG, fmt, ##__VA_ARGS__)
#define FATAL(fmt, ...) VmiLogPrint(ANDROID_LOG_FATAL, LOG_TAG, fmt, ##__VA_ARGS__)

/**
 * print only once for second you set
 *  this is not a time-precise macro. it only guarantees that no more than on log could be printed out with in a second.
 */
#define LOG_RATE_LIMIT(level, second, fmt, ...)                                                                  \
    do {                                                                                                         \
        auto now = std::chrono::steady_clock::now();                                                             \
        static std::chrono::time_point<std::chrono::steady_clock> lastSecond = now;                              \
        auto diff = now - lastSecond;                                                                            \
        if (now == lastSecond ||                                                                                 \
            std::chrono::duration_cast<std::chrono::seconds>(diff).count() >= second) {                          \
            VmiLogPrint(level, LOG_TAG, "rate limited: " fmt,                                                    \
                        ##__VA_ARGS__);                                                                          \
            lastSecond = now;                                                                                    \
        }                                                                                                        \
    } while (0)

#endif /* LOGGING_H */
