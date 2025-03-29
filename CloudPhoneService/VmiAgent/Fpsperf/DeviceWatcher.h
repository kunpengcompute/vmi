/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 */

#ifndef DEVICEWATCHER_H
#define DEVICEWATCHER_H

#include <string>
#include "Fpsperf.h"

enum class FpsperfMode {
    TERMINAL = '1',
    LOCAL = '2',
    VIDEO = '3'
};

class DeviceWatcher {
public:
    DeviceWatcher() = default;
    ~DeviceWatcher() = default;

    bool Init();
    void RunWatcher(const char mode);
    void GetFpsUsage(const char mode, int &fps, int &jank, int &bjank);

private:
    struct TimeT {
        std::chrono::time_point<std::chrono::system_clock> startTP;
        std::time_t start;
        std::tm* startTM;
    };

    struct FileName {
        std::string logFpsinfoName;
        std::string logTimestampName;
    };

    bool m_isInitialized = false;
    TimeT m_times{};
    FileName m_fileName{};
    std::string m_topLayerName;
    std::string m_topPkgName;
    Fpsperf m_fpsWatcher {};
};

#endif