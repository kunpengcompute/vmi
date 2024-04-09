/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2021. All rights reserved.
 * Description: Video Engine Client - DecodeController
 */

#ifndef DECODECONTROLLER_H
#define DECODECONTROLLER_H

#include <atomic>
#include <thread>
#include "DecoderBase.h"

namespace Vmi {
constexpr uint32_t VMI_VIDEO_ENGINE_CLIENT_SUCCESS = 0;
constexpr uint32_t VMI_VIDEO_ENGINE_CLIENT_INIT_FAIL = 0x0A070001;
constexpr uint32_t VMI_VIDEO_ENGINE_CLIENT_PARAM_INVALID = 0x0A070002;
constexpr uint32_t VMI_VIDEO_ENGINE_CLIENT_PARAM_UNSUPPORTED = 0x0A070003;
constexpr uint32_t VMI_VIDEO_ENGINE_CLIENT_START_ERR = 0x0A070004;
constexpr uint32_t VMI_VIDEO_ENGINE_CLIENT_DECODE_ERR = 0x0A070005;
constexpr uint32_t VMI_VIDEO_ENGINE_CLIENT_STOP_ERR = 0x0A070006;
constexpr uint32_t VMI_VIDEO_ENGINE_CLIENT_SDK_UNSUPPORTED = 0x0A07007;

// Video Engine Decoder Statistics
struct DecoderStatistics {
    uint32_t decodeFps;
};

// Video Engine Status
enum class EngineStat {
    VMI_ENGINE_INIT = 0x01,
    VMI_ENGINE_RUNNING = 0x02,
    VMI_ENGINE_STOP = 0x03,
    VMI_ENGINE_INVALID = 0x0
};
class DecodeController {
public:
    /*
     * @fn GetInstance
     * @brief to get DecodeController singleton
     */
    static DecodeController &GetInstance();

    /*
     * @fn Init
     * @brief to initialise DecodeController instance
     * @param[in] type: decoder type(avc/hevc)
     * @param[in] surface: view to display
     * @param[in] rotation: rotation to display
     * @return errno: VMI_VIDEO_ENGINE_CLIENT_SUCCESS
     *                VMI_VIDEO_ENGINE_CLIENT_INIT_FAIL
     *                VMI_VIDEO_ENGINE_CLIENT_SDK_UNSUPPORTED
     */
    uint32_t Init(DecoderType type, uint64_t surface, int32_t rotation);

    /*
     * @fn Start
     * @brief to start DecodeController instance
     * @return errno: VMI_VIDEO_ENGINE_CLIENT_SUCCESS
     *                VMI_VIDEO_ENGINE_CLIENT_START_ERR
     */
    uint32_t Start();

    /*
     * @fn Decode
     * @brief DecodeController instance places H264 into Decoder
     * @param[in] packetPair Initial address and length of current frame
     * @return errno: VMI_VIDEO_ENGINE_CLIENT_SUCCESS
     *                VMI_VIDEO_ENGINE_CLIENT_DECODE_ERR
     *                VMI_VIDEO_ENGINE_CLIENT_PARAM_INVALID
     */
    uint32_t Decode(std::pair<uint8_t *, uint32_t> &packetPair);

    /*
     * @fn Stop
     * @brief to stop DecodeController instance
     * @return errno: VMI_VIDEO_ENGINE_CLIENT_SUCCESS
     *                VMI_VIDEO_ENGINE_CLIENT_STOP_ERR
     */
    uint32_t Stop();

    /*
     * @fn Destroy
     * @brief to destroy DecodeController instance
     */
    void Destroy();

    /*
     * @fn SetSubThreadStatus
     * @brief to set DecodeController sub-thread work status
     * @param[in] status, true: sub-thread running, false: sub-thread stopped
     * @return void
     */
    void SetSubThreadStatus(bool status);

    /*
     * @fn IsSubThreadRunning
     * @brief to check if DecodeController sub-thread running
     * @return true: sub-thread running, false: sub-thread stopped
     */
    bool IsSubThreadRunning() const;

    /*
     * @fn GetStatus
     * @brief to get DecodeController instance status
     * @return EngineStat
     */
    uint32_t GetStatus(EngineStat &status) const;

    /*
     * @fn SetStatus
     * @brief to set DecodeController instance status
     * @param[in] stat DecodeController instance status, pass by value, of (type <tt>enum EngineStat</tt>)
     * @return void
     */
    void SetStatus(const EngineStat stat);

    /*
     * @fn GetStatics
     * @brief to get DecodeController instance statistics information
     * @return DecoderStatics
     */
    uint32_t GetStatistics(DecoderStatistics &statistics) const;

    uint32_t OutputAndDisplay();
private:
    /*
     * @fn DecodeController
     * @brief constructor
     */
    DecodeController();

    /*
     * @fn ~DecodeController
     * @brief destructor
     */
    ~DecodeController();

    /*
     * @fn copy constructor
     * @brief forbidden by "= delete"
     */
    DecodeController(const DecodeController &) = delete;

    /*
     * @fn move constructor
     * @brief forbidden by "= delete"
     */
    DecodeController(DecodeController &&) = delete;

    /*
     * @fn copy operator
     * @brief forbidden by "= delete"
     */
    DecodeController &operator = (const DecodeController &) = delete;

    /*
     * @fn move operator
     * @brief forbidden by "= delete"
     */
    DecodeController &operator = (DecodeController &&) = delete;
    /*
     * @fn IsStatus
     * @brief to judge DecodeController instance status
     * @param[in] stat DecodeController instance status, pass by value, of (type <tt>enum EngineStat</tt>)
     * @return bool
     */
    bool IsStatus(const EngineStat stat) const;

    /*
    * @brief act as a thread entry to output processed data and to display
    */
    void OutputTaskEntry();

    void *m_libHandle = nullptr;
    using CreateNdkDecoderFuncPtr = DecoderRetCode (*)(DecoderBase **decoder);
    using DestroyNdkDecoderFuncPtr = DecoderRetCode (*)(DecoderBase *decoder);
    CreateNdkDecoderFuncPtr m_createNdkDecoder = nullptr;
    DestroyNdkDecoderFuncPtr m_destroyNdkDecoder = nullptr;
    bool m_ndkDecoderLoaded = false;
    bool LoadNdkDecoderSymbols();
    void UnloadNdkDecoderSymbols();

    DecoderBase *m_decoder { nullptr };
    std::atomic<bool> m_subThreadRunning { false };
    std::atomic<EngineStat> m_engineStatus { EngineStat::VMI_ENGINE_INVALID };
    std::thread m_outputAndDisplay;
};
}

#endif // DECODECONTROLLER_H
