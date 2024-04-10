/*
 * 版权所有 (c) 华为技术有限公司 2022-2022
 * 功能描述：该文件封装了设置和读取属性的功能接口
 */
#include <string>

int32_t GetIntEncParam(const char *inputValue);
std::string GetStrEncParam(const char *inputValue);
void SetEncParam(const char *key, const char *value);
int32_t StrToInt(std::string inputValue);