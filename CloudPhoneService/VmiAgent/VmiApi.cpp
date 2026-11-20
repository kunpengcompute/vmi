/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2026. All rights reserved.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */

#include "VmiApi.h"

const char *GetVersion()
{
    return nullptr;
}

VmiErrCode InitVmiEngine(VmiConfigEngine * /*config*/)
{
    return VmiErrCode::OK;
}

VmiErrCode DeInitVmiEngine()
{
    return VmiErrCode::OK;
}

VmiModuleStatus GetStatus(VmiDataType /*module*/)
{
    return VmiModuleStatus::MODULE_NOT_INIT;
}

VmiErrCode StartModule(VmiDataType /*module*/, uint8_t* /*config*/, uint32_t /*size*/)
{
    return VmiErrCode::OK;
}

VmiErrCode StopModule(VmiDataType /*module*/)
{
    return VmiErrCode::OK;
}

VmiErrCode InjectData(VmiDataType /*module*/, VmiCmd /*cmd*/, uint8_t */*data*/, uint32_t /*size*/)
{
    return VmiErrCode::OK;
}

VmiErrCode SetParam(VmiDataType /*module*/, VmiCmd /*cmd*/, uint8_t */*param*/, uint32_t /*size*/)
{
    return VmiErrCode::OK;
}

VmiErrCode GetParam(VmiDataType /*module*/, VmiCmd /*cmd*/, uint8_t */*param*/, uint32_t /*size*/)
{
    return VmiErrCode::OK;
}