/*
 * 版权所有 (c) 华为技术有限公司 2023-2023
 * 功能描述: 创建GpuEncTurbo模块（C接口）
 */

#ifndef GPU_ENCODER_FACTORY_H
#define GPU_ENCODER_FACTORY_H

#include <cstdint>
#include "GpuEncoderBase.h"

#ifdef __cplusplus
extern "C" {
#endif
    void QueryModule(ModuleInfo **moduleList, uint32_t *listSize);

    void *CreateModule(uint32_t moduleId);

    void DestroyModule(void *module);
#ifdef __cplusplus
}
#endif
#endif
