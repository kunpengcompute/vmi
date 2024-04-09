/*
 * 版权所有 (c) 华为技术有限公司 2023-2023
 * 功能描述：芯动GPU编码va接口
 */

#include "VaEncInno.h"

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <cassert>
#include <cerrno>
#include <cmath>
#include <fcntl.h>
#include <dlfcn.h>
#include <pthread.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <va/va_android.h>
#include <va/va_drmcommon.h>
#include "logging.h"

namespace Vmi {
bool VaEncInno::Start()
{
    if (CheckVaapi()) {
        m_initFlag = InitVaapi();
        return m_initFlag;
    } else {
        ERR("Check Vaapi fail");
    }
    return false;
}

void VaEncInno::Stop()
{
    DeinitVaapi();
    m_initFlag = false;
}

bool VaEncInno::CheckVaapi()
{
    static uint32_t gFakeDisplay = 0xdeada01d;
    m_vaDisplay = vaGetDisplay(&gFakeDisplay);
    if (m_vaDisplay == nullptr) {
        ERR("error: failed to initialize display");
        return false;
    }
    static char vaDriverName[] = "awm";
    VAStatus vaStatus = vaSetDriverName(m_vaDisplay, &vaDriverName[0]);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("VA error: %s failed, status: %d", "vaSetDriverName", vaStatus);
        return false;
    }
    int32_t majorVersion;
    int32_t minorVersion;
    vaStatus = vaInitialize(m_vaDisplay, &majorVersion, &minorVersion);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("VA error: %s failed, status: %d", "vaInitialize", vaStatus);
        return false;
    }
    CheckVaProfile();
    GetConfigAttributes();
    ConfigAttribRTFormat();
    ConfigAttribRateControl();
    ConfigAttribEncPackedHeaders();
    ConfigAttribEncInterlaced();
    ConfigAttribEncMaxRefFrames();
    ConfigAttribEncMaxSlices();
    ConfigAttribEncSliceStructure();
    ConfigAttribEncMacroblockInfo();
    INFO("Vaenc Inno Check Vaapi pass");
    return true;
}

void VaEncInno::CheckVaProfile()
{
    DBG("Using EntryPoint - %d ", m_vaEntrypoint);
    switch (m_vaProfile) {
        case VAProfileH264ConstrainedBaseline:
            DBG("Use profile VAProfileH264ConstrainedBaseline");
            break;
        case VAProfileH264Main:
            DBG("Use profile VAProfileH264Main");
            break;
        case VAProfileH264High:
            DBG("Use profile VAProfileH264High");
            break;
        case VAProfileHEVCMain:
            DBG("Use profile VAProfileHEVCMain");
            break;
        default:
            DBG("unknow profile. Set to Constrained Baseline");
            m_vaProfile = VAProfileH264ConstrainedBaseline;
            break;
    }
}

bool VaEncInno::GetConfigAttributes()
{
    // find out the format for the render target, and rate control mode
    for (int32_t i = 0; i < VAConfigAttribTypeMax; i++) {
        m_vaConfigAttribSupport[i].type = static_cast<VAConfigAttribType>(i);
    }

    VAStatus vaStatus = vaGetConfigAttributes(m_vaDisplay, m_vaProfile, m_vaEntrypoint,
        &m_vaConfigAttribSupport[0], VAConfigAttribTypeMax);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("VA error: %s failed, status: %d", "vaGetConfigAttributes", vaStatus);
        return false;
    }
    return true;
}

void VaEncInno::ConfigAttribRTFormat()
{
    if ((m_vaConfigAttribSupport[VAConfigAttribRTFormat].value & VA_RT_FORMAT_YUV420) == 0) {
        ERR("Not find desired YUV420 RT format");
        Stop();
    } else {
        m_vaConfigAttribArray[m_vaConfigAttribNum].type = VAConfigAttribRTFormat;
        m_vaConfigAttribArray[m_vaConfigAttribNum].value = VA_RT_FORMAT_YUV420;
        m_vaConfigAttribNum++;
    }
}

void VaEncInno::ConfigAttribRateControl()
{
    if (m_vaConfigAttribSupport[VAConfigAttribRateControl].value != VA_ATTRIB_NOT_SUPPORTED) {
        int32_t tmp = m_vaConfigAttribSupport[VAConfigAttribRateControl].value;
        DBG("Support VAConfigAttribRateControl (0x%x):", tmp);
        if (!(tmp & VA_RC_CBR) || !(tmp & VA_RC_VBR)) {
            ERR("Not find desired ratecontrol mode");
            Stop();
        }
        m_vaConfigAttribArray[m_vaConfigAttribNum].type = VAConfigAttribRateControl;
        m_vaConfigAttribArray[m_vaConfigAttribNum].value = m_useVbr ? VA_RC_VBR : VA_RC_CBR;
        m_vaConfigAttribNum++;
    }
}


void VaEncInno::ConfigAttribEncPackedHeaders()
{
    if (m_vaConfigAttribSupport[VAConfigAttribEncPackedHeaders].value != VA_ATTRIB_NOT_SUPPORTED) {
        int32_t tmp = m_vaConfigAttribSupport[VAConfigAttribEncPackedHeaders].value;
        DBG("Support VAConfigAttribEncPackedHeaders (0x%x):", tmp);
        m_vaConfigAttribArray[m_vaConfigAttribNum].type = VAConfigAttribEncPackedHeaders;
        m_vaConfigAttribArray[m_vaConfigAttribNum].value = VA_ENC_PACKED_HEADER_NONE;
        m_vaConfigAttribNum++;
    }
}

void VaEncInno::ConfigAttribEncInterlaced()
{
    if (m_vaConfigAttribSupport[VAConfigAttribEncInterlaced].value != VA_ATTRIB_NOT_SUPPORTED) {
        int32_t tmp = m_vaConfigAttribSupport[VAConfigAttribEncInterlaced].value;
        DBG("Support VAConfigAttribEncInterlaced (0x%x):", tmp);
        m_vaConfigAttribArray[m_vaConfigAttribNum].type = VAConfigAttribEncInterlaced;
        m_vaConfigAttribArray[m_vaConfigAttribNum].value = VA_ENC_INTERLACED_NONE;
        m_vaConfigAttribNum++;
    }
}

void VaEncInno::ConfigAttribEncMaxRefFrames()
{
    if (m_vaConfigAttribSupport[VAConfigAttribEncMaxRefFrames].value != VA_ATTRIB_NOT_SUPPORTED) {
        uint32_t h264MaxRef = m_vaConfigAttribSupport[VAConfigAttribEncMaxRefFrames].value;
        DBG("Support %d RefPicList0 and %d RefPicList1", h264MaxRef & 0xffff,
            (h264MaxRef >> VAConfigAttribEncMacroblockInfo) & 0xffff);
    }
}

void VaEncInno::ConfigAttribEncMaxSlices()
{
    if (m_vaConfigAttribSupport[VAConfigAttribEncMaxSlices].value != VA_ATTRIB_NOT_SUPPORTED) {
        DBG("Support VAConfigAttribEncMaxSlices %d", m_vaConfigAttribSupport[VAConfigAttribEncMaxSlices].value);
    }
}

void VaEncInno::ConfigAttribEncSliceStructure()
{
    if (m_vaConfigAttribSupport[VAConfigAttribEncSliceStructure].value != VA_ATTRIB_NOT_SUPPORTED) {
        int32_t tmp = m_vaConfigAttribSupport[VAConfigAttribEncSliceStructure].value;
        DBG("Support VAConfigAttribEncSliceStructure (0x%x):", tmp);
    }
}

void VaEncInno::ConfigAttribEncMacroblockInfo()
{
    if (m_vaConfigAttribSupport[VAConfigAttribEncMacroblockInfo].value != VA_ATTRIB_NOT_SUPPORTED) {
        DBG("Support VAConfigAttribEncMacroblockInfo");
    }
}

bool VaEncInno::InitVaapi()
{
    VAStatus vaStatus = vaCreateConfig(
        m_vaDisplay, m_vaProfile, m_vaEntrypoint, m_vaConfigAttribArray, m_vaConfigAttribNum, &m_vaConfigId);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("VA error: %s failed, status: %d", "vaCreateConfig", vaStatus);
        return false;
    }
    if (!InitEncResource()) {
        ERR("InitEncResource failed");
        return false;
    }
    if (m_vaProfile == VAProfileHEVCMain) {
        SetVaapiBufferTypeHEVC();
    } else {
        SetVaapiBufferTypeH264();
    }
    if (!SetVAEncMiscParameter()) {
        ERR("SetVAEncMiscParameter failed");
        return false;
    }

    for (uint32_t i = 0; i < YUV_SURFACE_NUM; ++i) {
        m_freeYuvSlot.push(i);
    }
    for (uint32_t i = 0; i < STREAM_SURFACE_NUM; ++i) {
        m_freeStreamSlot.push(i);
    }

    INFO("Vaenc Inno Init Vaapi pass");
    return true;
}

bool VaEncInno::InitEncResource()
{
    VAStatus vaStatus = vaCreateSurfaces(m_vaDisplay, VA_RT_FORMAT_YUV420, m_frameWidthAligned,
                                         m_frameHeightAligned, m_srcSurface, YUV_SURFACE_NUM, nullptr, 0);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("VA error: %s failed, status: %d", "vaCreateSurfaces", vaStatus);
        return false;
    }
    // Create a context for this encode pipe
    vaStatus = vaCreateContext(m_vaDisplay, m_vaConfigId, m_frameWidthAligned, m_frameHeightAligned,
                               VA_PROGRESSIVE, m_srcSurface, YUV_SURFACE_NUM, &m_vaContextId);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("VA error: %s failed, status: %d", "vaCreateContext", vaStatus);

        return false;
    }
    if (!CreateCodeBuffer()) {
        ERR("CreateCodeBuffer failed");
        return false;
    }
    // VAEncPackedHeaderParameterBufferType
    vaStatus = vaCreateBuffer(m_vaDisplay, m_vaContextId, VAEncPackedHeaderParameterBufferType,
                              sizeof(VAEncPackedHeaderParameterBuffer), 1,
                              nullptr, &m_vaPackedHeaderParamBufferId);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("VA error: %s failed, status: %d", "vaCreateBuffer", vaStatus);
        return false;
    }
    vaStatus = vaMapBuffer(m_vaDisplay, m_vaPackedHeaderParamBufferId,
                           reinterpret_cast<void **>(&m_vaPackedHeaderParam));
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("VA error: %s failed, status: %d", "vaMapBuffer", vaStatus);
        return false;
    }
    std::fill_n(reinterpret_cast<uint8_t *>(m_vaPackedHeaderParam), sizeof(VAEncPackedHeaderParameterBuffer), 0);
    m_vaPackedHeaderParam->type = VAEncPackedHeaderRawData;

    // VAEncPackedHeaderDataBufferType
    vaStatus = vaCreateBuffer(m_vaDisplay, m_vaContextId, VAEncPackedHeaderDataBufferType,
                              BITSTREAM_ALLOCATE_STEPPING, 1, nullptr, &m_vaPackedheaderDataBufferId);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("VA error: %s failed, status: %d", "vaCreateBuffer", vaStatus);
        return false;
    }
    vaStatus = vaMapBuffer(m_vaDisplay, m_vaPackedheaderDataBufferId, reinterpret_cast<void **>(&m_vaPackedData));
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("VA error: %s failed, status: %d", "vaMapBuffer", vaStatus);
        return false;
    }
    std::fill_n(m_vaPackedData, BITSTREAM_ALLOCATE_STEPPING, 0);
    return true;
}

bool VaEncInno::CreateCodeBuffer()
{
    // codebuffer用来存放转码后的码流，转码后获取的码流大小一定小于转码yuv的大小
    // 所以将codeBuffer的大小设置为yuv的大小，yuv大小约为帧宽高乘积的1.5倍，400和256用作大概的获取1.5倍
    int32_t codedBufferSize = ((m_frameWidthAligned * m_frameHeightAligned) * 400) / 256;
    for (uint32_t i = 0; i < STREAM_SURFACE_NUM; i++) {
        /* create coded buffer once for all
         * other VA buffers which won't be used again after vaRenderPicture.
         * so APP can always vaCreateBuffer for every frame
         * but coded buffer need to be mapped and accessed after vaRenderPicture/vaEndPicture
         * so VA won't maintain the coded buffer
         */
        VAStatus vaStatus = vaCreateBuffer(m_vaDisplay, m_vaContextId, VAEncCodedBufferType,
                                           codedBufferSize, 1, nullptr, &m_vaStreamBufferIdArray[i]);
        if (vaStatus != VA_STATUS_SUCCESS) {
            ERR("VA error: %s failed, status: %d", "vaCreateBuffer", vaStatus);
            return false;
        }
    }
    return true;
}

bool VaEncInno::SetVaapiBufferTypeHEVC()
{
    VAStatus vaStatus = vaCreateBuffer(m_vaDisplay, m_vaContextId, VAEncSequenceParameterBufferType,
                                       sizeof(VAEncSequenceParameterBufferHEVC), 1, nullptr, &m_vaSpsBufferId);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("VA error: %s failed, status: %d", "vaCreateBuffer", vaStatus);
        return false;
    }
    vaStatus = vaMapBuffer(m_vaDisplay, m_vaSpsBufferId, reinterpret_cast<void **>(&m_vaHevcSps));
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("VA error: %s failed, status: %d", "vaMapBuffer", vaStatus);
        return false;
    }
    std::fill_n(reinterpret_cast<uint8_t *>(m_vaHevcSps), sizeof(VAEncSequenceParameterBufferHEVC), 0);

    vaStatus = vaCreateBuffer(m_vaDisplay, m_vaContextId, VAEncPictureParameterBufferType,
                              sizeof(VAEncPictureParameterBufferHEVC), 1, nullptr, &m_vaPpsBufferId);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("VA error: %s failed, status: %d", "vaCreateBuffer", vaStatus);
        return false;
    }
    vaStatus = vaMapBuffer(m_vaDisplay, m_vaPpsBufferId, reinterpret_cast<void **>(&m_vaHevcPps));
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("VA error: %s failed, status: %d", "vaMapBuffer", vaStatus);
        return false;
    }
    std::fill_n(reinterpret_cast<uint8_t *>(m_vaHevcPps), sizeof(VAEncPictureParameterBufferHEVC), 0);
    std::fill_n(reinterpret_cast<uint8_t *>(m_vaHevcPps->reference_frames),
        HEVC_REFERENCE_FRAME_SIZE * sizeof(VAPictureHEVC), 0xff);

    vaStatus = vaCreateBuffer(m_vaDisplay, m_vaContextId, VAEncSliceParameterBufferType,
                              sizeof(VAEncSliceParameterBufferHEVC), 1, nullptr, &m_vaSliceParamBufferId);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("VA error: %s failed, status: %d", "vaCreateBuffer", vaStatus);
        return false;
    }
    vaStatus = vaMapBuffer(m_vaDisplay, m_vaSliceParamBufferId, reinterpret_cast<void **>(&m_vaHevcSliceParam));
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("VA error: %s failed, status: %d", "vaMapBuffer", vaStatus);
        return false;
    }
    std::fill_n(reinterpret_cast<uint8_t *>(m_vaHevcSliceParam), sizeof(VAEncSliceParameterBufferHEVC), 0);
    return true;
}

bool VaEncInno::SetVaapiBufferTypeH264()
{
    VAStatus vaStatus = vaCreateBuffer(m_vaDisplay, m_vaContextId, VAEncSequenceParameterBufferType,
                                       sizeof(VAEncSequenceParameterBufferH264), 1, nullptr, &m_vaSpsBufferId);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("VA error: %s failed, status: %d", "vaCreateBuffer", vaStatus);
        return false;
    }
    vaStatus = vaMapBuffer(m_vaDisplay, m_vaSpsBufferId, reinterpret_cast<void **>(&m_vaH264Sps));
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("VA error: %s failed, status: %d", "vaMapBuffer", vaStatus);
        return false;
    }
    std::fill_n(reinterpret_cast<uint8_t *>(m_vaH264Sps), sizeof(VAEncSequenceParameterBufferH264), 0);

    vaStatus = vaCreateBuffer(m_vaDisplay, m_vaContextId, VAEncPictureParameterBufferType,
                              sizeof(VAEncPictureParameterBufferH264), 1, nullptr, &m_vaPpsBufferId);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("VA error: %s failed, status: %d", "vaCreateBuffer", vaStatus);
        return false;
    }
    vaStatus = vaMapBuffer(m_vaDisplay, m_vaPpsBufferId, reinterpret_cast<void **>(&m_vaH264Pps));
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("VA error: %s failed, status: %d", "vaMapBuffer", vaStatus);
        return false;
    }
    std::fill_n(reinterpret_cast<uint8_t *>(m_vaH264Pps), sizeof(VAEncPictureParameterBufferH264), 0);
    // invalid all
    std::fill_n(reinterpret_cast<uint8_t *>(m_vaH264Pps->ReferenceFrames),
        H264_REFERENCE_FRAME_SIZE * sizeof(VAPictureH264), 0xff);

    vaStatus = vaCreateBuffer(m_vaDisplay, m_vaContextId, VAEncSliceParameterBufferType,
                              sizeof(VAEncSliceParameterBufferH264), 1, nullptr, &m_vaSliceParamBufferId);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("VA error: %s failed, status: %d", "vaCreateBuffer", vaStatus);
        return false;
    }
    vaStatus = vaMapBuffer(m_vaDisplay, m_vaSliceParamBufferId, reinterpret_cast<void **>(&m_vaH264SliceParam));
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("VA error: %s failed, status: %d", "vaMapBuffer", vaStatus);
        return false;
    }
    std::fill_n(reinterpret_cast<uint8_t *>(m_vaH264SliceParam), sizeof(VAEncSliceParameterBufferH264), 0);
    return true;
}

bool VaEncInno::SetVAEncMiscParameter()
{
    VAEncMiscParameterBuffer *miscParamBuffer = nullptr;
    int32_t bufferSize = sizeof(VAEncMiscParameterBuffer) + sizeof(VAEncMiscParameterRateControl);
    VAStatus vaStatus = vaCreateBuffer(
        m_vaDisplay, m_vaContextId, VAEncMiscParameterBufferType, bufferSize, 1, nullptr, &m_vaMiscRateControlBufferId);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("VA error: %s failed, status: %d", "vaCreateBuffer", vaStatus);
        return false;
    }
    vaStatus = vaMapBuffer(m_vaDisplay, m_vaMiscRateControlBufferId, reinterpret_cast<void **>(&miscParamBuffer));
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("VA error: %s failed, status: %d", "vaMapBuffer", vaStatus);
        return false;
    }
    std::fill_n(reinterpret_cast<uint8_t *>(miscParamBuffer), bufferSize, 0);
    miscParamBuffer->type = VAEncMiscParameterTypeRateControl;
    m_vaMiscRateControl = reinterpret_cast<VAEncMiscParameterRateControl *>(miscParamBuffer->data);

    bufferSize = sizeof(VAEncMiscParameterBuffer) + sizeof(VAEncMiscParameterFrameRate);
    vaStatus = vaCreateBuffer(
        m_vaDisplay, m_vaContextId, VAEncMiscParameterBufferType, bufferSize, 1, nullptr, &m_vaMiscFrameRateBufferId);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("VA error: %s failed, status: %d", "vaCreateBuffer", vaStatus);
        return false;
    }
    vaStatus = vaMapBuffer(m_vaDisplay, m_vaMiscFrameRateBufferId, reinterpret_cast<void **>(&miscParamBuffer));
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("VA error: %s failed, status: %d", "vaMapBuffer", vaStatus);
        return false;
    }
    std::fill_n(reinterpret_cast<uint8_t *>(miscParamBuffer), bufferSize, 0);
    miscParamBuffer->type = VAEncMiscParameterTypeFrameRate;
    m_vaMiscFrameRate = reinterpret_cast<VAEncMiscParameterFrameRate *>(miscParamBuffer->data);

    bufferSize = sizeof(VAEncMiscParameterBuffer) + sizeof(VAEncMiscParameterRIR);
    vaStatus = vaCreateBuffer(
        m_vaDisplay, m_vaContextId, VAEncMiscParameterBufferType, bufferSize, 1, nullptr, &m_vaMiscRirBufferId);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("VA error: %s failed, status: %d", "vaCreateBuffer", vaStatus);
        return false;
    }
    vaStatus = vaMapBuffer(m_vaDisplay, m_vaMiscRirBufferId, reinterpret_cast<void **>(&miscParamBuffer));
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("VA error: %s failed, status: %d", "vaMapBuffer", vaStatus);
        return false;
    }
    std::fill_n(reinterpret_cast<uint8_t *>(miscParamBuffer), bufferSize, 0);
    miscParamBuffer->type = VAEncMiscParameterTypeRIR;
    m_vaMiscRir = reinterpret_cast<VAEncMiscParameterRIR *>(miscParamBuffer->data);
    return true;
}

void VaEncInno::DeinitVaapi()
{
    m_vaPackedHeaderParam = nullptr;
    m_vaPackedData = nullptr;
    m_vaH264Sps = nullptr;
    m_vaH264Pps = nullptr;
    m_vaH264SliceParam = nullptr;
    m_vaHevcSps = nullptr;
    m_vaHevcPps = nullptr;
    m_vaHevcSliceParam = nullptr;
    m_vaMiscRateControl = nullptr;
    m_vaMiscFrameRate = nullptr;
    m_vaMiscRir = nullptr;
    vaUnmapBuffer(m_vaDisplay, m_vaPackedHeaderParamBufferId);
    vaDestroyBuffer(m_vaDisplay, m_vaPackedHeaderParamBufferId);
    vaUnmapBuffer(m_vaDisplay, m_vaPackedheaderDataBufferId);
    vaDestroyBuffer(m_vaDisplay, m_vaPackedheaderDataBufferId);
    vaUnmapBuffer(m_vaDisplay, m_vaSpsBufferId);
    vaDestroyBuffer(m_vaDisplay, m_vaSpsBufferId);
    vaUnmapBuffer(m_vaDisplay, m_vaPpsBufferId);
    vaDestroyBuffer(m_vaDisplay, m_vaPpsBufferId);
    vaUnmapBuffer(m_vaDisplay, m_vaSliceParamBufferId);
    vaDestroyBuffer(m_vaDisplay, m_vaSliceParamBufferId);
    vaUnmapBuffer(m_vaDisplay, m_vaMiscRateControlBufferId);
    vaDestroyBuffer(m_vaDisplay, m_vaMiscRateControlBufferId);
    vaUnmapBuffer(m_vaDisplay, m_vaMiscFrameRateBufferId);
    vaDestroyBuffer(m_vaDisplay, m_vaMiscFrameRateBufferId);
    vaUnmapBuffer(m_vaDisplay, m_vaMiscRirBufferId);
    vaDestroyBuffer(m_vaDisplay, m_vaMiscRirBufferId);

    m_usedYuvSlot.clear();
    m_freeYuvSlot = std::queue<uint32_t>();
    m_usedStreamSlot.clear();
    m_freeStreamSlot = std::queue<uint32_t>();

    vaDestroySurfaces(m_vaDisplay, &m_srcSurface[0], YUV_SURFACE_NUM);

    for (uint32_t i = 0; i < YUV_SURFACE_NUM; i++) {
        vaDestroyBuffer(m_vaDisplay, m_vaStreamBufferIdArray[i]);
    }

    vaDestroyContext(m_vaDisplay, m_vaContextId);
    vaDestroyConfig(m_vaDisplay, m_vaConfigId);
    vaTerminate(m_vaDisplay);
    m_vaDisplay = nullptr;

    INFO("Vaenc Inno Deinited");
}

void VaEncInno::SetResolution(uint32_t width, uint32_t height, uint32_t widthAligned, uint32_t heightAligned)
{
    m_frameWidth = width;
    m_frameHeight = height;
    m_frameWidthAligned = widthAligned;
    m_frameHeightAligned = heightAligned;
}

void VaEncInno::SetForceIFrame(int32_t offset)
{
    m_forceIdrOffset = offset;
}

void VaEncInno::SetProfile(VAProfile profile)
{
    m_vaProfile = profile;
}

void VaEncInno::SetIntraRefresh(uint32_t intraRefresh, uint32_t intraRefreshArg)
{
    m_intraRefresh = intraRefresh;
    m_intraRefreshArg = intraRefreshArg;
}
void VaEncInno::SetFrameRate(int32_t frameRate)
{
    m_frameRate = frameRate;
}

void VaEncInno::SetGopParam(int32_t gopSize, GopPreset gopPreset)
{
    m_gopSize = gopSize;
    m_gopPreset = gopPreset;
}

void VaEncInno::SetIntraPeriod(int32_t intraPeriod)
{
    m_intraPeriod = intraPeriod;
}

void VaEncInno::SetQp(int32_t minQp, int32_t maxQp, int32_t qp)
{
    m_minQp = minQp;
    m_maxQp = maxQp;
    m_qp = qp;
}

void VaEncInno::SetUseVbr(bool isUse)
{
    m_useVbr = isUse;
}

void VaEncInno::SetRenderSequence()
{
    m_sequenceFlag = true;
}

void VaEncInno::SetEntropyMode(EntropyMode entropyMode)
{
    m_entropyMode = entropyMode;
}

void VaEncInno::SetBitrate(int32_t frameBitrate)
{
    std::unique_lock<std::mutex> lock(m_dynamicParamMutex);
    m_frameBitrate = frameBitrate;
    m_rateControlFlag = true;
}

int32_t VaEncInno::RenderPackedHeader()
{
    std::unique_lock<std::mutex> lock(m_dynamicParamMutex);
    VAStatus vaStatus = vaRenderPicture(m_vaDisplay, m_vaContextId, &m_vaPackedHeaderParamBufferId, 1);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("VA error: %s failed, status: %d", "vaRenderPicture", vaStatus);
        return -1;
    }
    vaStatus = vaRenderPicture(m_vaDisplay, m_vaContextId, &m_vaPackedheaderDataBufferId, 1);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("VA error: %s failed, status: %d", "vaRenderPicture", vaStatus);
        return -1;
    }
    return 0;
}

int32_t VaEncInno::RenderSequenceHEVC()
{
    m_vaHevcSps->general_level_idc = HEVC_DEFAULT_LEVEL;
    m_vaHevcSps->bits_per_second = m_frameBitrate;
    m_vaHevcSps->intra_idr_period = m_gopSize;
    m_vaHevcSps->intra_period = m_intraPeriod;
    switch (m_gopPreset) {
        case GOP_PRESET_IP:
            m_vaHevcSps->ip_period = 1; // 1: P帧在I帧后一位
            break;
        case GOP_PRESET_IBPBP:
            m_vaHevcSps->ip_period = 2; // 2: P帧在I帧后两位
            break;
        case GOP_PRESET_IBBBP:
            m_vaHevcSps->ip_period = 4; // 4: P帧在I帧后四位
            break;
        default:
            break;
    }
    m_vaHevcSps->seq_fields.bits.chroma_format_idc = 1;  // 1:420   2:422   3:444
    // m_vaHevcSps->seq_fields.bits.frame_mbs_only_flag默认设置为0
    // frame_mbs_only_flag=0表示编码视频序列的编码图像可能是编码场或编码帧。
    // frame_mbs_only_flag=1表示编码视频序列的每个编码图像都是一个仅包含帧宏块的编码帧
    if (m_frameWidth != m_frameWidthAligned || m_frameHeight != m_frameHeightAligned) {
        m_vaHevcSps->va_reserved[0] = 0; // 0: VA HEVC SPS预留宽度左对齐位
        m_vaHevcSps->va_reserved[1] = (m_frameWidthAligned - m_frameWidth) >> 1; // 1: VA HEVC SPS预留宽度右对齐位
        m_vaHevcSps->va_reserved[2] = 0; // 2: VA HEVC SPS预留高度上对齐位
        m_vaHevcSps->va_reserved[3] = (m_frameHeightAligned - m_frameHeight) >> 1; // 3: VA HEVC SPS预留高度下对齐位
    }

    m_vaHevcSps->va_reserved[0] = m_cropLeftOffset; // 0: VA HEVC SPS预留宽度左对齐位
    m_vaHevcSps->va_reserved[1] = // 1: VA HEVC SPS预留宽度右对齐位
        m_cropRightOffset > m_vaHevcSps->va_reserved[1] ? m_cropRightOffset : m_vaHevcSps->va_reserved[1];
    m_vaHevcSps->va_reserved[2] = m_cropTopOffset; // 2: VA HEVC SPS预留高度上对齐位
    m_vaHevcSps->va_reserved[3] = // 3: VA HEVC SPS预留高度下对齐位
        m_cropBottomOffset > m_vaHevcSps->va_reserved[3] ? m_cropBottomOffset : m_vaHevcSps->va_reserved[3];

    VAStatus vaStatus = vaRenderPicture(m_vaDisplay, m_vaContextId, &m_vaSpsBufferId, 1);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("VA error: %s failed, status: %d", "vaRenderPicture", vaStatus);
        return -1;
    }
    return 0;
}

int32_t VaEncInno::RenderSequenceH264()
{
    m_vaH264Sps->level_idc = H264_DEFAULT_LEVEL;
    m_vaH264Sps->picture_width_in_mbs = m_frameWidthAligned / H264_MB_WIDTH;
    m_vaH264Sps->picture_height_in_mbs = m_frameHeightAligned / H264_MB_WIDTH;

    m_vaH264Sps->bits_per_second = m_frameBitrate;

    m_vaH264Sps->intra_period = m_intraPeriod;
    m_vaH264Sps->intra_idr_period = m_gopSize;
    switch (m_gopPreset) {
        case GOP_PRESET_IP:
            m_vaH264Sps->ip_period = 1; // 1: P帧在I帧后一位
            break;
        case GOP_PRESET_IBPBP:
            m_vaH264Sps->ip_period = 2; // 2: P帧在I帧后两位
            break;
        case GOP_PRESET_IBBBP:
            m_vaH264Sps->ip_period = 4; // 4: P帧在I帧后四位
            break;
        default:
            break;
    }

    m_vaH264Sps->seq_fields.bits.chroma_format_idc = 1;  // 1:420   2:422   3:444
    // frame_mbs_only_flag =0表示编码视频序列的编码图像可能是编码场或编码帧。
    // frame_mbs_only_flag =1表示编码视频序列的每个编码图像都是一个仅包含帧宏块的编码帧
    m_vaH264Sps->seq_fields.bits.frame_mbs_only_flag = 1;

    m_vaH264Sps->frame_cropping_flag = 1;
    if (m_frameWidth != m_frameWidthAligned || m_frameHeight != m_frameHeightAligned) {
        m_vaH264Sps->frame_crop_left_offset = 0;
        m_vaH264Sps->frame_crop_right_offset = (m_frameWidthAligned - m_frameWidth) >> 1;
        m_vaH264Sps->frame_crop_top_offset = 0;
        m_vaH264Sps->frame_crop_bottom_offset = (m_frameHeightAligned - m_frameHeight) >> 1;
    }

    m_vaH264Sps->frame_crop_left_offset = m_cropLeftOffset;
    m_vaH264Sps->frame_crop_right_offset = m_cropRightOffset > m_vaH264Sps->frame_crop_right_offset
                                                ? m_cropRightOffset
                                                : m_vaH264Sps->frame_crop_right_offset;
    m_vaH264Sps->frame_crop_top_offset = m_cropTopOffset;
    m_vaH264Sps->frame_crop_bottom_offset = m_cropBottomOffset > m_vaH264Sps->frame_crop_bottom_offset
                                                ? m_cropBottomOffset
                                                : m_vaH264Sps->frame_crop_bottom_offset;

    VAStatus vaStatus = vaRenderPicture(m_vaDisplay, m_vaContextId, &m_vaSpsBufferId, 1);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("VA error: %s failed, status: %d", "vaRenderPicture", vaStatus);
        return -1;
    }
    return 0;
}

int32_t VaEncInno::RenderSequence()
{
    int32_t ret = 0;
    if (m_vaProfile == VAProfileHEVCMain) {
        ret = RenderSequenceHEVC();
    } else {
        ret = RenderSequenceH264();
    }
    if (ret != 0) {
        return ret;
    }

    // VAEncMiscParameterFrameRate中，帧率由numerator/denominator计算得到，这两个数据以两个uint16组合后写入一个uint32中
    m_vaMiscFrameRate->framerate = 1 << 16; // 16: 将denominator写入前两个Byte
    m_vaMiscFrameRate->framerate |= static_cast<uint32_t>(m_frameRate) & 0xffff; // ffff: 将numerator写入后两个Byte
    VAStatus vaStatus = vaRenderPicture(m_vaDisplay, m_vaContextId, &m_vaMiscFrameRateBufferId, 1);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("VA error: %s failed, status: %d", "vaRenderPicture", vaStatus);
        return -1;
    }
    return 0;
}

int32_t VaEncInno::RenderRateControl()
{
    std::unique_lock<std::mutex> lock(m_dynamicParamMutex);
    m_vaMiscRateControl->bits_per_second = m_frameBitrate;
    m_vaMiscRateControl->min_qp = m_minQp;
    m_vaMiscRateControl->max_qp = m_maxQp;
    m_vaMiscRateControl->initial_qp = m_qp;
    VAStatus vaStatus = vaRenderPicture(m_vaDisplay, m_vaContextId, &m_vaMiscRateControlBufferId, 1);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("VA error: %s failed, status: %d", "vaRenderPicture", vaStatus);
        return -1;
    }
    return 0;
}

int32_t VaEncInno::RenderRir()
{
    std::unique_lock<std::mutex> lock(m_dynamicParamMutex);
    m_vaMiscRir->rir_flags.bits.enable_rir_column = m_intraRefresh & 0x00000001;
    m_vaMiscRir->rir_flags.bits.enable_rir_row = (m_intraRefresh & 0x00000002) >> 1;
    m_vaMiscRir->intra_insert_size = m_intraRefreshArg;
    VAStatus vaStatus = vaRenderPicture(m_vaDisplay, m_vaContextId, &m_vaMiscRirBufferId, 1);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("VA error: %s failed, status: %d", "vaRenderPicture", vaStatus);
        return -1;
    }
    return 0;
}

int32_t VaEncInno::RenderPicture(uint32_t srcIndex, uint32_t dstIndex)
{
    if (m_vaProfile == VAProfileHEVCMain) {
        m_vaHevcPps->decoded_curr_pic.picture_id = m_srcSurface[srcIndex];
        m_vaHevcPps->decoded_curr_pic.flags = 0;
        m_vaHevcPps->pic_fields.bits.idr_pic_flag = (m_currentFrameType == FRAME_IDR);
        m_vaHevcPps->coded_buf = m_vaStreamBufferIdArray[dstIndex];
        m_vaHevcPps->pic_init_qp = m_maxQp;
    } else {
        m_vaH264Pps->CurrPic.picture_id = m_srcSurface[srcIndex];
        m_vaH264Pps->CurrPic.flags = 0;
        m_vaH264Pps->pic_fields.bits.idr_pic_flag = (m_currentFrameType == FRAME_IDR);
        m_vaH264Pps->pic_fields.bits.entropy_coding_mode_flag = m_entropyMode;
        m_vaH264Pps->coded_buf = m_vaStreamBufferIdArray[dstIndex];
        m_vaH264Pps->pic_init_qp = m_maxQp;
    }
    VAStatus vaStatus = vaRenderPicture(m_vaDisplay, m_vaContextId, &m_vaPpsBufferId, 1);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("VA error: %s failed, status: %d", "vaRenderPicture", vaStatus);
        return -1;
    }
    return 0;
}

int32_t VaEncInno::RenderSlice()
{
    if (m_vaProfile == VAProfileHEVCMain) {
        if (m_forceIdrOffset == 0) {
            m_vaHevcSliceParam->slice_type = HEVC_FRAME_FORCE_I;
            m_forceIdrOffset = -1;
        } else {
            m_vaHevcSliceParam->slice_type = (m_currentFrameType == FRAME_IDR) ? FRAME_I : m_currentFrameType;
        }
        m_vaHevcSliceParam->slice_qp_delta = 0;
    } else {
        if (m_forceIdrOffset == 0) {
            m_vaH264SliceParam->slice_type = H264_FRAME_FORCE_I;
            m_forceIdrOffset = -1;
        } else {
            m_vaH264SliceParam->slice_type = (m_currentFrameType == FRAME_IDR) ? FRAME_I : m_currentFrameType;
        }
        m_vaH264SliceParam->slice_qp_delta = 0;
    }
    if (m_forceIdrOffset > 0) {
        --m_forceIdrOffset;
    }
    VAStatus vaStatus = vaRenderPicture(m_vaDisplay, m_vaContextId, &m_vaSliceParamBufferId, 1);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("VA error: %s failed, status: %d", "vaRenderPicture", vaStatus);
        return -1;
    }

    return 0;
}

int32_t VaEncInno::EncodeFrame(int32_t yuvIndex, int32_t streamIndex)
{
    if (!m_initFlag) {
        ERR("VA encode inno is not inited");
        return -1;
    }
    if (m_currentFrameNum != 0) {
        m_currentFrameType = (m_currentFrameNum % static_cast<uint64_t>(m_gopSize) == 0) ? FRAME_I : FRAME_P;
    }
    VAStatus vaStatus = vaBeginPicture(m_vaDisplay, m_vaContextId, m_srcSurface[yuvIndex]);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("VA error: %s failed, status: %d", "vaBeginPicture", vaStatus);
        return -1;
    }
    if (m_sequenceFlag) {
        RenderSequence();
        m_sequenceFlag = false;
    }
    if (m_rateControlFlag) {
        RenderRateControl();
        m_rateControlFlag = false;
    }
    if (m_rirFlag) {
        RenderRir();
        m_rirFlag = false;
    }
    RenderPicture(yuvIndex, streamIndex);
    RenderSlice();
    vaStatus = vaEndPicture(m_vaDisplay, m_vaContextId);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("VA error: %s failed, status: %d", "vaEndPicture", vaStatus);
        return -1;
    }
    m_currentFrameNum++;
    return 0;
}

bool VaEncInno::GetYuvBuffer(uint32_t &slot)
{
    if (m_freeYuvSlot.empty()) {
        return false;
    }
    slot = m_freeYuvSlot.front();
    m_usedYuvSlot.insert(slot);
    m_freeYuvSlot.pop();
    return true;
}

bool VaEncInno::ReleaseYuvBuffer(uint32_t slot)
{
    auto &&it = m_usedYuvSlot.find(slot);
    if (it == m_usedYuvSlot.end()) {
        return false;
    }
    m_usedYuvSlot.erase(it);
    m_freeYuvSlot.push(slot);
    return true;
}

bool VaEncInno::GetStreamBuffer(uint32_t &slot)
{
    if (m_freeStreamSlot.empty()) {
        return false;
    }
    slot = m_freeStreamSlot.front();
    m_usedStreamSlot.insert(slot);
    m_freeStreamSlot.pop();
    return true;
}

bool VaEncInno::ReleaseStreamBuffer(uint32_t slot)
{
    auto &&it = m_usedStreamSlot.find(slot);
    if (it == m_usedStreamSlot.end()) {
        return false;
    }
    m_usedStreamSlot.erase(it);
    m_freeStreamSlot.push(slot);
    return true;
}

int32_t VaEncInno::GetYuvBufferFd(uint32_t slot)
{
    if (slot >= YUV_SURFACE_NUM) {
        ERR("invalid slot :%d", slot);
        return -1;
    }
    VADRMPRIMESurfaceDescriptor desc;
    VAStatus vaStatus = vaExportSurfaceHandle(m_vaDisplay, m_srcSurface[slot],
        VA_SURFACE_ATTRIB_MEM_TYPE_ANDROID_ION, VA_EXPORT_SURFACE_SEPARATE_LAYERS, &desc);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("vaExportSurfaceHandle error");
        return -1;
    }
    return desc.objects[0].fd;
}

uint32_t VaEncInno::RetriveStreamData(uint32_t slot, uint8_t *mem, uint32_t memLen)
{
    uint32_t streamSize = 0;
    VACodedBufferSegment *bufList = nullptr;
    VAStatus vaStatus = vaMapBuffer(m_vaDisplay, m_vaStreamBufferIdArray[slot], reinterpret_cast<void **>(&bufList));
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("Map stream buffer fail");
    }
    while (bufList != nullptr) {
        if (bufList->buf == nullptr) {
            WARN("Stream data is null, skip data copy");
            streamSize = 0;
            break;
        }
        streamSize += bufList->size;
        if (streamSize > memLen) {
            WARN("Stream size exceed given buffer size [%u/%u], skip data copy", streamSize, memLen);
            streamSize = 0;
            break;
        }
        std::copy_n(static_cast<uint8_t *>(bufList->buf), bufList->size, mem);
        mem += bufList->size;
        bufList = (VACodedBufferSegment *)bufList->next;
    }
    vaUnmapBuffer(m_vaDisplay, m_vaStreamBufferIdArray[slot]);
    return streamSize;
}

bool VaEncInno::MapStreamBuffer(uint32_t slot, VACodedBufferSegment **bufList)
{
    VAStatus vaStatus = vaMapBuffer(m_vaDisplay, m_vaStreamBufferIdArray[slot], reinterpret_cast<void **>(bufList));
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("Map stream buffer fail");
        return false;
    }
    return true;
}

bool VaEncInno::UnmapStreamBuffer(uint32_t slot)
{
    VAStatus vaStatus = vaUnmapBuffer(m_vaDisplay, m_vaStreamBufferIdArray[slot]);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("Map stream buffer fail");
        return false;
    }
    return true;
}
}