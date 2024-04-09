/*
 * 版权所有 (c) 华为技术有限公司 2024-2024
 * 功能说明:对外接口的测试套复合场景测试列表
 */

#ifndef API_TEST_LIST_H
#define API_TEST_LIST_H

#include <cstdint>

enum ApiTestList :uint32_t {
    BASE_TEST = 1u,

    START_STOP = 1u << 1,

    RESOLUTION_CHANGE =  1u << 2,

    KEEP_RECV_DATA = 1u << 3,
    VIDEO_SET_PARAM = 1u << 4,
    AUDIO_SET_PARAM = 1u << 5,
    MIC_INJECT_DATA = 1u << 6,
    TOUCH_INJECT_DATA = 1u << 7,
};

constexpr uint32_t RUNNING_TIME_MS = 7 * 24 * 60 * 1000; // 7x24H

#endif