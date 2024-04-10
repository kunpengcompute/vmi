/*
 * 版权所有 (c) 华为技术有限公司 2017-2023
 * 功能说明：视频流引擎客户端接口实现函数。提供引擎启动、停止、接收和发送数据等函数接口。
 */
#define LOG_TAG "CloudPhoneController"
#include "CloudPhoneController.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <chrono>
#include <cstring>
#include "logging.h"
#include "PacketManager/PacketManager.h"
#include "PacketHandle/PacketHandle.h"
#include "SmartPtrMacro.h"
#include "VideoEngineClient.h"
#include "SystemProperty.h"
#include "EngineEventHandler.h"
#include "VmiDef.h"

using namespace Vmi;

namespace {

enum class CloudPhoneDelayTime : uint32_t {
    TIME_1_MS = 1000,
    TIME_2_MS = 2000,
    TIME_10_MS = 10000,
    TIME_100_MS = 100000,
    TIME_1000_MS = 1000000
};

/* FrameTimeStampParamInPacket */
struct FrameTimeStamp {
    uint64_t t3;
    uint64_t t8;
    uint64_t t9;
    uint64_t t10;
    uint64_t t11;
    uint64_t t12;
    uint64_t t13;
};

std::unordered_map<uint8_t, int32_t> g_orientationMap {
    {0, 0},
    {1, 270},
    {2, 180},
    {3, 90}
};
}

CloudPhoneController &CloudPhoneController::GetInstance()
{
    static CloudPhoneController instance;
    return instance;
}

bool CloudPhoneController::SetupVideoEngine()
{
    INFO("SetupVideoEngine");
    DecodeController &decodeController = DecodeController::GetInstance();
    uint32_t ret = decodeController.Init(m_decoderType, m_surface, g_orientationMap[m_orientation]);
    if (ret != VMI_SUCCESS) {
        ERR("SetupVideoEngine VideoEngine init decoder fail");
        return false;
    }
    ret = decodeController.Start();
    if (ret != VMI_VIDEO_ENGINE_CLIENT_SUCCESS) {
        ERR("SetupVideoEngine VideoEngine start decoder fail");
        decodeController.Destroy();
        return false;
    }
    return true;
}

/*
 * @功能描述：处理视频流数据包
 * @返回值：true代表成功，false代表失败
 */
bool CloudPhoneController::StripPacket(std::pair<uint8_t *, uint32_t> &packetPair)
{
    uint8_t *buf = packetPair.first;
    uint32_t length = packetPair.second;
    constexpr uint32_t cmdLen = sizeof(VmiCmd);
    constexpr uint32_t extBufLen = sizeof(ExtDataVideo);
    constexpr uint32_t dataBufLen = sizeof(uint32_t);
    constexpr uint32_t headerLen = cmdLen + extBufLen + dataBufLen;
    if (length < headerLen) {
        ERR("Input packet length too short, length = %u", length);
        return false;
    }

    // Deal with ext buffer
    VmiCmd cmd;
    memcpy(&cmd, buf, cmdLen);
    if (cmd != VmiCmd::VIDEO_RETURN_VIDEO_DATA) {
        ERR("Input packet cmd error, value: %u", cmd);
        return false;
    }

    ExtDataVideo extBuf = {};
    memcpy(&extBuf, buf + cmdLen, extBufLen);
    uint32_t dataSize = 0;
    memcpy(&dataSize, buf + cmdLen + extBufLen, dataBufLen);
    if (dataSize != length - headerLen) {
        ERR("Input packet length error, length = %u, data size = %u", length, length - headerLen);
        return false;
    }
    std::pair<uint8_t *, uint32_t> subPacketPair = std::make_pair(buf + headerLen, length - headerLen);
    if (m_isFirstFrame && !HandleDecoderType(subPacketPair)) {
        return false;
    }
    m_isFirstFrame = false;
    return HandlePacket(subPacketPair, extBuf.orientation);
}

bool CloudPhoneController::HandleDecoderType(const std::pair<uint8_t *, uint32_t>& packetPair)
{
    m_decoderType = ParseDecTypeFromFirstFrame(packetPair);
    if (m_decoderType == DecoderType::DEC_TYPE_UNKNOWN) {
        ERR("Decoder type is unknown");
        return false;
    } else if (m_decoderType == DecoderType::DEC_TYPE_H264) {
        return true;
    }
    DecodeController &decodeController = DecodeController::GetInstance();
    decodeController.Destroy();
    if (!SetupVideoEngine()) {
        ERR("HandleDecoderType setup VideoEngine fail");
        return false;
    }
    return true;
}

DecoderType CloudPhoneController::ParseDecTypeFromFirstFrame(const std::pair<uint8_t *, uint32_t>& packetPair)
{
    INFO("Start parse decoder type");
    constexpr uint32_t maxStartCodeLen = 4;
    constexpr uint32_t bitsOfByte = 8;
    constexpr uint32_t h264TypeMask = 0x1F;
    constexpr uint32_t h265TypeMask = 0x7E;
    constexpr uint32_t h264NalSps = 7;
    constexpr uint32_t h265NalVps = 32;
    constexpr uint32_t h265NalAud = 35;

    if (packetPair.second < sizeof(uint32_t) + maxStartCodeLen + 1) {
        ERR("Failed to parse decode type, invalid data size(%u)", packetPair.second);
        return DecoderType::DEC_TYPE_UNKNOWN;
    }

    uint8_t *firstFrameData = packetPair.first;
    uint32_t code = 0xffffffff;
    for (uint32_t i = 0; i < maxStartCodeLen + 1; ++i) {
        code = (code << bitsOfByte) + firstFrameData[i];
        DBG("Parse code = %#x", code);
        if ((code & 0xffffff00) == 0x100) {  // 条件满足时code数据为起始码+NAL类型，即0x000001或0x00000001+type(8bits)
            uint32_t typeH264 = code & h264TypeMask;
            uint32_t typeH265 = (code & h265TypeMask) >> 1;
            if (typeH264 == h264NalSps) {
                INFO("Parse decoder type is DEC_TYPE_H264");
                return DecoderType::DEC_TYPE_H264;
            } else if (typeH265 == h265NalVps || typeH265 == h265NalAud) {
                INFO("Parse decoder type is DEC_TYPE_H265");
                return DecoderType::DEC_TYPE_H265;
            } else {
                INFO("Parse decoder type is DEC_TYPE_UNKNOWN");
                return DecoderType::DEC_TYPE_UNKNOWN;
            }
        }
    }
    return DecoderType::DEC_TYPE_UNKNOWN;
}

void CloudPhoneController::DecodeTask()
{
    INFO("Create decode mainLoop taskentry ...");
    int fuse = static_cast<int>(CloudPhoneDelayTime::TIME_1000_MS);
    while (CloudPhoneController::GetInstance().GetState() == CloudPhoneState::CLOUD_PHONE_EXITED && fuse != 0) {
        fuse -= static_cast<int>(CloudPhoneDelayTime::TIME_100_MS);
        usleep(static_cast<int>(CloudPhoneDelayTime::TIME_100_MS));
        INFO("DecodeTask waiting flag CLOUD_PHONE_RUNNING...");
    }
    if (fuse == 0) {
        ERR("DecodeTask waiting for CLOUD_PHONE_RUNNING Timeout!!!");
        return;
    }
    while (CloudPhoneController::GetInstance().GetState() == CloudPhoneState::CLOUD_PHONE_RUNNING) {
        std::pair<uint8_t *, uint32_t> packetPair =
            PacketManager::GetInstance().GetNextPkt(VMIMsgType::VIDEO_RR2);
        if (packetPair.first != nullptr && packetPair.second > 0) {
            auto currTime = std::chrono::steady_clock::now();
            int64_t nowTimeStamp =
                std::chrono::duration_cast<std::chrono::microseconds>(currTime.time_since_epoch()).count();
            m_videoUtil.SetTimestamp(nowTimeStamp);
            if ((m_surface != 0) && !CloudPhoneController::GetInstance().StripPacket(packetPair)) {
                CloudPhoneController::GetInstance().SetState(CloudPhoneState::CLOUD_PHONE_DESTROYING);
                usleep(static_cast<int>(CloudPhoneDelayTime::TIME_10_MS));
            }
            free(packetPair.first);
            packetPair.first = nullptr;
        }
        usleep(static_cast<int>(CloudPhoneDelayTime::TIME_2_MS));
    }
    INFO("Decode mainloop exit!");
    return;
}

int CloudPhoneController::Start(uint64_t surface)
{
    std::unique_lock<std::mutex> lk(m_lock);
    m_surface = surface;
    const std::string modelKbox = "Kbox";
    const std::string modelMonbox = "Monbox";
    std::string model = GetPropertyWithDefault("ro.product.model", "");
    if (model.compare(modelKbox) == 0 || model.compare(modelMonbox) == 0) {
        m_isSimulator = true;
    } else {
        m_isSimulator = false;
    }
    if ((m_surface != 0) && !SetupVideoEngine()) {
        ERR("Init videoEngine failed");
        return VMI_CLIENT_START_FAIL;
    }
    m_state = CloudPhoneState::CLOUD_PHONE_RUNNING;
    MAKE_UNIQUE_NOTHROW(m_decodeThread, std::thread, &CloudPhoneController::DecodeTask, this);
    if (m_decodeThread == nullptr) {
        ERR("Out of memory to alloc decode thread");
        m_state = CloudPhoneState::CLOUD_PHONE_EXITED;
        return VMI_CLIENT_START_FAIL;
    }
    return VMI_SUCCESS;
}

void CloudPhoneController::Stop()
{
    std::unique_lock<std::mutex> lk(m_lock);
    if (m_state == CloudPhoneState::CLOUD_PHONE_EXITED) {
        INFO("CloudPhoneController has exited");
        return;
    }
    m_videoUtil.Release();
    m_state = CloudPhoneState::CLOUD_PHONE_DESTROYING;
    if (m_decodeThread != nullptr) {
        INFO("Set decode thread to exit");
        if (m_decodeThread->joinable()) {
            m_decodeThread->join();
        }
        m_decodeThread = nullptr;
    }

    if (m_surface != 0) {
        DecodeController &decodeController = DecodeController::GetInstance();
        uint32_t ret = decodeController.Stop();
        if (ret != VMI_SUCCESS) {
            ERR("m_videoEngine.StopDecoder Fail");
        }
        decodeController.Destroy();
    }
    m_surface = 0;
    m_orientation = 0;
    m_isFirstFrame = true;
    m_state = CloudPhoneState::CLOUD_PHONE_EXITED;
    INFO("CloudPhoneController Stop complete");
}

/*
 * @功能描述：获取SDK当前状态
 * @返回值：参见CloudPhoneState枚举定义
 */
CloudPhoneState CloudPhoneController::GetState()
{
    DBG("CloudPhoneController get state %d", m_state.load());
    return m_state.load();
}

/*
 * @功能描述：设置SDK状态
 * @参数 [in] state：参见CloudPhoneState枚举定义
 */
void CloudPhoneController::SetState(CloudPhoneState state)
{
    INFO("CloudPhoneController set state to %d", state);
    m_state = state;
}

uint32_t CloudPhoneController::GetRecvFrameRate() const
{
    uint32_t recvFrameRate = m_videoUtil.GetFps();
    INFO("Recv Frame Rate: %u fps", recvFrameRate);
    return recvFrameRate;
}

uint32_t CloudPhoneController::GetDecodeFrameRate()
{
    if (m_surface == 0) {
        return 0;
    }
    DecoderStatistics statistics = {};
    DecodeController &decodeController = DecodeController::GetInstance();
    uint32_t ret = decodeController.GetStatistics(statistics);
    if (ret == VMI_SUCCESS) {
        return statistics.decodeFps;
    } else {
        ERR("GetDecoderStatics Fail");
        return 0;
    }
}

/*
 * @功能描述：处理视频流数据包
 * @返回值：true代表成功，false代表失败
 */
bool CloudPhoneController::HandlePacket(std::pair<uint8_t *, uint32_t> &packetPair, uint8_t rotation)
{
    // Deal with Acual VideoData
    EngineStat stat = EngineStat::VMI_ENGINE_INVALID;
    DecodeController &decodeController = DecodeController::GetInstance();
    uint32_t ret = decodeController.GetStatus(stat);
    if (ret != VMI_VIDEO_ENGINE_CLIENT_SUCCESS || stat != EngineStat::VMI_ENGINE_RUNNING) {
        ERR("ERROR! GetDecoderStatus ret[%u], Engine Stat is [%d]", ret, stat);
        return false;
    }
    if (!ProcessResolution(packetPair)) {
        return false;
    }
    if (m_decoderType == DecoderType::DEC_TYPE_H264) {
        if (!ProcessVideoInfo(packetPair, rotation)) {
            return false;
        }
    } else if (m_decoderType == DecoderType::DEC_TYPE_H265) {
        if (!ProcessVideoInfo265(packetPair, rotation)) {
            return false;
        }
    }
    ret = decodeController.Decode(packetPair);
    if (ret != VMI_VIDEO_ENGINE_CLIENT_SUCCESS) {
        ERR("Send Frame to VideoEngine Fail![%u]", ret);
        return false;
    }
    return true;
}

/*
 * @功能描述：识别H264视频帧分辨率，检查当前解码器分辨率是否支持该帧分辨率。
 *           非商用模块提供动态开关OIOO功能
 * @返回值：处理工作完成返回true, 反之返回false
 */
bool CloudPhoneController::ProcessResolution(std::pair<uint8_t *, uint32_t> &packetPair)
{
    Sps sps = {};
    if (m_parser.ExtractSPS(packetPair, sps)) {
        uint32_t videoWidth = m_parser.GetH264Width(sps);
        uint32_t videoHeight = m_parser.GetH264Height(sps);
        if (videoWidth > videoHeight) {
            uint32_t tmp = videoWidth;
            videoWidth = videoHeight;
            videoHeight = tmp;
        }
    }
    return true;
}

/*
 * @功能描述：切换客户端旋转信号
 * @返回值：处理工作完成返回true, 反之返回false
 */
bool CloudPhoneController::SwitchOrientation(uint8_t rotation)
{
    if (g_orientationMap.find(rotation) == g_orientationMap.end()) {
        ERR("SwitchOrientation rotation[%u] is invalid", rotation);
        return false;
    }
    INFO("Layout rotation current rotation[%u] -> new rotation[%u]", m_orientation, rotation);
    m_orientation = rotation;
    if (m_isSimulator) {
        DecodeController &decodeController = DecodeController::GetInstance();
        decodeController.Destroy();
        if (!SetupVideoEngine()) {
            ERR("SwitchOrientation setup VideoEngine fail");
            return false;
        }
    }
    EngineEvent engineEvent = {VMI_ENGINE_EVENT_ORIENTATION_CHANGED, m_orientation, 0, 0, 0};
    EngineEventHandler::GetInstance().CreateEvent(engineEvent);
    return true;
}

/*
 * @功能描述：识别H264视频帧类型，进行如下处理：
 *           收到关键帧时处理横竖屏切换场景下的旋转信号；
 *           收到非关键帧时处理手机自动旋转场景下的旋转信号。
 * @返回值：处理工作完成返回true, 反之返回false
 */
bool CloudPhoneController::ProcessVideoInfo(std::pair<uint8_t *, uint32_t> &packetPair, uint8_t rotation)
{
    constexpr uint8_t offset = 2;
    switch (m_parser.GetFrameType(packetPair)) {
        case NalUnitType::NAL_SLICE:
        case NalUnitType::NAL_SLICE_DPA:
        case NalUnitType::NAL_SLICE_DPB:
        case NalUnitType::NAL_SLICE_DPC:
            if (m_orientation + offset == rotation || m_orientation - offset == rotation) {
                return SwitchOrientation(rotation);
            }
            break;
        case NalUnitType::NAL_SLICE_IDR:
        case NalUnitType::NAL_SEI:
        case NalUnitType::NAL_SPS:
        case NalUnitType::NAL_PPS:
        case NalUnitType::NAL_AUD:
        case NalUnitType::NAL_FILLER:
            if (rotation != m_orientation) {
                return SwitchOrientation(rotation);
            }
            break;
        default: break;
    }
    return true;
}

/*
 * @功能描述：识别H265视频帧类型，进行如下处理：
 *           收到关键帧时处理横竖屏切换场景下的旋转信号；
 *           收到非关键帧时处理手机自动旋转场景下的旋转信号。
 * @返回值：处理工作完成返回true, 反之返回false
 */
bool CloudPhoneController::ProcessVideoInfo265(std::pair<uint8_t *, uint32_t> &packetPair, uint8_t rotation)
{
    constexpr uint8_t offset = 2;
    switch (m_parser.GetFrameType265(packetPair)) {
        case NalUnitType265::TRAIL_N:
        case NalUnitType265::TRAIL_R:
        case NalUnitType265::TSA_N:
        case NalUnitType265::TSA_R:
        case NalUnitType265::STSA_N:
        case NalUnitType265::STSA_R:
            if (m_orientation + offset == rotation || m_orientation - offset == rotation) {
                return SwitchOrientation(rotation);
            }
            break;
        case NalUnitType265::IDR_W_RADL:
        case NalUnitType265::IDR_N_LP:
        case NalUnitType265::CRA_NUT:
        case NalUnitType265::VPS_NUT:
        case NalUnitType265::PPS_NUT:
        case NalUnitType265::AUD_NUT:
            if (rotation != m_orientation) {
                return SwitchOrientation(rotation);
            }
            break;
        default: break;
    }
    return true;
}