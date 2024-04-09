/*
 * 版权所有 (c) 华为技术有限公司 2017-2021
 * 功能说明: 指令流引擎定义的函数以及返回值
 */

#ifndef COMMON_DEFINE_H
#define COMMON_DEFINE_H
#include <string>

namespace Vmi {

// 版本号,用于匹配客户端和服务端的版本一致性
static const std::string ENGINE_VERSION = "Kunpeng BoostKit 24.0.RC1";
} // namespace Vmi

#if defined (__cplusplus)
extern "C" {
#endif

using RenderServerHandleHook = uint32_t (*)(std::pair<uint8_t *, uint32_t> data);

#if defined (__cplusplus)
}
#endif

#endif
