/*
 * 版权所有 (c) 华为技术有限公司 2017-2022
 * 功能说明：视频流引擎客户端接口实现函数。提供引擎启动、停止、接收和发送数据等函数接口。
 */
#ifndef CLOUDPHONE_CONTROLLER_H
#define CLOUDPHONE_CONTROLLER_H

#include <thread>
#include <mutex>
#include <atomic>

#include "DecodeController.h"
#include "VideoUtil.h"
#include "Parser.h"

namespace Vmi {
enum class CloudPhoneState : uint32_t {
    CLOUD_PHONE_RUNNING = 0,
    CLOUD_PHONE_DESTROYING = 1,
    CLOUD_PHONE_EXITED = 2,
};

class CloudPhoneController {
public:
    static CloudPhoneController &GetInstance();
    int Start(uint64_t surface);
    void Stop();
    uint32_t GetRecvFrameRate() const;
    uint32_t GetDecodeFrameRate();

private:
    CloudPhoneController() = default;
    ~CloudPhoneController() = default;
    CloudPhoneController(const CloudPhoneController& control) = delete;
    CloudPhoneController& operator=(const CloudPhoneController& control) = delete;
    CloudPhoneController(const CloudPhoneController&& control) = delete;
    CloudPhoneController& operator=(const CloudPhoneController&& control) = delete;

    bool SetupVideoEngine();
    void DecodeTask();

    /*
     * @功能描述：Process Packet
     */
    bool StripPacket(std::pair<uint8_t *, uint32_t> &packetPair);
    bool HandlePacket(std::pair<uint8_t *, uint32_t> &packetPair, uint8_t rotation);

    bool ProcessResolution(std::pair<uint8_t *, uint32_t> &packetPair);
    bool SwitchOrientation(uint8_t rotation);
    bool ProcessVideoInfo(std::pair<uint8_t *, uint32_t> &packetPair, uint8_t rotation);
    bool ProcessVideoInfo265(std::pair<uint8_t *, uint32_t> &packetPair, uint8_t rotation);

    bool HandleDecoderType(const std::pair<uint8_t *, uint32_t>& packetPair);
    DecoderType ParseDecTypeFromFirstFrame(const std::pair<uint8_t *, uint32_t>& packetPair);

    /*
     * @功能描述：设置SDK状态
     * @参数 [in] state：参见JNIState枚举定义
     */
    void SetState(CloudPhoneState state);

    /*
     * @功能描述：获取SDK当前状态
     * @返回值：参见JNIState枚举定义
     */
    CloudPhoneState GetState();

    std::mutex m_lock {};
    std::unique_ptr<std::thread> m_decodeThread {nullptr};

    std::atomic<enum CloudPhoneState> m_state {CloudPhoneState::CLOUD_PHONE_EXITED};
    Parser m_parser {};
    DecoderType m_decoderType = DecoderType::DEC_TYPE_H264;
    bool m_isFirstFrame = true;

    uint64_t m_surface {0};
    uint8_t m_orientation {0};
    bool m_isSimulator {false};

    // 统计项
    VideoUtil m_videoUtil {};
};
}

using CloudPhoneController = Vmi::CloudPhoneController;

#endif  // CLOUDPHONE_CONTROLLER_H
