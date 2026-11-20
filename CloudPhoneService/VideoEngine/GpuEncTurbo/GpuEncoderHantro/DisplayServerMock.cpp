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

/*
 * 功能描述：瀚博GPU抓图库假实现
 */

#include "DisplayServer/DisplayServerWrap.h"

va_buffer_handle_wrap getHandleWrap(pVaGraphicBuffer)
{
    return nullptr;
}

uint64_t getPaddrWrap(va_buffer_handle_wrap)
{
    return 0;
}

uint32_t getWidthWrap(va_buffer_handle_wrap)
{
    return 0;
}

uint32_t getHeightWrap(va_buffer_handle_wrap)
{
    return 0;
}
int32_t getFormatWrap(va_buffer_handle_wrap)
{
    return 0;
}

uint32_t getStrideWrap(va_buffer_handle_wrap)
{
    return 0;
}

uint32_t getStrideVWrap(va_buffer_handle_wrap)
{
    return 0;
}

uint64_t getBufferIDWrap(va_buffer_handle_wrap)
{
    return 0;
}

void* getAndroidGraphicBufferWrap(pVaGraphicBuffer)
{
    return nullptr;
}

pDisplayServer CreateDisplayServer(DisplayServerMode)
{
    return nullptr;
}

void DestroyDisplayServer(pDisplayServer) {}

void DisplayServerRun(pDisplayServer) {}

bool DisplayServerStop(pDisplayServer)
{
    return true;
}

bool DisplayServerInit(pDisplayServer, PresentBufferCallbackWrap, int)
{
    return true;
}

bool DisplayServerInitGetPresentLayerCallback(pDisplayServer, GetPresentLayerCallback gplCallback)
{
    return true;
}

bool DisplayServerRefreshDisplay(pDisplayServer, RefreshReqWrapper)
{
    return true;
}

bool DisplayServerGetDisplayInfo(pDisplayServer, unsigned int *, unsigned int *, unsigned int *, unsigned int *)
{
    return true;
}

void DisplayServerAcquireBuffer(pDisplayServer, pVaGraphicBuffer, int*, int*)
{
    return;
}

void DisplayServerReleaseBuffer(pDisplayServer, pVaGraphicBuffer)
{
    return;
}

bool DisplayServerSetWmSize(pDisplayServer, unsigned int, unsigned int)
{
    return true;
}
