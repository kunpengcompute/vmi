/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2021. All rights reserved.
 * Description:  logging
 */

#ifndef LOGINFO_H
#define LOGINFO_H

#include "logging.h"

class LogInfo {
public:
    static LogInfo& GetInstance();
#ifndef __ANDROID__
    int GetLogLevel(int level) const;

    void SetCallBack(LogCallbackFunction logCallback);

    bool IsCallBackNull() const;

    void Callback(const int level, const std::string& tag, const std::string& log) const;

#endif /* __ANDROID__  */
    int GetLogLevel() const;

#ifdef __ANDROID__
    void GetLogLevelProp();
#endif

private:
    LogInfo();
    ~LogInfo();
    LogInfo(const LogInfo&) = delete;
    LogInfo& operator=(const LogInfo&) = delete;
    LogInfo(LogInfo &&) = delete;
    LogInfo& operator=(LogInfo &&) = delete;

private:
#ifndef __ANDROID__
    static const int mapSize = 9;
    const int m_logLevelMap[mapSize] = {
        0,                // ANDROID_LOG_UNKNOWN
        0,                // ANDROID_LOG_DEFAULT
        0,                // ANDROID_LOG_VERBOSE
        LOG_DEBUG,        // ANDROID_LOG_DEBUG
        LOG_INFO,         // ANDROID_LOG_INFO
        LOG_WARNING,      // ANDROID_LOG_WARN
        LOG_ERR,          // ANDROID_LOG_ERROR
        LOG_CRIT,         // ANDROID_LOG_FATAL
        0                 // ANDROID_LOG_SILENT
    };
    LogCallbackFunction m_logCallback;
#endif /* __ANDROID__  */

    int m_logLevel = ANDROID_LOG_INFO;
};

#endif