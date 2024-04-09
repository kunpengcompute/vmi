/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2023. All rights reserved.
 * Description: Video Engine Client - Decoder
 *
 * AMediaFormat_delete returns OK internally, so no need to check return value.
 * AMediaCodec_delete returns OK internally, so no need to check return value.
 */

#include "DecoderMedia.h"

#include <algorithm>
#include <climits>
#include <vector>
#include <string>
#include <unordered_map>
#include <sys/system_properties.h>
#include <cinttypes>

#include "logging.h"

namespace Vmi {
namespace {
constexpr int KEY_WIDTH = 720;
constexpr int KEY_HEIGHT = 1280;
std::unordered_map<DecoderType, std::string> DecTypeNames = {
    {DecoderType::DEC_TYPE_H264, "video/avc"},
    {DecoderType::DEC_TYPE_H265, "video/hevc"}
};
// the end-of-picture bits appending to the origin h264 frame
const std::vector<uint8_t> H264_NAL_EOPIC = { 0x00, 0x00, 0x01, 0x1D, 0x00, 0x00, 0x01, 0x1E,
                                              0x48, 0x53, 0x50, 0x49, 0x43, 0x45, 0x4E, 0x44 };
const int32_t SDK_VERSION_BASELINE = 24;
const int32_t DECIMAL = 10;
// This indicates that the buffer only contains part of a frame, and
// the decoder should batch the data until a buffer
// without this flag appears before decoding the frame.
const uint32_t BUFFER_FLAG_PARTIAL_FRAME = 8;
const uint32_t RETRY_LIMITS = 3;  // 获取解码输入buffer的重试次数，兼容解码性能弱的场景，保证可用性
const int64_t MAX_INPUT_TIMEOUT_USEC = -1; // -1: block input
const int64_t MAX_OUTPUT_TIMEOUT_USEC = 1000LL; // 1ms
const int64_t KILO = 1000LL;
constexpr uint64_t DURATION_USEC = 1000000ULL;

void onOutputFormatChanged(AMediaCodec *mediaCodec)
{
    AMediaFormat *fmt = AMediaCodec_getOutputFormat(mediaCodec);
    if (fmt != nullptr) {
        INFO("Output format change to: %s.", AMediaFormat_toString(fmt));
        AMediaFormat_delete(fmt);
    } else {
        INFO("Output format change to nullptr.");
    }
}
} // namespace

/*
 * @fn ~Decoder
 * @brief destructor
 */
DecoderMedia::~DecoderMedia()
{
    Destroy();
}

/*
 * @fn Init
 * @brief to create and configure MediaCodec.
 * @param[in] type: decoder type(avc/hevc)
 * @param[in] surface: view to display
 * @param[in] rotation: rotation to display
 * @return errno: VMI_DECODER_SUCCESS
 * VMI_DECODER_INIT_ERR
 * VMI_DECODER_SDK_UNSUPPORTED
 */
DecoderRetCode DecoderMedia::Init(DecoderType type, uint64_t surface, int32_t rotation)
{
    if (!CheckSDKVersion()) {
        return VMI_DECODER_SDK_UNSUPPORTED;
    }
    m_decoderType = type;
    m_mediaCodec = AMediaCodec_createDecoderByType(DecTypeNames[type].c_str());
    if (m_mediaCodec == nullptr) {
        ERR("Failed to create MediaCodec.");
        return VMI_DECODER_INIT_ERR;
    }
    AMediaFormat *fmt = CreateMediaCodecFmt(rotation);
    if (fmt == nullptr) {
        ERR("Failed to create MediaCodec format.");
        return VMI_DECODER_INIT_ERR;
    }
    ANativeWindow* nativeWindow = reinterpret_cast<ANativeWindow*>(surface);
    int32_t rc = AMediaCodec_configure(m_mediaCodec, fmt, nativeWindow, nullptr, 0);
    AMediaFormat_delete(fmt);
    if (rc != AMEDIA_OK) {
        ERR("Failed to configure MediaCodec, errno: %d.", rc);
        return VMI_DECODER_INIT_ERR;
    }
    INFO("Initialise MediaCodec success.");
    return VMI_DECODER_SUCCESS;
}

/*
 * @fn Start
 * @brief to start MediaCodec
 * @return errno: VMI_DECODER_SUCCESS
 * VMI_DECODER_START_ERR
 */
DecoderRetCode DecoderMedia::Start()
{
    int32_t rc = AMediaCodec_start(m_mediaCodec);
    if (rc != AMEDIA_OK) {
        ERR("Failed to start MediaCodec, errno: %d.", rc);
        return VMI_DECODER_START_ERR;
    }
    INFO("Start MediaCodec success.");
    return VMI_DECODER_SUCCESS;
}

/*
 * @fn Input
 * @brief to place processing data into InputBuffer, and MediaCodec would
 * fetch it to process asynchronously.
 * @param[in] packetPair Initial address and length of processing data
 * @return errno: VMI_DECODER_SUCCESS
 * VMI_DECODER_INPUT_ERR
 */
DecoderRetCode DecoderMedia::Input(std::pair<uint8_t *, uint32_t> &packetPair)
{
    uint8_t *buf = packetPair.first;
    uint32_t len = packetPair.second;
    size_t dataLen = len; // size in byte
    std::vector<uint8_t> data(dataLen);
    if (m_decoderType == DecoderType::DEC_TYPE_H264) {
        dataLen += H264_NAL_EOPIC.size();
        data.resize(dataLen);
        (void)std::copy(H264_NAL_EOPIC.begin(), H264_NAL_EOPIC.end(), data.begin() + len);
    }
    (void)std::copy(buf, buf + len, data.begin());
    size_t bufSize = 0;
    ssize_t bufId = 0;
    size_t feedLen = 0;
    for (size_t pos = 0; pos != dataLen;) {
        uint8_t *ptr = GetInputBuffer(bufSize, bufId);
        if (ptr == nullptr) {
            ERR("Failed to get input buffer.");
            return VMI_DECODER_INPUT_ERR;
        }
        feedLen = (dataLen - pos) > bufSize ? bufSize : (dataLen - pos);
        (void)std::copy_n(data.begin() + pos, feedLen, ptr);
        pos += feedLen;
        // Send the specified buffer to the codec for processing.
        auto currTime = std::chrono::steady_clock::now();
        auto now = std::chrono::duration_cast<std::chrono::microseconds>(currTime.time_since_epoch()).count();
        
        int rc = AMediaCodec_queueInputBuffer(m_mediaCodec, bufId, 0, feedLen, now,
            pos != dataLen ? BUFFER_FLAG_PARTIAL_FRAME : 0);
        if (rc != AMEDIA_OK) {
            ERR("Failed to queue input buffer, errno: %d id: %zd.", rc, bufId);
            return VMI_DECODER_INPUT_ERR;
        }
        DBG("Frame(%lld) @buffer(%zd) input success.", now, bufId);
    }
    return VMI_DECODER_SUCCESS;
}

/*
 * @fn Handle Buffer Id
 * @brief to process brachees when bufId < 0 in dequeueing output buffer
 * @return void
 */
void DecoderMedia::HandleBufferId(const ssize_t bufId)
{
    switch (bufId) {
        case AMEDIACODEC_INFO_OUTPUT_BUFFERS_CHANGED:
            INFO("Output buffers changed.");
            break;
        case AMEDIACODEC_INFO_OUTPUT_FORMAT_CHANGED:
            onOutputFormatChanged(m_mediaCodec);
            break;
        case AMEDIACODEC_INFO_TRY_AGAIN_LATER:
            DBG("Retry to get output buffer.");
            break;
        default:
            ERR("Unknown Error id: %zd.", bufId);
            return;
    }
}

/*
 * @fn OutputAndDisplay
 * @brief to output processed data into OutputBuffer from MediaCodec,
 * and release OutputBuffer to render for display
 * @return errno: VMI_DECODER_SUCCESS
 * VMI_DECODER_OUTPUT_RETRY
 * VMI_DECODER_OUTPUT_ERR
 */
DecoderRetCode DecoderMedia::OutputAndDisplay()
{
    AMediaCodecBufferInfo info;
    constexpr uint32_t drainOutBound = 16;
    uint32_t drainOutNum = 0;
    std::vector<ssize_t> bufferIds;
    // Get the index of the next available buffer of processed data.
    ssize_t bufId = AMediaCodec_dequeueOutputBuffer(m_mediaCodec, &info, MAX_OUTPUT_TIMEOUT_USEC);
    while (bufId >= 0) {
        drainOutNum++;
        bufferIds.push_back(bufId);
        bufId = AMediaCodec_dequeueOutputBuffer(m_mediaCodec, &info, MAX_OUTPUT_TIMEOUT_USEC);
        if (drainOutNum > drainOutBound) {
            break;
        }
    }
    if (bufferIds.size() > 0) {
        for (size_t i = 0; i < bufferIds.size() - 1; i++) {
            AMediaCodec_releaseOutputBuffer(m_mediaCodec, bufferIds[i], false);
        }
        ssize_t lastBufferId = bufferIds[bufferIds.size() - 1];
        int64_t frameUs = info.presentationTimeUs;
        auto currTime = std::chrono::steady_clock::now();
        auto nowUs = static_cast<int64_t>(std::chrono::duration_cast<std::chrono::microseconds>(currTime.time_since_epoch()).count());
        
        if (nowUs > frameUs) {
            INFO("Frame(%" PRId64 ") @buffer(%zd) output success, decode latency %ju ms", frameUs, lastBufferId,
                (nowUs - frameUs) / KILO);
        }
        SetTimestamp(nowUs);
        // If you are done with a buffer, use this call to return the buffer to the codec.
        // If you have not specified an output surface when configuring this video codec,
        // this call will simply return the buffer to the codec.
        AMediaCodec_releaseOutputBuffer(m_mediaCodec, lastBufferId, true);
        return VMI_DECODER_SUCCESS;
    }
    HandleBufferId(bufId);
    return VMI_DECODER_OUTPUT_RETRY;
}

/*
 * @fn GetFps
 * @brief to get the realtime fps
 * @return uint32_t, fps
 */
uint32_t DecoderMedia::GetFps()
{
    uint32_t fps = 0;
    std::lock_guard<std::mutex> lockGuard(m_lock);
    if (!m_frameQueue.IsEmpty()) {
        auto currTime = std::chrono::steady_clock::now();
        auto now = std::chrono::duration_cast<std::chrono::microseconds>(currTime.time_since_epoch()).count();

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
 * @fn Destroy
 * @brief to destroy MediaCodec, and release the resources it possesses.
 * @return uint32_t, errno: VMI_DECODER_SUCCESS
 */
void DecoderMedia::Destroy()
{
    if (m_mediaCodec != nullptr) {
        AMediaCodec_stop(m_mediaCodec);
        AMediaCodec_delete(m_mediaCodec);
        m_mediaCodec = nullptr;
    }
    Release();
    INFO("Destroy MediaCodec success.");
}

/*
 * @fn CreateMediaCodecFmt
 * @brief to create AMediaFormat for MediaCodec configuration.
 * @param[in] rotation: rotation to display
 * @return AMediaFormat *, AMediaFormat object for MediaCodec configuration
 */
AMediaFormat *DecoderMedia::CreateMediaCodecFmt(int32_t rotation) const
{
    AMediaFormat *fmt = AMediaFormat_new();
    if (fmt != nullptr) {
        AMediaFormat_setString(fmt, AMEDIAFORMAT_KEY_MIME, DecTypeNames[m_decoderType].c_str());
        AMediaFormat_setInt32(fmt, AMEDIAFORMAT_KEY_WIDTH, KEY_WIDTH);
        AMediaFormat_setInt32(fmt, AMEDIAFORMAT_KEY_HEIGHT, KEY_HEIGHT);
        AMediaFormat_setInt32(fmt, "rotation-degrees", rotation);
    }
    INFO("Create MediaCodec format success.");
    return fmt;
}

/*
 * @fn GetInputBuffer
 * @brief to append h264 frame bits with the eopic bits
 * @param[out] bufSize size of input buffer allocated by MediaCodec of (type <tt>size_t</tt>)
 * @param[out] bufId id of the input buffer of (type <tt>size_t</tt>)
 * @return pointer to the input buffer, nullptr when failure
 */
uint8_t *DecoderMedia::GetInputBuffer(size_t &bufSize, ssize_t &bufId) const
{
    int retriedTimes = 0;
    do {
        // Get the index of the next available input buffer.
        // An app will typically use this with getInputBuffer() to get a pointer to the buffer,
        // then copy the data to be encoded or decoded into the buffer before passing it
        // to the codec.
        bufId = AMediaCodec_dequeueInputBuffer(m_mediaCodec, MAX_INPUT_TIMEOUT_USEC);
        if (bufId == -1) {
            if (++retriedTimes == RETRY_LIMITS) {
                ERR("Failed to re-dequeue input buffer, id: %zd.", bufId);
                return nullptr;
            }
            DBG("No Available Buffer, retried %d times to dequeue input buffer.", retriedTimes);
        }
        if (bufId < -1) {
            ERR("Failed to dequeue input buffer, id: %zd.", bufId);
            return nullptr;
        }
    } while (bufId < 0);
    // Get an input buffer.
    // The specified buffer index must have been previously obtained from dequeueInputBuffer,
    // and not yet queued.
    return AMediaCodec_getInputBuffer(m_mediaCodec, bufId, &bufSize);
}

/*
 * @fn CheckSDKVersion
 * @brief to check if the sdn version is Android 7.0 or above
 * @return true if sdk version supported
 */
bool DecoderMedia::CheckSDKVersion()
{
    return GetSdkVersion() >= SDK_VERSION_BASELINE;
}

int32_t DecoderMedia::GetSdkVersion()
{
    char sdkVersion[PROP_VALUE_MAX] = {0};
    int len = __system_property_get("ro.build.version.sdk", sdkVersion);
    if (len == 0) {
        return 0;
    }
    int32_t result = 0;
    for (int i = 0; i < len; ++i) {
        int digit = sdkVersion[i] - '0';
        if (digit < 0 || digit > 9) { // make sure the num is within [0,9]
            // Non-numeric SDK version, assume it's higher than expected;
            return INT32_MAX;
        }
        result = result * DECIMAL + digit;
    }
    if (result == 28) { // Android 9 (API level 28)
        char versionCodeName[PROP_VALUE_MAX] = {0};
        len = __system_property_get("ro.build.version.codename", versionCodeName);
        if (len > 0) {
            if (versionCodeName[0] == 'Q') {
                return 29; // Android 10 (API level 29)
            }
        }
    }
    return result;
}

/*
 * @fn SetFps
 * @brief to push latest frame TS back into queue
 * @param[in] latest timeStamp of (type <tt>timestamp_t</tt>)
 */
void DecoderMedia::SetTimestamp(int64_t timeStamp)
{
    std::lock_guard<std::mutex> lockGuard(m_lock);
    m_frameQueue.PutItem(timeStamp);
}

/*
 * @fn Release
 * @brief to release VideoUtil resource
 */
void DecoderMedia::Release()
{
    m_frameQueue.ClearQueue();
}
}
