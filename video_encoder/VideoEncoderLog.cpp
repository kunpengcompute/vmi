/*
 * 版权所有 (c) 华为技术有限公司 2021-2021
 * 功能说明: 提供日志记录功能
 */

#include "VideoEncoderLog.h"
#include <cstring>
#include <string>
#include <android/log.h>
#include <cinttypes>
#include <sys/system_properties.h>

namespace {
    std::string PROP_VMI_LOG_LEVEL = "ro.vmi.loglevel";
}

VideoEncoderLog& VideoEncoderLog::GetInstance()
{
    static VideoEncoderLog videoEncoderLog;
    return videoEncoderLog;
}

VideoEncoderLog::VideoEncoderLog()
{
    GetLogLevelProp();
}

void VideoEncoderLog::GetLogLevelProp()
{
    char prop[PROP_VALUE_MAX] = {'\0'};
    int len = __system_property_get(PROP_VMI_LOG_LEVEL.c_str(), prop);
    if (len == 0) {
        __android_log_print(ANDROID_LOG_ERROR, "VMI_Logging", "get system property[%s] failed",
            PROP_VMI_LOG_LEVEL.c_str());
        return;
    }
    char *end = nullptr;
    intmax_t result = strtoimax(prop, &end, 0);
    if (prop == end || (result < ANDROID_LOG_DEFAULT || result > ANDROID_LOG_FATAL)) {
        __android_log_print(ANDROID_LOG_ERROR, "VMI_Logging", "property[%s]'s value[%s] is invalid",
            PROP_VMI_LOG_LEVEL.c_str(), prop);
        return;
    }
    m_logLevel = result;
}

int VideoEncoderLog::GetLogLevel() const
{
    return m_logLevel;
}

void VmiLogPrint(int level, const char *vmiTag, const char *fmt, ...)
{
    if (level < VideoEncoderLog::GetInstance().GetLogLevel() || level > ANDROID_LOG_SILENT || fmt == nullptr) {
        return;
    }
    std::string fullTag = ((vmiTag == nullptr) ? "VMI_Native" : ("VMI_" + std::string(vmiTag)));

    constexpr int logBufSize = 512;
    char szBuff[logBufSize] = {0};
    va_list ap;
    va_start(ap, fmt);
    int ret = vsnprintf(szBuff, logBufSize - 1, fmt, ap);
    va_end(ap);

    if (ret <= 0) {
        return;
    }

    if (static_cast<int64_t>(ret) < static_cast<int64_t>(logBufSize)) {
        szBuff[ret] = '\0';
    }

    (void) __android_log_write(level, fullTag.c_str(), szBuff);
}
