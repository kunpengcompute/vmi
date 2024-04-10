/*
 * 版权所有 (c) 华为技术有限公司 2021-2021
 * 功能说明: 媒体日志管理模块
 */

#include "MediaLogManager.h"
#include <unordered_map>
#ifdef __ANDROID__
#include <android/log.h>
#else
#include <ctime>
#include <chrono>
#include <unistd.h>
#include <sys/syscall.h>
#ifdef SYS_gettid
#define GetTid() syscall(SYS_gettid)
#else
#define GetTid() (0)
#endif
#endif

#ifdef __ANDROID__
namespace {
    std::unordered_map<int, int> g_logLevelMap = {
        { LOG_LEVEL_DEBUG, ANDROID_LOG_DEBUG },
        { LOG_LEVEL_INFO, ANDROID_LOG_INFO },
        { LOG_LEVEL_WARN, ANDROID_LOG_WARN },
        { LOG_LEVEL_ERROR, ANDROID_LOG_ERROR },
        { LOG_LEVEL_FATAL, ANDROID_LOG_FATAL },
    };
}
#else
namespace {
    std::unordered_map<int, std::string> g_logLevelMap = {
        { LOG_LEVEL_DEBUG, "D" },
        { LOG_LEVEL_INFO, "I" },
        { LOG_LEVEL_WARN, "W" },
        { LOG_LEVEL_ERROR, "E" },
        { LOG_LEVEL_FATAL, "F" },
    };
    constexpr uint32_t TIME_LENGTH = 128;
}
#endif

void DefaultLogCallback(int level, const char *tag, const char *fmt)
{
#ifdef __ANDROID__
    (void) __android_log_write(g_logLevelMap[level], tag, fmt);
#else
    auto nowTimePoint = std::chrono::system_clock::now();
    auto nowTimeMs = std::chrono::time_point_cast<std::chrono::microseconds>(nowTimePoint);
    time_t nowTime = std::chrono::system_clock::to_time_t(nowTimePoint);
    struct tm *time = localtime(&nowTime);
    if (time == nullptr) {
        printf("localtime get failed");
        return;
    }
    char timeStampBuf[TIME_LENGTH] = {0};
    int err = sprintf(timeStampBuf, "[%02d-%02d %02d:%02d:%02d.%03ld]",
        ++(time->tm_mon), time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec,
        nowTimeMs.time_since_epoch().count() % 1000);
    if (err < 0) {
        printf("sprintf failed: %d", err);
        return;
    }
    printf("%s %d %ld %s %s: %s\n",
        timeStampBuf, getpid(), GetTid(), g_logLevelMap[level].c_str(), tag, fmt);
#endif
}

MediaLogManager& MediaLogManager::GetInstance()
{
    static MediaLogManager logManager;
    return logManager;
}

MediaLogManager::MediaLogManager() : m_logCallback(DefaultLogCallback) {}

void MediaLogManager::SetLogCallback(MediaLogCallbackFunc logCallback)
{
    m_logCallback = logCallback;
    m_logLevel = LOG_LEVEL_DEBUG;
}

bool MediaLogManager::IsLogCallbackNull() const
{
    return m_logCallback == nullptr;
}

void MediaLogManager::Callback(int level, const std::string &tag, const std::string &logData) const
{
    if (m_logCallback != nullptr) {
        m_logCallback(level, tag.c_str(), logData.c_str());
    }
}

MediaLogLevel MediaLogManager::GetLogLevel() const
{
    return m_logLevel;
}
