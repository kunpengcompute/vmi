/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * Description: Fpsperf
 */

#define LOG_TAG "DeviceWatcher"
#include "DeviceWatcher.h"
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <chrono>
#include "logging.h"
#include <unistd.h>

namespace {
    constexpr int WAIT_TIMES = 1; // 每1s获取一次源数据
}

bool DeviceWatcher::Init()
{
    int sdk = m_fpsWatcher.GetSdkVersion();
    m_topPkgName = m_fpsWatcher.GetTopPkgName(sdk);
    m_topLayerName = m_fpsWatcher.GetTopLayer();

    if (sdk == -1 || m_topPkgName.empty() || m_topLayerName.empty()) {
        ERR("Failed to init DeviceWatcher!");
        return false;
    }

    // 此处layer名称可能较长，因此换行展示
    INFO("Watch Package: %s\nWatch Layer: %s", m_topPkgName.c_str(), m_topLayerName.c_str());
    m_isInitialized = true;
    return true;
}

void DeviceWatcher::RunWatcher(const char mode)
{
    if (!m_isInitialized) {
        return;
    }
    m_times.startTP = std::chrono::system_clock::now();
    m_times.start = std::chrono::system_clock::to_time_t(m_times.startTP);
    m_times.startTM = std::localtime(&m_times.start);
    std::ostringstream oss;
    oss << std::put_time(m_times.startTM, "%Y-%m-%d_%H-%M-%S");

    // 非连接视频流模式，创建用于保存 timestamp 的文件
    if (mode != static_cast<char>(FpsperfMode::VIDEO)) {
        m_fileName.logTimestampName = "/data/local/tmp/" + oss.str() + "_timestamp.txt";
    }

    // 若为终端输出则先给出标题行
    if (mode == static_cast<char>(FpsperfMode::TERMINAL)) {
        std::cout << "fps" << "\t" << "jank" << "\t" << "bjank" << std::endl;
    }

    // 若为本地保存则先建文件
    if (mode == static_cast<char>(FpsperfMode::LOCAL)) {
        m_fileName.logFpsinfoName = "/data/local/tmp/" + oss.str() + "_fpsinfo.csv";
        std::ofstream logFpsinfo(m_fileName.logFpsinfoName);
        if (!logFpsinfo.is_open()) {
            ERR("cannot open fpsinfo csv");
        } else {
            logFpsinfo << "fps,jank,bjank" << std::endl;
            logFpsinfo.close();
        }
    }
}

void DeviceWatcher::GetFpsUsage(const char mode, int &fps, int &jank, int &bjank)
{
    if (!m_isInitialized || m_topLayerName.empty() || m_topPkgName.empty()) {
        return;
    }
    m_fpsWatcher.GetFps(m_topLayerName);

    // 非连接视频流模式，存储 timestamp
    if (mode != static_cast<char>(FpsperfMode::VIDEO)) {
        std::ofstream logTimestamp(m_fileName.logTimestampName, std::ios::app);
        if (!logTimestamp.is_open()) {
            ERR("cannot open timestamp.txt");
            return;
        }

        for (unsigned int i = 0; i < m_fpsWatcher.m_fpsdata.actPreTime.size(); ++i) {
            logTimestamp << m_fpsWatcher.m_fpsdata.actPreTime[i] << std::endl;
        }
        logTimestamp.close();
    }

    m_fpsWatcher.CalJank();

    fps = m_fpsWatcher.m_fpsdata.fps;
    jank = m_fpsWatcher.m_janks.jank;
    bjank = m_fpsWatcher.m_janks.bjank;

    // 终端输出模式
    if (mode == static_cast<char>(FpsperfMode::TERMINAL)) {
        std::cout << fps << "\t" << jank << "\t" << bjank << std::endl;
    }
    
    // 本地保存模式
    if (mode == static_cast<char>(FpsperfMode::LOCAL)) {
        std::ofstream logFpsinfo(m_fileName.logFpsinfoName, std::ios::app);
        if (!logFpsinfo.is_open()) {
            ERR("cannot open fpsinfo csv to write fps_info");
        } else {
            logFpsinfo << fps << "," << jank << "," << bjank << std::endl;
            logFpsinfo.close();
        }
    }
    sleep(WAIT_TIMES);
}