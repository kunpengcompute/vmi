/**
 * 版权所有 (c) 华为技术有限公司 2020-2021
 * 功能说明:客户端SDK的Opus的解码和编码JNI函数
 */
#include "JNIOpusWrapper.h"
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "logging.h"

namespace {
    // opus音频采样率
    constexpr int SAMPLE_RATE = 48000;

    // opus音频声道数
    constexpr int CHANNELS = 2;

    // opus音频格式
    constexpr int APPLICATION = OPUS_APPLICATION_AUDIO;

    // opus 压缩的一帧音频数据的每个声道上的采样数 = OPUS_SAMPLING_RATE * 10 ms
    constexpr int FRAME_SIZE = 960;

    // opus 压缩的一帧音频数据的每个声道上的最大采样数 = OPUS_SAMPLING_RATE * 60 ms
    constexpr int MAX_FRAME_SIZE = FRAME_SIZE * 6;

    // opus 存放压缩后音频数据的最大的大小值
    constexpr int MAX_PACKET_SIZE = 3840;

    // opus音频编码器全局指针
    OpusEncoder* g_encoder = nullptr;

    // opus_decoder_create接口返回成功
    constexpr int OK = 0;

    // 函数调用成功返回码
    constexpr int SUCC = 1;

    // 函数调用失败返回码
    constexpr int FAIL = -1;
}  // namespace
/**
 * @功能描述：创建opus解码器，调用处已控制不会重复调用
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] obj：代表native方法的类的class对象实例
 * @返回值：0代表失败，其他值代表opus解码器指针地址
 */
extern "C" JNIEXPORT jlong JNICALL OPUS_JNI(createOpusDecoder)(JNIEnv* /* env */, jobject /* obj */)
{
    int err = FAIL;
    uintptr_t handler = 0;
    OpusDecoder* decoder = OpusDecoderCreateApi(SAMPLE_RATE, CHANNELS, &err);
    if (err != OK) {
        ERR("failed to create decoder: %s", OpusStrerrorApi(err));
        return static_cast<jlong>(handler);
    }
    handler = reinterpret_cast<uintptr_t>(decoder);
    return static_cast<jlong>(handler);
}

/**
 * @功能描述：销毁opus解码器，调用处已控制不会重复调用
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] obj：代表native方法的类的class对象实例
 * @参数 [in] decoder：代表opus解码器指针
 * @返回值：1代表销毁opus解码器成功
 */
extern "C" JNIEXPORT jint JNICALL OPUS_JNI(destroyOpusDecoder)(JNIEnv* /* env */, jobject /* obj */, jlong decoder)
{
    uintptr_t handler = static_cast<uintptr_t>(decoder);
    OpusDecoder* decoderPtr = reinterpret_cast<OpusDecoder*>(handler);
    if (decoderPtr != nullptr) {
        OpusDecoderDestroyApi(decoderPtr);
    }
    return static_cast<jint>(SUCC);
}

/**
 * @功能描述：opus解码函数，将opus音频解码成pcm
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] obj：代表native方法的类的class对象实例
 * @参数 [in] inputBuffer：代表待解码opus音频数据(包含数据头)
 * @参数 [in] inputBufferLen：代表opus纯音频数据长度(不包含数据头长度)
 * @参数 [in] outputBuffer：代表解码出来的pcm数据
 * @返回值：-1代表解码失败，其他代表解码出来的pcm数据长度
 */
extern "C" JNIEXPORT jint JNICALL OPUS_JNI(opusDecode)(JNIEnv* env, jobject /* obj */, jlong decoder,
    jbyteArray inputBuffer, int inputBufferLen, jshortArray outputBuffer)
{
    uintptr_t handler = static_cast<uintptr_t>(decoder);
    OpusDecoder* decoderPtr = reinterpret_cast<OpusDecoder*>(handler);
    if (decoderPtr == nullptr) {
        return static_cast<jint>(FAIL);
    }
    jbyte* inputData = env->GetByteArrayElements(inputBuffer, nullptr);
    if (inputData == nullptr) {
        return static_cast<jint>(FAIL);
    }

    jshort* outputData = env->GetShortArrayElements(outputBuffer, nullptr);
    if (outputData == nullptr) {
        env->ReleaseByteArrayElements(inputBuffer, inputData, 0);
        return static_cast<jint>(FAIL);
    }

    int outLen = 0;
    constexpr int bufferHeadSize = 30; // inputBuffer包含30字节的数据头
    if (inputBufferLen > 0) {
        int decodeLen = OpusDecodeApi(decoderPtr, reinterpret_cast<unsigned char*>(inputData + bufferHeadSize),
            inputBufferLen, reinterpret_cast<opus_int16*>(outputData), MAX_FRAME_SIZE, 0);
        if (decodeLen < 0) {
            env->ReleaseByteArrayElements(inputBuffer, inputData, 0);
            env->ReleaseShortArrayElements(outputBuffer, outputData, 0);
            ERR("Failed to decode, opusDecode decodeLen: %d, inputBufferLen=%d", decodeLen, inputBufferLen);
            return static_cast<jint>(FAIL);
        }
        // decodeLen为解码后一帧PCM格式音频据长度，最大为MAX_FRAME_SIZE，乘以CHANNELS（最大为2）也不会发生整数溢出情况
        outLen += reinterpret_cast<int>(decodeLen * CHANNELS);
    }
    env->ReleaseByteArrayElements(inputBuffer, inputData, 0);
    env->ReleaseShortArrayElements(outputBuffer, outputData, 0);
    return static_cast<jint>(outLen);
}

/**
 * @功能描述：创建opus编码器函数
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] obj：代表native方法的类的class对象实例
 * @参数 [in] sampleRate：代表opus音频的编码采样率
 * @参数 [in] channels：代表opus音频的声道数
 * @参数 [in] bitrate：代表opus音频的比特率
 * @返回值：-1代表创建编码器失败，1代表创建成功
 */
extern "C" JNIEXPORT jint JNICALL OPUS_JNI(createOpusEncoder)(JNIEnv* /* env */, jobject /* obj */, jint sampleRate,
    jint channels, jint bitrate)
{
    // 防止重复调用
    if (g_encoder != nullptr) {
        return static_cast<jint>(FAIL);
    }
    int err = FAIL;
    g_encoder = OpusEncoderCreateApi(sampleRate, channels, APPLICATION, &err);
    if (err < 0) {
        ERR("Failed to create opus encoder, failed to call create api: %s", OpusStrerrorApi(err));
        if (g_encoder != nullptr) {
            OpusEncoderDestroyApi(g_encoder);
        }
        g_encoder = nullptr;
        return static_cast<jint>(FAIL);
    }

    err = OpusEncoderCtlApi(g_encoder, OPUS_SET_BITRATE(bitrate));
    if (err < 0) {
        ERR("Failed to create opus encoder, failed to set bitrate(%d): %s", bitrate, OpusStrerrorApi(err));
        if (g_encoder != nullptr) {
            OpusEncoderDestroyApi(g_encoder);
        }
        g_encoder = nullptr;
        return static_cast<jint>(FAIL);
    }
    OpusEncoderCtlApi(g_encoder, OPUS_SET_BANDWIDTH(OPUS_BANDWIDTH_FULLBAND));
    OpusEncoderCtlApi(g_encoder, OPUS_SET_VBR(0));
    OpusEncoderCtlApi(g_encoder, OPUS_SET_VBR_CONSTRAINT(1));
    OpusEncoderCtlApi(g_encoder, OPUS_SET_COMPLEXITY(9));
    OpusEncoderCtlApi(g_encoder, OPUS_SET_PACKET_LOSS_PERC(0));
    return static_cast<jint>(SUCC);
}

/**
 * @功能描述：销毁opus编码器函数
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] obj：代表native方法的类的class对象实例
 * @返回值：1代表销毁opus编码器成功
 */
extern "C" JNIEXPORT jint JNICALL OPUS_JNI(destroyOpusEncoder)(JNIEnv* /* env */, jobject /* obj */)
{
    if (g_encoder != nullptr) {
        OpusEncoderDestroyApi(g_encoder);
    }
    g_encoder = nullptr;
    return static_cast<jint>(SUCC);
}

/**
 * @功能描述：opus编码函数，将pcm编码成opus格式
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] obj：代表native方法的类的class对象实例
 * @参数 [in] inputBuffer：代表待编码的pcm数据
 * @参数 [in] frameSize：代表pcm数据大小
 * @参数 [in] outBuffer：代表opus音频数据
 * @返回值：-1代表编码失败，其他代表opus音频数据长度
 */
extern "C" JNIEXPORT jint JNICALL OPUS_JNI(opusEncode)(JNIEnv* env, jobject /* obj */, jshortArray inputBuffer,
                                                       jint frameSize, jbyteArray outBuffer)
{
    jshort* inputData = env->GetShortArrayElements(inputBuffer, nullptr);
    jbyte* outputData = env->GetByteArrayElements(outBuffer, nullptr);

    if (inputData == nullptr) {
        env->ReleaseShortArrayElements(inputBuffer, inputData, 0);
        env->ReleaseByteArrayElements(outBuffer, outputData, 0);
        return static_cast<jint>(FAIL);
    }

    if (outputData == nullptr) {
        env->ReleaseShortArrayElements(inputBuffer, inputData, 0);
        env->ReleaseByteArrayElements(outBuffer, outputData, 0);
        return static_cast<jint>(FAIL);
    }

    int compressedSize = OpusEncodeApi(g_encoder, reinterpret_cast<opus_int16*>(inputData),
        frameSize, reinterpret_cast<unsigned char*>(outputData), MAX_PACKET_SIZE);
    if (compressedSize <= 0) {
        env->ReleaseShortArrayElements(inputBuffer, inputData, 0);
        env->ReleaseByteArrayElements(outBuffer, outputData, 0);
        return static_cast<jint>(FAIL);
    }

    env->ReleaseShortArrayElements(inputBuffer, inputData, 0);
    env->ReleaseByteArrayElements(outBuffer, outputData, 0);
    return static_cast<jint>(compressedSize);
}

/**
 * @功能描述：获取opus编码器状态
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] obj：代表native方法的类的class对象实例
 * @返回值：-1代表opus编码器未创建，1代表编码器已创建
 */
extern "C" JNIEXPORT jint JNICALL OPUS_JNI(getOpusEncoderStatus)(JNIEnv* /* env */, jobject /* obj */)
{
    if (g_encoder == nullptr) {
        return static_cast<jint>(FAIL);
    } else {
        return static_cast<jint>(SUCC);
    }
}
