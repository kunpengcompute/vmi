/*
 * 版权所有 (c) 华为技术有限公司 2017-2024
 * 功能描述：SystemPropty中包含了VMIEngine可设置和读取的属性
 */
#ifndef VMI_SYSTEMPROPERTY_H
#define VMI_SYSTEMPROPERTY_H

#include <string>
#include <sys/system_properties.h>

namespace Vmi {
std::string GetPropertyWithDefault(const std::string& name, const std::string& defaultVal);
int32_t GetPropertyWithDefault(const std::string& name, int32_t defaultVal);
void SetProperty(const std::string& name, const std::string& val);
}

#endif // VMI_SYSTEMPROPERTY_H
