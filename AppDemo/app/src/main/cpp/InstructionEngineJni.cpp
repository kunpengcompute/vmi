//
// Created by Administrator on 2021/9/6.
//

#include "InstructionEngineJni.h"
#include <string>
#include <sstream>
#include <arpa/inet.h>
#include "dlfcn.h"
#include <android/native_window_jni.h>
#include <android/log.h>
#include "InstructionEngineClient.h"

// 定义各种类型的LOG输出方式
#define TAG "InstructionEngineJni"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL, TAG, __VA_ARGS__)

namespace {
    // Java环境全局指针
    JNIEnv* g_env = nullptr;

    // Java类对象
    jobject g_obj = nullptr;

    // 全局JVM指针
    JavaVM *g_pvm = nullptr;

    // 设置JVM环境的标志
    bool g_alertLogInited = false;

    // 标志位，表示是否已经load二进制文件
    void *g_handle = nullptr;

    // 定义通信库常量
    const std::string LIB_COMM_NAME = "libCommunication.so";

    // 设置内部通信库连接的IP和端口常量
    const std::string CONFIG_NET_ADDRESS_FUNC = "ConfigNetAddress";

    // 设置内部通信库接口的定义方式
    using ConfigNetAddressFunc = void (*)(unsigned int, unsigned short int, unsigned int);

    // 内部通信库接口初始化
    ConfigNetAddressFunc g_configNetAddressFunc = nullptr;

    // 指令流事件通知回调java方法
    const std::string INST_ENG_EVENT_NOTICE_JAVA_METHOD_NAME = "onVmiInstructionEngineEvent";
}

/**
 * @功能描述：字符串IP转主机字节序函数
 * @参数 [in] ip：代表字符串ip
 * @返回值：返回整形，主机字节序的ip地址
 */
unsigned int GetIpHostOrder(std::string ip) {
    struct in_addr addr = {};
    int result = inet_aton(ip.c_str(), &addr);
    if (result == 0) {
        LOGE("invaild ip, please check ip.");
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
std::string Jstring2String(JNIEnv* env, jstring jStr) {
    // 定义java String类 stringClass
    const jclass stringClass = env->GetObjectClass(jStr);
    // 获取String(byte[], String)的构造器，用于将本地byte[]数组转化为一个新的String
    const jmethodID getBytes = env->GetMethodID(stringClass, "getBytes", "(Ljava/lang/String;)[B");
    // 创建一个新的字符串对象，并自动转化成java所支持的UTF-8编码格式
    const jstring strenCode = env->NewStringUTF("UTF-8");
    // 通过实例类调用getBytes方法名
    const jbyteArray stringJbytes = reinterpret_cast<jbyteArray>(env->CallObjectMethod(jStr, getBytes, strenCode));
    // 获取数组大小
    size_t length = static_cast<size_t>(env->GetArrayLength(stringJbytes));
    // 本地的数组指针直接指向java端的数组地址
    jbyte* pBytes = env->GetByteArrayElements(stringJbytes, JNI_FALSE);
    // 将char*转化为string类型，返回string值
    std::string retString = std::string(reinterpret_cast<char *>(pBytes), length);
    // 指针释放
    env->ReleaseByteArrayElements(stringJbytes, pBytes, 0);
    // 删除localRef所指向的局部引用
    env->DeleteLocalRef(stringJbytes);
    env->DeleteLocalRef(stringClass);
    return retString;
}

void CallJavaVoidFunction(const std::string& funName, EngineEvent engineEvent) {
    if (!g_alertLogInited) {
        LOGE("g_alertLogInited is false.");
        return;
    }
    JNIEnv *env = nullptr;
    int status = g_pvm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_4);
    if (status == JNI_EDETACHED) {
        status = g_pvm->AttachCurrentThread(&env, nullptr);
        if (status != JNI_OK) {
            LOGE("AttachCurrentThread failed, status: %d.", status);
            env = nullptr;
            return;
        }
    }

    if (env == nullptr) {
        LOGE("call void function, can't get java environment.");
        return;
    }

    jclass cls = env->GetObjectClass(g_obj);
    jmethodID mid = env->GetMethodID(cls, funName.c_str(), "(IIIII)V");
    if (mid != nullptr) {
        env->CallVoidMethod(g_obj, mid, engineEvent.event, engineEvent.reserved[0],  engineEvent.reserved[1],
                            engineEvent.reserved[2],  engineEvent.reserved[3]);
    } else {
        LOGE("can't found %s method.", funName.c_str());
        return;
    }
    (void)g_pvm->DetachCurrentThread();
}

/**
 * @功能描述：回调java层的onVmiInstructionEngineEvent函数，通知指令流引擎事件产生
 * @参数 [in] event：代表指令流引擎事件，
 *                   目前有VMI_INSTRUCTION_ENGINE_EVENT_SOCK_DISCONN和VMI_INSTRUCTION_ENGINE_EVENT_PKG_BROKEN事件
 */
void CallJavaOnVmiInstructionEngineEvent(EngineEvent engineEvent) {
    CallJavaVoidFunction(INST_ENG_EVENT_NOTICE_JAVA_METHOD_NAME, engineEvent);
}

/**
* @功能描述：指令流引擎初始化函数
* @参数 [in] env：代表Java环境指针
* @参数 [in] obj：代表native方法的类的class对象实例
* @返回值：VMI_SUCCESS，代表引擎初始化成功；其他代表引擎初始化失败。
*/
JNIEXPORT jint JNICALL INSTUCTION_JNI(initialize)(JNIEnv* env, jclass cls) {
    (void) env;
    (void) cls;
    uint32_t result = Initialize(CallJavaOnVmiInstructionEngineEvent);
    return static_cast<jint>(result);
}

/**
* @功能描述：指令流引擎初始化启动函数
* @参数 [in] env：代表Java环境指针
* @参数 [in] obj：代表native方法的类的class对象实例
* @参数 [in] surface：代表java层传下来的surface
* @参数 [in] width：代表surface的宽度
* @参数 [in] height：代表surface的高度
* @参数 [in] densityDpi：代表surface的像素密度
* @返回值：VMI_SUCCESS，代表启动RenderWindow成功；其他代表启动RenderWindow失败。
*/
JNIEXPORT jint JNICALL INSTUCTION_JNI(start)(JNIEnv* env, jclass cls, jobject surface, jint width, jint height, jfloat densityDpi) {
    (void) env;
    (void) cls;

    // 参数校验
    if (env == nullptr) {
        LOGE("start window, can't get java environment.");
        return VMI_INSTRUCTION_CLIENT_START_FAIL;
    }

    if (surface == nullptr) {
        LOGE("surface view is nullptr.");
        return VMI_INSTRUCTION_CLIENT_START_FAIL;
    }

    ANativeWindow* nativeWindow = ANativeWindow_fromSurface(env, surface);
    if (nativeWindow == nullptr) {
        LOGE("can't get native window from surface");
        return VMI_INSTRUCTION_CLIENT_START_FAIL;
    }

    uint32_t result = Start(reinterpret_cast<uint64_t>(nativeWindow), static_cast<uint32_t>(width),
                            static_cast<uint32_t>(height), static_cast<uint32_t>(densityDpi));
    return result;
}

/**
* @功能描述：指令流引擎停止函数
* @参数 [in] env：代表Java环境指针
* @参数 [in] obj：代表native方法的类的class对象实例
*/
JNIEXPORT void JNICALL INSTUCTION_JNI(stop)(JNIEnv* env, jclass cls) {
    (void) env;
    (void) cls;
    Stop();
}

/**
 * @功能描述：指令流引擎获取帧率信息
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] obj：代表native方法的类的class对象实例
 * @返回值：返回帧率统计信息
 */
JNIEXPORT jstring JNICALL INSTUCTION_JNI(getStat)(JNIEnv* env, jclass cls) {
    (void) env;
    (void) cls;
    std::stringstream strStream;

    // 参数校验
    if (env == nullptr) {
        LOGE("get string, can't get java environment.");
        return nullptr;
    }

    StatusInfo status = GetStat();
    uint64_t bytesRecv = status.bytesRecv; // 单次查询累计网络接收数据字节数
    uint64_t bytesSend = status.bytesSend; // 单次查询累计网络接收数据字节数
    uint64_t lag = status.lag; // 网络时延(ms)
    strStream << "BytesRecv: " << bytesRecv << "RecvEnd\n";
    strStream << "BytesSend: " << bytesSend << "SendEnd\n";
    strStream << "LAG: " << lag << "ms\n";
    for (int i = 0; i < 4; i++) {
        uint32_t tid = status.info[i].tid;
        if (status.info[i].fps != 0) {
            strStream << "tid: " << tid << ", " << "fps: " << status.info[i].fps << ", "
            << "frameCount: " << status.info[i].frameCount << "\n";
        }
    }
    return env->NewStringUTF(strStream.str().c_str());
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
JNIEXPORT jint JNICALL INSTUCTION_JNI(recvData)(JNIEnv* env, jclass cls, jbyte type,
        jbyteArray jData, jint length){
    (void) env;
    (void) cls;

    // 参数校验
    if (env == nullptr) {
        LOGE("recv data, can't get java environment.");
        return 0;
    }

    uint8_t* data = reinterpret_cast<uint8_t *>(env->GetByteArrayElements(jData, nullptr));
    if (data == nullptr) {
        LOGE("recv data, data is nullptr.");
        return 0;
    }

    // 接收服务端传递的数据
    int ret = RecvData(static_cast<VMIMsgType>(type), data, length);
    env->ReleaseByteArrayElements(jData, reinterpret_cast<jbyte *>(data), 0);
    return ret;
}

/**
* @功能描述：发送控数据到agent端
* @参数 [in] env：代表Java环境指针
* @参数 [in] cls：代表对应JAVA类对象
* @参数 [in] jData：代表音频数据
* @参数 [in] length: 代表音频数据长度
* @返回值：发送结果。JNI_TRUE代表发送音频数据成功，JNI_FALSE代表发送音频数据失败
*/
JNIEXPORT jboolean JNICALL INSTUCTION_JNI(sendAudioDataArray)(JNIEnv* env, jclass cls,
        jbyteArray jData, jint length) {
    (void) env;
    (void) cls;

    int audioLength = (int)length;

    // 参数校验
    if (env == nullptr) {
        LOGE("send audio event, can't get java environment.");
        return JNI_FALSE;
    }

    uint8_t* data = reinterpret_cast<uint8_t *>(env->GetByteArrayElements(jData, nullptr));
    if (data == nullptr) {
        LOGE("Error: send audio, src buffer is nullptr");
        return JNI_FALSE;
    }

    uint32_t result = SendData(VMIMsgType::AUDIO, data, audioLength);
    env->ReleaseByteArrayElements(jData, reinterpret_cast<jbyte *>(data), 0);
    return (result == VMI_SUCCESS) ? JNI_TRUE : JNI_FALSE;
}

/**
* @功能描述：发送触控数据到agent端
* @参数 [in] env：代表Java环境指针
* @参数 [in] cls：代表对应JAVA类对象
* @参数 [in] data：代表多点触控Y坐标
* @参数 [in] length：代表发送触控信息的长度
* @返回值：发送结果。JNI_TRUE代表发送触控信息成功，JNI_FALSE代表发送触控信息失败
*/
JNIEXPORT jboolean JNICALL INSTUCTION_JNI(sendTouchEventArray)(JNIEnv* env, jclass cls,
        jbyteArray jData, jint length){
    (void) env;
    (void) cls;

    int touchLength = (int)length;

    // 参数校验
    if (env == nullptr) {
        LOGE("send touch event, can't get java environment.");
        return JNI_FALSE;
    }

    uint8_t* data = reinterpret_cast<uint8_t *>(env->GetByteArrayElements(jData, nullptr));
    if (data == nullptr) {
        LOGE("Error: send touch, src buffer is nullptr");
        return JNI_FALSE;
    }

    uint32_t result = SendData(VMIMsgType::TOUCH_INPUT, data, touchLength);
    env->ReleaseByteArrayElements(jData, reinterpret_cast<jbyte *>(data), 0);
    return (result == VMI_SUCCESS) ? JNI_TRUE : JNI_FALSE;
}

/**
* @功能描述：发送模拟导航栏触控数据到agent端
* @参数 [in] env：代表Java环境指针
* @参数 [in] cls：代表对应JAVA类对象
* @参数 [in] jData：代表触控数据
* @参数 [in] length：代表发送触控信息的长度
* @返回值：发送结果。JNI_TRUE代表发送触控信息成功，JNI_FALSE代表发送触控信息失败
*/
JNIEXPORT jboolean JNICALL INSTUCTION_JNI(sendKeyEvent)(JNIEnv* env, jclass cls,
        jbyteArray jData, jint length) {
    (void) env;
    (void) cls;

    int keyLength = (int)length;

    // 参数校验
    if (env == nullptr) {
        LOGE("send navbar event, can't get java environment.");
        return JNI_FALSE;
    }

    uint8_t* data = reinterpret_cast<uint8_t *>(env->GetByteArrayElements(jData, nullptr));
    if (data == nullptr) {
        LOGE("Error: send navbar, src buffer is nullptr");
        return JNI_FALSE;
    }

    uint32_t result = SendData(VMIMsgType::NAVBAR_INPUT, data, keyLength);
    env->ReleaseByteArrayElements(jData, reinterpret_cast<jbyte *>(data), 0);
    return (result == VMI_SUCCESS) ? JNI_TRUE : JNI_FALSE;
}

/**
 * @功能描述：设置JVM环境
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] obj：代表native方法的类的class对象实例
 */
JNIEXPORT void JNICALL CB_JNI(setNativeCallback)(JNIEnv* env, jobject obj) {
    (void) env;

    // 参数校验
    if (env == nullptr) {
        LOGE("send callback, can't get java environment.");
        return;
    }

    // 获取cls类
    jclass cls = env->GetObjectClass(obj);
    // 获取mid方法
    jmethodID min = env->GetMethodID(cls, INST_ENG_EVENT_NOTICE_JAVA_METHOD_NAME.c_str(), "(IIIII)V");
    if (min == nullptr) {
        LOGE("set native callback, mid is nullptr.");
        return;
    }

    // 创建一个obj
    g_obj = env->NewGlobalRef(obj);
    // 创建一个env全局指针
    g_env = env;
    g_alertLogInited = true;
    jint result = env->GetJavaVM(&g_pvm);
    if (result != JNI_OK) {
        LOGE("Error: set callback, can't get java VM");
    }
}

/**
* @功能描述：加载原型通信库函数
* @参数 [in] env：代表Java环境指针
* @参数 [in] obj：代表native方法的类的class对象实例
* @返回值：true，代表加载通信库运行；false代表加载通信库失败。
*/
JNIEXPORT jboolean JNICALL NET_CONFIG_JNI(initialize)(JNIEnv* env, jclass cls) {
    (void) env;
    (void) cls;
    if (g_handle != nullptr) {
        LOGI("Initialization completed.");
        return true;
    }

    // 加载通信的动态库
    g_handle = dlopen(LIB_COMM_NAME.c_str(), RTLD_GLOBAL | RTLD_LAZY);
    if (g_handle == nullptr) {
        LOGE("Failed to open shared library: %s, detail error information: %s",
             LIB_COMM_NAME.c_str(), dlerror());
        return false;
    }

    // 寻找到ConfigNetAddress设置IP和端口函数
    g_configNetAddressFunc = reinterpret_cast<ConfigNetAddressFunc>(dlsym(g_handle, CONFIG_NET_ADDRESS_FUNC.c_str()));
    if (g_configNetAddressFunc == nullptr) {
        LOGE("Failed to find function symbol: %s", CONFIG_NET_ADDRESS_FUNC.c_str());
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
* @参数 [in] type：代表通信连接类型
* @返回值：true，代表加载通信库运行；false代表加载通信库失败。
*/
JNIEXPORT jboolean JNICALL NET_CONFIG_JNI(setNetConfig)(JNIEnv* env, jclass cls,
        jstring ip, jint port, jint type) {
    (void) env;
    (void) cls;

    if (g_configNetAddressFunc == nullptr) {
        LOGE("setNetConfig failed, g_configNetAddressFunc is nullptr.");
        return false;
    }
    // 设置IP和端口
    g_configNetAddressFunc(GetIpHostOrder(Jstring2String(env, ip)), port, type);
    return true;
}