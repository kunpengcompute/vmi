/*
 * 版权所有 (c) 华为技术有限公司 2024-2024
 * 功能说明: 时延信息打印模块
 * 会读取两个属性
 * 1.vmi.save.stream为1时，则会保存码流
 * 2.vmi.latency.test为1时，则收到触控数据时会打印日志，收到视频数据是会打印视频详细信息
 */

#define LOG_TAG "LatencyTest"
#include "Latency.h"
#include <sstream>
#include "SystemProperty.h"
#include "logging.h"
namespace Vmi {
void Latency::Init(uint32_t dataOffset)
{
    m_isPrintLatency = (GetPropertyWithDefault("vmi.latency.test", 0) == 1);
    m_isSaveStremFile = (GetPropertyWithDefault("vmi.save.stream", 0) == 1);
    m_dataOffset = dataOffset;
}

std::string Latency::GetCurrentTimestamp()
{
    auto now  = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm     = *std::localtime(&t);

    char buf[32] = {0};
    std::strftime(buf, sizeof(buf), "%Y%m%d__%H%M%S", &tm);
    return std::string(buf);
}

void Latency::Start(VideoFrameType frameType) {
    m_frameCount = 0;
    if (!m_isSaveStremFile) {
        return;
    }
    std::string suffix = "";
    switch (frameType) {
    case VideoFrameType::H264:
        suffix = ".h264";
        break;
    case VideoFrameType::YUV:
        suffix = ".yuv";
        break;
    case VideoFrameType::RGB:
        suffix = ".rgb";
        break;
    case VideoFrameType::H265:
        suffix = ".h265";
        break;
    default:
        suffix = ".unknown";
        break;
    }
    std::string ts   = GetCurrentTimestamp();
    std::string path = "/sdcard/Movies/stream_" + ts + suffix;

    m_file = fopen(path.c_str(), "wb");
    if (m_file == nullptr) {
        ERR("Failed to open %s, errno:%d:%s", path.c_str(), errno, strerror(errno));
        return;
    }
}

void Latency::Stop()
{
    if (!m_isSaveStremFile) {
        return;
    }
    if (m_file == nullptr) {
        return;
    }
    fclose(m_file);
    m_file = nullptr;
}

void Latency::RecvInputEvent()
{
    if (!m_isPrintLatency) {
        return;
    }
    INFO("Recv input event");
}

void Latency::RecvVideoData(VmiCmd cmd, uint8_t *data, uint32_t size)
{
    if (!m_isPrintLatency && !m_isSaveStremFile) {
        return;
    }
    if (cmd != VmiCmd::VIDEO_RETURN_VIDEO_DATA || data == nullptr || size < sizeof(VideoData)) {
        LOG_RATE_LIMIT(ANDROID_LOG_ERROR, 1, "Failed to parse video data, cmd:%u, size:%u, offset:%u",
                       cmd, size, m_dataOffset);
        return;
    }
    VideoData* videoData = reinterpret_cast<VideoData*>(data + m_dataOffset);
    uint32_t videoSize = size - sizeof(VideoData);
    if (m_isPrintLatency) {
        std::stringstream ss;
        ss << TimestampToStr(videoData->extData.latencyInfo.startCaptureTimestamp) << ", " << TimestampToStr(videoData->extData.latencyInfo.endCaptureTimestamp) << ", " <<
            TimestampToStr(videoData->extData.latencyInfo.startEncodeTimestamp) << ", " << TimestampToStr(videoData->extData.latencyInfo.endEncodeTimestamp);
        INFO("Send video data, frameCount:%u, frameSize:%u, Time:%s", m_frameCount, videoSize, ss.str().c_str());
        m_frameCount++;
    }
    if (m_isSaveStremFile) {
        if (m_file == nullptr) {
            return;
        }
        size_t ret = fwrite(videoData->data, videoSize, 1, m_file);
        if (ret != 1) {
            ERR("Failed to write video:%u to file, ret:%zu", videoSize, ret);
            return;
        }
    }
}

std::string Latency::TimestampToStr(uint64_t time)
{
    auto timePoint = std::chrono::system_clock::time_point(std::chrono::milliseconds(time));
    std::time_t timeT = std::chrono::system_clock::to_time_t(timePoint);
    std::tm* timeInfo = std::localtime(&timeT);
    char buffer[80] = {0};
    std::strftime(buffer, sizeof(buffer), "%H:%M:%S", timeInfo);
    return std::string(buffer) + "." + std::to_string(time % 1000);
}
}