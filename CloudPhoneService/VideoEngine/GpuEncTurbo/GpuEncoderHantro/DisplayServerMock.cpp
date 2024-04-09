/*
 * 版权所有 (c) 华为技术有限公司 2022-2022
 * 功能描述：瀚博GPU抓图库假实现
 */

#include "DisplayServer/DisplayServer.h"

uint64_t getPaddr(buffer_handle_t)
{
    return 0;
}

uint32_t getWidth(buffer_handle_t)
{
    return 0;
}

uint32_t getHeight(buffer_handle_t)
{
    return 0;
}
int32_t getFormat(buffer_handle_t)
{
    return 0;
}

uint32_t getStride(buffer_handle_t)
{
    return 0;
}

uint32_t getStrideV(buffer_handle_t)
{
    return 0;
}

uint64_t getBufferID(buffer_handle_t)
{
    return 0;
}

DisplayServer::DisplayServer(DisplayServer::Mode)
{
    (void)m_Impl;
}

DisplayServer::~DisplayServer() {}

void DisplayServer::Run() {}

bool DisplayServer::Stop()
{
    return true;
}

bool DisplayServer::Init(PresentBufferCallback, int)
{
    return true;
}

bool DisplayServer::RefreshDisplay(RefreshReq)
{
    return true;
}

bool DisplayServer::GetDisplayInfo(unsigned int *, unsigned int *, unsigned int *, unsigned int *)
{
    return true;
}

void DisplayServer::AcquireBuffer(android::GraphicBuffer *, int *, int *)
{
    return;
}

void DisplayServer::ReleaseBuffer(android::GraphicBuffer *)
{
    return;
}