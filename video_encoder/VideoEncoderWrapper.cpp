/*
 * 版权所有 (c) 华为技术有限公司 2021-2021
 * 功能说明: 视频编码器对外提供接口，包括编码器创建、初始化、启动、编码、停止、销毁等
 */

#define LOG_TAG "VideoEncoderWrapper"
#include "VideoEncoderWrapper.h"
#include <unordered_map>
#include <string>
#include <memory>
#include <dlfcn.h>
#include <cinttypes>
#include <sys/system_properties.h>
#include "VideoCodecApi.h"
#include "VideoEncoderLog.h"

namespace {
    uint32_t g_encHandle = INVALID_ENCODER_HANDLE;
    struct EncoderObject {
        uint32_t encType = 0;
        VideoEncoder *encoder = nullptr;
    };
    std::unordered_map<uint32_t, EncoderObject> g_vencHandleMap = {};

    std::string PROP_ENCODER_TYPE = "vmi.demo.video.encoder.type";

    const std::string SHARED_LIB_NAME = "libVideoCodec.so";
    const std::string REGISTER_MEDIA_LOG_CALLBACK = "RegisterMediaLogCallback";
    const std::string CREATE_VIDEO_ENCODER = "CreateVideoEncoder";
    const std::string DESTROY_VIDEO_ENCODER = "DestroyVideoEncoder";
    using RegisterMediaLogCallbackFuncPtr = bool (*)(const MediaLogCallbackFunc logCallback);
    using CreateVideoEncoderFuncPtr = EncoderRetCode (*)(uint32_t encType, VideoEncoder** encoder);
    using DestroyVideoEncoderFuncPtr = EncoderRetCode (*)(uint32_t encType, VideoEncoder* encoder);
    RegisterMediaLogCallbackFuncPtr g_registerMediaLogCallback = nullptr;
    CreateVideoEncoderFuncPtr g_createVideoEncoder = nullptr;
    DestroyVideoEncoderFuncPtr g_destroyVideoEncoder = nullptr;
    void *g_libHandle = nullptr;

    std::unordered_map<int, int> LOG_LEVEL_MAP = {
        { LOG_LEVEL_DEBUG, ANDROID_LOG_DEBUG },
        { LOG_LEVEL_INFO, ANDROID_LOG_INFO },
        { LOG_LEVEL_WARN, ANDROID_LOG_WARN },
        { LOG_LEVEL_ERROR, ANDROID_LOG_ERROR },
        { LOG_LEVEL_FATAL, ANDROID_LOG_FATAL },
    };
}

void UnloadVideoCodecSharedLib()
{
    if (g_libHandle != nullptr) {
        DBG("unload %s", SHARED_LIB_NAME.c_str());
        (void) dlclose(g_libHandle);
        g_libHandle = nullptr;
    }
    g_registerMediaLogCallback = nullptr;
    g_createVideoEncoder = nullptr;
    g_destroyVideoEncoder = nullptr;
}

bool LoadVideoCodecSharedLib()
{
    INFO("load %s", SHARED_LIB_NAME.c_str());
    g_libHandle = dlopen(SHARED_LIB_NAME.c_str(), RTLD_NOW);
    if (g_libHandle == nullptr) {
        const char *errStr = dlerror();
        ERR("load %s error:%s", SHARED_LIB_NAME.c_str(), (errStr != nullptr) ? errStr : "unknown");
        return false;
    }

    g_registerMediaLogCallback = reinterpret_cast<RegisterMediaLogCallbackFuncPtr>(
        dlsym(g_libHandle, REGISTER_MEDIA_LOG_CALLBACK.c_str()));
    if (g_registerMediaLogCallback == nullptr) {
        ERR("failed to load RegisterMediaLogCallback");
        UnloadVideoCodecSharedLib();
        return false;
    }

    g_createVideoEncoder = reinterpret_cast<CreateVideoEncoderFuncPtr>(
        dlsym(g_libHandle, CREATE_VIDEO_ENCODER.c_str()));
    if (g_createVideoEncoder == nullptr) {
        ERR("failed to load CreateVideoEncoder");
        UnloadVideoCodecSharedLib();
        return false;
    }

    g_destroyVideoEncoder = reinterpret_cast<DestroyVideoEncoderFuncPtr>(
        dlsym(g_libHandle, DESTROY_VIDEO_ENCODER.c_str()));
    if (g_destroyVideoEncoder == nullptr) {
        ERR("failed to load DestroyVideoEncoder");
        UnloadVideoCodecSharedLib();
        return false;
    }
    return true;
}

void MediaLogCallback(int level, const char *tag, const char *fmt)
{
    int vmiLevel = LOG_LEVEL_MAP[level];
    if (vmiLevel < VideoEncoderLog::GetInstance().GetLogLevel() || vmiLevel > ANDROID_LOG_SILENT || fmt == nullptr) {
        return;
    }
    (void) __android_log_write(vmiLevel, tag, fmt);
}

/**
 * @功能描述: 创建编码器
 * @参数 [out] encHandle: 编码器对象句柄
 * @返回值: VIDEO_ENCODER_SUCCESS 成功
 *          VIDEO_ENCODER_CREATE_FAIL 创建编码器失败
 */
EncoderRetCode VencCreateEncoder(uint32_t *encHandle)
{
    ++g_encHandle;
    if (g_encHandle == INVALID_ENCODER_HANDLE) {
        ERR("VencCreateEncoder failed: encoder handle exceeds max instances");
        return VIDEO_ENCODER_CREATE_FAIL;
    }
    char prop[PROP_VALUE_MAX] = {'\0'};
    int len = __system_property_get(PROP_ENCODER_TYPE.c_str(), prop);
    if (len == 0) {
        ERR("VencCreateEncoder failed: get system property[%s] failed", PROP_ENCODER_TYPE.c_str());
        return VIDEO_ENCODER_CREATE_FAIL;
    }
    char *end = nullptr;
    intmax_t result = strtoimax(prop, &end, 0);
    if (prop == end || (result < INT32_MIN || result > INT32_MAX)) {
        ERR("VencCreateEncoder failed: property[%s]'s value[%s] is not in range of int32",
            PROP_ENCODER_TYPE.c_str(), prop);
        return VIDEO_ENCODER_CREATE_FAIL;
    }
    uint32_t encType = static_cast<uint32_t>(result);
    VideoEncoder *encoder = nullptr;
    if (!LoadVideoCodecSharedLib()) {
        ERR("VencCreateEncoder failed: load video codec shared lib failed");
        return VIDEO_ENCODER_CREATE_FAIL;
    }
    if (!g_registerMediaLogCallback(MediaLogCallback)) {
        ERR("VencCreateEncoder failed: register media log callback failed");
        return VIDEO_ENCODER_CREATE_FAIL;
    }
    auto createRet = (*g_createVideoEncoder)(encType, &encoder);
    if (createRet != VIDEO_ENCODER_SUCCESS || encoder == nullptr) {
        ERR("VencCreateEncoder failed: create video encoder failed %u", createRet);
        return VIDEO_ENCODER_CREATE_FAIL;
    }
    EncoderObject encObj = { encType, encoder };
    auto ret = g_vencHandleMap.emplace(g_encHandle, encObj);
    if (!ret.second) {
        ERR("VencCreateEncoder failed: video encoder object insert into map failed");
        (void) (*g_destroyVideoEncoder)(encType, encoder);
        return VIDEO_ENCODER_CREATE_FAIL;
    }
    *encHandle = g_encHandle;
    return VIDEO_ENCODER_SUCCESS;
}

/**
 * @功能描述: 初始化编码器
 * @参数 [in] encHandle: 编码器对象句柄
 * @参数 [in] encParams: 编码参数结构体
 * @返回值: VIDEO_ENCODER_SUCCESS 成功
 *          VIDEO_ENCODER_INIT_FAIL 初始化编码器失败
 */
EncoderRetCode VencInitEncoder(uint32_t encHandle, const EncoderParams encParams)
{
    if (g_vencHandleMap.find(encHandle) == g_vencHandleMap.end()) {
        ERR("VencInitEncoder failed: encoder handle %#x does not exist.", encHandle);
        return VIDEO_ENCODER_INIT_FAIL;
    }
    EncoderRetCode ret = g_vencHandleMap[encHandle].encoder->InitEncoder(encParams);
    if (ret != VIDEO_ENCODER_SUCCESS) {
        ERR("VencInitEncoder failed: video encoder %#x init encoder error %#x", encHandle, ret);
        return VIDEO_ENCODER_INIT_FAIL;
    }
    return VIDEO_ENCODER_SUCCESS;
}

/**
 * @功能描述: 启动编码器
 * @参数 [in] encHandle: 编码器对象句柄
 * @返回值: VIDEO_ENCODER_SUCCESS 成功
 *          VIDEO_ENCODER_START_FAIL 启动编码器失败
 */
EncoderRetCode VencStartEncoder(uint32_t encHandle)
{
    if (g_vencHandleMap.find(encHandle) == g_vencHandleMap.end()) {
        ERR("VencStartEncoder failed: encoder handle %#x does not exist.", encHandle);
        return VIDEO_ENCODER_START_FAIL;
    }
    EncoderRetCode ret = g_vencHandleMap[encHandle].encoder->StartEncoder();
    if (ret != VIDEO_ENCODER_SUCCESS) {
        ERR("VencStartEncoder failed: video encoder %#x start encoder error %#x", encHandle, ret);
        return VIDEO_ENCODER_START_FAIL;
    }
    return VIDEO_ENCODER_SUCCESS;
}

/**
 * @功能描述: 编码器编码一帧数据
 * @参数 [in] encHandle: 编码器对象句柄
 * @参数 [in] inputData: 编码输入数据地址
 * @参数 [in] inputSize: 编码输入数据大小
 * @参数 [out] outputData: 编码输出数据地址
 * @参数 [out] outputSize: 编码输出数据大小
 * @返回值: VIDEO_ENCODER_SUCCESS 成功
 *          VIDEO_ENCODER_ENCODE_FAIL 编码一帧失败
 */
EncoderRetCode VencEncodeOneFrame(uint32_t encHandle, const uint8_t *inputData, uint32_t inputSize,
    uint8_t **outputData, uint32_t * outputSize)
{
    if (g_vencHandleMap.find(encHandle) == g_vencHandleMap.end()) {
        ERR("VencEncodeOneFrame failed: encoder handle %#x does not exist.", encHandle);
        return VIDEO_ENCODER_ENCODE_FAIL;
    }
    EncoderRetCode ret = g_vencHandleMap[encHandle].encoder->
        EncodeOneFrame(inputData, inputSize, outputData, outputSize);
    if (ret != VIDEO_ENCODER_SUCCESS) {
        ERR("VencEncodeOneFrame failed: video encoder %#x encode one frame error %#x", encHandle, ret);
        return VIDEO_ENCODER_ENCODE_FAIL;
    }
    return VIDEO_ENCODER_SUCCESS;
}

/**
 * @功能描述: 停止编码器
 * @参数 [in] encHandle: 编码器对象句柄
 * @返回值: VIDEO_ENCODER_SUCCESS 成功
 *          VIDEO_ENCODER_STOP_FAIL 停止编码器失败
 */
EncoderRetCode VencStopEncoder(uint32_t encHandle)
{
    if (g_vencHandleMap.find(encHandle) == g_vencHandleMap.end()) {
        ERR("VencStopEncoder failed: encoder handle %#x does not exist.", encHandle);
        return VIDEO_ENCODER_STOP_FAIL;
    }
    EncoderRetCode ret = g_vencHandleMap[encHandle].encoder->StopEncoder();
    if (ret != VIDEO_ENCODER_SUCCESS) {
        ERR("VencStopEncoder failed: video encoder %#x stop encoder error %#x", encHandle, ret);
        return VIDEO_ENCODER_STOP_FAIL;
    }
    return VIDEO_ENCODER_SUCCESS;
}

/**
 * @功能描述: 销毁编码器
 * @参数 [in] encHandle: 编码器对象句柄
 * @返回值: VIDEO_ENCODER_SUCCESS 成功
 *          VIDEO_ENCODER_DESTROY_FAIL 销毁编码器失败
 */
EncoderRetCode VencDestroyEncoder(uint32_t encHandle)
{
    if (g_vencHandleMap.find(encHandle) == g_vencHandleMap.end()) {
        ERR("VencDestroyEncoder failed: encoder handle %#x does not exist.", encHandle);
        return VIDEO_ENCODER_DESTROY_FAIL;
    }
    g_vencHandleMap[encHandle].encoder->DestroyEncoder();
    (void) (*g_destroyVideoEncoder)(g_vencHandleMap[encHandle].encType, g_vencHandleMap[encHandle].encoder);
    (void) g_vencHandleMap.erase(encHandle);
    UnloadVideoCodecSharedLib();
    return VIDEO_ENCODER_SUCCESS;
}
