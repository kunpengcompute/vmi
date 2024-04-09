/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2021. All rights reserved.
 * Description: Video Engine Client - Utility for Video Engine
 */

#include "VideoUtil.h"
#include <chrono>
#include "logging.h"

namespace Vmi {
namespace {
constexpr uint64_t DURATION_USEC = 1000000ULL;
} // namespace

/*
 * @brief: deconstruct
 */
VideoUtil::~VideoUtil()
{
    Release();
}

/*
 * @fn GetFps
 * @brief to get the realtime fps
 * @return uint32_t, fps
 */
uint32_t VideoUtil::GetFps() const
{
    uint32_t fps = 0;
    std::lock_guard<std::mutex> lockGuard(m_lock);
    if (!m_frameQueue.IsEmpty()) {
        auto currTime = std::chrono::steady_clock::now();
        int64_t now = std::chrono::duration_cast<std::chrono::microseconds>(currTime.time_since_epoch()).count();
        uint32_t index = 0;
        auto timeStamp = m_frameQueue.GetItemAt(index);
        while (timeStamp <= now && timeStamp > 0) {
            if (now - timeStamp <= DURATION_USEC) {
                fps++;
            }
            // when hits the end
            if (m_frameQueue.GetItemNum() <= index + 1) {
                break;
            }
            index++;
            timeStamp = m_frameQueue.GetItemAt(index);
        }
    }
    return fps;
}

/*
 * @fn SetFps
 * @brief to push latest frame TS back into queue
 * @param[in] latest timeStamp of (type <tt>timestamp_t</tt>)
 */
void VideoUtil::SetTimestamp(int64_t timeStamp)
{
    std::lock_guard<std::mutex> lockGuard(m_lock);
    m_frameQueue.PutItem(timeStamp);
}

/*
 * @fn Release
 * @brief to release VideoUtil resource
 */
void VideoUtil::Release()
{
    m_frameQueue.ClearQueue();
}
}
