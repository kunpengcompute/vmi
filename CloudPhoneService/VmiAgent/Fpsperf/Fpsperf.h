/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 */

#ifndef FPSPERF_H
#define FPSPERF_H

#include <string>
#include <vector>


class Fpsperf {
public:
    struct Janks {
        int jank;
        int bjank;
    };

    struct FpsData {
        std::vector<std::string> src;
        std::vector<std::int64_t> actPreTime; // actualPresentTime
        std::vector<float> framedata;
        int64_t lFTime; // lastFrameTime, 用于数据去重
        float vsync;  // s
        int fps;
    };

    Fpsperf() = default;
    ~Fpsperf() = default;
    std::string RunCmd(const char *cmd);
    int GetSdkVersion();
    std::string GetTopPkgName(const int sdk);
    std::string GetTopLayer();
    int GetFpsSrc(const std::string &sflayer);
    void GetFps(const std::string &sflayer);
    void CalJank();

    FpsData m_fpsdata{};
    Janks m_janks{};

private:
    std::vector<std::string> Split(const std::string &s, char delimite);
    bool IsNum(const std::string &s);
};


#endif