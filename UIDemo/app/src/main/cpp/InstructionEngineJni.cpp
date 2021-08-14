/*
 * 版权所有 (c) 华为技术有限公司 2017-2021
 * 功能说明:指令流引擎jni接口函数，供java层调用
 */
#define LOG_TAG "InstructionEngineClientJni"
#include "InstructionEngineJni.h"
#include <string>
#include <sstream>
#include <arpa/inet.h>
#include "dlfcn.h"
#include <android/native_window_jni.h>
#include <android/log.h>
#include "InstructionEngineClient.h"

namespace {
    // Java环境全局指针
    JNIEnv* g_env = nullptr;

    // Java类对象
    jobject g_obj = nullptr;

    // 全局JVM指针
    JavaVM *g_pvm = nullptr;

    // 设置JVM环境的标志
    bool g_alertLogInited = false;

    // 指令流事件通知回调Java方法
    const std::string INST_ENG_EVENT_NOTICE_JAVA_METHOD_NAME = "onVmiInstructionEngineEvent";

    const std::string LIB_COMM_NAME = "libCommunication.so";
    
    void *g_handle = nullptr;
    
    using ConfigNetAddressFunc = void (*)(unsigned int, unsigned short int, unsigned int);
    
    ConfigNetAddressFunc g_configNetAddressFunc = nullptr;
}

#ifndef LOG_TAG
#define LOG_TAG "InstructionEngineClientJni"
#endif

#define DBG(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define INFO(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define WARN(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define ERR(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define FATAL(...) __android_log_print(ANDROID_LOG_FATAL, LOG_TAG, __VA_ARGS__)

/**
 * @功能描述：字符串IP转主机字节序函数
 * @参数 [in] ip：代表字符串ip
 * @返回值：返回整形，主机字节序的ip地址
 */
unsigned int GetIpHostOrder(std::string ip)
{
    struct in_addr addr = {};
    int ret = inet_aton(ip.c_str(), &addr);
    if (ret == 0) {
        ERR("error: Invalid ip, please check");
        return 0;
    }
    return ntohl(addr.s_addr);
}

/**
 * @功能描述：jstring转换成string
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] jStr：jstring字符窗
 * @返回值：返回string字符串
 */
std::string Jstring2String(JNIEnv* env, jstring jStr)
{
    const jclass stringClass = env->GetObjectClass(jStr);
    const jmethodID getBytes = env->GetMethodID(stringClass, "getBytes", "(Ljava/lang/String;)[B");
    const jbyteArray stringJbytes =
        reinterpret_cast<jbyteArray>(env->CallObjectMethod(jStr, getBytes, env->NewStringUTF("UTF-8")));
    size_t length = static_cast<size_t>(env->GetArrayLength(stringJbytes));
    jbyte* pBytes = env->GetByteArrayElements(stringJbytes, nullptr);
    std::string ret = std::string(reinterpret_cast<char *>(pBytes), length);
    env->ReleaseByteArrayElements(stringJbytes, pBytes, 0);
    env->DeleteLocalRef(stringJbytes);
    env->DeleteLocalRef(stringClass);
    return ret;
}

/**
 * @功能描述：加载原型通信库函数
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] obj：代表native方法的类的class对象实例
 * @返回值：true，代表加载通信库运行；false代表加载通信库失败。
 */
JNIEXPORT jboolean JNICALL NET_CONFIG_JNI(initialize)(JNIEnv* env, jclass cls)
{
    (void) env;
    (void) cls;
    if (g_handle != nullptr) {
        return true;
    }

    // libCommunication.so 内置于客户端 apk 中，对于攻击者（一般处于其他进程）没法替换本进程 apk 内置的动态库
    // 此处可以使用相对路径来调用动态库
    g_handle = dlopen(LIB_COMM_NAME.c_str(), RTLD_GLOBAL | RTLD_LAZY);
    if (g_handle == nullptr) {
        ERR("error: Failed to open shared library:%s", LIB_COMM_NAME.c_str());
        return false;
    }

    const std::string CONFIG_NET_ADDRESS_FUNC = "ConfigNetAddress";
    g_configNetAddressFunc = reinterpret_cast<ConfigNetAddressFunc>(
        dlsym(g_handle, CONFIG_NET_ADDRESS_FUNC.c_str()));
    if (g_configNetAddressFunc == nullptr) {
        ERR("error: Failed to find function symbol:%s", CONFIG_NET_ADDRESS_FUNC.c_str());
        dlclose(g_handle);
        return false;
    }

    return true;
}

/**
 * @功能描述：设置网路参数，云手机的IP或者port
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] obj：代表native方法的类的class对象实例
 * @参数 [in] ip：代表云手机服务器的IP
 * @参数 [in] port：代表云手机服务器的port
 * @参数 [in] connectType：代表通信连接类型
 * @返回值：true，代表加载通信库运行；false代表加载通信库失败。
 */
JNIEXPORT jboolean JNICALL NET_CONFIG_JNI(setNetConfig)(JNIEnv* env, jclass cls,
    jstring ip, jint port, jint connectType)
{
    (void) cls;
    if (g_configNetAddressFunc == nullptr) {
        ERR("Error: setting net config failed, config net function is nullptr");
        return false;
    }

    g_configNetAddressFunc(GetIpHostOrder(Jstring2String(env, ip)), port, connectType);
    return true;
}

void CallJavaVoidFunction(const std::string& funName, EngineEvent engineEvent)
{
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
    jmethodID mid = env->GetMethodID(cls, funName.c_str(), "(IIIII)V");
    if (mid != nullptr) {
        env->CallVoidMethod(g_obj, mid, engineEvent.event, engineEvent.reserved[0],  engineEvent.reserved[1],
             engineEvent.reserved[2],  engineEvent.reserved[3]);
    } else {
        ERR("Error: Can't found %s method", funName.c_str());
    }
    (void) g_pvm->DetachCurrentThread();
}

/**
 * @功能描述：回调java层的onVmiInstructionEngineEvent函数，通知指令流引擎事件产生
 * @参数 [in] event：代表指令流引擎事件，
 *                   目前有VMI_INSTRUCTION_ENGINE_EVENT_SOCK_DISCONN和VMI_INSTRUCTION_ENGINE_EVENT_PKG_BROKEN事件
 */
void CallJavaOnVmiInstructionEngineEvent(EngineEvent engineEvent)
{
    CallJavaVoidFunction(INST_ENG_EVENT_NOTICE_JAVA_METHOD_NAME, engineEvent);
}

/**
 * @功能描述：指令流引擎初始化函数
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] cls：代表native方法的类的class对象实例
 * @返回值：VMI_SUCCESS，代表引擎初始化成功；其他代表引擎初始化失败。
 */
JNIEXPORT jint JNICALL INSTUCTION_JNI(initialize)(JNIEnv* env, jclass cls)
{
    (void) env;
    (void) cls;
    uint32_t ret = Initialize(CallJavaOnVmiInstructionEngineEvent);
    return static_cast<jint>(ret);
}

/**
 * @功能描述：指令流引擎初始化启动函数
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] cls：代表native方法的类的class对象实例
 * @参数 [in] surface：代表java层传下来的surface
 * @参数 [in] width：代表surface的宽度
 * @参数 [in] height：代表surface的高度
 * @参数 [in] densityDpi：代表surface的像素密度
 * @返回值：VMI_SUCCESS，代表启动RenderWindow成功；其他代表启动RenderWindow失败。
 */
JNIEXPORT jint JNICALL INSTUCTION_JNI(start)(JNIEnv *env, jclass cls, jobject surface, jint width,
    jint height, jfloat densityDpi)
{
    (void) cls;
    if (env == nullptr) {
        ERR("Error: start window, can't get java environment");
        return VMI_INSTRUCTION_CLIENT_START_FAIL;
    }

    if (surface == nullptr) {
        ERR("Error: surface view is nullptr");
        return VMI_INSTRUCTION_CLIENT_START_FAIL;
    }

    ANativeWindow* nativeWindow = ANativeWindow_fromSurface(env, surface);
    if (nativeWindow == nullptr) {
        ERR("Error: can't get native window from surface");
        return VMI_INSTRUCTION_CLIENT_START_FAIL;
    }

    uint32_t result = Start(reinterpret_cast<uint64_t>(nativeWindow), static_cast<uint32_t>(width),
        static_cast<uint32_t>(height), static_cast<uint32_t>(densityDpi));
    return result;
}

/**
 * @功能描述：SDK停止销毁函数，断开socket，销毁消息队列和逻辑线程
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] cls：代表对应JAVA类对象
 * @参数 [in] surface：代表上层传来的surfaceview对象
 */
JNIEXPORT void JNICALL INSTUCTION_JNI(stop)(JNIEnv* env, jclass cls)
{
    (void) env;
    (void) cls;
    Stop();
}

/**
 * @功能描述：指令流引擎获取帧率信息
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] cls：代表native方法的类的class对象实例
 * @返回值：返回帧率统计信息
 */
JNIEXPORT jstring JNICALL INSTUCTION_JNI(getStat)(JNIEnv* env, jclass cls)
{
    (void) cls;
    if (env == nullptr) {
        ERR("Error: get string, can't get java environment");
        return nullptr;
    }

    StatusInfo status = GetStat();
    std::stringstream ss;
    uint64_t lag = status.lag;
    uint64_t recv = status.bytesRecv;
    uint64_t send = status.bytesSend;
    ss << "BytesRecv: " << recv << "RecvEnd \n";
    ss << "BytesSend: " << send << "SendEnd \n";
    ss << "LAG: " << lag << "ms \n";
    ss << "FrameRate:\n";
    for (int i = 0; i < 4; i++) {
        uint32_t tid = status.info[i].tid;
        if (status.info[i].fps != 0) {
            ss << "tid:" << tid << ", " << "fps:" << status.info[i].fps
                << ", " << "frameCount:" << status.info[i].frameCount << "\n";
        }
    }

    return env->NewStringUTF(ss.str().c_str());
}

/**
 * @功能描述：设置JVM环境
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] obj：代表native方法的类的class对象实例
 */
JNIEXPORT void JNICALL CB_JNI(setNativeCallback)(JNIEnv* env, jobject obj)
{
    if (env == nullptr) {
        ERR("Error: set callback, can't get java environment");
        return;
    }

    jclass cls = env->GetObjectClass(obj);
    jmethodID mid = env->GetMethodID(cls, INST_ENG_EVENT_NOTICE_JAVA_METHOD_NAME.c_str(), "(IIIII)V");
    g_obj = env->NewGlobalRef(obj);
    g_env = env;
    if (mid == nullptr) {
        ERR("Error: set callback, mid is nullptr");
        return;
    }
    g_alertLogInited = true;
    jint ret = env->GetJavaVM(&g_pvm);
    if (ret != JNI_OK) {
        ERR("Error: set callback, can't get java VM");
    }
}

/**
 * @功能描述：发送触控数据到agent端
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] cls：代表对应JAVA类对象
 * @参数 [in] jData：代表多点触控Y坐标
 * @参数 [in] length：代表发送触控信息的长度
 * @返回值：发送结果。JNI_TRUE代表发送触控信息成功，JNI_FALSE代表发送触控信息失败
 */
JNIEXPORT jboolean JNICALL INSTUCTION_JNI(sendTouchEventArray)(JNIEnv* env, jclass cls, jbyteArray jData, int length)
{
    (void) cls;
    if (env == nullptr) {
        ERR("Error: send touch event, can't get java environment");
        return JNI_FALSE;
    }

    uint8_t* data = reinterpret_cast<uint8_t *>(env->GetByteArrayElements(jData, nullptr));
    if (data == nullptr) {
        ERR("Error: send touch, src buffer is nullptr");
        return JNI_FALSE;
    }

    uint32_t ret = SendData(VMIMsgType::TOUCH_INPUT, data, length);
    env->ReleaseByteArrayElements(jData, reinterpret_cast<jbyte *>(data), 0);
    return (ret == VMI_SUCCESS) ? JNI_TRUE : JNI_FALSE;
}

/**
 * @功能描述：发送模拟导航栏触控数据到agent端
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] cls：代表对应JAVA类对象
 * @参数 [in] jData：代表触控数据
 * @参数 [in] length：代表发送触控信息的长度
 * @返回值：发送结果。JNI_TRUE代表发送触控信息成功，JNI_FALSE代表发送触控信息失败
 */
JNIEXPORT jboolean JNICALL INSTUCTION_JNI(sendKeyEvent)(JNIEnv* env, jclass cls, jbyteArray jData, int length)
{
    (void) cls;
    if (env == nullptr) {
        ERR("Error: send navbar input event, can't get java environment");
        return JNI_FALSE;
    }

    uint8_t* data = reinterpret_cast<uint8_t *>(env->GetByteArrayElements(jData, nullptr));
    if (data == nullptr) {
        ERR("Error: send touch, src buffer is nullptr");
        return JNI_FALSE;
    }

    uint32_t ret = SendData(VMIMsgType::NAVBAR_INPUT, data, length);
    env->ReleaseByteArrayElements(jData, reinterpret_cast<jbyte *>(data), 0);
    return (ret == VMI_SUCCESS) ? JNI_TRUE : JNI_FALSE;
}

/**
 * @功能描述：发送控数据到agent端
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] cls：代表对应JAVA类对象
 * @参数 [in] jData：代表音频数据
 * @参数 [in] length: 代表音频数据长度
 * @返回值：发送结果。JNI_TRUE代表发送音频数据成功，JNI_FALSE代表发送音频数据失败
 */
JNIEXPORT jboolean JNICALL INSTUCTION_JNI(sendAudioDataArray)(JNIEnv* env, jclass cls, jbyteArray jData, int length)
{
    (void) cls;
    if (env == nullptr) {
        ERR("Error: send audio event, can't get java environment");
        return JNI_FALSE;
    }

    uint8_t* data = reinterpret_cast<uint8_t *>(env->GetByteArrayElements(jData, nullptr));
    if (data == nullptr) {
        ERR("Error: send audio, src buffer is nullptr");
        return JNI_FALSE;
    }

    uint32_t ret = SendData(VMIMsgType::AUDIO, data, length);
    env->ReleaseByteArrayElements(jData, reinterpret_cast<jbyte *>(data), 0);
    return (ret == VMI_SUCCESS) ? JNI_TRUE : JNI_FALSE;
}

/**
 * @功能描述：获取agent端传输过来的数据，指令流，心跳，音频信息
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] cls：代表对应JAVA类对象
 * @参数 [in] type：代表数据类型
 * @参数 [out] jData：代表接收数组buffer
 * @参数 [in] length：代表数组长度
 * @返回值：实际接受到数据长度
 */
JNIEXPORT jint JNICALL INSTUCTION_JNI(recvData)(JNIEnv* env, jclass cls, jbyte type, jbyteArray jData, int length)
{
    (void) cls;
    if (env == nullptr) {
        ERR("Error: recv data, can't get java environment");
        return 0;
    }

    uint8_t* data = reinterpret_cast<uint8_t *>(env->GetByteArrayElements(jData, nullptr));
    if (data == nullptr) {
        ERR("Error: recv data, src buffer is nullptr");
        return 0;
    }
    // 从队列里面取数据
    int ret = RecvData(static_cast<VMIMsgType>(type), data, length);
    env->ReleaseByteArrayElements(jData, reinterpret_cast<jbyte *>(data), 0);
    return ret;
}