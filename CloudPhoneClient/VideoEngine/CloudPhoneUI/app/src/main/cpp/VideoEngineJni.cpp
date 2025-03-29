/**
 * 版权所有 (c) 华为技术有限公司 2017-2022
 * 功能说明：视频流引擎客户端JNI接口实现，供java层调用
 */
#define LOG_TAG "VideoEngineJni"

#include "VideoEngineJni.h"

#include <android/native_activity.h>
#include <android/native_window_jni.h>
#include <cstdlib>
#include <jni.h>
#include <map>
#include <sstream>
#include <string>
#include <android/log.h>
#include <arpa/inet.h>
#include <dlfcn.h>
#include <unistd.h>

#include "VideoEngineClient.h"
#include "Connection.h"
#include "VmiDef.h"
#include <memory>

using namespace std;
namespace {
    // VmiMonitorEnter或者VmiMonitorExit函数循环休眠间隔
    constexpr int VMI_MONITOR_SLEEP_TIME = 1000;

    // Java环境全局指针
    JNIEnv *g_env = nullptr;

    // Java类对象
    jobject g_obj = nullptr;

    // 全局JVM指针
    JavaVM *g_pvm = nullptr;

    // 设置JVM环境的标志
    bool g_alertLogInited = false;

    // 指令流事件通知回调Java方法
    const std::string VIDEO_ENG_EVENT_NOTICE_JAVA_METHOD_NAME = "onVmiVideoEngineEvent";

    using ConfigNetAddressFunc = void (*)(unsigned int, unsigned int);

    struct RfbTouchEventMsg {
        uint8_t id;
        uint8_t action;
        uint16_t x;
        uint16_t y;
        uint16_t pressure;
    } __attribute__((packed));

    struct RfbKeyEventMsg {
        uint16_t keycode;
        uint16_t action;
    } __attribute__((packed));

    ANativeWindow *g_nativeWindow = nullptr;
}  // namespace

#define DBG(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define INFO(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define WARN(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define ERR(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define FATAL(...) __android_log_print(ANDROID_LOG_FATAL, LOG_TAG, __VA_ARGS__)

/**
 * @功能描述：JVM的同步代码，进入同步区
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] obj：代表native方法的类的class对象实例
 */
void VmiMonitorEnter(JNIEnv *env, jobject obj) {
    while (((*env).MonitorEnter(obj)) != JNI_OK) {
        // LOG_RATE_LIMIT(Vmi::AndroidLogPriority::ANDROID_LOG_INFO, VMI_MONITOR_INTERVAL, "JNI Enter critical sections failed");
        usleep(VMI_MONITOR_SLEEP_TIME);
    }
    INFO("JNI critical sections entered");
}

/**
 * @功能描述：JVM的同步代码，退出同步区
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] obj：代表native方法的类的class对象实例
 */
void VmiMonitorExit(JNIEnv *env, jobject obj) {
    while (((*env).MonitorExit(obj)) != JNI_OK) {
        // LOG_RATE_LIMIT(Vmi::AndroidLogPriority::ANDROID_LOG_INFO, VMI_MONITOR_INTERVAL, "JNI Exit critical sections failed");
        usleep(VMI_MONITOR_SLEEP_TIME);
    }
    INFO("JNI critical sections exited");
}

/**
 * @功能描述：jstring转换成string
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] jStr：代表要转换的jstring对象
 * @返回值：string对象
 */
std::string Jstring2String(JNIEnv *env, jstring jStr) {
    if (jStr == nullptr) {
        return "";
    }
    const jclass stringClass = env->GetObjectClass(jStr);
    const jmethodID getBytes = env->GetMethodID(stringClass, "getBytes", "(Ljava/lang/String;)[B");
    const jbyteArray stringJbytes = (jbyteArray) env->CallObjectMethod(jStr, getBytes,
                                                                       env->NewStringUTF("UTF-8"));
    size_t length = static_cast<size_t>(env->GetArrayLength(stringJbytes));
    jbyte *pBytes = env->GetByteArrayElements(stringJbytes, nullptr);
    std::string ret = std::string(reinterpret_cast<char *>(pBytes), length);
    env->ReleaseByteArrayElements(stringJbytes, pBytes, JNI_ABORT);
    env->DeleteLocalRef(stringJbytes);
    env->DeleteLocalRef(stringClass);
    return ret;
}

/**
 * @功能描述：字符串IP转主机字节序函数
 * @参数 [in] ip：代表字符串ip
 * @返回值：返回整形，主机字节序的ip地址
 */
unsigned int GetIpHostOrder(std::string ip) {
    struct in_addr addr = {};
    int ret = inet_aton(ip.c_str(), &addr);
    if (ret == 0) {
        ERR("error: Invalid ip, please check");
        return 0;
    }
    return ntohl(addr.s_addr);
}

extern "C" {
JNIEXPORT jboolean JNICALL NET_CONFIG_JNI(initialize)(JNIEnv *env, jclass cls) {
    (void) env;
    (void) cls;
    return true;
}

JNIEXPORT jboolean JNICALL NET_CONFIG_JNI(setNetConfig)(JNIEnv *env, jclass cls,
                                                        jstring ip, jint port, jint connectType) {
    (void) cls;
    (void) connectType;
    VmiConfigClientAddress(GetIpHostOrder(Jstring2String(env, ip)), port);
    return true;
}

JNIEXPORT void JNICALL CB_JNI(setObj)(JNIEnv *env, jobject obj) {
    if (env == nullptr) {
        ERR("Error: set callback, can't get java environment");
        return;
    }
    if (g_alertLogInited) {
        return;
    }
    if (g_obj != nullptr) {
        g_env->DeleteGlobalRef(g_obj);
    }
    g_obj = env->NewGlobalRef(obj);
    g_env = env;

    g_alertLogInited = true;
    jint ret = env->GetJavaVM(&g_pvm);
    if (ret != JNI_OK) {
        ERR("cannot get java VM");
    }
}

void CallJavaVoidFunction(const std::string &funName, EngineEvent engineEvent) {
    if (!g_alertLogInited) {
        ERR("g_alertLogInited is false");
        return;
    }

    JNIEnv *env = nullptr;
    int status = g_pvm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_4);
    if (status == JNI_EDETACHED) {
        status = g_pvm->AttachCurrentThread(&env, nullptr);
        if (status != JNI_OK) {
            ERR("AttachCurrentThread failed, status:%d", status);
            env = nullptr;
            return;
        }
    }

    if (env == nullptr) {
        ERR("Error: call void function, can't get java environment");
        return;
    }
    jclass cls = env->GetObjectClass(g_obj);
    jmethodID mid = env->GetMethodID(cls, funName.c_str(), "(IIIIILjava/lang/String;)V");
    if (mid != nullptr) {
        env->CallVoidMethod(g_obj, mid, engineEvent.event, engineEvent.para1, engineEvent.para2,
                            engineEvent.para3, engineEvent.para4,
                            env->NewStringUTF(engineEvent.additionInfo));
    } else {
        ERR("Error: Can't found %s method", funName.c_str());
    }
    env->DeleteLocalRef(cls);
    (void) g_pvm->DetachCurrentThread();
}

/**
 * @功能描述：回调java层的onVmiVideoEngineEvent函数，通知视频流引擎事件产生
 * @参数 [in] event：代表视频流引擎事件，
 *                  目前有VMI_VIDEO_ENGINE_EVENT_SOCK_DISCONN事件。
 */
void CallJavaOnVmiVideoEngineEvent(EngineEvent engineEvent) {
    (void) engineEvent;
    CallJavaVoidFunction(VIDEO_ENG_EVENT_NOTICE_JAVA_METHOD_NAME, engineEvent);
}

JNIEXPORT jint JNICALL OPENGL_JNI(initialize)(JNIEnv *env, jclass cls) {
    (void) env;
    (void) cls;
    uint32_t ret = Initialize(CallJavaOnVmiVideoEngineEvent);
    return static_cast<jint>(ret);
}

JNIEXPORT jint JNICALL OPENGL_JNI(start)(JNIEnv *env, jclass cls, jobject surface, jint width,
                                         jint height, jfloat densityDpi) {
    (void) env;
    (void) cls;
    INFO("OpenGLJNIWrapper_start enter");
    VmiMonitorEnter(env, cls);
    if (surface == nullptr) {
        ERR("nativeWindow set to nullptr");
        VmiMonitorExit(env, cls);
        return VMI_CLIENT_START_FAIL;
    }
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);
    if (nativeWindow == nullptr) {
        ERR("nativeWindow is nullptr");
        VmiMonitorExit(env, cls);
        return VMI_CLIENT_START_FAIL;
    }
    g_nativeWindow = nativeWindow;
    std::string ipAndPort = "";
    uint32_t ret = Start(reinterpret_cast<uint64_t>(nativeWindow), static_cast<uint32_t>(width),
                         static_cast<uint32_t>(height), static_cast<uint32_t>(densityDpi), ipAndPort);
    if (ret != VMI_SUCCESS) {
        ERR("Failed to start, ret: %u", ret);
        VmiMonitorExit(env, cls);
        return VMI_CLIENT_START_FAIL;
    }
    VmiMonitorExit(env, cls);
    return VMI_SUCCESS;
}

JNIEXPORT void JNICALL OPENGL_JNI(stop)(JNIEnv *env, jclass cls) {
    (void) env;
    (void) cls;
    INFO("OpenGLJNIWrapper_stop enter");
    VmiMonitorEnter(env, cls);
    Stop();
    if (g_nativeWindow != nullptr) {
        ANativeWindow_release(g_nativeWindow);
        g_nativeWindow = nullptr;
    }
    VmiMonitorExit(env, cls);
}

JNIEXPORT jstring JNICALL OPENGL_JNI(getStatistics)(JNIEnv *env, jclass cls) {
    (void) env;
    (void) cls;
    Vmi::StatisticsInfo info;
    GetStatistics(info);
    uint64_t lag = info.lag;
    uint32_t recvFps = info.recvFps;
    uint32_t decFps = info.decFps;
    std::stringstream ss;
    ss << "LAG: " << lag << "ms \n";
    ss << "接收帧率: " << recvFps << "fps \n";
    ss << "解码帧率: " << decFps << "fps \n";
    return env->NewStringUTF(ss.str().c_str());
}

JNIEXPORT jint JNICALL
OPENGL_JNI(recvData)(JNIEnv *env, jclass cls, jbyte type, jbyteArray jData, int length) {
    (void) env;
    (void) cls;
    uint8_t *data = reinterpret_cast<uint8_t *>(env->GetByteArrayElements(jData, nullptr));
    // 从队列里面取数据
    int ret = RecvData(static_cast<VMIMsgType>(type), data, length);
    env->ReleaseByteArrayElements(jData, reinterpret_cast<jbyte *>(data), 0);
    return ret;
}

JNIEXPORT jboolean JNICALL
OPENGL_JNI(sendKeyEvent)(JNIEnv *env, jclass cls, jbyteArray jData, int length) {
    (void) env;
    (void) cls;
    if (env == nullptr) {
        ERR("Error: send navbar input event, can't get java environment");
        return JNI_FALSE;
    }

    uint8_t *data = reinterpret_cast<uint8_t *>(env->GetByteArrayElements(jData, nullptr));
    if (data == nullptr) {
        ERR("Error: send touch, src buffer is nullptr");
        return JNI_FALSE;
    }

    uint32_t ret = SendData(VMIMsgType::TOUCH_INPUT, data, length);
    env->ReleaseByteArrayElements(jData, reinterpret_cast<jbyte *>(data), 0);
    return (ret == VMI_SUCCESS) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
OPENGL_JNI(sendTouchEvent)(JNIEnv *env, jclass cls, jbyteArray jData, jint length) {
    (void) env;
    (void) cls;
    uint8_t *data = reinterpret_cast<uint8_t *>(env->GetByteArrayElements(jData, nullptr));
    uint32_t ret = SendData(VMIMsgType::TOUCH_INPUT, data, length);
    env->ReleaseByteArrayElements(jData, reinterpret_cast<jbyte *>(data), 0);
    if (ret == 0) {
        return JNI_TRUE;
    } else {
        ERR("SendTouchInputData failed, ret = %u", ret);
        return JNI_FALSE;
    }
}
JNIEXPORT jboolean JNICALL
OPENGL_JNI(sendAudioDataArray)(JNIEnv *env, jclass cls, jbyteArray jData, int length) {
    (void) cls;
    if (env == nullptr) {
        ERR("Error: send audio event, can't get java environment");
        return JNI_FALSE;
    }

    uint8_t *data = reinterpret_cast<uint8_t *>(env->GetByteArrayElements(jData, nullptr));
    if (data == nullptr) {
        ERR("Error: send audio, src buffer is nullptr");
        return JNI_FALSE;
    }

    uint32_t ret = SendData(VMIMsgType::AUDIO, data, length);
    env->ReleaseByteArrayElements(jData, reinterpret_cast<jbyte *>(data), 0);
    if (ret == 0) {
        return JNI_TRUE;
    } else {
        ERR("sendAudioDataArray failed, ret = %u", ret);
        return JNI_FALSE;
    }
}

struct AudioMicData {
    VmiCmd cmd;
    AudioData audioData;
} __attribute__((packed));


JNIEXPORT jboolean JNICALL
OPENGL_JNI(composeMicData)(JNIEnv *env, jclass cls, jbyteArray jData, jint length, jint audioType,
                           jint sampleInterval) {
    (void) cls;
    if (env == nullptr) {
        ERR("Error: send audio event, can't get java environment");
        return JNI_FALSE;
    }

    uint8_t *data = reinterpret_cast<uint8_t *>(env->GetByteArrayElements(jData, nullptr));
    if (data == nullptr) {
        ERR("Error: send audio, src buffer is nullptr");
        return JNI_FALSE;
    }

    uint32_t cmdLength = sizeof(VmiCmd);
    uint32_t paramLength = sizeof(ExtDataAudio) + sizeof(uint32_t);
    uint32_t totalLength = cmdLength + paramLength + length;

    std::unique_ptr<uint8_t[]> total_Data = make_unique<uint8_t[]>(sizeof(AudioMicData) + length);
    reinterpret_cast<AudioMicData *>(total_Data.get())->cmd = MIC_SEND_MIC_DATA;
    reinterpret_cast<AudioMicData *>(total_Data.get())->audioData.size = length;
    reinterpret_cast<AudioMicData *>(total_Data.get())->audioData.extData.audioType = audioType;
    reinterpret_cast<AudioMicData *>(total_Data.get())->audioData.extData.channels = 2;
    reinterpret_cast<AudioMicData *>(total_Data.get())->audioData.extData.audioBitDepth = 16;
    reinterpret_cast<AudioMicData *>(total_Data.get())->audioData.extData.sampleRate = 48000;
    reinterpret_cast<AudioMicData *>(total_Data.get())->audioData.extData.sampleInterval = sampleInterval;
    std::chrono::system_clock::time_point currTime = std::chrono::system_clock::now();
    int64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            currTime.time_since_epoch()).count();
    reinterpret_cast<AudioMicData *>(total_Data.get())->audioData.extData.timestamp = timestamp;

    memcpy(reinterpret_cast<AudioMicData *>(total_Data.get())->audioData.data, (uint8_t *) (data),
           length);
    uint32_t ret = SendData(VMIMsgType::MIC, total_Data.get(), totalLength);
    env->ReleaseByteArrayElements(jData, reinterpret_cast<jbyte *>(data), 0);

    if (ret == 0) {
        return JNI_TRUE;
    } else {
        ERR("sendAudioDataArray failed, ret = %u", ret);
        return JNI_FALSE;
    }
}

JNIEXPORT jboolean JNICALL OPENGL_JNI(startTouch)(JNIEnv *env, jclass cls) {
    (void) env;
    (void) cls;
    INFO("OpenGLJNIWrapper_startTouch enter");
    VmiConfigTouch stVmiConfigTouch;
    stVmiConfigTouch.version = static_cast<uint16_t>(CUR_TOUCH_VERSION);
    uint8_t vmiCmd = DATA_TOUCH;
    uint32_t cmdLength = sizeof(vmiCmd);
    uint32_t paramLength = sizeof(stVmiConfigTouch);
    uint32_t length = cmdLength + paramLength;
    uint8_t *data = static_cast<uint8_t *>(malloc(length));
    if (data == nullptr) {
        ERR("Error: malloc touch buffer is nullptr");
        return JNI_FALSE;
    }
    memcpy(data, &vmiCmd, cmdLength);
    memcpy(data + cmdLength, &stVmiConfigTouch, paramLength);
    uint32_t ret = SendData(VMIMsgType::START, data, length);
    if (ret == 0) {
        free(data);
        data = nullptr;
        return JNI_TRUE;
    } else {
        free(data);
        data = nullptr;
        ERR("startTouch failed, ret = %u", ret);
        return JNI_FALSE;
    }
}

JNIEXPORT jboolean JNICALL OPENGL_JNI(startVideo)(JNIEnv *env, jclass cls, jobject obj) {
    (void) env;
    (void) cls;
    INFO("OpenGLJNIWrapper_startVideo enter");
    VmiConfigVideo stVmiConfigVideo;
    jclass objClass = env->FindClass("com/huawei/cloudphonesdk/maincontrol/config/VmiConfigVideo");
    jfieldID jencoderType = env->GetFieldID(objClass, "encoderType", "I");
    jfieldID jvideoFrameType = env->GetFieldID(objClass, "videoFrameType", "I");
    jfieldID jwidth = env->GetFieldID(objClass, "width", "I");
    jfieldID jheight = env->GetFieldID(objClass, "height", "I");
    jfieldID jwidthAligned = env->GetFieldID(objClass, "widthAligned", "I");
    jfieldID jheightAligned = env->GetFieldID(objClass, "heightAligned", "I");
    jfieldID jdensity = env->GetFieldID(objClass, "density", "I");
    jfieldID jrenderOptimize = env->GetFieldID(objClass, "renderOptimize", "Z");
    jfieldID jbitrate = env->GetFieldID(objClass, "bitrate", "I");
    jfieldID jgopSize = env->GetFieldID(objClass, "gopSize", "I");
    jfieldID jprofile = env->GetFieldID(objClass, "profile", "I");
    jfieldID jrcMode = env->GetFieldID(objClass, "rcMode", "I");
    jfieldID jforceKeyFrame = env->GetFieldID(objClass, "forceKeyFrame", "I");
    jfieldID jinterpolation = env->GetFieldID(objClass, "interpolation", "Z");
    jfieldID jcrf = env->GetFieldID(objClass, "crf", "I");
    jfieldID jmaxCrfRate = env->GetFieldID(objClass, "maxCrfRate", "I");
    jfieldID jvbvBufferSize = env->GetFieldID(objClass, "vbvBufferSize", "I");
    jfieldID jstreamWidth = env->GetFieldID(objClass, "streamWidth", "I");
    jfieldID jstreamHeight = env->GetFieldID(objClass, "streamHeight", "I");

    jint encoderType = env->GetIntField(obj, jencoderType);
    jint videoFrameType = env->GetIntField(obj, jvideoFrameType);
    jint width = env->GetIntField(obj, jwidth);
    jint height = env->GetIntField(obj, jheight);
    jint widthAligned = env->GetIntField(obj, jwidthAligned);
    jint heightAligned = env->GetIntField(obj, jheightAligned);
    jint density = env->GetIntField(obj, jdensity);
    jboolean renderOptimize = env->GetBooleanField(obj, jrenderOptimize);
    jint bitrate = env->GetIntField(obj, jbitrate);
    jint gopSize = env->GetIntField(obj, jgopSize);
    jint profile = env->GetIntField(obj, jprofile);
    jint rcMode = env->GetIntField(obj, jrcMode);
    jint forceKeyFrame = env->GetIntField(obj, jforceKeyFrame);
    jboolean interpolation = env->GetBooleanField(obj, jinterpolation);
    jint crf = env->GetIntField(obj, jcrf);
    jint maxCrfRate = env->GetIntField(obj, jmaxCrfRate);
    jint vbvBufferSize = env->GetIntField(obj, jvbvBufferSize);
    jint streamWidth = env->GetIntField(obj, jstreamWidth);
    jint streamHeight = env->GetIntField(obj, jstreamHeight);

    stVmiConfigVideo.encoderType = static_cast<EncoderType>(encoderType);
    stVmiConfigVideo.videoFrameType = static_cast<VideoFrameType>(videoFrameType);
    stVmiConfigVideo.resolution.width = static_cast<uint32_t>(width);
    stVmiConfigVideo.resolution.height = static_cast<uint32_t>(height);
    stVmiConfigVideo.resolution.widthAligned = static_cast<uint32_t>(widthAligned);
    stVmiConfigVideo.resolution.heightAligned = static_cast<uint32_t>(heightAligned);
    stVmiConfigVideo.density = static_cast<uint32_t>(density);
    stVmiConfigVideo.renderOptimize = static_cast<bool>(renderOptimize);
    stVmiConfigVideo.encodeParams.bitrate = static_cast<uint32_t>(bitrate);
    stVmiConfigVideo.encodeParams.gopSize = static_cast<uint32_t>(gopSize);
    stVmiConfigVideo.encodeParams.profile = static_cast<ProfileType>(profile);
    stVmiConfigVideo.encodeParams.rcMode = static_cast<RCMode>(rcMode);
    stVmiConfigVideo.encodeParams.forceKeyFrame = static_cast<uint32_t>(forceKeyFrame);
    stVmiConfigVideo.encodeParams.interpolation = static_cast<bool>(interpolation);
    stVmiConfigVideo.encodeParams.crf = static_cast<uint32_t>(crf);
    stVmiConfigVideo.encodeParams.maxCrfRate = static_cast<uint32_t>(maxCrfRate);
    stVmiConfigVideo.encodeParams.vbvBufferSize = static_cast<int32_t>(vbvBufferSize);
    stVmiConfigVideo.encodeParams.streamWidth = static_cast<uint32_t>(streamWidth);
    stVmiConfigVideo.encodeParams.streamHeight = static_cast<uint32_t>(streamHeight);
    stVmiConfigVideo.version = static_cast<uint16_t>(CUR_VIDEO_VERSION);

    uint8_t vmiCmd = DATA_VIDEO;
    uint32_t cmdLength = sizeof(vmiCmd);
    uint32_t paramLength = sizeof(stVmiConfigVideo);
    uint32_t length = cmdLength + paramLength;
    uint8_t *data = static_cast<uint8_t *>(malloc(length));
    if (data == nullptr) {
        ERR("Error: malloc video buffer is nullptr");
        return JNI_FALSE;
    }
    memcpy(data, &vmiCmd, cmdLength);
    memcpy(data + cmdLength, &stVmiConfigVideo, paramLength);
    uint32_t ret = SendData(VMIMsgType::START, data, length);
    if (ret == 0) {
        free(data);
        data = nullptr;
        return JNI_TRUE;
    } else {
        free(data);
        data = nullptr;
        ERR("startVideo failed, ret = %u", ret);
        return JNI_FALSE;
    }
}

JNIEXPORT void JNICALL OPENGL_JNI(getEncodeParam)(JNIEnv *env, jclass cls) {
    (void) env;
    (void) cls;
    uint32_t vmiCmd = VIDEO_GET_ENCODER_PARAM;
    uint32_t cmdLength = sizeof(vmiCmd);
    EncodeParams encodeParams;
    uint32_t paramLength = sizeof(encodeParams);
    uint32_t length = cmdLength + paramLength;
    uint8_t *data = static_cast<uint8_t *>(malloc(length));
    memcpy(data, &vmiCmd, cmdLength);
    memcpy(data + cmdLength, &encodeParams, paramLength);
    uint32_t ret = SendData(VMIMsgType::VIDEO_RR2, data, length);
    if (ret == 0) {
        INFO("GetEncodeParam command cmd success!");
    } else {
        ERR("GetEncodeParam command cmd failed, ret = %u", ret);
        data = nullptr;
    }
}

JNIEXPORT jboolean JNICALL OPENGL_JNI(setEncodeParam)(JNIEnv *env, jclass cls, jobject obj) {

    (void) env;
    (void) cls;
    INFO("OpenGLJNIWrapper_setEncodeParam enter");
    EncodeParams encodeParams;
    jclass objClass = env->FindClass("com/huawei/cloudphonesdk/maincontrol/config/EncodeParams");
    jfieldID jbitrate = env->GetFieldID(objClass, "bitrate", "I");
    jfieldID jgopSize = env->GetFieldID(objClass, "gopSize", "I");
    jfieldID jprofile = env->GetFieldID(objClass, "profile", "I");
    jfieldID jrcMode = env->GetFieldID(objClass, "rcMode", "I");
    jfieldID jforceKeyFrame = env->GetFieldID(objClass, "forceKeyFrame", "I");
    jfieldID jinterpolation = env->GetFieldID(objClass, "interpolation", "Z");
    jfieldID jcrf = env->GetFieldID(objClass, "crf", "I");
    jfieldID jmaxCrfRate = env->GetFieldID(objClass, "maxCrfRate", "I");
    jfieldID jvbvBufferSize = env->GetFieldID(objClass, "vbvBufferSize", "I");
    jfieldID jstreamWidth = env->GetFieldID(objClass, "streamWidth", "I");
    jfieldID jstreamHeight = env->GetFieldID(objClass, "streamHeight", "I");

    jint bitrate = env->GetIntField(obj, jbitrate);
    jint gopSize = env->GetIntField(obj, jgopSize);
    jint profile = env->GetIntField(obj, jprofile);
    jint rcMode = env->GetIntField(obj, jrcMode);
    jint forceKeyFrame = env->GetIntField(obj, jforceKeyFrame);
    jboolean interpolation = env->GetBooleanField(obj, jinterpolation);
    jint crf = env->GetIntField(obj, jcrf);
    jint maxCrfRate = env->GetIntField(obj, jmaxCrfRate);
    jint vbvBufferSize = env->GetIntField(obj, jvbvBufferSize);
    jint streamWidth = env->GetIntField(obj, jstreamWidth);
    jint streamHeight = env->GetIntField(obj, jstreamHeight);

    encodeParams.bitrate = static_cast<uint32_t>(bitrate);
    encodeParams.gopSize = static_cast<uint32_t>(gopSize);
    encodeParams.profile = static_cast<ProfileType>(profile);
    encodeParams.rcMode = static_cast<RCMode>(rcMode);
    encodeParams.forceKeyFrame = static_cast<uint32_t>(forceKeyFrame);
    encodeParams.interpolation = static_cast<bool>(interpolation);
    encodeParams.crf = static_cast<uint32_t>(crf);
    encodeParams.maxCrfRate = static_cast<uint32_t>(maxCrfRate);
    encodeParams.vbvBufferSize = static_cast<int32_t>(vbvBufferSize);
    encodeParams.streamWidth = static_cast<uint32_t>(streamWidth);
    encodeParams.streamHeight = static_cast<uint32_t>(streamHeight);

    uint32_t vmiCmd = VIDEO_SET_ENCODER_PARAM;
    uint32_t cmdLength = sizeof(vmiCmd);
    uint32_t paramLength = sizeof(encodeParams);
    uint32_t length = cmdLength + paramLength;
    uint8_t *data = static_cast<uint8_t *>(malloc(length));
    if (data == nullptr) {
        ERR("Error: malloc video buffer is nullptr");
        return JNI_FALSE;
    }
    memcpy(data, &vmiCmd, cmdLength);
    memcpy(data + cmdLength, &encodeParams, paramLength);
    uint32_t ret = SendData(VMIMsgType::VIDEO_RR2, data, length);
    if (ret == 0) {
        free(data);
        data = nullptr;
        return JNI_TRUE;
    } else {
        free(data);
        data = nullptr;
        ERR("setEncodeParam failed, ret = %u", ret);
        return JNI_FALSE;
    }
}

JNIEXPORT jboolean JNICALL OPENGL_JNI(setAudioParam)(JNIEnv *env, jclass cls, jobject obj) {
    (void) env;
    (void) cls;
    INFO("OpenGLJNIWrapper_setAudioParam enter");

    VmiConfigAudio stVmiConfigAudio;
    jclass objClass = env->FindClass("com/huawei/cloudphonesdk/maincontrol/config/VmiConfigAudio");
    jfieldID jaudioType = env->GetFieldID(objClass, "audioType", "I");
    jfieldID jsampleInterval = env->GetFieldID(objClass, "sampleInterval", "I");
    jfieldID jbitrate = env->GetFieldID(objClass, "bitrate", "I");

    jint audioType = env->GetIntField(obj, jaudioType);
    jint sampleInterval = env->GetIntField(obj, jsampleInterval);
    jint bitrate = env->GetIntField(obj, jbitrate);

    stVmiConfigAudio.audioType = static_cast<AudioType>(audioType);
    stVmiConfigAudio.params.sampleInterval = static_cast<uint32_t>(sampleInterval);
    stVmiConfigAudio.params.bitrate = static_cast<uint32_t>(bitrate);
    uint32_t vmiCmd = static_cast<uint32_t>(AUDIO_SET_AUDIOPLAY_PARAM);
    uint32_t cmdLength = sizeof(VmiCmd);
    uint32_t paramLength = sizeof(AudioPlayParams);
    uint32_t length = sizeof(VmiCmd) + sizeof(AudioPlayParams);

    uint8_t *data = static_cast<uint8_t *>(malloc(length));
    if (data == nullptr) {
        ERR("Error: malloc audio buffer is nullptr");
        return JNI_FALSE;
    }
    memcpy(data, &vmiCmd, cmdLength);
    memcpy(data + cmdLength, &stVmiConfigAudio.params, paramLength);
    uint32_t ret = SendData(VMIMsgType::AUDIO, data, length);
    if (ret == 0) {
        free(data);
        data = nullptr;
        return JNI_TRUE;
    } else {
        free(data);
        data = nullptr;
        ERR("setAudioParam failed, ret = %u", ret);
        return JNI_FALSE;
    }
}

JNIEXPORT jboolean JNICALL OPENGL_JNI(setMicParam)(JNIEnv *env, jclass cls, jobject obj) {
    (void) env;
    (void) cls;
    INFO("OpenGLJNIWrapper_setMicParam enter");

    VmiConfigMic stVmiConfigMic;
    jclass objClass = env->FindClass("com/huawei/cloudphonesdk/maincontrol/config/VmiConfigMic");
    jfieldID jaudioType = env->GetFieldID(objClass, "audioType", "I");

    jint audioType = env->GetIntField(obj, jaudioType);

    stVmiConfigMic.audioType = static_cast<AudioType>(audioType);

    uint32_t length = sizeof(VmiConfigMic);
    uint8_t *data = static_cast<uint8_t *>(malloc(length));
    if (data == nullptr) {
        ERR("Error: malloc Mic buffer is nullptr");
        return JNI_FALSE;
    }
    memcpy(data, &stVmiConfigMic, length);
    uint32_t ret = SendData(VMIMsgType::START, data, length);
    if (ret == 0) {
        free(data);
        data = nullptr;
        INFO("OpenGLJNIWrapper_setMicParam success");
        return JNI_TRUE;
    } else {
        free(data);
        data = nullptr;
        ERR("setAudioParam failed, ret = %u", ret);
        return JNI_FALSE;
    }
}

/**
 * @功能描述：发送传感器数据到agent端
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] cls：代表对应JAVA类对象
 * @参数 [in] jData：代表传感器数据
 * @参数 [in] length：代表传感器数据长度
 * @返回值：发送结果，JNI_TRUE代表发送Sensor数据成功，JNI_FALSE代表发送失败
 */
JNIEXPORT jboolean JNICALL
OPENGL_JNI(sendSensorDataArray)(JNIEnv *env, jclass cls, jbyteArray jData, int length) {
    (void) cls;
    if (env == nullptr) {
        ERR("Error: send sensor data, can't get java environment");
        return JNI_FALSE;
    }

    uint8_t *sensorData = reinterpret_cast<uint8_t *>(env->GetByteArrayElements(jData, nullptr));
    if (sensorData == nullptr) {
        ERR("Error: send sensor, src buffer is nullptr");
        return JNI_FALSE;
    }

    uint32_t vmiCmd = static_cast<uint32_t>(SENSOR_SEND_SENSOR_DATA);
    uint32_t cmdLength = sizeof(VmiCmd);
    uint32_t totalLength = sizeof(VmiCmd) + static_cast<uint32_t>(length);
    uint8_t *data = static_cast<uint8_t *>(malloc(totalLength));
    if (data == nullptr) {
        ERR("Error: malloc sensor data buffer is nullptr");
        env->ReleaseByteArrayElements(jData, reinterpret_cast<jbyte *>(sensorData), 0);
        return JNI_FALSE;
    }

    memcpy(data, &vmiCmd, cmdLength);
    memcpy(data + cmdLength, sensorData, length);

    uint32_t ret = SendData(VMIMsgType::SENSOR, data, totalLength);
    env->ReleaseByteArrayElements(jData, reinterpret_cast<jbyte *>(sensorData), 0);
    free(data);
    if (ret != 0) {
        ERR("sendSensorDataArray failed, ret = %u", ret);
        return JNI_FALSE;
    }
    return JNI_TRUE;
}
/**
 * @功能描述：发送GPS信息到Agent
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] cls：代表对应JAVA类对象
 * @参数 [in] latitude：代表GPS纬度
 * @参数 [in] longitude：代表GPS经度
 * @参数 [in] altitude：代表海拔高度
 * @参数 [in] bearing：代表方位信息
 * @参数 [in] speed：代表重力加速器值
 * @返回值：发送结果，JNI_TRUE代表发送Gps数据成功，JNI_FALSE代表发送失败
 */
JNIEXPORT jboolean JNICALL
OPENGL_JNI(sendGpsLocation)(JNIEnv *env, jclass cls, double latitude, double longitude,
                            double altitude, float speed, float bearing, float accuracy,
                            int64_t timestamp) {
    (void) cls;
    if (env == nullptr) {
        ERR("Error: send location data, can't get java environment");
        return JNI_FALSE;
    }

    VmiGpsLocationData location = {latitude, longitude, altitude, speed, bearing, accuracy,
                                   timestamp};
    uint32_t vmiCmd = static_cast<uint32_t>(GPS_SEND_LOCATION_DATA);
    uint32_t cmdLength = sizeof(VmiCmd);
    uint32_t totalLength = sizeof(VmiCmd) + sizeof(VmiGpsLocationData);
    uint8_t *data = static_cast<uint8_t *>(malloc(totalLength));
    if (data == nullptr) {
        ERR("Error: malloc location data buffer is nullptr");
        return JNI_FALSE;
    }

    memcpy(data, &vmiCmd, cmdLength);
    memcpy(data + cmdLength, &location, sizeof(VmiGpsLocationData));

    uint32_t ret = SendData(VMIMsgType::GPS, data, totalLength);
    free(data);
    if (ret != 0) {
        ERR("Failed to send location data, ret = %u", ret);
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

/**
 * @功能描述：发送GPS的Nmea码到Agent
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] cls：代表对应JAVA类对象
 * @参数 [in] jData：GPS的NMEA码数据
 * @参数 [in] length：jData的长度
 * @返回值：发送结果，JNI_TRUE代表发送Gps的NMEA码成功，JNI_FALSE代表发送失败
 */
JNIEXPORT jboolean JNICALL
OPENGL_JNI(sendGpsNmea)(JNIEnv *env, jclass cls, jbyteArray jData, int length) {
    (void) cls;
    if (env == nullptr) {
        ERR("Error: send nmea data, can't get java environment");
        return JNI_FALSE;
    }

    uint8_t *nmeaData = reinterpret_cast<uint8_t *>(env->GetByteArrayElements(jData, nullptr));
    if (nmeaData == nullptr) {
        ERR("Error: send nmea, src buffer is nullptr");
        return JNI_FALSE;
    }

    uint32_t vmiCmd = static_cast<uint32_t>(GPS_SEND_NMEA_DATA);
    uint32_t cmdLength = sizeof(VmiCmd);
    uint32_t totalLength = sizeof(VmiCmd) + static_cast<uint32_t>(length);
    uint8_t *data = static_cast<uint8_t *>(malloc(totalLength));
    if (data == nullptr) {
        ERR("Error: malloc nmea data buffer is nullptr");
        env->ReleaseByteArrayElements(jData, reinterpret_cast<jbyte *>(nmeaData), 0);
        return JNI_FALSE;
    }

    memcpy(data, &vmiCmd, cmdLength);
    memcpy(data + cmdLength, nmeaData, length);

    uint32_t ret = SendData(VMIMsgType::GPS, data, totalLength);
    env->ReleaseByteArrayElements(jData, reinterpret_cast<jbyte *>(nmeaData), 0);
    free(data);
    if (ret != 0) {
        ERR("Failed to send Nmea data, ret = %u", ret);
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL
OPENGL_JNI(sendFpsTestCommand)(JNIEnv *env, jclass cls, jbyteArray jData, int length) {
    (void) env;
    (void) cls;
    uint8_t *data = reinterpret_cast<uint8_t *>(env->GetByteArrayElements(jData, nullptr));
    uint32_t ret = SendData(VMIMsgType::FPS, data, length);
    env->ReleaseByteArrayElements(jData, reinterpret_cast<jbyte *>(data), 0);
    if (ret == 0) {
        return JNI_TRUE;
    } else {
        ERR("Send fps test command cmd failed, ret = %u", ret);
        return JNI_FALSE;
    }
}
}
