/*
 * 版权所有 (c) 华为技术有限公司 2023-2023
 * 功能描述: 创建GpuEncTurbo模块（C接口）
 */


#include "GpuEncoderFactory.h"
#include <mutex>
#include <map>
#include <string>
#include <memory>
#include <vector>
#include <cstdint>
#include <sstream>
#include <cinttypes>
#include <dlfcn.h>
#include <sys/system_properties.h>
#include "GpuEncoderBase.h"
#include "logging.h"

using namespace Vmi::GpuEncoder;

namespace {
    int32_t GetPropertyWithDefault(const std::string &name, int32_t defaultVal)
    {
        char propBuf[PROP_VALUE_MAX] = {'\0'};
        int propLen = __system_property_get(name.c_str(), propBuf);
        if (propLen == 0) {
            return defaultVal;
        }
        auto propStr = std::string(propBuf);
        if (propStr.empty()) {
            return defaultVal;
        }

        std::stringstream stream;
        stream << propStr;
        intmax_t propVal;
        stream >> propVal;
        if (propVal < INT32_MIN || propVal > INT32_MAX) {
            ERR("Propery[%s]'s is out range of int32, Get property failed!", name.c_str());
            return defaultVal;
        }
        return propVal;
    }

    using CreateEncTurboFunc = void *(*)(uint32_t);

    class ModuleLib {
    public:
        ModuleLib(const char *path) : m_libPath(path) {}
        ModuleLib(std::string &path) : m_libPath(path) {} 
        ~ModuleLib()
        {
            if (m_lib != nullptr) {
                dlclose(m_lib);
                m_lib = nullptr;
                m_createFunc = nullptr;
            }
        }

        bool Open()
        {
            if (m_lib != nullptr && m_createFunc != nullptr) {
                return true;
            }
            m_lib = dlopen(m_libPath.c_str(), RTLD_LAZY);
            if (m_lib == nullptr) {
                ERR("Cannot open encturbo module lib, error: %s", dlerror());
                return false;
            }
            m_createFunc = reinterpret_cast<CreateEncTurboFunc>(dlsym(m_lib, "CreateGpuTurbo"));
            if (m_createFunc == nullptr) {
                ERR("GpuTurbo get craete enc turbo method failed, error: %s", dlerror());
                dlclose(m_lib);
                m_lib = nullptr;
                return false;
            }
            return true;
        }

        void *CreateModule(uint32_t type)
        {
            if (m_createFunc == nullptr) {
                return nullptr;
            }
            if (useCount < MAX_INSTANCE_PER_LIB) {
                ++useCount;
            } else {
                ERR("Create too many module for %d, max: %u", type, MAX_INSTANCE_PER_LIB);
                return nullptr;
            }
            return m_createFunc(type);
        }

        void Unref()
        {
            if (useCount > 0) {
                --useCount;
            } else {
                dlclose(m_lib);
                m_lib = nullptr;
                m_createFunc = nullptr;
            }
        }
    private:
        static constexpr uint32_t MAX_INSTANCE_PER_LIB = 32;
        uint32_t useCount = 0;
        std::string m_libPath {};
        void *m_lib {nullptr};
        CreateEncTurboFunc m_createFunc {nullptr};
    };

    enum LibIndex :uint32_t {
        GPU_INNO_LIB = 1,
        GPU_A_LIB,
        GPU_HANTRO_LIB,
        GPU_SOFT_LIB,
    };

    const std::map<uint32_t, const char *> MODULE_LIB_PATH_MAP = {
#ifdef __LP64__
#ifndef IS_VENDOR
        {GPU_INNO_LIB, "/system/lib64/libVmiEncTurboInno.so"},
        {GPU_A_LIB, "/system/lib64/libVmiEncTurboAcard.so"},
        {GPU_SOFT_LIB, "/system/lib64/libVmiEncTurboCpuSys.so"}
#else
        {GPU_HANTRO_LIB, "/vendor/lib64/libVmiEncTurboHantro.so"},
        {GPU_SOFT_LIB, "/vendor/lib64/libVmiEncTurboCpu.so"}
#endif
#else
#ifndef IS_VENDOR
        {GPU_INNO_LIB, "/system/lib/libVmiEncTurboInno.so"},
        {GPU_A_LIB, "/system/lib/libVmiEncTurboAcard.so"},
        {GPU_SOFT_LIB, "/system/lib/libVmiEncTurboCpuSys.so"}
#else
        {GPU_SOFT_LIB, "/vendor/lib/libVmiEncTurboCpu.so"}
#endif
#endif
    };

    const std::map<uint32_t, uint32_t> TYPE_TO_LIB_MAP = {
        {GPU_INNO_G1P, GPU_INNO_LIB},
        {GPU_A_W6600, GPU_A_LIB},
        {GPU_A_W6800, GPU_A_LIB},
        {GPU_HANTRO_SG100, GPU_HANTRO_LIB},
        {GPU_SOFT, GPU_SOFT_LIB},
    };

    const std::map<uint32_t, ModuleInfo> MODULE_INFO_MAP = {
        {GPU_INNO_G1P, {GPU_INNO_G1P, ((1u << CAP_VA_ENCODE_H264) | (1u << CAP_VA_ENCODE_HEVC))}},
        {GPU_A_W6600, {GPU_A_W6600, (1u << CAP_FORMAT_CONVERT)}},
        {GPU_A_W6800, {GPU_A_W6800, (1u << CAP_FORMAT_CONVERT)}},
        {GPU_HANTRO_SG100, {GPU_HANTRO_SG100, ((1u << CAP_VA_ENCODE_H264) | (1u << CAP_VA_ENCODE_HEVC))}},
        {GPU_SOFT, {GPU_SOFT, (1u << CAP_FORMAT_CONVERT)}},
    };

    std::map<uint32_t, ModuleLib> MODULE_LIB_MAP = {};

    std::map<void *, uint32_t> MODULE_INSTANCE_MAP = {};

    std::mutex g_lock;
}

void QueryModule(ModuleInfo **moduleList, uint32_t *listSize)
{
    if (moduleList == nullptr || listSize == nullptr) {
        return;
    }

    std::vector<ModuleInfo> moduleVector;
    int32_t gpuType = GetPropertyWithDefault("ro.vmi.hardware.gpu", 0);
    auto gpuIt = MODULE_INFO_MAP.find(gpuType);
    if (gpuIt != MODULE_INFO_MAP.end()) {
        moduleVector.emplace_back(gpuIt->second);
    }

    auto cpuIt = MODULE_INFO_MAP.find(GPU_SOFT);
    if (cpuIt != MODULE_INFO_MAP.end()) {
        moduleVector.emplace_back(cpuIt->second);
    }

    auto modules = new (std::nothrow) ModuleInfo[moduleVector.size()];
    if (modules == nullptr) {
        *listSize = 0;
        return;
    }

    for (uint32_t i = 0; i < moduleVector.size(); ++i) {
        modules[i] = moduleVector[i];
    }

    *moduleList = modules;
    *listSize = moduleVector.size();
}

void *CreateModule(uint32_t moduleId)
{
    std::unique_lock<std::mutex> lk(g_lock);
    uint32_t libId = moduleId;
    auto maskIt = TYPE_TO_LIB_MAP.find(moduleId);
    if (maskIt != TYPE_TO_LIB_MAP.end()) {
        libId = maskIt->second;
    }

    auto libIt = MODULE_LIB_MAP.find(libId);
    if (libIt == MODULE_LIB_MAP.end()) {
        auto pathIt = MODULE_LIB_PATH_MAP.find(libId);
        if (pathIt == MODULE_LIB_PATH_MAP.end()) {
            return nullptr;
        }
        auto tmp = MODULE_LIB_MAP.emplace(libId, ModuleLib(pathIt->second));
        libIt = tmp.first;
    }
    if (!libIt->second.Open()) {
        return nullptr;
    }

    void *instance = nullptr;
    instance = libIt->second.CreateModule(moduleId);

    if (instance == nullptr) {
        return nullptr;
    }
    MODULE_INSTANCE_MAP.emplace(instance, libId);
    return instance;
}

void DestroyModule(void *module)
{
    std::unique_lock<std::mutex> lk(g_lock);
    auto ctxIt = MODULE_INSTANCE_MAP.find(module);
    if (ctxIt == MODULE_INSTANCE_MAP.end()) {
        return;
    }

    delete reinterpret_cast<GpuEncoderBase *>(ctxIt->first);
    auto libIt = MODULE_LIB_MAP.find(ctxIt->second);
    if (libIt != MODULE_LIB_MAP.end()) {
        libIt->second.Unref();
    }
    MODULE_INSTANCE_MAP.erase(ctxIt);
}
