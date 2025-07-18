/*
 * 版权所有 (c) 华为技术有限公司 2024-2024
 * 功能说明: 时延信息打印模块
 * 会读取两个属性
 * 1.vmi.save.stream为1时，则会保存码流
 * 2.vmi.latency.test为1时，则收到触控数据时会打印日志，收到视频数据是会打印视频详细时间戳信息
 */

#ifndef LATENCY_H
#define LATENCY_H
#include <stdio.h>
#include <string>
#include "VmiApi.h"
namespace Vmi {
class Latency {
public:
    void Init(uint32_t dataOffset);
    void Start(VideoFrameType frameType);
    void Stop();
    void RecvInputEvent();
    void RecvVideoData(VmiCmd cmd, uint8_t *data, uint32_t size);
private:
    std::string TimestampToStr(uint64_t time);
    std::string GetCurrentTimestamp();
private:
    bool m_isPrintLatency = false;
    bool m_isSaveStremFile = false;
    uint32_t m_dataOffset = 0;
    uint32_t m_frameCount = 0; // 输出的frameCount序号从0开始，即第一帧序号为0，保持和Elecard StreamEye帧序号一致
    FILE* m_file = nullptr;
};
}
#endif