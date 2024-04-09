/*
 * 版权所有 (c) 华为技术有限公司 2017-2024
 * 功能描述：SystemPropty中包含了VMIEngine可设置和读取的属性
 */
#define LOG_TAG "SystemProperty"
#include "SystemProperty.h"
#include <sstream>
#include <cinttypes>
#include "logging.h"

namespace Vmi {
std::string GetPropertyWithDefault(const std::string& name, const std::string& defaultVal)
{
    char propVal[PROP_VALUE_MAX] = {'\0'};
    int propLen = __system_property_get(name.c_str(), propVal);
    if (propLen > 0) {
        return std::string(propVal);
    }
    return defaultVal;
}

int32_t GetPropertyWithDefault(const std::string& name, int32_t defaultVal)
{
    std::string propStr = GetPropertyWithDefault(name, "");
    if (propStr.empty()) {
        return defaultVal;
    }
    for (char c : propStr) {
        if (c < '0' || '9' < c) {
            ERR("Failed to get property:[%s], value:[%s] contains non-numeric characters!",
                name.c_str(), propStr.c_str());
            return defaultVal;
        }
    }

    std::stringstream stream;
    stream << propStr;
    intmax_t propVal;
    stream >> propVal;
    if (stream.fail()) {
        ERR("Failed to get property:[%s], stringstream can't change value:[%s] to int!", name.c_str(), propStr.c_str());
        return defaultVal;
    }
    if (propVal < INT32_MIN || propVal > INT32_MAX) {
        ERR("Failed to get property:[%s], value:[%" PRIdMAX "] is not in range of int32!", name.c_str(), propVal);
        return defaultVal;
    }
    return propVal;
}

void SetProperty(const std::string& name, const std::string& val)
{
    int ret = __system_property_set(name.c_str(), val.c_str());
    if (ret < 0) {
        ERR("Failed to set property[%s], call __system_property_set failed, value:[%s], error:[%d]",
            name.c_str(), val.c_str(), ret);
    }
}
}
