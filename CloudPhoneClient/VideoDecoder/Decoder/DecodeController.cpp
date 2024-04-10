/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2021. All rights reserved.
 * Description: Video Engine Client - DecodeController
 */
#define LOG_TAG "DecodeController"
#include "DecodeController.h"
#include <unistd.h>
#include <dlfcn.h>
#include <functional>
#include "logging.h"

namespace Vmi {
namespace {
constexpr uint32_t DATA_LENGTH_MAX = 10485760; // 10MB
const std::string SHARED_LIB_NAME = "libNdkDecoder.so";
const std::string CREATE_NDK_DECODER = "CreateVideoDecoder";
const std::string DESTROY_NDK_DECODER = "DestroyVideoDecoder";
}

/*
 * @fn DecodeController
 * @brief constructor
 */
DecodeController::DecodeController() {}

/*
 * @fn ~DecodeController
 * @brief destructor
 */
DecodeController::~DecodeController()
{
    if (m_destroyNdkDecoder != nullptr) {
        (void)(*m_destroyNdkDecoder)(m_decoder);
    } else {
        ERR("destroy NdkDecoder failed");
    }

    UnloadNdkDecoderSymbols();
    Destroy();
}

/*
 * @fn GetInstance
 * @brief to get DecodeController singleton
 */
DecodeController &DecodeController::GetInstance()
{
    static DecodeController instance;
    return instance;
}

/*
 * @brief act as a thread entry to output processed data and to display
 */
void DecodeController::OutputTaskEntry()
{
    while (m_engineStatus == EngineStat::VMI_ENGINE_RUNNING) {
        if (OutputAndDisplay() == VMI_DECODER_OUTPUT_ERR) {
            ERR("Sub-Thread exited.");
            break;
        }
    }
    SetSubThreadStatus(false);
}

/*
 * @fn Init
 * @brief to initialise DecodeController instance
 * @param[in] type: decoder type(avc/hevc)
 * @param[in] surface: view to display
 * @param[in] rotation: rotation to display
 * @return errno: VMI_VIDEO_ENGINE_CLIENT_SUCCESS
 * VMI_VIDEO_ENGINE_CLIENT_INIT_FAIL
 * VMI_VIDEO_ENGINE_CLIENT_SDK_UNSUPPORTED
 */
uint32_t DecodeController::Init(DecoderType type, uint64_t surface, int32_t rotation)
{
    if (!IsStatus(EngineStat::VMI_ENGINE_INVALID)) {
        ERR("Destroy needed first.");
        return VMI_VIDEO_ENGINE_CLIENT_INIT_FAIL;
    }
    if (!m_ndkDecoderLoaded) {
        if (!LoadNdkDecoderSymbols()) {
            ERR("load ndk decoder shared lib failed");
            return -1;
        }
    }
    DecoderRetCode ret = (*m_createNdkDecoder)(&m_decoder);
    if (ret != VMI_DECODER_SUCCESS || m_decoder == nullptr) {
        ERR("Failed to instantiate Decoder.");
        return VMI_VIDEO_ENGINE_CLIENT_INIT_FAIL;
    }
    ret = m_decoder->Init(type, surface, rotation);
    if (ret != VMI_DECODER_SUCCESS) {
        if (ret == VMI_DECODER_SDK_UNSUPPORTED) {
            ERR("Unsupported SDK version.");
            return VMI_VIDEO_ENGINE_CLIENT_SDK_UNSUPPORTED;
        }
        ERR("Failed to initialise Decoder.");
        return VMI_VIDEO_ENGINE_CLIENT_INIT_FAIL;
    }
    SetStatus(EngineStat::VMI_ENGINE_INIT);
    INFO("Init success.");
    return VMI_VIDEO_ENGINE_CLIENT_SUCCESS;
}

/*
 * @fn Start
 * @brief to start DecodeController instance
 * @return errno: VMI_VIDEO_ENGINE_CLIENT_SUCCESS
 * VMI_VIDEO_ENGINE_CLIENT_START_ERR
 */
uint32_t DecodeController::Start()
{
    INFO("DecodeController Start");
    if (IsStatus(EngineStat::VMI_ENGINE_INVALID)) {
        ERR("Init needed first.");
        return VMI_VIDEO_ENGINE_CLIENT_START_ERR;
    }
    if (IsStatus(EngineStat::VMI_ENGINE_RUNNING)) {
        ERR("Already started.");
        return VMI_VIDEO_ENGINE_CLIENT_START_ERR;
    }
    if (IsStatus(EngineStat::VMI_ENGINE_INIT)) {
        if (m_decoder->Start() != VMI_DECODER_SUCCESS) {
            ERR("Failed to start Decoder.");
            return VMI_VIDEO_ENGINE_CLIENT_START_ERR;
        }
    }
    SetSubThreadStatus(true);
    SetStatus(EngineStat::VMI_ENGINE_RUNNING);
    // to start sub-thread for the output of decoded frame
    m_outputAndDisplay = std::thread(std::bind(&DecodeController::OutputTaskEntry, this));
    INFO("DecodeController Start success.");
    return VMI_VIDEO_ENGINE_CLIENT_SUCCESS;
}

/*
 * @fn Decode
 * @brief DecodeController instance places H264 into Decoder
 * @param[in] packetPair Initial address and length of current frame
 * @return errno: VMI_VIDEO_ENGINE_CLIENT_SUCCESS
 * VMI_VIDEO_ENGINE_CLIENT_DECODE_ERR
 * VMI_VIDEO_ENGINE_CLIENT_PARAM_INVALID
 */
uint32_t DecodeController::Decode(std::pair<uint8_t *, uint32_t> &packetPair)
{
    if (packetPair.first == nullptr || packetPair.second == 0) {
        ERR("No Input Data.");
        return VMI_VIDEO_ENGINE_CLIENT_PARAM_INVALID;
    }
    if (packetPair.second > DATA_LENGTH_MAX) {
        ERR("Exceed max data length");
        return VMI_VIDEO_ENGINE_CLIENT_PARAM_INVALID;
    }
    if (!IsStatus(EngineStat::VMI_ENGINE_RUNNING)) {
        ERR("Not running.");
        return VMI_VIDEO_ENGINE_CLIENT_DECODE_ERR;
    }
    if (!IsSubThreadRunning()) {
        ERR("Sub-Thread not running. Stopping decoder.");
        (void)Stop();
        return VMI_VIDEO_ENGINE_CLIENT_DECODE_ERR;
    }
    if (m_decoder->Input(packetPair) != VMI_DECODER_SUCCESS) {
        ERR("Failed to process input data.");
        return VMI_VIDEO_ENGINE_CLIENT_DECODE_ERR;
    }
    return VMI_VIDEO_ENGINE_CLIENT_SUCCESS;
}

/*
 * @fn Stop
 * @brief to stop DecodeController instance
 * @return errno: VMI_VIDEO_ENGINE_CLIENT_SUCCESS
 * VMI_VIDEO_ENGINE_CLIENT_STOP_ERR
 */
uint32_t DecodeController::Stop()
{
    if (IsStatus(EngineStat::VMI_ENGINE_RUNNING)) {
        SetStatus(EngineStat::VMI_ENGINE_STOP);
        if (m_outputAndDisplay.joinable()) {
            m_outputAndDisplay.join();
        }
        INFO("Stop success.");
        return VMI_VIDEO_ENGINE_CLIENT_SUCCESS;
    }
    ERR("Invalid Status.");
    return VMI_VIDEO_ENGINE_CLIENT_STOP_ERR;
}

/*
 * @fn Destroy
 * @brief to destroy DecodeController instance
 */
void DecodeController::Destroy()
{
    if (Stop() == VMI_VIDEO_ENGINE_CLIENT_STOP_ERR) {
        // The invocation to check the necessity to stop Decoder.
        // For information, log level is defined INFO.
        INFO("Not Running");
    }
    // to release decoder
    if (m_decoder != nullptr) {
        m_decoder->Destroy();
    }
    SetStatus(EngineStat::VMI_ENGINE_INVALID);
    INFO("Destroy success.");
}

/*
 * @fn SetSubThreadStatus
 * @brief to set DecodeController sub-thread work status
 * @param[in] status, true: sub-thread running, false: sub-thread stopped
 * @return void
 */
void DecodeController::SetSubThreadStatus(bool status)
{
    m_subThreadRunning = status;
}

/*
 * @fn IsSubThreadRunning
 * @brief to check if DecodeController sub-thread running
 * @return true: sub-thread running, false: sub-thread stopped
 */
bool DecodeController::IsSubThreadRunning() const
{
    return m_subThreadRunning;
}

/*
 * @fn GetStatus
 * @brief to get DecodeController instance status
 * @return EngineStat
 */
uint32_t DecodeController::GetStatus(EngineStat &status) const
{
    status = m_engineStatus;
    return VMI_VIDEO_ENGINE_CLIENT_SUCCESS;
}

/*
 * @fn SetStatus
 * @brief to set DecodeController instance status
 * @param[in] stat DecodeController instance status, pass by value, of (type <tt>enum EngineStat</tt>)
 * @return void
 */
void DecodeController::SetStatus(const EngineStat stat)
{
    m_engineStatus = stat;
}

/*
 * @fn IsStatus
 * @brief to judge DecodeController instance status
 * @param[in] stat DecodeController instance status, pass by value, of (type <tt>enum EngineStat</tt>)
 * @return bool
 */
bool DecodeController::IsStatus(const EngineStat stat) const
{
    return m_engineStatus == stat;
}

/*
 * @fn GetStatics
 * @brief to get DecodeController instance statistics information
 * @return DecoderStatics
 */
uint32_t DecodeController::GetStatistics(DecoderStatistics &statistics) const
{
    DecoderStatistics decoderStatistics = { 0 };
    if (IsStatus(EngineStat::VMI_ENGINE_RUNNING)) {
        decoderStatistics.decodeFps = m_decoder->GetFps();
    }
    INFO("Decode Frame Rate: %u fps", decoderStatistics.decodeFps);
    statistics = decoderStatistics;
    return VMI_VIDEO_ENGINE_CLIENT_SUCCESS;
}

uint32_t DecodeController::OutputAndDisplay()
{
    if (m_decoder != nullptr) {
        return m_decoder->OutputAndDisplay();
    }
    return VMI_DECODER_OUTPUT_ERR;
}

bool DecodeController::LoadNdkDecoderSymbols()
{
    INFO("load %s", SHARED_LIB_NAME.c_str());
    m_libHandle = dlopen(SHARED_LIB_NAME.c_str(), RTLD_NOW);
    if (m_libHandle == nullptr) {
        auto errStr = dlerror();
        ERR("load %s error:%s", SHARED_LIB_NAME.c_str(), (errStr != nullptr) ? errStr : "unknown");
        UnloadNdkDecoderSymbols();
        return false;
    }

    m_createNdkDecoder =
        reinterpret_cast<CreateNdkDecoderFuncPtr>(dlsym(m_libHandle, CREATE_NDK_DECODER.c_str()));
    if (m_createNdkDecoder == nullptr) {
        ERR("failed to load CreateNdkDecoder");
        UnloadNdkDecoderSymbols();
        return false;
    }

    m_destroyNdkDecoder =
        reinterpret_cast<DestroyNdkDecoderFuncPtr>(dlsym(m_libHandle, DESTROY_NDK_DECODER.c_str()));
    if (m_destroyNdkDecoder == nullptr) {
        ERR("failed to load DestroyNdkDecoder");
        UnloadNdkDecoderSymbols();
        return false;
    }
    m_ndkDecoderLoaded = true;
    return true;
}

void DecodeController::UnloadNdkDecoderSymbols()
{
    if (m_libHandle != nullptr) {
        DBG("unload %s", SHARED_LIB_NAME.c_str());
        (void)dlclose(m_libHandle);
        m_libHandle = nullptr;
    }
    m_createNdkDecoder = nullptr;
    m_destroyNdkDecoder = nullptr;
}
}
