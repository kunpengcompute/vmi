/*
 * 版权所有 (c) 华为技术有限公司 2022-2022
 * 功能说明：视频流引擎客户端JNI接口，供java层调用
 */
#ifndef VIDEO_ENGINE_JNI_H
#define VIDEO_ENGINE_JNI_H

#include <jni.h>

#define OPENGL_JNI(func) Java_com_huawei_cloudphonesdk_maincontrol_OpenGLJniWrapper_##func
#define CB_JNI(func) Java_com_huawei_cloudphonesdk_maincontrol_OpenGLJniCallback_##func
#define NET_CONFIG_JNI(func) Java_com_huawei_cloudphonesdk_maincontrol_NetConfig_##func

extern "C" {
/**
 * @功能描述：加载原型通信库函数
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] cls：代表native方法的类的class对象实例
 * @返回值：true，代表加载通信库运行；false代表加载通信库失败。
 */
JNIEXPORT jboolean JNICALL NET_CONFIG_JNI(initialize)(JNIEnv* env, jclass cls);

/**
 * @功能描述：设置网路参数，云手机的IP或者port
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] cls：代表native方法的类的class对象实例
 * @参数 [in] ip：代表云手机服务器的IP
 * @参数 [in] port：代表云手机服务器的port
 * @参数 [in] connectType：代表通信连接类型
 * @返回值：true，代表加载通信库运行；false代表加载通信库失败。
 */
JNIEXPORT jboolean JNICALL NET_CONFIG_JNI(setNetConfig)(JNIEnv* env, jclass cls,
    jstring ip, jint port, jint connectType);

/**
 * @功能描述：设置JVM环境
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] obj：代表native方法的类的class对象实例
 */
JNIEXPORT void JNICALL CB_JNI(setObj)(JNIEnv *env, jobject obj);

/**
 * @功能描述：初始化视频流引擎客户端，包括网络通信组件等
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] obj：代表native方法的类的class对象实例
 * @返回值：VMI_SUCCESS，代表引擎初始化成功；其他代表引擎初始化失败。
 */
JNIEXPORT jint JNICALL OPENGL_JNI(initialize)(JNIEnv *env, jclass cls);

/**
 * @功能描述：启动视频流引擎客户端，包括启动视频解码线程和网络连接等
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] cls：代表对应JAVA类对象
 * @参数 [in] surface：代表上层传来的surfaceview对象
 * @参数 [in] width：代表surface的宽度
 * @参数 [in] height：代表surface的高度
 * @参数 [in] densityDpi：代表surface的像素密度
 * @返回值：VMI_SUCCESS，代表SDK启动成功；其他代表SDK启动失败。
 */
JNIEXPORT jint JNICALL OPENGL_JNI(start)(JNIEnv *env, jclass cls, jobject surface, jint width,
    jint height, jfloat densityDpi);

/**
 * @功能描述：停止视频流引擎客户端，包括停止视频解码线程和网络连接等；该函数是阻塞式的，要等所有线程退出，资源被释放后才会返回
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] cls：代表对应JAVA类对象
 */
JNIEXPORT void JNICALL OPENGL_JNI(stop)(JNIEnv *env, jclass cls);

/**
 * @功能描述：获取视频流引擎客户端的统计数据，包括网络时延、接收帧率、解码帧率等
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] cls：代表对应JAVA类对象
 * @返回值：视频流引擎客户端统计信息字符串
 */
JNIEXPORT jstring JNICALL OPENGL_JNI(getStatistics)(JNIEnv *env, jclass cls);

/**
 * @功能描述：视频流引擎客户端接收数据
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] cls：代表对应JAVA类对象
 * @参数 [in] type：代表数据类型
 * @参数 [out] jData：代表接收数组buffer
 * @参数 [in] length：代表数组长度
 * @返回值：实际接受到数据长度
 */
JNIEXPORT jint JNICALL OPENGL_JNI(recvData)(JNIEnv *env, jclass cls, jbyte type, jbyteArray jData, int length);

/**
 * @功能描述：发送按键数据
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] cls：代表对应JAVA类对象
 * @参数 [in] jData：代表触控数据
 * @参数 [in] length：代表发送触控信息的长度
 * @返回值：发送结果。JNI_TRUE代表发送按键信息成功，JNI_FALSE代表发送按键信息失败
 */
JNIEXPORT jboolean JNICALL OPENGL_JNI(sendKeyEvent)(JNIEnv *env, jclass cls,  jbyteArray jData, int length);

/**
 * @功能描述：发送触控数据
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] cls：代表对应JAVA类对象
 * @参数 [in] data：代表多点触控数据
 * @参数 [in] length：代表多点触控数据长度
 * @返回值：发送结果。JNI_TRUE代表发送触控信息成功，JNI_FALSE代表发送触控信息失败
 */
JNIEXPORT jboolean JNICALL OPENGL_JNI(sendTouchEvent)(JNIEnv *env, jclass cls, jbyteArray jData, jint length);

/**
 * @功能描述：发送控数据到agent端
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] cls：代表对应JAVA类对象
 * @参数 [in] jData：代表音频数据
 * @参数 [in] length: 代表音频数据长度
 * @返回值：发送结果。JNI_TRUE代表发送音频数据成功，JNI_FALSE代表发送音频数据失败
 */
JNIEXPORT jboolean JNICALL OPENGL_JNI(sendAudioDataArray)(JNIEnv *env, jclass cls, jbyteArray jData, int length);

JNIEXPORT jboolean JNICALL OPENGL_JNI(setVideoParam)(JNIEnv *env, jclass cls, jobject obj);

JNIEXPORT jboolean JNICALL OPENGL_JNI(setMicParam)(JNIEnv *env, jclass cls, jobject obj);

JNIEXPORT jboolean JNICALL OPENGL_JNI(setAudioParam)(JNIEnv *env, jclass cls, jobject obj);

JNIEXPORT jboolean JNICALL OPENGL_JNI(composeMicData)(JNIEnv* env, jclass cls, jbyteArray jData, jint length, jint audioType, jint sampleInterval);
}

#endif
