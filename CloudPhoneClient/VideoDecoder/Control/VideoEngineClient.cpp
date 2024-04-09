/*
 * 版权所有 (c) 华为技术有限公司 2022-2023
 * 功能说明：视频流引擎客户端的对外C接口实现
 */
#define LOG_TAG "VideoEngineClient"
#include <mutex>
#include <map>
#include <cinttypes>
#include <cstring>
#include "NetController.h"
#include "logging.h"
#include "SmartPtrMacro.h"
#include "PacketManager/PacketManager.h"
#include "EngineEventHandler.h"
#include "CloudPhoneController.h"
#include "VideoEngineClient.h"

namespace Vmi {
// 初始化视频流队列
bool g_videoPacketQueueInited = false;
std::mutex g_mutex = {};

// 用于记录当前状态机
enum class VideoClientState : uint32_t {
    NONE = 0,
    INIT,
    START
};

enum VideoClientState g_videoState = VideoClientState::NONE;

uint32_t ClientAudioHandleHook(std::pair<uint8_t *, uint32_t> data)
{
    if (!PacketManager::GetInstance().PutPkt(VMIMsgType::AUDIO, data)) {
        ERR("Failed to put audio packet!");
        return 1;
    }
    return 0;
}

uint32_t ClientVideoHandleHook(std::pair<uint8_t *, uint32_t> data)
{
    if (!PacketManager::GetInstance().PutPkt(VMIMsgType::VIDEO_RR2, data)) {
        ERR("Failed to put video packet!");
        return 1;
    }
    return 0;
}

uint32_t ClientMicHandleHook(std::pair<uint8_t *, uint32_t> data)
{
    if (!PacketManager::GetInstance().PutPkt(VMIMsgType::MIC, data)) {
        ERR("Failed to put video packet!");
        return 1;
    }
    return 0;
}

bool RegisterVideoHandleHooks()
{
    std::shared_ptr<PacketHandle> audioHandle = nullptr;
    MAKE_SHARED_NOTHROW(audioHandle, PacketHandle, VMIMsgType::AUDIO, ClientAudioHandleHook, true);
    std::shared_ptr<PacketHandle> videoHandle = nullptr;
    MAKE_SHARED_NOTHROW(videoHandle, PacketHandle, VMIMsgType::VIDEO_RR2, ClientVideoHandleHook, true);
    std::shared_ptr<PacketHandle> micHandle = nullptr;
    MAKE_SHARED_NOTHROW(micHandle, PacketHandle, VMIMsgType::MIC, ClientMicHandleHook, true);

    if (audioHandle == nullptr || videoHandle == nullptr || micHandle == nullptr) {
        ERR("Failed to create packet handler");
        micHandle = nullptr;
        audioHandle = nullptr;
        videoHandle = nullptr;
        return false;
    }

    uint32_t ret = NetController::GetInstance().SetNetCommHandle(VMIMsgType::AUDIO, audioHandle);
    if (ret != VMI_SUCCESS) {
        ERR("Failed to register audio handle hook, ret=%u", ret);
        return false;
    }
    ret = NetController::GetInstance().SetNetCommHandle(VMIMsgType::VIDEO_RR2, videoHandle);
    if (ret != VMI_SUCCESS) {
        ERR("Failed to register video handle hook, ret=%u", ret);
        return false;
    }
    ret = NetController::GetInstance().SetNetCommHandle(VMIMsgType::MIC, micHandle);
    if (ret != VMI_SUCCESS) {
        ERR("Failed to register mic handle hook, ret=%u", ret);
        return false;
    }
    return true;
}

/**
 * @功能描述：初始化视频流包队列
 * @返回值：成功返回true，有错误返回false
 */
bool InitVideoPacketQueues()
{
    if (g_videoPacketQueueInited) {
        return true;
    }

    std::map<VMIMsgType, bool> pipMap = {
        { VMIMsgType::HEARTBEAT, false },
        { VMIMsgType::AUDIO, false },
        { VMIMsgType::TOUCH_INPUT, false },
        { VMIMsgType::NAVBAR_INPUT, false },
        { VMIMsgType::MIC, false },
        { VMIMsgType::VIDEO_RR2, false }
    };

    for (auto &&it : pipMap) {
        if (!PacketManager::GetInstance().CreateQueue(it.first, it.second)) {
            ERR("Create packet queue(%u) failed", it.first);
            PacketManager::GetInstance().ClearPkt();
            return false;
        }
    }

    if (!RegisterVideoHandleHooks()) {
        ERR("Failed to register video handle hooks");
        return false;
    }
    g_videoPacketQueueInited = true;
    return true;
}

OnVmiEngineEvent g_userEventHandle = nullptr;
static void ClientEngineEventHandler(EngineEvent event)
{
    if (event.event == VMI_ENGINE_EVENT_NEW_CONNECTION) {
        (void)NetController::GetInstance().OnNewConnect(event.para1);
        return;
    }

    if (g_userEventHandle != nullptr) {
        g_userEventHandle(event);
    }
}
}

using namespace Vmi;

uint32_t Initialize(OnVmiEngineEvent eventHandleFunc)
{
    std::lock_guard<std::mutex> lock(g_mutex);
    if (g_videoState == VideoClientState::INIT) {
        ERR("Video engine repeat initialize failed");
        return VMI_CLIENT_EXEC_ORDER_FAIL;
    }

    if (g_videoState == VideoClientState::START) {
        ERR("Video engine exec order error, current state:%u", static_cast<uint32_t>(g_videoState));
        return VMI_CLIENT_EXEC_ORDER_FAIL;
    }

    g_userEventHandle = eventHandleFunc;
    EngineEventHandler::GetInstance().SetEventCallback(ClientEngineEventHandler);
    if (!NetController::GetInstance().Init()) {
        ERR("NetController init failed");
        return VMI_CLIENT_INITIALIZE_FAIL;
    }
    g_videoState = VideoClientState::INIT;
    return VMI_SUCCESS;
}

uint32_t Start(uint64_t surface, uint32_t width, uint32_t height, uint32_t densityDpi)
{
    std::lock_guard<std::mutex> lock(g_mutex);
    if (g_videoState != VideoClientState::INIT) {
        ERR("This step is to execute start, please check whether engine init steps is normal, "
            "current state:%u", static_cast<uint32_t>(g_videoState));
        return VMI_CLIENT_EXEC_ORDER_FAIL;
    }

    if (surface == 0) {
        WARN("Input surface is 0, only receive data not to decode frame");
    } else if (width == 0 || height == 0 || densityDpi == 0) {
        ERR("Input width , height or densityDpi is invalid parameter");
        return VMI_CLIENT_INVALID_PARAM;
    }

    if (!InitVideoPacketQueues()) {
        ERR("Init video stream packet queues failed");
        return VMI_CLIENT_START_FAIL;
    }

    int32_t ret = CloudPhoneController::GetInstance().Start(surface);
    if (ret != VMI_SUCCESS) {
        ERR("CloudPhoneController start failed, ret: %d", ret);
        return VMI_CLIENT_START_FAIL;
    }

    ret = NetController::GetInstance().Start();
    if (ret != VMI_SUCCESS) {
        ERR("NetController start failed, ret: %d", ret);
        return VMI_CLIENT_START_FAIL;
    }
    g_videoState = VideoClientState::START;
    return VMI_SUCCESS;
}

void Stop()
{
    std::lock_guard<std::mutex> lock(g_mutex);
    if (g_videoState != VideoClientState::INIT && g_videoState != VideoClientState::START) {
        ERR("This step is to execute stop, please check whether engine init or start steps is normal, "
            "current state:%u", static_cast<uint32_t>(g_videoState));
        return;
    }
    NetController::GetInstance().Stop();
    CloudPhoneController::GetInstance().Stop();
    g_videoPacketQueueInited = false;
    PacketManager::GetInstance().ClearPkt();
    g_videoState = VideoClientState::NONE;
}

void GetStatistics(StatisticsInfo &info)
{
    std::lock_guard<std::mutex> lock(g_mutex);
    if (g_videoState != VideoClientState::START && g_videoState != VideoClientState::INIT) {
        WARN("Warning: This step is to execute get stat, please check whether engine init or start steps is normal, "
            "current state:%u", static_cast<uint32_t>(g_videoState));
        return;
    }

    int64_t lag = NetController::GetInstance().GetLag();
    if (lag < 0) {
        WARN("Receive negative lag = %" PRId64 ", set to 0", lag);
        info.lag = 0;
    } else {
        info.lag = static_cast<uint64_t>(lag);
    }
    info.recvFps = CloudPhoneController::GetInstance().GetRecvFrameRate();
    info.decFps = CloudPhoneController::GetInstance().GetDecodeFrameRate();
}

int32_t RecvData(VMIMsgType type, uint8_t* data, uint32_t length)
{
    std::lock_guard<std::mutex> lock(g_mutex);
    if (g_videoState != VideoClientState::START && g_videoState != VideoClientState::INIT) {
        LOG_RATE_LIMIT(ANDROID_LOG_WARN, 1, "This step is to execute receive data, "
            "please check whether engine start steps is normal, current state:%u",
            static_cast<uint32_t>(g_videoState));
        return VMI_CLIENT_RECV_ORDER_FAIL;
    }

    if (data == nullptr || length == 0) {
        ERR("buffer is empty, length:%u", length);
        return VMI_CLIENT_RECV_INVALID_PAPAM;
    }

    std::pair<uint8_t*, uint32_t> packetPair = PacketManager::GetInstance().GetNextPkt(type);
    uint32_t dataLen = packetPair.second;
    uint8_t* payLoad = packetPair.first;
    if (payLoad == nullptr || dataLen == 0) {
        DBG("recv payload or datalen is nullptr");
        return VMI_CLIENT_RECV_DATA_NULL;
    }

    if (length < dataLen) {
        ERR("Input buffer is not large enough, length:%d, dataLen:%u", length, dataLen);
        free(payLoad);
        payLoad = nullptr;
        return VMI_CLIENT_RECV_LENGTH_NO_MATCH;
    }

    memcpy(data, payLoad, dataLen);

    free(payLoad);
    payLoad = nullptr;
    return dataLen;
}

uint32_t SendData(VMIMsgType type, uint8_t* data, uint32_t length)
{
    std::lock_guard<std::mutex> lock(g_mutex);
    if (g_videoState != VideoClientState::START && g_videoState != VideoClientState::INIT) {
        LOG_RATE_LIMIT(ANDROID_LOG_WARN, 1, "This step is to execute send data, "
            "please check whether engine start steps is normal, current state:%u",
            static_cast<uint32_t>(g_videoState));
        return VMI_CLIENT_EXEC_ORDER_FAIL;
    }
    std::pair<uint8_t *, size_t> pairData = {data, length};
    uint32_t ret = NetController::GetInstance().NetCommSend(type, pairData);
    if (ret != VMI_SUCCESS) {
        ERR("Send data(type: %u) failed, ret: %u", type, ret);
        return VMI_CLIENT_SEND_FAIL;
    }
    return VMI_SUCCESS;
}
