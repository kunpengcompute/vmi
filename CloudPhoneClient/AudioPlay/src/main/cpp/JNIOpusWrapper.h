/**
 * 版权所有 (c) 华为技术有限公司 2020-2021
 * 功能说明:客户端SDK的Opus的解码和编码JNI函数
 */

#ifndef DECODER_LIBOPUS_WRAPPER_H
#define DECODER_LIBOPUS_WRAPPER_H

#if defined(ANDROID)
#include <jni.h>
#include "OpusApi.h"

#define OPUS_JNI(func) Java_com_huawei_cloudgame_audioplay_OpusJniWrapper_##func

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @功能描述：创建opus解码器
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] obj：代表native方法的类的class对象实例
 * @返回值：0代表失败，其他值代表opus解码器指针地址
 */
JNIEXPORT jlong JNICALL OPUS_JNI(createOpusDecoder)(JNIEnv* env, jobject obj);

/**
 * @功能描述：销毁opus解码器
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] obj：代表native方法的类的class对象实例
 * @参数 [in] decoder：代表opus解码器指针
 * @返回值：1代表销毁opus解码器成功
 */
JNIEXPORT jint JNICALL OPUS_JNI(destroyOpusDecoder)(JNIEnv* env, jobject obj, jlong decoder);

/**
 * @功能描述：opus解码函数，将opus音频解码成pcm
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] obj：代表native方法的类的class对象实例
 * @参数 [in] inputBuffer：代表待解码opus音频数据(包含数据头)
 * @参数 [in] inputBufferLen：代表opus纯音频数据长度(不包含数据头长度)
 * @参数 [in] outputBuffer：代表解码出来的pcm数据
 * @返回值：-1代表解码失败，其他代表解码出来的pcm数据长度
 */
JNIEXPORT jint JNICALL OPUS_JNI(opusDecode)(JNIEnv* env, jobject obj, jlong decoder, jbyteArray inputBuffer,
                                            jint inputBufferLen, jshortArray outputBuffer);

/**
 * @功能描述：创建opus编码器函数
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] obj：代表native方法的类的class对象实例
 * @参数 [in] sampleRate：代表opus音频的编码采样率
 * @参数 [in] channels：代表opus音频的声道数
 * @参数 [in] bitrate：代表opus音频的比特率
 * @返回值：-1代表创建编码器失败，1代表创建成功
 */
JNIEXPORT jint JNICALL OPUS_JNI(createOpusEncoder)(JNIEnv* env, jobject obj, jint sampleRate, jint channels,
                                                   jint bitrate);

/**
 * @功能描述：销毁opus编码器函数
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] obj：代表native方法的类的class对象实例
 * @返回值：1代表销毁opus编码器成功
 */
JNIEXPORT jint JNICALL OPUS_JNI(destroyOpusEncoder)(JNIEnv* env, jobject obj);

/**
 * @功能描述：opus编码函数，将pcm编码成opus格式
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] obj：代表native方法的类的class对象实例
 * @参数 [in] inputBuffer：代表待编码的pcm数据
 * @参数 [in] frameSize：代表pcm数据大小
 * @参数 [in] outBuffer：代表opus音频数据
 * @返回值：-1代表编码失败，其他代表opus音频数据长度
 */
JNIEXPORT jint JNICALL OPUS_JNI(opusEncode)(JNIEnv* env, jobject obj, jshortArray inputBuffer,
                                            jint frameSize, jbyteArray outBuffer);

/**
 * @功能描述：获取opus编码器状态
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] obj：代表native方法的类的class对象实例
 * @返回值：-1代表opus编码器未创建，1代表编码器已创建
 */
JNIEXPORT jint JNICALL OPUS_JNI(getOpusEncoderStatus)(JNIEnv* env, jobject obj);

#ifdef __cplusplus
}
#endif
#endif  /* ANDROID */
#endif  // DECODER_LIBOPUS_WRAPPER_H
