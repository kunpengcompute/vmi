/*
 * 版权所有 (c) 华为技术有限公司 2023-2024
 * 功能说明: Agent程序的主逻辑
 */
#include "VmiAgent.h"
#include <mutex>
#include "NetworkComm/NetworkExport.h"
#include "logging.h"
#include "SystemProperty.h"
#include "VmiMsgHead.h"
#include "MainLooper.h"
#include "SmartPtrMacro.h"

namespace Vmi {
namespace {
std::mutex g_netMutex;
std::shared_ptr<INetworkComm> g_netcomm = nullptr;
constexpr uint32_t DEFAULT_DATA_OFFSET = sizeof(StreamMsgHead) + sizeof(VmiCmd);
uint32_t g_dataOffset = DEFAULT_DATA_OFFSET;
}

RecvDataRunnable::RecvDataRunnable(VmiDataType dataType, VmiCmd cmd,
    std::pair<uint8_t*, uint32_t> data) : m_dataType(dataType), m_cmd(cmd), m_data(data) {}

int RecvDataRunnable::Run()
{
    VmiErrCode err = VmiErrCode::OK;
    CmdHelp cmdTemp;
    cmdTemp.cmd = m_cmd;
    if (cmdTemp.info.cmdType == VmiCmdType::CMD_TRANS_DATA) {
        err = InjectData(m_dataType, m_cmd, m_data.first + sizeof(VmiCmd), m_data.second - sizeof(VmiCmd));
        if (err != OK) {
            ERR("Failed to InjectData Module:%u, cmd:%u err:%u", m_dataType, m_cmd, err);
            free(m_data.first);
            m_data.first = nullptr;
            return 1;
        }
        free(m_data.first);
	m_data.first = nullptr;
    } else if (cmdTemp.info.cmdType == VmiCmdType::CMD_SET_PARAM) {
        err = SetParam(m_dataType, m_cmd, m_data.first + sizeof(VmiCmd), m_data.second - sizeof(VmiCmd));
        if (err != OK) {
            ERR("Failed to SetParam Module:%u, cmd:%u err:%u", m_dataType, m_cmd, err);
            free(m_data.first);
            m_data.first = nullptr;
            return 1;
        }
        free(m_data.first);
	m_data.first = nullptr;
    } else {
        ERR("Failed to deal cmd:%u, type:%u size:%u data, cmd not support", static_cast<uint32_t>(cmdTemp.info.cmdType),
            static_cast<uint32_t>(m_dataType), m_data.second);
        free(m_data.first);
        m_data.first = nullptr;
        return 1;
    }
    return 0;
}

int CallbackForSend(VmiDataType module, VmiCmd cmd, uint8_t *data, uint32_t size)
{
    std::lock_guard<std::mutex> lock(g_netMutex);
    if (g_netcomm == nullptr) {
        WARN("g_netcomm is nullptr, don't send data, module: %u", module);
        return 0;
    }
    if (data == nullptr) {
        ERR("Failed to send data, data is nullptr, module:%u, cmd:%u, size:%u", module, cmd, size);
        return -1;
    }

    std::unique_ptr<uint8_t[]> sendDataBuf = nullptr;
    uint8_t *realSendData = nullptr;
    if (g_dataOffset != DEFAULT_DATA_OFFSET) {
        MAKE_UNIQUE_NOTHROW(sendDataBuf, uint8_t[], size + DEFAULT_DATA_OFFSET);
        if (sendDataBuf == nullptr) {
            ERR("Failed to send data, malloc send data buffer failed, module:%u, cmd:%u, size:%u", module, cmd, size);
            return -1;
        }
        if (size != 0) { // size为0在部分场景下为正常情况，此时仅需保证最终发送数据的dataOffset合理即可
            memcpy(sendDataBuf.get() + DEFAULT_DATA_OFFSET, data + g_dataOffset, size);
        }
        realSendData = sendDataBuf.get();
    } else {
        realSendData = data;
    }
    int ret = -1;
    uint32_t *cmdOffset = reinterpret_cast<uint32_t *>(realSendData + sizeof(StreamMsgHead));
    *cmdOffset = cmd;
    ret = g_netcomm->SendWithReservedByte(module, realSendData, size + sizeof(VmiCmd));
    if (ret != 0) {
        ERR("Failed to send data of type: %u, err: %u", module, ret);
    }
    return ret;
}

uint32_t CallbackForRecv(VmiDataType type, std::pair<uint8_t*, uint32_t> data)
{
    if (data.first == nullptr || data.second < sizeof(uint32_t)) {
        ERR("Failed to deal recv data of Module: %u, ptr is null or size:%u is small.", type, data.second);
        return 0;
    }
    if (data.second < sizeof(VmiCmd)) {
        ERR("Failed to deal recv data of Module: %u, length: %u is too small.", type, data.second);
        return 0;
    }

    // 投递任务，避免堵塞网络模块
    uint32_t cmd = *reinterpret_cast<uint32_t*>(data.first);
    std::unique_ptr<RecvDataRunnable> runnable = nullptr;
    std::pair<uint8_t*, uint32_t> runnableData {data.first, data.second};
    MAKE_UNIQUE_NOTHROW(runnable, RecvDataRunnable, type, VmiCmd(cmd), runnableData);
    if (runnable == nullptr) {
        ERR("Failed to post recv data runnable, new failed");
        return 0;
    }
    MainLooper::GetInstance().SendMsg(std::move(runnable));

    return 0;
}

uint32_t CallbackForRecvVideo(std::pair<uint8_t*, uint32_t> data)
{
    return CallbackForRecv(DATA_VIDEO, data);
}

uint32_t CallbackForRecvTouch(std::pair<uint8_t*, uint32_t> data)
{
    return CallbackForRecv(DATA_TOUCH, data);
}

uint32_t CallbackForRecvAudio(std::pair<uint8_t*, uint32_t> data)
{
    return CallbackForRecv(DATA_AUDIO, data);
}

uint32_t CallbackForRecvMic(std::pair<uint8_t*, uint32_t> data)
{
    return CallbackForRecv(DATA_MIC, data);
}

VmiErrCode StartVideoModule()
{
    VmiConfigVideo vmiConfigVideo{};
    vmiConfigVideo.version = VmiVersion::VIDEO_CUR_VERSION;
    vmiConfigVideo.encoderType =
        static_cast<EncoderType>(GetPropertyWithDefault("vmi.video.encodertype", ENCODE_TYPE_MAX));
    vmiConfigVideo.videoFrameType =
        static_cast<VideoFrameType>(GetPropertyWithDefault("vmi.video.videoframetype", FRAME_TYPE_MAX));
    vmiConfigVideo.renderOptimize = GetPropertyWithDefault("vmi.video.renderoptimize", 0) == 0 ? false : true;
    vmiConfigVideo.encodeParams.bitrate = GetPropertyWithDefault("vmi.video.encode.bitrate", 0);
    vmiConfigVideo.encodeParams.gopSize = GetPropertyWithDefault("vmi.video.encode.gopsize", 0);
    vmiConfigVideo.encodeParams.profile =
        static_cast<ProfileType>(GetPropertyWithDefault("vmi.video.encode.profile", UINT32_MAX));
    vmiConfigVideo.encodeParams.rcMode =
        static_cast<RCMode>(GetPropertyWithDefault("vmi.video.encode.rcmode", RC_MODE_MAX));
    vmiConfigVideo.encodeParams.forceKeyFrame = GetPropertyWithDefault("vmi.video.encode.forcekeyframe", UINT32_MAX);
    vmiConfigVideo.encodeParams.interpolation =
        GetPropertyWithDefault("vmi.video.encode.interpolation", 0) == 0 ? false : true;
    // 以下4个属性暂未实际使用
    vmiConfigVideo.resolution.width = GetPropertyWithDefault("vmi.video.frame.width", 720);
    vmiConfigVideo.resolution.height = GetPropertyWithDefault("vmi.video.frame.height", 1280);
    vmiConfigVideo.resolution.widthAligned = GetPropertyWithDefault("vmi.video.frame.widthaligned", 720);
    vmiConfigVideo.resolution.heightAligned = GetPropertyWithDefault("vmi.video.frame.heightaligned", 1280);
    vmiConfigVideo.density = GetPropertyWithDefault("vmi.video.frame.density", 320);
    VmiErrCode err = StartModule(VmiDataType::DATA_VIDEO, reinterpret_cast<uint8_t*>(&vmiConfigVideo), sizeof(vmiConfigVideo));
    if (err != OK) {
        ERR("Start module: %u failed, err: %u", VmiDataType::DATA_VIDEO, err);
        return err;
    }

    return OK;
}

VmiErrCode StartAudioModule()
{
    VmiConfigAudio config {};
    config.version = VmiVersion::AUDIO_CUR_VERSION;
    config.audioType = static_cast<AudioType>(GetPropertyWithDefault("vmi.audio.audiotype", 2));

    config.params.sampleInterval = GetPropertyWithDefault("vmi.audio.encode.sampleinterval", 0);
    config.params.bitrate = GetPropertyWithDefault("vmi.audio.encode.bitrate", 0);

    VmiErrCode err = StartModule(VmiDataType::DATA_AUDIO, reinterpret_cast<uint8_t*>(&config), sizeof(VmiConfigAudio));
    if (err != OK) {
        ERR("Start module: %u failed, err: %u", VmiDataType::DATA_AUDIO, err);
        return err;
    }
    return OK;
}

VmiErrCode StartMicModule()
{
    VmiConfigMic config {};
    config.version = VmiVersion::MIC_CUR_VERSION;
    config.audioType = static_cast<AudioType>(GetPropertyWithDefault("vmi.mic.audiotype", 2));
    VmiErrCode err = StartModule(VmiDataType::DATA_MIC, reinterpret_cast<uint8_t*>(&config), sizeof(VmiConfigMic));
    if (err != OK) {
        ERR("Start module: %u failed, err: %u", VmiDataType::DATA_MIC, err);
        return err;
    }
    return OK;
}

VmiErrCode StartTouchModule()
{
    VmiConfig vmiConfig;
    vmiConfig.version = VmiVersion::TOUCH_CUR_VERSION;
    VmiErrCode err = StartModule(VmiDataType::DATA_TOUCH, reinterpret_cast<uint8_t*>(&vmiConfig), sizeof(vmiConfig));
    if (err != OK) {
        ERR("Start module: %u failed, err: %u", VmiDataType::DATA_TOUCH, err);
        return err;
    }
    return OK;
}

VmiErrCode StartModules()
{
    VmiErrCode err;
    err = StartVideoModule();
    if (err != OK) {
        ERR("Start Video module: %u failed, err: %u", VmiDataType::DATA_VIDEO, err);
        return err;
    }
    err = StartAudioModule();
    if (err != OK) {
        ERR("Start Audio module: %u failed, err: %u", VmiDataType::DATA_AUDIO, err);
        return err;
    }
    err = StartMicModule();
    if (err != OK) {
        ERR("Start Mic module: %u failed, err: %u", VmiDataType::DATA_MIC, err);
        return err;
    }
    err = StartTouchModule();
    if (err != OK) {
        ERR("Start Touch module: %u failed, err: %u", VmiDataType::DATA_TOUCH, err);
        return err;
    }
    return OK;
}

bool DealNetworkOffline(std::shared_ptr<Vmi::INetworkComm> netcomm)
{
    std::lock_guard<std::mutex> lock(g_netMutex);
    if (g_netcomm == nullptr) {
        ERR("Failed to deal network offline, cur network is nullptr");
        return false;
    }
    if (g_netcomm != netcomm) {
        ERR("Failed to deal network offline, network obj is not same");
        return false;
    }
    g_netcomm->RegisterRecvDataCallback(DATA_TOUCH, nullptr, false);
    g_netcomm->RegisterRecvDataCallback(DATA_AUDIO, nullptr, false);
    g_netcomm->RegisterRecvDataCallback(DATA_VIDEO, nullptr, false);
    g_netcomm->RegisterRecvDataCallback(DATA_MIC, nullptr, false);
    g_netcomm = nullptr;
    return true;
}

void DealNetworkOnline(std::shared_ptr<Vmi::INetworkComm> netcomm)
{
    std::lock_guard<std::mutex> lock(g_netMutex);
    if (g_netcomm != nullptr) {
        WARN("The previous connection was not released");
    }
    g_netcomm = netcomm;
    g_netcomm->RegisterRecvDataCallback(DATA_TOUCH,
        std::bind(&CallbackForRecvTouch, std::placeholders::_1), false);
    g_netcomm->RegisterRecvDataCallback(DATA_AUDIO,
        std::bind(&CallbackForRecvAudio, std::placeholders::_1), false);
    g_netcomm->RegisterRecvDataCallback(DATA_VIDEO,
        std::bind(&CallbackForRecvVideo, std::placeholders::_1), false);
    g_netcomm->RegisterRecvDataCallback(DATA_MIC,
        std::bind(&CallbackForRecvMic, std::placeholders::_1), false);
}

void NetworkChange(bool isOnline, std::shared_ptr<Vmi::INetworkComm> netcomm)
{
    if (netcomm == nullptr) {
        ERR("Failed to deal network status change, netcomm is nullptr");
        return;
    }
    if (!isOnline) {
        if (!DealNetworkOffline(netcomm)) {
            ERR("Failed to deal network offline");
            return;
        }
        // 离线状态，停止各种模块
        VmiErrCode err = OK;
        for (uint32_t i = 0; i < DATA_TYPE_MAX; ++i) {
            err = StopModule(static_cast<VmiDataType>(i));
            if (err != OK) {
                ERR("Failed to deal network offline, stop Module: %u failed, err: %u", i, err);
            }
        }
    } else {
        // 设置回调函数用于处理接收数据，音频、触控
        DealNetworkOnline(netcomm);
        // 在线状态，此处启全部模块
        VmiErrCode err = StartModules();
        if (err != OK) {
            netcomm->ActiveDisconnect();
        }
    }
}

bool VmiAgent::Initialize()
{
    // 初始化云手机服务端
    g_dataOffset = GetPropertyWithDefault("demo.data.offset", DEFAULT_DATA_OFFSET);
    INFO("Set data offset = %u, default = %u", g_dataOffset, DEFAULT_DATA_OFFSET);
    VmiConfigEngine vmiConfigEngine;
    vmiConfigEngine.dataCallback = CallbackForSend;
    for (int i = 0; i < DATA_TYPE_MAX; ++i) {
        vmiConfigEngine.dataTypeConfig[i].shouldInit = true;
        vmiConfigEngine.dataTypeConfig[i].sendDataOffset = g_dataOffset;
    }
    VmiErrCode err = InitVmiEngine(&vmiConfigEngine);
    if (err != OK) {
        ERR("Failed to init agent, init vim engine failed, err: %u", err);
        return false;
    }

    // 网络模块初始化
    int32_t netTypeInt = GetPropertyWithDefault("vmi.network.type", 0);
    if (netTypeInt <= static_cast<int32_t>(NetworkType::ERROR) ||
        netTypeInt > static_cast<int32_t>(NetworkType::ELASTIC_GPU)) {
        ERR("Failed to init agent, get network:%d is error", netTypeInt);
        return false;
    }
    NetworkType netType = static_cast<NetworkType>(netTypeInt);
    RegisterNetworkChangeStatusCallback(NetworkChange);
    if (!NetworkInitialize(netType)) {
        ERR("Failed to init agent, network init failed");
        return false;
    }
    return true;
}

VmiAgent::~VmiAgent()
{
    DeInitVmiEngine();
}
}
