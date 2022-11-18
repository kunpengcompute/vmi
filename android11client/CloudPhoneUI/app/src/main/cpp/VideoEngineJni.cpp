/*
 * 版权所有 (c) 华为技术有限公司 2017-2021
 * 功能说明:指令流引擎jni接口函数，供java层调用
 */

#define LOG_TAG "VideoEngineJni"

#include "VideoEngineJni.h"
#include <android/native_activity.h>
#include <cstdlib>
#include <jni.h>
#include <map>
#include <string>
#include <sstream>
#include <arpa/inet.h>
#include <dlfcn.h>
#include <unistd.h>
#include <android/native_window_jni.h>
#include <android/log.h>
#include "VideoEngineClient.h"

namespace {

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

const std::string LIB_COMM_NAME = "libCommunication.so";

void *g_handle = nullptr;

using ConfigNetAddressFunc = void (*)(unsigned int, unsigned short int, unsigned int);

ConfigNetAddressFunc g_configNetAddressFunc = nullptr;

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
}__attribute__((packed));

ANativeWindow *g_nativeWindow = nullptr;
}

#define DBG(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define INFO(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define WARN(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define ERR(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define FATAL(...) __android_log_print(ANDROID_LOG_FATAL, LOG_TAG, __VA_ARGS__)

void VmiMonitorEnter(JNIEnv *env, jobject obj) {
  while (((*env).MonitorEnter(obj)) != JNI_OK) {
    usleep(VMI_MONITOR_SLEEP_TIME);
  }
  INFO("JNI critical sections entered");
}

void VmiMonitorExit(JNIEnv *env, jobject obj) {
  while (((*env).MonitorExit(obj)) != JNI_OK) {
    usleep(VMI_MONITOR_SLEEP_TIME);
  }
  INFO("JNI critical sections exited");
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

/**
 * @功能描述：jstring转换成string
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] jStr：jstring字符窗
 * @返回值：返回string字符串
 */
std::string Jstring2String(JNIEnv *env, jstring
jStr) {
  const jclass stringClass = env->GetObjectClass(jStr);
  const jmethodID getBytes = env->GetMethodID(stringClass, "getBytes", "(Ljava/lang/String;)[B");
  const jbyteArray stringJbytes =
      reinterpret_cast<jbyteArray>(env->CallObjectMethod(jStr, getBytes,
                                                         env->NewStringUTF("UTF-8")));
  size_t length = static_cast<size_t>(env->GetArrayLength(stringJbytes));
  jbyte *pBytes = env->GetByteArrayElements(stringJbytes, nullptr);
  std::string ret = std::string(reinterpret_cast<char *>(pBytes), length);
  env->
      ReleaseByteArrayElements(stringJbytes, pBytes,2);
  env->
      DeleteLocalRef(stringJbytes);
  env->
      DeleteLocalRef(stringClass);
  return
      ret;
}

/**
 * @功能描述：加载原型通信库函数
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] obj：代表native方法的类的class对象实例
 * @返回值：true，代表加载通信库运行；false代表加载通信库失败。
 */
extern "C" {
JNIEXPORT jboolean
JNICALL NET_CONFIG_JNI(initialize)(JNIEnv *env, jclass cls) {
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
JNIEXPORT jboolean
JNICALL NET_CONFIG_JNI(setNetConfig)(JNIEnv *env, jclass
cls,
                                     jstring ip, jint
                                     port,
                                     jint connectType
) {
  (void)cls;
  (void) env;
  if (g_configNetAddressFunc == nullptr) {
    ERR("Error: setting net config failed, config net function is nullptr");
    return false;
  }

  g_configNetAddressFunc(GetIpHostOrder(Jstring2String(env, ip)), port, connectType
  );
  return true;
}

JNIEXPORT void JNICALL CB_JNI(setObj)(JNIEnv *env, jobject obj) {
  if (env == nullptr) {
    ERR("Error: set callback,can't get java environment");
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
    ERR("cannot get java vm");
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
  jmethodID mid = env->GetMethodID(cls, funName.c_str(), "(IIIII)V");
  if (mid != nullptr) {
    env->CallVoidMethod(g_obj, mid, engineEvent.event, engineEvent.para1,
                        engineEvent.para2,
                        engineEvent.para3, engineEvent.para4);
  } else {
    ERR("Error: Can't found %s method", funName.c_str());
  }
  env->DeleteLocalRef(cls);
  (void) g_pvm->DetachCurrentThread();
}

/**
 * @功能描述：回调java层的onVmiInstructionEngineEvent函数，通知指令流引擎事件产生
 * @参数 [in] event：代表指令流引擎事件，
 *                   目前有VMI_INSTRUCTION_ENGINE_EVENT_SOCK_DISCONN和VMI_INSTRUCTION_ENGINE_EVENT_PKG_BROKEN事件
 */
void CallJavaOnVmiVideoEngineEvent(EngineEvent engineEvent) {
  (void) engineEvent;
  CallJavaVoidFunction(VIDEO_ENG_EVENT_NOTICE_JAVA_METHOD_NAME, engineEvent);
}

/**
 * @功能描述：指令流引擎初始化函数
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] cls：代表native方法的类的class对象实例
 * @返回值：VMI_SUCCESS，代表引擎初始化成功；其他代表引擎初始化失败。
 */
JNIEXPORT jint

JNICALL OPENGL_JNI(initialize)(JNIEnv *env, jclass cls) {
  (void) env;
  (void) cls;
  uint32_t ret = Initialize(CallJavaOnVmiVideoEngineEvent);
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
JNIEXPORT jint OPENGL_JNI(start)(JNIEnv *env,
                                 jclass cls,
                                 jobject surface,
                                 jint width,
                                 jint height,
                                 jfloat densityDpi) {
  (void) env;
  (void) cls;
  INFO("OpenGLJNIWrapper start enter");
  VmiMonitorEnter(env, cls);
  if (surface == nullptr) {
    ERR("nativeWindow set to nullptr");
    VmiMonitorExit(env, cls);
    return VMI_CLIENT_START_FAIL;
  }
  ANativeWindow *native_window = ANativeWindow_fromSurface(env, surface);
  if (native_window == nullptr) {
    ERR("nativeWindow is nullptr");
    VmiMonitorExit(env, cls);
    return VMI_CLIENT_START_FAIL;
  }
  g_nativeWindow = native_window;
  uint32_t ret = Start(reinterpret_cast<u_int64_t>(native_window), static_cast<uint32_t>(width),
                       static_cast<uint32_t>(height), static_cast<uint32_t>(densityDpi));
  if (ret != VMI_SUCCESS) {
    ERR("Fail to start ,ret: %u", ret);
    VmiMonitorExit(env, cls);
    return VMI_CLIENT_START_FAIL;
  }
  VmiMonitorExit(env, cls);
  return VMI_SUCCESS;
}

JNIEXPORT void JNICALL OPENGL_JNI(stop)(JNIEnv *env, jclass cls) {
  (void) env;
  (void) cls;
  INFO("OpenGLWrapper stop enter");
  VmiMonitorEnter(env, cls);
  Stop();
  if (g_nativeWindow != nullptr) {
    ANativeWindow_release(g_nativeWindow);
    g_nativeWindow = nullptr;
  }
  VmiMonitorExit(env, cls);
}

/**
 * @功能描述：指令流引擎获取帧率信息
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] cls：代表native方法的类的class对象实例
 * @返回值：返回帧率统计信息
 */
JNIEXPORT jstring JNICALL OPENGL_JNI(getStatistics)(JNIEnv *env, jclass cls) {
  (void) cls;
  (void) env;
  StatisticsInfo info;
  GetStatistics(info);
  uint64_t lag = info.lag;
  uint32_t recvFps = info.recvFps;
  uint32_t decFps = info.decFps;
  std::stringstream ss;
  ss << "LAG: " << lag << "ms \n";
  ss << "接受帧率: " << recvFps << "fps \n";
  ss << "解码帧率: " << decFps << "fps \n";
  return env->NewStringUTF(ss.str().c_str());
}

/**
 * @功能描述：设置JVM环境
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] obj：代表native方法的类的class对象实例
 */
JNIEXPORT jint JNICALL OPENGL_JNI(recvData)(JNIEnv *env,
                                            jclass cls,
                                            jbyte type,
                                            jbyteArray jData,
                                            int length) {
  (void) cls;
  (void) env;
  uint8_t *data = reinterpret_cast<uint8_t *>(env->GetByteArrayElements(jData, nullptr));
  int ret = RecvData(static_cast<VMIMsgType>(type), data, length);
  env->ReleaseByteArrayElements(jData, reinterpret_cast<jbyte *>(data), 0);
  return ret;
}


/**
 * @功能描述：发送模拟导航栏触控数据到agent端
 * @参数 [in] env：代表Java环境指针
 * @参数 [in] cls：代表对应JAVA类对象
 * @参数 [in] jData：代表触控数据
 * @参数 [in] length：代表发送触控信息的长度
 * @返回值：发送结果。JNI_TRUE代表发送触控信息成功，JNI_FALSE代表发送触控信息失败
 */
JNIEXPORT jboolean JNICALL OPENGL_JNI(sendKeyEvent)(JNIEnv *env,
                                                    jclass cls,
                                                    jbyteArray jData,
                                                    int length) {
  (void) cls;
  (void) env;
  if (env == nullptr) {
    ERR("Error: send navbar input event, can't get java environment");
    return
        JNI_FALSE;
  }

  uint8_t *data = reinterpret_cast<uint8_t *>(env->GetByteArrayElements(jData, nullptr));
  if (data == nullptr) {
    ERR("Error: send touch, src buffer is nullptr");
    return JNI_FALSE;
  }

  uint32_t ret = SendData(VMIMsgType::NAVBAR_INPUT, data, length);
  env->ReleaseByteArrayElements(jData, reinterpret_cast<jbyte *>(data), 0);
  return (ret == VMI_SUCCESS) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL OPENGL_JNI(sendTouchEvent)(JNIEnv *env,
                                                      jclass cls,
                                                      jbyteArray jData,
                                                      jint length) {
  (void) cls;
  (void) env;
  uint8_t *data = reinterpret_cast<uint8_t *>(env->GetByteArrayElements(jData, nullptr));
  uint32_t ret = SendData(VMIMsgType::TOUCH_INPUT, data, length);
  env->ReleaseByteArrayElements(jData, reinterpret_cast<jbyte *>(data), 0);
  if (ret == 0) {
    return JNI_TRUE;
  } else {
    ERR("sendTouchEvent failed,ret = %u", ret);
    return JNI_FALSE;
  }
}
}