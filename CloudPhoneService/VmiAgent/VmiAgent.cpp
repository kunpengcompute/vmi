/*
 * 版权所有 (c) 华为技术有限公司 2023-2024
 * 功能说明: Agent程序的主逻辑
 */
#define LOG_TAG "VmiAgent"

#include "VmiAgent.h"
#include <array>
#include <numeric>
#include <mutex>
#include <chrono>
#include <thread>
#include "NetworkComm/NetworkExport.h"
#include "logging.h"
#include "SystemProperty.h"
#include "VmiMsgHead.h"
#include "MainLooper.h"
#include "SmartPtrMacro.h"
#include "Latency/Latency.h"
#include "DeviceWatcher.h"

namespace Vmi {
namespace {
std::mutex g_netMutex;
std::mutex g_startMutex;
std::shared_ptr<INetworkComm> g_netcomm = nullptr;
constexpr uint32_t DEFAULT_DATA_OFFSET = sizeof(StreamMsgHead) + sizeof(VmiCmd);
constexpr uint32_t MIN_WIDTH = 360;
constexpr uint32_t VPU_QUADRA_ENCODER_MIN_STREAM_WIDTH_OR_HEIGHT = 240;
uint32_t g_dataOffset = DEFAULT_DATA_OFFSET;
Latency g_latency;
std::array<bool, DATA_TYPE_MAX> moduleInitState;
std::atomic<bool> g_startCollectFps(false);
std::atomic<bool> g_collectFpsRunning(false);
std::thread g_collectFpsThread {};
}

VmiErrCode StartModules();

RecvDataRunnable::RecvDataRunnable(VmiDataType dataType, VmiCmd cmd,
    std::pair<uint8_t*, uint32_t> data) : m_dataType(dataType), m_cmd(cmd), m_data(data) {}

void CallbackForSendVideoConfig(uint8_t* data, uint32_t size)
{
    if (data == nullptr) {
        ERR("Failed to send video config, data is nullptr");
        return;
    }
    EncodeParams* params = reinterpret_cast<EncodeParams*>(data);
    INFO("params->streamWidth: %u, params->streamHeight: %u", params->streamWidth, params->streamHeight);
    std::unique_ptr<uint8_t[]> sendDataBuf = nullptr;
    MAKE_UNIQUE_NOTHROW(sendDataBuf, uint8_t[], size + DEFAULT_DATA_OFFSET);
    if (sendDataBuf == nullptr) {
        ERR("Failed to send video Config, malloc video send data buffer failed");
        return;
    }
    VmiCmd cmd = VmiCmd::VIDEO_GET_ENCODER_PARAM;
    memcpy(sendDataBuf.get() + sizeof(StreamMsgHead), &cmd, sizeof(VmiCmd));
    memcpy(sendDataBuf.get() + DEFAULT_DATA_OFFSET, data, size);
    int ret = g_netcomm->SendWithReservedByte(VMIMsgType::CONFIG, sendDataBuf.get(), size + DEFAULT_DATA_OFFSET);
    if (ret != 0) {
        ERR("Failed to send video Config, send config data failed");
        return;
    }
}

uint32_t gcd(uint32_t a, uint32_t b)
{
    uint32_t r;
    while (b > 0) {
        r = a % b;
        a = b;
        b = r;
    }
    return a;
}

int SetVideoStreamWidthAndHeight(EncodeParams* params)
{
    std::unique_ptr<uint8_t[]> paramBuffer = nullptr;
    MAKE_UNIQUE_NOTHROW(paramBuffer, uint8_t[], sizeof(EncodeParams));
    if (paramBuffer == nullptr) {
        ERR("Failed to get video Config, malloc video get data buffer failed");
        return 1;
    }
    VmiErrCode err = GetParam(VmiDataType::DATA_VIDEO, VmiCmd::VIDEO_GET_ENCODER_PARAM,
        paramBuffer.get(), sizeof(EncodeParams));
    if (err != OK) {
        ERR("Failed to GetParam Module:%u, cmd:%u err:%u", VmiDataType::DATA_VIDEO,
            VmiCmd::VIDEO_GET_ENCODER_PARAM, err);
        return 1;
    }
    EncodeParams* originParams = reinterpret_cast<EncodeParams*>(paramBuffer.get());
    INFO("originParams->streamWidth: %u, originParams->streamHeight: %u",
        originParams->streamWidth, originParams->streamHeight);
    // 以传入的宽为基准，计算出既符合当前宽高比又是8的倍数的数
    uint32_t gcdResult = gcd(originParams->streamWidth, originParams->streamHeight);
    uint32_t widthBase = 8 * originParams->streamWidth / gcdResult;
    uint32_t heightBase = 8 * originParams->streamHeight / gcdResult;
    uint32_t multiple = params->streamWidth / widthBase;
    if (multiple == 0) {
        ERR("Failed to set encodeParams, illegal streamWidth %u", params->streamWidth);
        return 1;
    }
    params->streamWidth = multiple * widthBase;
    params->streamHeight = multiple * heightBase;

    int32_t netTypeInt = GetPropertyWithDefault("vmi.network.type", 0);
    return 0;
}

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
        // video设置分辨率时需要获取到当前的宽高比来修正设置的参数
        if (m_dataType == VmiDataType::DATA_VIDEO) {
            EncodeParams* encodeParams = reinterpret_cast<EncodeParams*>(m_data.first + sizeof(VmiCmd));
            SetVideoStreamWidthAndHeight(encodeParams);
        }
        err = SetParam(m_dataType, m_cmd, m_data.first + sizeof(VmiCmd), m_data.second - sizeof(VmiCmd));
        if (err != OK) {
            ERR("Failed to SetParam Module:%u, cmd:%u err:%u", m_dataType, m_cmd, err);
            free(m_data.first);
            m_data.first = nullptr;
            return 1;
        }
        free(m_data.first);
	m_data.first = nullptr;
    } else if (cmdTemp.info.cmdType == VmiCmdType::CMD_GET_PARAM) {
        err = GetParam(m_dataType, m_cmd, m_data.first + sizeof(VmiCmd), m_data.second - sizeof(VmiCmd));
        if (err != OK) {
            ERR("Failed to GetParam Module:%u, cmd:%u err:%u", m_dataType, m_cmd, err);
            free(m_data.first);
            m_data.first = nullptr;
            return 1;
        }
        if (m_dataType == VmiDataType::DATA_VIDEO) {
            CallbackForSendVideoConfig(m_data.first + sizeof(VmiCmd), m_data.second - sizeof(VmiCmd));
        }
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
    if (module == VmiDataType::DATA_VIDEO) {
        g_latency.RecvVideoData(cmd, data, size);
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

int CallbackForRecv(VmiDataType type, std::pair<uint8_t*, uint32_t> data)
{
    if (data.first == nullptr || data.second < sizeof(uint32_t)) {
        ERR("Failed to deal recv data of Module: %u, ptr is null or size:%u is small.", type, data.second);
        return -1;
    }
    if (data.second < sizeof(VmiCmd)) {
        ERR("Failed to deal recv data of Module: %u, length: %u is too small.", type, data.second);
        return -1;
    }

    // 投递任务，避免堵塞网络模块
    uint32_t cmd = *reinterpret_cast<uint32_t*>(data.first);
    std::unique_ptr<RecvDataRunnable> runnable = nullptr;
    std::pair<uint8_t*, uint32_t> runnableData {data.first, data.second};
    MAKE_UNIQUE_NOTHROW(runnable, RecvDataRunnable, type, VmiCmd(cmd), runnableData);
    if (runnable == nullptr) {
        ERR("Failed to post recv data runnable, new failed");
        return -1;
    }
    MainLooper::GetInstance().SendMsg(std::move(runnable));

    return 0;
}

void CollectFpsTask()
{
    INFO("Collect FPS thread entering");
    std::unique_ptr<DeviceWatcher> fpsWatcher = nullptr;
    MAKE_UNIQUE_NOTHROW(fpsWatcher, DeviceWatcher);
    if (fpsWatcher == nullptr) {
        ERR("Failed to collect fps, create fps watcher failed, Collect FPS thread exited");
        g_startCollectFps = false;
        return;
    }
    bool ret = fpsWatcher->Init();
    if (!ret) {
        ERR("Failed to collect fps, fps watcher init failed, Collect FPS thread exited");
        g_startCollectFps = false;
        return;
    }
    g_collectFpsRunning = true;
    fpsWatcher->RunWatcher(static_cast<char>(FpsperfMode::VIDEO));
    while (g_startCollectFps) {
        int fps;
        int jank;
        int bjank;
        fpsWatcher->GetFpsUsage(static_cast<char>(FpsperfMode::VIDEO), fps, jank, bjank);
        DBG("Get fps:%d, jank:%d, bjank:%d", fps, jank, bjank);
        VmiFpsPerfData perfData = {fps, jank, bjank};
        std::unique_ptr<uint8_t[]> sendDataBuf = nullptr;
        MAKE_UNIQUE_NOTHROW(sendDataBuf, uint8_t[], sizeof(StreamMsgHead) + sizeof(VmiFpsPerfData));
        if (sendDataBuf == nullptr) {
            ERR("Failed to collect fps, malloc fps perf send data buffer failed, Collect FPS thread exited");
            g_startCollectFps = false;
            g_collectFpsRunning = false;
            return;
        }
        memcpy(sendDataBuf.get() + sizeof(StreamMsgHead), &perfData, sizeof(VmiFpsPerfData));
        int ret = g_netcomm->SendWithReservedByte(VMIMsgType::FPS, sendDataBuf.get(), sizeof(VmiFpsPerfData));
        if (ret != 0) {
            ERR("Failed to collect fps, send fps perf data failed, ret:[%d], Collect FPS thread exited", ret);
            g_startCollectFps = false;
            g_collectFpsRunning = false;
            return;
        }
    }
    INFO("Collect FPS thread exited");
}

int CallbackForRecvFps(std::pair<uint8_t*, uint32_t> data)
{
    bool recvFlag = static_cast<bool>(*(data.first));
    free(data.first);
    data.first = nullptr;
    if (recvFlag && g_collectFpsRunning) {
        WARN("Callback fps receive start flag but thread is stil running, ignore");
        return 0;
    }
    if (!recvFlag && !g_startCollectFps) {
        WARN("Callback fps receive stop flag but server is already stopped, ignore");
        return 0;
    }
    g_startCollectFps = recvFlag;
    if (g_startCollectFps) {
        g_collectFpsThread = std::thread(CollectFpsTask);
    } else {
        if (g_collectFpsThread.joinable()) {
            g_collectFpsThread.join();
        }
        g_collectFpsRunning = false;
    }
    return 0;
}

void AmendFrameSize(VmiConfigVideo& vmiConfigVideo)
{
    FrameSize frameSize = vmiConfigVideo.resolution;
    uint32_t height = vmiConfigVideo.resolution.height;
    uint32_t width = vmiConfigVideo.resolution.width;
    if (height < VPU_QUADRA_ENCODER_MIN_STREAM_WIDTH_OR_HEIGHT || width < VPU_QUADRA_ENCODER_MIN_STREAM_WIDTH_OR_HEIGHT) {
        return;
    }
    // 修正分辨率使其能缩放到宽度为360，并且宽高都是8的倍数，保证宽高比变化最小，变化后的比值为MIN_WIDTH：minHeight
    uint32_t minHeight = MIN_WIDTH * height / width;
    minHeight = minHeight - (minHeight % 8);
    // 以MIN_WIDTH:minHeight为基准修正原来的值
    uint32_t gcdResult = gcd(MIN_WIDTH, minHeight);
    uint32_t widthBase = 8 * MIN_WIDTH / gcdResult;
    uint32_t heightBase = 8 * minHeight / gcdResult;
    uint32_t multiple = width / widthBase;
    vmiConfigVideo.resolution.width = multiple * widthBase;
    vmiConfigVideo.resolution.widthAligned = vmiConfigVideo.resolution.width;
    vmiConfigVideo.resolution.height = multiple * heightBase;
    vmiConfigVideo.resolution.heightAligned = vmiConfigVideo.resolution.height;
}

VmiErrCode StartVideoModule(std::pair<uint8_t*, uint32_t> data)
{
    std::lock_guard<std::mutex> lock(g_startMutex);
    if (sizeof(VmiConfigVideo) != data.second - sizeof(VmiDataType)) {
        ERR("VideoModule start failed input config size illegal, size: %u", data.second - sizeof(VmiDataType));
        return VmiErrCode::ERR_ILLEGAL_INPUT;
    }
    // 如果video模块已经启动则先stop再start
    VmiModuleStatus current_status = GetStatus(VmiDataType::DATA_VIDEO);
    INFO("Start module %u, current status is %u", VmiDataType::DATA_VIDEO, current_status);
    if (current_status == VmiModuleStatus::MODULE_STARTED) {
        VmiErrCode ret = StopModule(VmiDataType::DATA_VIDEO);
        if (ret != VmiErrCode::OK) {
            ERR("Failed to stop module:%u, stop failed, ret:%u", VmiDataType::DATA_VIDEO, static_cast<uint32_t>(ret));
            return ret;
        }
    }
    if (data.first == nullptr) {
        ERR("Failed to start module:%u, config data is nullptr", VmiDataType::DATA_VIDEO);
        return VmiErrCode::ERR_ILLEGAL_INPUT;
    }
    VmiConfigVideo vmiConfigVideo = *reinterpret_cast<VmiConfigVideo*>(data.first + sizeof(VmiDataType));
    AmendFrameSize(vmiConfigVideo);
    // 除了抓图分辨率和屏幕密度，其余的已default.prop为准
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
    vmiConfigVideo.encodeParams.crf = GetPropertyWithDefault("vmi.video.encode.crf", UINT32_MAX);
    vmiConfigVideo.encodeParams.maxCrfRate = GetPropertyWithDefault("vmi.video.encode.maxcrfrate", UINT32_MAX);
    vmiConfigVideo.encodeParams.vbvBufferSize = GetPropertyWithDefault("vmi.video.encode.vbvbuffersize", UINT32_MAX);
    vmiConfigVideo.encodeParams.streamWidth = vmiConfigVideo.resolution.width;
    vmiConfigVideo.encodeParams.streamHeight = vmiConfigVideo.resolution.height;
    INFO("config width: %u, height: %u", vmiConfigVideo.resolution.width, vmiConfigVideo.resolution.height);
    VmiErrCode err = StartModule(VmiDataType::DATA_VIDEO, reinterpret_cast<uint8_t*>(&vmiConfigVideo),
        data.second - sizeof(VmiDataType));
    if (err != OK) {
        ERR("Start module: %u failed with config, err: %u", VmiDataType::DATA_VIDEO, err);
        return err;
    }
    g_latency.Start(vmiConfigVideo.videoFrameType);
    return OK;
}

VmiErrCode StartAudioModule(std::pair<uint8_t*, uint32_t> data)
{
    if (sizeof(VmiConfigAudio) != data.second - sizeof(VmiDataType)) {
        ERR("AudioModule start failed input config size illegal, size: %u", data.second - sizeof(VmiDataType));
        return VmiErrCode::ERR_ILLEGAL_INPUT;
    }
    if (data.first == nullptr) {
        ERR("Failed to start module:%u, config data is nullptr", VmiDataType::DATA_AUDIO);
        return VmiErrCode::ERR_ILLEGAL_INPUT;
    }
    VmiErrCode err = StartModule(VmiDataType::DATA_AUDIO, data.first + sizeof(VmiDataType),
        data.second - sizeof(VmiDataType));
    if (err != OK) {
        ERR("Start module: %u failed, err: %u", VmiDataType::DATA_AUDIO, err);
        return err;
    }
    return OK;
}

VmiErrCode StartMicModule(std::pair<uint8_t*, uint32_t> data)
{
    if (sizeof(VmiConfigMic) != data.second - sizeof(VmiDataType)) {
        ERR("MicModule start failed input config size illegal, size: %u", data.second - sizeof(VmiDataType));
        return VmiErrCode::ERR_ILLEGAL_INPUT;
    }
    if (data.first == nullptr) {
        ERR("Failed to start module:%u, config data is nullptr", VmiDataType::DATA_MIC);
        return VmiErrCode::ERR_ILLEGAL_INPUT;
    }
    VmiErrCode err = StartModule(VmiDataType::DATA_MIC, data.first + sizeof(VmiDataType),
        data.second - sizeof(VmiDataType));
    if (err != OK) {
        ERR("Start module: %u failed, err: %u", VmiDataType::DATA_MIC, err);
        return err;
    }
    return OK;
}

VmiErrCode StartTouchModule(std::pair<uint8_t*, uint32_t> data)
{
    if (sizeof(VmiConfigTouch) != data.second - sizeof(VmiDataType)) {
        ERR("TouchModule start failed input config size illegal, size: %u", data.second - sizeof(VmiDataType));
        return VmiErrCode::ERR_ILLEGAL_INPUT;
    }
    if (data.first == nullptr) {
        ERR("Failed to start module:%u, config data is nullptr", VmiDataType::DATA_TOUCH);
        return VmiErrCode::ERR_ILLEGAL_INPUT;
    }
    VmiErrCode err = StartModule(VmiDataType::DATA_TOUCH, data.first + sizeof(VmiDataType),
        data.second - sizeof(VmiDataType));
    if (err != OK) {
        ERR("Start module: %u failed, err: %u", VmiDataType::DATA_TOUCH, err);
        return err;
    }
    return OK;
}

VmiErrCode StartSensorModule(std::pair<uint8_t*, uint32_t> data)
{
    if (sizeof(VmiConfig) != data.second - sizeof(VmiDataType)) {
        ERR("SensorModule start failed input config size illegal, size: %u", data.second - sizeof(VmiDataType));
        return VmiErrCode::ERR_ILLEGAL_INPUT;
    }
    if (data.first == nullptr) {
        ERR("Failed to start module:%u, config data is nullptr", VmiDataType::DATA_SENSOR);
        return VmiErrCode::ERR_ILLEGAL_INPUT;
    }
    VmiErrCode err = StartModule(VmiDataType::DATA_SENSOR, data.first + sizeof(VmiDataType),
        data.second - sizeof(VmiDataType));
    if (err != OK) {
        ERR("Start module: %u failed, err: %u", VmiDataType::DATA_SENSOR, err);
        return err;
    }
    return OK;
}

VmiErrCode StartGpsModule(std::pair<uint8_t*, uint32_t> data)
{
    if (sizeof(VmiConfig) != data.second - sizeof(VmiDataType)) {
        ERR("GpsModule start failed input config size illegal, size: %u", data.second - sizeof(VmiDataType));
        return VmiErrCode::ERR_ILLEGAL_INPUT;
    }
    if (data.first == nullptr) {
        ERR("Failed to start module:%u, config data is nullptr", VmiDataType::DATA_GPS);
        return VmiErrCode::ERR_ILLEGAL_INPUT;
    }
    VmiErrCode err = StartModule(VmiDataType::DATA_GPS, data.first + sizeof(VmiDataType),
        data.second - sizeof(VmiDataType));
    if (err != OK) {
        ERR("Start module: %u failed, err: %u", VmiDataType::DATA_GPS, err);
        return err;
    }
    return OK;
}

int CallbackForRecvGeneric(VmiDataType type, std::pair<uint8_t*, uint32_t> data)
{
    if (type == DATA_TOUCH) {
        g_latency.RecvInputEvent();  // Special handling for touch events
    }
    return CallbackForRecv(type, data);
}

int CallbackForRecvStart(std::pair<uint8_t*, uint32_t> data)
{
    VmiDataType type = *reinterpret_cast<VmiDataType*>(data.first);
    if (!moduleInitState[type]) {
        INFO("Module %u is not allowed to init", type);
        return 0; // 如果不允许初始化则跳过此模块
    }
    VmiErrCode err;
    switch (type) {
        case DATA_VIDEO:
            err = StartVideoModule(data);
            break;
        case DATA_AUDIO:
            err = StartAudioModule(data);
            break;
        case DATA_MIC:
            err = StartMicModule(data);
            break;
        case DATA_TOUCH:
            err = StartTouchModule(data);
            break;
        case DATA_SENSOR:
            err = StartSensorModule(data);
            break;
        case DATA_GPS:
            err = StartGpsModule(data);
            break;
        default:
            ERR("Unknown module type: %u", type);
            err = ERR_INVALID_MODULE;
    }
    if (err != OK) {
        ERR("Start module %u failed, err: %u", type, err);
        return err;
    }
    INFO("Start module %u with config successed", type);
    return 0;
}

void DealNetworkOnline(std::shared_ptr<Vmi::INetworkComm> netcomm)
{
    std::lock_guard<std::mutex> lock(g_netMutex);
    if (g_netcomm != nullptr) {
        WARN("The previous connection was not released");
    }
    g_netcomm = netcomm;

    for (uint32_t i = 0; i < DATA_TYPE_MAX; ++i) {
        VmiDataType type = static_cast<VmiDataType>(i);
        if (moduleInitState[type]) {
            g_netcomm->RegisterRecvDataCallback(type,
                std::bind(&CallbackForRecvGeneric, type, std::placeholders::_1), false);
        }
    }
    g_netcomm->RegisterRecvDataCallback(VMIMsgType::FPS,
        std::bind(&CallbackForRecvFps, std::placeholders::_1), false);
    g_netcomm->RegisterRecvDataCallback(VMIMsgType::START,
        std::bind(&CallbackForRecvStart, std::placeholders::_1), false);
}

bool DealNetworkOffline(std::shared_ptr<Vmi::INetworkComm> netcomm)
{
    std::lock_guard<std::mutex> lock(g_netMutex);
    if (g_netcomm == nullptr || g_netcomm != netcomm) {
        ERR("Failed to deal network offline, network obj is invalid");
        return false;
    }

    for (uint32_t i = 0; i < DATA_TYPE_MAX; ++i) {
        VmiDataType type = static_cast<VmiDataType>(i);
        if (moduleInitState[type]) {
            g_netcomm->RegisterRecvDataCallback(type, nullptr, false);
        }
    }
    g_netcomm->RegisterRecvDataCallback(VMIMsgType::FPS, nullptr, false);
    g_startCollectFps = false;
    if (g_collectFpsThread.joinable()) {
        g_collectFpsThread.join();
    }
    g_collectFpsRunning = false;
    g_netcomm = nullptr;
    return true;
}

VmiErrCode StartVideoModule()
{
    std::lock_guard<std::mutex> lock(g_startMutex);
    VmiModuleStatus current_status = GetStatus(VmiDataType::DATA_VIDEO);
    INFO("Start module %u,  current status is %u", VmiDataType::DATA_VIDEO, current_status);
    // 如果video组件已经启动了，说明start命令已经拉起了该模块，就不需要默认启动了
    if (current_status == VmiModuleStatus::MODULE_STARTED) {
        return OK;
    }
    // GetPropertyWithDefault函数中的默认值均应为非法值
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
    vmiConfigVideo.encodeParams.crf = GetPropertyWithDefault("vmi.video.encode.crf", UINT32_MAX);
    vmiConfigVideo.encodeParams.maxCrfRate = GetPropertyWithDefault("vmi.video.encode.maxcrfrate", UINT32_MAX);
    vmiConfigVideo.encodeParams.vbvBufferSize = GetPropertyWithDefault("vmi.video.encode.vbvbuffersize", UINT32_MAX);
    vmiConfigVideo.resolution.width = GetPropertyWithDefault("vmi.video.frame.width", UINT32_MAX);
    vmiConfigVideo.resolution.height = GetPropertyWithDefault("vmi.video.frame.height", UINT32_MAX);
    vmiConfigVideo.resolution.widthAligned = GetPropertyWithDefault("vmi.video.frame.widthaligned", UINT32_MAX);
    vmiConfigVideo.resolution.heightAligned = GetPropertyWithDefault("vmi.video.frame.heightaligned", UINT32_MAX);
    vmiConfigVideo.encodeParams.streamWidth = vmiConfigVideo.resolution.width;
    vmiConfigVideo.encodeParams.streamHeight = vmiConfigVideo.resolution.height;
    vmiConfigVideo.density = GetPropertyWithDefault("vmi.video.frame.density", UINT32_MAX);
    INFO("default width: %u, height: %u", vmiConfigVideo.resolution.width, vmiConfigVideo.resolution.height);
    VmiErrCode err = StartModule(VmiDataType::DATA_VIDEO, reinterpret_cast<uint8_t*>(&vmiConfigVideo), sizeof(vmiConfigVideo));
    if (err != OK) {
        ERR("Start module: %u failed with default, err: %u", VmiDataType::DATA_VIDEO, err);
        return err;
    }
    g_latency.Start(vmiConfigVideo.videoFrameType);
    INFO("Start module: %u successed with default", VmiDataType::DATA_VIDEO);
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

VmiErrCode StartSensorModule()
{
    VmiConfig vmiConfig;
    vmiConfig.version = VmiVersion::SENSOR_CUR_VERSION;
    VmiErrCode err = StartModule(VmiDataType::DATA_SENSOR, reinterpret_cast<uint8_t*>(&vmiConfig), sizeof(vmiConfig));
    if (err != OK) {
        ERR("Start module: %u failed, err: %u", VmiDataType::DATA_SENSOR, err);
        return err;
    }
    return OK;
}

VmiErrCode StartGpsModule()
{
    VmiConfig vmiConfig;
    vmiConfig.version = VmiVersion::GPS_CUR_VERSION;
    VmiErrCode err = StartModule(VmiDataType::DATA_GPS, reinterpret_cast<uint8_t*>(&vmiConfig), sizeof(vmiConfig));
    if (err != OK) {
        ERR("Start module: %u failed, err: %u", VmiDataType::DATA_GPS, err);
        return err;
    }
    return OK;
}

VmiErrCode StartModules()
{
    VmiErrCode err = OK;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    for (uint32_t i = 0; i < DATA_TYPE_MAX; ++i) {
        VmiDataType type = static_cast<VmiDataType>(i);

        if (!moduleInitState[type]) {
            INFO("Module %u is not allowed to init", i);
            continue;  // 如果不允许初始化则跳过此模块
        }

        switch (type) {
            case DATA_VIDEO:
                err = StartVideoModule();
                break;
            case DATA_AUDIO:
                err = StartAudioModule();
                break;
            case DATA_MIC:
                err = StartMicModule();
                break;
            case DATA_TOUCH:
                err = StartTouchModule();
                break;
            case DATA_SENSOR:
                err = StartSensorModule();
                break;
            case DATA_GPS:
                err = StartGpsModule();
                break;
            default:
                ERR("Unknown module type: %u", i);
                err = ERR_INVALID_MODULE;
        }
        if (err != OK) {
            ERR("Start module %u failed, err: %u", i, err);
            return err;
        }
    }
    return OK;
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
            // 检查是否需要停止该模块
            if (!moduleInitState[i]) {
                INFO("No need to stop module: %u.", i);
                continue;
            }
            err = StopModule(static_cast<VmiDataType>(i));
            if (err != OK) {
                ERR("Failed to deal network offline, stop Module: %u failed, err: %u", i, err);
            }
        }
        g_latency.Stop();
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

void InitializeModuleArray()
{
    // 初始设置所有模块为true
    moduleInitState.fill(true);

    // 根据系统属性设置特定模块
    moduleInitState[DATA_GPS] = (GetPropertyWithDefault("ro.vmi.gps.passthrough", 0) != 0);
    moduleInitState[DATA_SENSOR] = (GetPropertyWithDefault("ro.vmi.sensor.passthrough", 0) != 0);
    moduleInitState[DATA_AUDIO] = (GetPropertyWithDefault("ro.vmi.audio.mic.passthrough", 0) != 0);
    moduleInitState[DATA_MIC] = (GetPropertyWithDefault("ro.vmi.audio.mic.passthrough", 0) != 0);
}

bool VmiAgent::Initialize()
{
    InitializeModuleArray();
    // 初始化云手机服务端
    g_dataOffset = GetPropertyWithDefault("demo.data.offset", DEFAULT_DATA_OFFSET);
    VmiConfigEngine vmiConfigEngine;
    vmiConfigEngine.dataCallback = CallbackForSend;
    for (int i = 0; i < DATA_TYPE_MAX; ++i) {
        vmiConfigEngine.dataTypeConfig[i].shouldInit = moduleInitState[i];
        vmiConfigEngine.dataTypeConfig[i].sendDataOffset = g_dataOffset;
    }
    VmiErrCode err = InitVmiEngine(&vmiConfigEngine);
    if (err != OK) {
        ERR("Failed to init agent, init vim engine failed, err: %u", err);
        return false;
    }
    g_latency.Init(g_dataOffset);
    // 网络模块初始化
    int32_t netTypeInt = GetPropertyWithDefault("vmi.network.type", 0);
    if (netTypeInt <= static_cast<int32_t>(NetworkType::ERROR) ||
        netTypeInt >= static_cast<int32_t>(NetworkType::INVALID)) {
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
}
