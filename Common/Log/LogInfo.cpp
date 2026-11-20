/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2026. All rights reserved.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */

/*
 * Description:  logging
 */

#include "LogInfo.h"
#include <cinttypes>
#ifdef __ANDROID__
#include <sys/system_properties.h>
#endif

LogInfo::LogInfo()
#ifndef __ANDROID__
    :m_logCallback(nullptr)
#endif
{
#ifdef __ANDROID__
    GetLogLevelProp();
#endif
}

LogInfo::~LogInfo()
{
}

#ifdef __ANDROID__
void LogInfo::GetLogLevelProp()
{
    char propValue[PROP_VALUE_MAX] = { 0 };
    int len = __system_property_get("ro.vmi.loglevel", propValue);
    if (len <= 0) {
        return;
    }
    std::string propStr(propValue);
    int logLevel = strtoimax(propStr.c_str(), nullptr, 0);
    if (logLevel > ANDROID_LOG_FATAL || logLevel < ANDROID_LOG_DEFAULT) {
        return;
    }
    m_logLevel = logLevel;
}
#endif

LogInfo& LogInfo::GetInstance()
{
    static LogInfo logInfoInstance;
    return logInfoInstance;
}

#ifndef __ANDROID__
int LogInfo::GetLogLevel(int level) const
{
    if (level >= static_cast<int>(sizeof(m_logLevelMap) / sizeof(m_logLevelMap[0]))) {
        return 0;
    }
    return m_logLevelMap[level];
}

void LogInfo::SetCallBack(LogCallbackFunction logCallback)
{
    m_logCallback = logCallback;
}

bool LogInfo::IsCallBackNull() const
{
    return m_logCallback == nullptr;
}

void LogInfo::Callback(const int level, const std::string& tag, const std::string& log) const
{
    if (m_logCallback != nullptr) {
        m_logCallback(level, tag.c_str(), log.c_str());
    }
}
#endif

int LogInfo::GetLogLevel() const
{
    return m_logLevel;
}