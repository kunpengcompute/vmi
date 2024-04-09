/*
 * 版权所有 (c) 华为技术有限公司 2022-2022
 * 功能描述：该文件封装了设置和读取属性的功能接口
 */
#include "Property.h"
#include <sstream>
#include <sys/system_properties.h>

int32_t GetIntEncParam(const char *inputValue)
{
    char prop[PROP_VALUE_MAX] = {'\0'};
    __system_property_get(inputValue, prop);
    std::string propValue = std::string(prop);
    
    std::stringstream strStream;
    strStream << propValue;
    int32_t result = -1;
    strStream >> result;

    return result;
}

std::string GetStrEncParam(const char *inputValue)
{
    char prop[PROP_VALUE_MAX] = {'\0'};
    __system_property_get(inputValue, prop);
    std::string propValue = std::string(prop);

    return propValue;
}

int32_t StrToInt(std::string inputValue)
{
    std::stringstream strStream;
    strStream << inputValue;
    int32_t result = -1;
    strStream >> result;

    return result;
}

void SetEncParam(const char *key, const char *value)
{
    __system_property_set(key, value);
}

