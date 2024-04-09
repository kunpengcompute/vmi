/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2023. All rights reserved.
 * Description: Video Engine Client - DecoderMedia
 */

#ifndef DECODER_H
#define DECODER_H

#include <media/NdkMediaCodec.h>
#include <media/NdkMediaFormat.h>
#include <mutex>
#include <deque>
#include <ItemQueue.h>
#include "DecoderBase.h"

namespace Vmi {
class DecoderMedia : public DecoderBase {
public:

    DecoderMedia() = default;
    ~DecoderMedia() override;
    DecoderRetCode Init(DecoderType type, uint64_t surface, int32_t rotation) override;
    DecoderRetCode Start() override;
    DecoderRetCode Input(std::pair<uint8_t *, uint32_t> &packetPair) override;
    DecoderRetCode OutputAndDisplay()  override;
    uint32_t GetFps() override;
    void Destroy() override;

private:
    /*
     * @fn CreateMediaCodecFmt
     * @brief to create AMediaFormat for MediaCodec configuration.
    * @param[in] rotation: rotation to display
     * @return AMediaFormat for MediaCodec configuration
     */
    AMediaFormat *CreateMediaCodecFmt(int32_t rotation) const;

    /*
     * @fn GetInputBuffer
     * @brief to get input buffer allocated by MediaCodec
     * @param[out] bufSize size of input buffer allocated by MediaCodec of (type <tt>size_t</tt>)
     * @param[out] bufId id of the input buffer of (type <tt>size_t</tt>)
     * @return pointer to the input buffer, nullptr when failure
     */
    uint8_t *GetInputBuffer(size_t &bufSize, ssize_t &bufId) const;

    /*
     * @fn GetSdkVersion
     * @brief to read the sdk version
     * @return sdk Veriosn No
     */
    int32_t GetSdkVersion();

    /*
     * @fn CheckSDKVersion
     * @brief to check if the sdn version is Android 7.0 or above
     * @return true if sdk version supported
     */
    bool CheckSDKVersion();

    /*
     * @fn Handle Buffer Id
     * @brief to process brachees when bufId < 0 in dequeueing output buffer
     * @return void
     */
    void HandleBufferId(const ssize_t bufId);

        /*
     * @fn SetFps
     * @brief to push latest frame TS back into queue
     * @param[in] latest timestamp of (type <tt>timestamp_t</tt>)
     */
    void SetTimestamp(int64_t timeStamp);

    /*
     * @fn Release
     * @brief to release VideoUtil resource
     */
    void Release();
    
    AMediaCodec *m_mediaCodec {nullptr};
    DecoderType m_decoderType = DecoderType::DEC_TYPE_UNKNOWN;
    mutable std::mutex m_lock = {};
    ItemQueue<int64_t> m_frameQueue {};
};
}

#endif // DECODER_H
