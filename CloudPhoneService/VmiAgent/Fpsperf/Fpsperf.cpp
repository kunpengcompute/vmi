/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * Description: Fpsperf
 */

#define LOG_TAG "Fpsperf"
#include "Fpsperf.h"
#include <iostream>
#include <cstdio>
#include <regex>
#include <sstream>
#include <cstring>
#include "logging.h"
#include "SystemProperty.h"

namespace {
    constexpr int BUFF_LEN = 1024;

    constexpr int NEW_TOPCMD_SDKVER = 33;
    constexpr int TOPPKGNAME_NUM = 4;
    constexpr int LESS_LEN = 2;
    constexpr int AVGFT_SIZE = 3;
    constexpr int LESS_CALJANK_SIZE = 4;

    constexpr float TWO_MOVIE_FRAMETIME = 1000.0 / 24 * 2;    // ms
    constexpr float THREE_MOVIE_FRAMETIME = 1000.0 / 24 * 3;  // ms

    constexpr float NS_TO_MS = 1e6;
    constexpr float NS_TO_S = 1e9;
    constexpr int64_t PENDING_FENCE_TIMESTAMP = 2 ^ 63 - 1;
}

std::string Fpsperf::RunCmd(const char *cmd)
{
    char msgBuff[BUFF_LEN] = {0};
    std::string ret{};
    FILE *fp = popen(cmd, "r");
    if (fp == nullptr) {
        ERR("Failed to execute command: %s", cmd);
        return ret;
    }
    while (fgets(msgBuff, BUFF_LEN, fp) != nullptr) {
        ret.append(msgBuff);
    }
    if (pclose(fp) == -1) {
        ERR("err in pclose");
    }
    return ret;
}

int Fpsperf::GetSdkVersion()
{
    int sdk = Vmi::GetPropertyWithDefault("ro.build.version.sdk", -1);
    if (sdk == -1) {
        ERR("Failed to get sdk version");
    }
    return sdk;
}

std::string Fpsperf::GetTopPkgName(const int sdk)
{
    std::string topActivitySrc = "";
    if (sdk >= NEW_TOPCMD_SDKVER) {
        topActivitySrc = RunCmd("dumpsys activity activities |grep topResumedActivity");
    } else {
        topActivitySrc = RunCmd("dumpsys activity activities |grep mResumedActivity");
    }

    if (std::regex_match(topActivitySrc, std::regex(".*ActivityRecord\\{.*\\}\n"))) {
        std::regex wordsRegex("\\s+|\\/");
        std::vector<std::string> words{ std::sregex_token_iterator(topActivitySrc.begin(), topActivitySrc.end(),
            wordsRegex, -1),
            std::sregex_token_iterator() };
        if (words.size() <= TOPPKGNAME_NUM) {
            ERR("Failed to get top package name");
            return "";
        }
        std::string topPkgName = words[TOPPKGNAME_NUM];
        return topPkgName;
    }
    ERR("Failed to get top package name");
    return "";
};

std::string Fpsperf::GetTopLayer(const std::string &topPkgName)
{
    std::vector<std::string> layerlist =
        Split(RunCmd(("dumpsys SurfaceFlinger --list |grep " + topPkgName).c_str()), '\n');
    if (layerlist.empty()) {
        ERR("Failed to get topLayer");
        return "";
    }
    std::string topLayer = layerlist.back(); // 无 SurfaceView 项时最后一行为所需行
    for (int i = layerlist.size() - 1; i >= 0 ; --i) {
        if (layerlist[i].find("SurfaceView") != std::string::npos) {
            topLayer = layerlist[i];
            break;
        }
    }
    return topLayer;
};

int Fpsperf::GetFpsSrc(const std::string &sflayer)
{
    m_fpsdata.src = Split(RunCmd(("dumpsys SurfaceFlinger --latency '" + sflayer + "'").c_str()), '\n');
    if (m_fpsdata.src.empty()) {
        ERR("Failed to get Fps Source");
        return -1;
    }
    m_fpsdata.src.pop_back(); // 删除最后一行（空白行）
    std::regex correct_num("\\d+\\s+\\d+\\s+\\d+");
    std::regex zero_num("0+\\s+0+\\s+0");

    // 数据无效则不进行后续计算
    if (m_fpsdata.src.size() < LESS_LEN || std::regex_match(m_fpsdata.src[m_fpsdata.src.size() - LESS_LEN], zero_num)) {
        ERR("Unexpected m_fpsdata.src");
        return -1;
    }

    // 删除回显首末行中可能涉及的"----TIME----" 行等非数字信息
    if (!IsNum(m_fpsdata.src.front())) {
        m_fpsdata.src.erase(m_fpsdata.src.begin());
    }
    if (!IsNum(m_fpsdata.src.back())) {
        m_fpsdata.src.pop_back();
    }

    // vsync 为处理后 src 的第一行
    m_fpsdata.vsync = std::stoll(m_fpsdata.src[0]) / NS_TO_S;
    m_fpsdata.src.erase(m_fpsdata.src.begin());

    // 去除全0无效行
    std::string zero = "0\t0\t0";
    auto itZ = find(m_fpsdata.src.rbegin(), m_fpsdata.src.rend(), zero);
    if (itZ != m_fpsdata.src.rend()) {
        m_fpsdata.src.erase(m_fpsdata.src.begin(), itZ.base());
    }
    return 0;
}

void Fpsperf::GetFps(const std::string &sflayer)
{
    int err = GetFpsSrc(sflayer);
    if (err != 0) {
        return;
    }

    // 清空actPreTime后更新
    m_fpsdata.actPreTime = {};
    for (const auto &s : m_fpsdata.src) {
        std::vector<std::string> tokens = Split(s, '\t');
        // 排除 vsync 行, "\\d \\d \\d" -> [\\d]
        if (tokens.size() > 1) {
            m_fpsdata.actPreTime.push_back(std::stoll(tokens[1])); // dump信息包含三个元素，第二个为所需上屏时间戳
        }
    }

    // 去重，避免 Jank 重复计数
    auto itlFT = find(m_fpsdata.actPreTime.begin(), m_fpsdata.actPreTime.end(), m_fpsdata.lFTime);
    bool findlFT = (itlFT != m_fpsdata.actPreTime.end());
    if (!findlFT) {
        INFO("cannot find same actPreTime as last");
    } else {
        m_fpsdata.actPreTime.erase(m_fpsdata.actPreTime.begin(), itlFT);
    }

    // 去除大数
    if (!m_fpsdata.actPreTime.empty() && (m_fpsdata.actPreTime.back() == PENDING_FENCE_TIMESTAMP)) {
        m_fpsdata.actPreTime.pop_back();
    }

    // 计算fps及frametime
    m_fpsdata.lFTime = m_fpsdata.actPreTime.back();

    m_fpsdata.fps =
        (m_fpsdata.actPreTime.size() - 1) / ((m_fpsdata.actPreTime.back() - m_fpsdata.actPreTime[0]) / NS_TO_S);

    if (findlFT && m_fpsdata.framedata.size() > AVGFT_SIZE) {
        m_fpsdata.framedata.erase(m_fpsdata.framedata.begin(), m_fpsdata.framedata.end() - AVGFT_SIZE);
    } else {
        INFO("cannot find lastFrameTime or size <= 3, clear framedata");
        m_fpsdata.framedata = {};
    }

    for (unsigned int i = 0; i < m_fpsdata.actPreTime.size() - 1; ++i) {
        m_fpsdata.framedata.push_back((m_fpsdata.actPreTime[i + 1] - m_fpsdata.actPreTime[i]) / NS_TO_MS); // ms
    }

    if (findlFT) {
        m_fpsdata.actPreTime.erase(m_fpsdata.actPreTime.begin());
    }
};

void Fpsperf::CalJank()
{
    // framedata 不足四个时无法计算 Jank
    if (m_fpsdata.framedata.size() < LESS_CALJANK_SIZE) {
        INFO("framedata size less than 4, cannot cal Jank");
        return;
    }

    std::vector<float> last3Frame{};
    float totalFrameTime = 0;
    float avgFrameTime = 0;

    // last3Frame 前三帧
    for (unsigned int i = 0; i < LESS_CALJANK_SIZE - 1; ++i) {
        last3Frame.push_back(m_fpsdata.framedata[i]);
        totalFrameTime += m_fpsdata.framedata[i];
    }

    // 判断部分
    for (unsigned int i = AVGFT_SIZE; i < m_fpsdata.framedata.size(); ++i) {
        avgFrameTime = totalFrameTime / AVGFT_SIZE;

        if (m_fpsdata.framedata[i] > avgFrameTime * 2 && m_fpsdata.framedata[i] > TWO_MOVIE_FRAMETIME) {
            m_janks.jank += 1;
        }
        if (m_fpsdata.framedata[i] > avgFrameTime * 2 && m_fpsdata.framedata[i] > THREE_MOVIE_FRAMETIME) {
            m_janks.bjank += 1;
        }

        totalFrameTime = totalFrameTime - last3Frame[0] + m_fpsdata.framedata[i];
        last3Frame.erase(last3Frame.begin());
        last3Frame.push_back(m_fpsdata.framedata[i]);
    }
};

std::vector<std::string> Fpsperf::Split(const std::string &s, char delimiter)
{
    std::regex to_regex(std::string(1, delimiter) + "+");
    return std::vector<std::string>(
        std::sregex_token_iterator(s.begin(), s.end(), to_regex, -1),
        std::sregex_token_iterator()
    );
};

bool Fpsperf::IsNum(const std::string &s)
{
    try {
        std::stoll(s);
        return true;
    } catch (...) {
        return false;
    }
};