/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2021. All rights reserved.
 * Description:  logging
 */
#include "logging.h"
#ifdef __cplusplus
#include <cstring>
#include <cstdarg>
#else
#include <string.h>
#endif
#include "LogInfo.h"

/**
 * @brief: VmiLogPrint, don't use it directly, use macro's instead
 * @param [in] level: print level
 * @param [in] vmiTag: android log tag, Nullptr allowed
 * @param fmt : as fmt in printf
 * @param ... : variable number of arguments
 */
void VmiLogPrint(const int level, const char* vmiTag, const char* fmt, ...)
{
    if (fmt == nullptr || level < LogInfo::GetInstance().GetLogLevel()) {
        return;
    }
    const size_t logBufSize = 512;

    // this is a nullptr that maybe passed by android system header file log.h
    // don't change this to nullptr
    // LOG_TAG define nullptr in system/core/include/log/log.h
    std::string tag = ((vmiTag == nullptr) ? "VMI_Native" : ("VMI_" + std::string(vmiTag)));

    char szBuff[logBufSize] = {0};
    va_list ap;
    va_start(ap, fmt);
    int ret = vsnprintf(szBuff, logBufSize, fmt, ap);
    va_end(ap);

    if (ret <= 0) {
        return;
    }

    if (static_cast<int64_t>(ret) < static_cast<int64_t>(logBufSize)) {
        szBuff[ret] = '\0';
    }
#ifdef __ANDROID__
    (void)__android_log_write(level, tag.c_str(), szBuff);
#else
    printf("%s\n", szBuff);
#endif
}

