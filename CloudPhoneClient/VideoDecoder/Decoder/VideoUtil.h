/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2021. All rights reserved.
 * Description: Video Engine Client - Utility for Video Engine
 */

#ifndef VIDEOUTIL_H
#define VIDEOUTIL_H

#include <cstdint>
#include "ItemQueue.h"

namespace Vmi {
class VideoUtil {
public:
    /*
     * @brief: construct
     */
    VideoUtil() = default;

    /*
     * @brief: deconstruct
     */
    ~VideoUtil();

    /*
     * @fn SetFps
     * @brief to push latest frame TS back into queue
     * @param[in] latest timestamp of (type <tt>timestamp_t</tt>)
     */
    void SetTimestamp(int64_t timeStamp);

    /*
     * @fn GetFps
     * @brief to get the realtime fps
     * @return uint32_t, fps
     */
    uint32_t GetFps() const;

    /*
     * @fn Release
     * @brief to release VideoUtil resource
     */
    void Release();

private:
    mutable std::mutex m_lock = {};
    ItemQueue<int64_t> m_frameQueue {};
};
}

#endif // VIDEOUTIL_H
