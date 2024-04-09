/*
 * 版权所有 (c) 华为技术有限公司 2023-2023
 * 功能描述: AMD显卡-VA接口层
 */

#include "VaEncoderAmd.h"
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <algorithm>
#include <memory>
#include <fcntl.h>
#include <string>
#include <unordered_map>
#include <va_drmcommon.h>
#include <va_drm.h>
#include "va.h"
#include "logging.h"

using namespace Vmi::GpuEncoder;
namespace {
constexpr int MAX_ENTRYPOINTS_NUM = 16;
constexpr int RGB_BYTE_ONE_PIXEL = 4;       // RGBA数据，一个像素占用4个byte

// 编码用
constexpr int MAX_FRAME_NUM_MASK = (1 << 12) - 1;
constexpr int LOG2_MAX_FRAME_NUM = 4;
constexpr int LOG2_MAX_PIC_ORDER_CNT_LSB = 4;
constexpr int PIC_INIT_QP_MINUS26 = 2;       // SUCH AS : 28-26
constexpr int MAX_VA_BUFFERS = 20;
constexpr int QP_DEFAULT_VALUE = 26;         // 默认的一个QP值，一般为26
constexpr int SIZE_ALIGN = 16;               // 16 byte对齐
constexpr int CPB_BR_NAL_FACTOR_BASE = 1200;
constexpr int CPB_BR_NAL_FACTOR_HIGH = 1500;
constexpr int CPB_REMOVAL_DELAY = 2;
constexpr int CURRENT_DPB_REMOVAL_DELTA = 2;
constexpr int MAX_DPB_FRAMES = 16;           // 16 max dpb
constexpr float MAX_BITRATE_MULTIPLE = 1.2;  // 定义最大的bitrate阈值，1.2倍，值可以增大的
constexpr int LEVEL_IDC_DEFAULT_VALUE = 40;

/**
 * 功能说明：编码level建模 264
 */
H264Level g_h264Levels[] = {
    // Name          MaxMBPS                   MaxBR              MinCR
    //  | level_idc     |       MaxFS            |    MaxCPB        | MaxMvsPer2Mb
    //  |     | cs3f    |         |  MaxDpbMbs   |       |  MaxVmvR |   |
    { "1",   10, 0,     1485,     99,    396,     64,    175,   64, 2,  0 },
    { "1b",  11, 1,     1485,     99,    396,    128,    350,   64, 2,  0 },
    { "1b",   9, 0,     1485,     99,    396,    128,    350,   64, 2,  0 },
    { "1.1", 11, 0,     3000,    396,    900,    192,    500,  128, 2,  0 },
    { "1.2", 12, 0,     6000,    396,   2376,    384,   1000,  128, 2,  0 },
    { "1.3", 13, 0,    11880,    396,   2376,    768,   2000,  128, 2,  0 },
    { "2",   20, 0,    11880,    396,   2376,   2000,   2000,  128, 2,  0 },
    { "2.1", 21, 0,    19800,    792,   4752,   4000,   4000,  256, 2,  0 },
    { "2.2", 22, 0,    20250,   1620,   8100,   4000,   4000,  256, 2,  0 },
    { "3",   30, 0,    40500,   1620,   8100,  10000,  10000,  256, 2, 32 },
    { "3.1", 31, 0,   108000,   3600,  18000,  14000,  14000,  512, 4, 16 },
    { "3.2", 32, 0,   216000,   5120,  20480,  20000,  20000,  512, 4, 16 },
    { "4",   40, 0,   245760,   8192,  32768,  20000,  25000,  512, 4, 16 },
    { "4.1", 41, 0,   245760,   8192,  32768,  50000,  62500,  512, 2, 16 },
    { "4.2", 42, 0,   522240,   8704,  34816,  50000,  62500,  512, 2, 16 },
    { "5",   50, 0,   589824,  22080, 110400, 135000, 135000,  512, 2, 16 },
    { "5.1", 51, 0,   983040,  36864, 184320, 240000, 240000,  512, 2, 16 },
    { "5.2", 52, 0,  2073600,  36864, 184320, 240000, 240000,  512, 2, 16 },
    { "6",   60, 0,  4177920, 139264, 696320, 240000, 240000, 8192, 2, 16 },
    { "6.1", 61, 0,  8355840, 139264, 696320, 480000, 480000, 8192, 2, 16 },
    { "6.2", 62, 0, 16711680, 139264, 696320, 800000, 800000, 8192, 2, 16 },
};
}

EncTurboCode VaEncoderAmd::ContextInit(EncoderConfig &config)
{
    m_frameIndex = 0;
    m_vaContext.outputFormat = config.capability;
    m_vaContext.drmDeviceNode = config.deviceNode;
    m_vaContext.fourcc = VA_FOURCC_NV12; // surface fourcc only support VA_FOURCC_NV12
    m_vaContext.downloadYuvFourcc = VA_FOURCC_I420; // support VA_FOURCC_I420/VA_FOURCC_NV12/VA_FOURCC_YV12
    m_vaContext.vpp.width = config.inSize.width;
    m_vaContext.vpp.height = config.inSize.height;
    m_vaContext.vpp.stride = config.inSize.widthAligned;
    m_vaContext.vpp.pixelFormat = VA_FOURCC_BGRX;
    m_vaContext.enc.pictureWidth = config.outSize.width;
    m_vaContext.enc.pictureHeight = config.outSize.height;

    m_vaContext.enc.ipPeriod = 1; // 0(I frames only)/1(I and P frames)，默认为I/P帧
    m_vaContext.enc.rateControlMethod = VA_RC_CBR; // Rate control支持VBR和CBR两种，默认CBR固定

    m_vaContext.enc.rtFormat = VA_RT_FORMAT_YUV420; // 支持的编码格式为YUV420
    m_vaContext.enc.selectEntrypoint = VAEntrypointEncSlice; // low-power mode:VAEntrypointEncSliceLP 编码类型
    m_vaContext.enc.contextID = VA_INVALID_ID;
    m_vaContext.enc.configID = VA_INVALID_ID;
    m_vaContext.enc.frameSize = ClcYuvSize(m_vaContext.enc.pictureWidth * m_vaContext.enc.pictureHeight);

    m_vaContext.enc.picInitQpMinus26 = PIC_INIT_QP_MINUS26;
    m_vaContext.enc.numRefFrames = REF_FRAMES_NUM;
    m_vaContext.enc.seqParamBufID = VA_INVALID_ID;
    m_vaContext.enc.picParamBufID = VA_INVALID_ID;
    m_vaContext.enc.sliceParamBufID = VA_INVALID_ID;
    m_vaContext.enc.miscRateControlBufID = VA_INVALID_ID;
    m_vaContext.enc.miscHrdBufID = VA_INVALID_ID;

    m_vaContext.enc.pictureWidthInMbs = (m_vaContext.enc.pictureWidth + SIZE_ALIGN - 1) / SIZE_ALIGN;
    m_vaContext.enc.pictureHeightInMbs = (m_vaContext.enc.pictureHeight + SIZE_ALIGN - 1) / SIZE_ALIGN;
    m_vaContext.enc.pictureWidthAlignedH265 = (m_vaContext.enc.pictureWidth + LCU_SIZE - 1) / LCU_SIZE;
    m_vaContext.enc.pictureHeightAlignedH265 = (m_vaContext.enc.pictureHeight + LCU_SIZE - 1) / LCU_SIZE;

    if (m_vaContext.outputFormat == CAP_VA_ENCODE_H264) {
        SetProfileIdcH264();
        EncSeqParamInitH264();      // 初始化SPS参数
        EncPicParamInitH264();      // 初始化PPS参数
    } else {
        m_vaContext.enc.profile = VAProfileHEVCMain;
        EncSeqParamInitH265();      // 初始化SPS参数
        EncPicParamInitH265();      // 初始化PPS参数
    }
    return ENC_TURBO_SUCCESS;
}

EncTurboCode VaEncoderAmd::VaDpyInit()
{
    INFO("Create encoder...");
    EncTurboCode encStatus = DisplayDrmOpen();
    if (encStatus != ENC_TURBO_SUCCESS) {
        ERR("EncodeCreate display drm open failed: %#x", encStatus);
        return encStatus;
    }
    INFO("Open device success.");
    int majorVer;
    int minorVer;
    VAStatus vaStatus = vaInitialize(m_vaContext.vaDpy, &majorVer, &minorVer);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("Encoder vaInitialize failed: %#x", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    INFO("Init mesa driver success.");
    return ENC_TURBO_SUCCESS;
}

EncTurboCode VaEncoderAmd::DisplayDrmOpen()
{
    const char* deviceName = nullptr;
    for (unsigned int i = 0; i < sizeof(AMD_DEVICE_TYPE_MAP) / sizeof(DevicePair); i++) {
        if (AMD_DEVICE_TYPE_MAP[i].deviceNode == m_vaContext.drmDeviceNode) {
            deviceName = AMD_DEVICE_TYPE_MAP[i].devicePath;
            break;
        }
    }
    if (deviceName == nullptr) {
        return ENC_TURBO_ERROR_INVALID_PARAMETER;
    }
    m_vaContext.drmFd = open(deviceName, O_RDWR);
    if (m_vaContext.drmFd < 0) {
        ERR("Failed to open the given device(%s) , code %d !", deviceName, m_vaContext.drmFd);
        return ENC_TURBO_ERROR_OPEN_DISPLAY_DEVICE;
    }
    m_vaContext.vaDpy = vaGetDisplayDRM(m_vaContext.drmFd);  // VA的API，获取vaDpy
    if (m_vaContext.vaDpy == nullptr) {
        ERR("Failed to a DRM display for the given device");
        (void)close(m_vaContext.drmFd);  // 关闭已经打开的显卡节点
        m_vaContext.drmFd = -1;   // 设定为无效
        return ENC_TURBO_ERROR_OPEN_DISPLAY_DEVICE;
    }
    return ENC_TURBO_SUCCESS;
}

EncTurboCode VaEncoderAmd::SetRateControl(uint32_t rateControl)
{
    switch (rateControl) {
        case ENC_RC_CBR:
            m_vaContext.enc.rateControlMethod = VA_RC_CBR;
            break;
        case ENC_RC_VBR:
            m_vaContext.enc.rateControlMethod = VA_RC_VBR;
            break;
        default:
            INFO("Unsupport rateControl mode!");
            return ENC_TURBO_ERROR_INVALID_PARAMETER;
    }
    return ENC_TURBO_SUCCESS;
}

EncTurboCode VaEncoderAmd::SetProfileIdc(uint32_t profileIdc)
{
    switch (profileIdc) {
        case ENC_PROFILE_IDC_BASELINE:
        case ENC_PROFILE_IDC_MAIN:
        case ENC_PROFILE_IDC_HIGH:
        case ENC_PROFILE_IDC_HEVC_MAIN:
            m_vaContext.enc.profileIdc = profileIdc;
            break;
        default:
            ERR("Set profileIdc failed, input profileIdc illegal: %u", profileIdc);
            return ENC_TURBO_ERROR_INVALID_PARAMETER;
    }
    return ENC_TURBO_SUCCESS;
}

void VaEncoderAmd::SetBitRate(uint32_t bitRate)
{
    m_vaContext.enc.bitRate = bitRate;
    m_vaContext.enc.maxBitRate = static_cast<uint32_t>(m_vaContext.enc.bitRate * MAX_BITRATE_MULTIPLE);
    return;
}

void VaEncoderAmd::SetFrameRate(uint32_t frameRate)
{
    m_vaContext.enc.frameRate = frameRate;
    return;
}

void VaEncoderAmd::SetGopSize(uint32_t gopSize)
{
    m_vaContext.enc.gopSize = gopSize;
    m_vaContext.enc.intraPeriod = m_vaContext.enc.gopSize;
    return;
}

void VaEncoderAmd::SetKeyFrame(uint32_t n)
{
    m_needKeyFrame = (n != 0);
    return;
}

EncTurboCode VaEncoderAmd::EncodeOpen()
{
    if (m_vaContext.vaDpy == nullptr) {
        ERR("Encoder open failed, dpy is null.");
        return ENC_TURBO_ERROR_CONTEXT_HANDLE;
    }
    // 查询GPU卡支持的entrypoints,并检查用户用户设置是否在支持列表内
    EncTurboCode encStatus = EncVaQueryConfigEntrypoints();
    if (encStatus != ENC_TURBO_SUCCESS) {
        ERR("Config entrypoints display drom open failed: %#x", encStatus);
        return encStatus;
    }

    encStatus = EncVaCreateContext();
    if (encStatus != ENC_TURBO_SUCCESS) {
        ERR("Create encoder context and config failed: %#x", encStatus);
        return encStatus;
    }

    // 只有需要H264/265编码的时候才去创建ref参考帧
    if (m_vaContext.outputFormat == CAP_VA_ENCODE_H264 ||
        m_vaContext.outputFormat == CAP_VA_ENCODE_HEVC) {
        encStatus = EncVaCreateSurfaces();
        if (encStatus != ENC_TURBO_SUCCESS) {
            ERR("Create ref surfaces failed : %#x", encStatus);
            EncodeDestroyContext();
            EncodeDestroyConfig();
            return encStatus;
        }
    }
    if (m_vaContext.outputFormat == CAP_VA_ENCODE_HEVC) {
        m_vaBitstream.FillBitstreamForSPS(
            m_vaContext.enc.seqParamH265, m_vaContext.enc.pictureWidth, m_vaContext.enc.pictureHeight);
    }

    INFO("Open encode device success.");
    return ENC_TURBO_SUCCESS;
}

EncTurboCode VaEncoderAmd::EncVaCreateSurfaces()
{
    // 创建ref参考帧
    constexpr int surfAttribsNum = 1;
    VASurfaceAttrib surfAttribs[surfAttribsNum];
    surfAttribs[0].flags = VA_SURFACE_ATTRIB_SETTABLE;
    surfAttribs[0].type = VASurfaceAttribPixelFormat;
    surfAttribs[0].value.type = VAGenericValueTypeInteger;
    surfAttribs[0].value.value.i = m_vaContext.fourcc;
    VAStatus vaStatus = vaCreateSurfaces(m_vaContext.vaDpy, m_vaContext.enc.rtFormat, m_vaContext.enc.pictureWidth,
                                         m_vaContext.enc.pictureHeight, m_vaContext.enc.refSurface,
                                         REF_SURFACE_NUM, surfAttribs, surfAttribsNum);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("EncVaCreateSurfaces vaCreateSurfaces failed : %#x", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    // 图像长宽初始化时已限制最大值为4090
    int codedbufSize = ClcYuvSize(m_vaContext.enc.pictureWidth * m_vaContext.enc.pictureHeight);
    if (m_stream.first != nullptr) {
        delete[] m_stream.first;
        m_stream.first = nullptr;
        m_stream.second = 0;
    }
    m_stream.first = new (std::nothrow) uint8_t[codedbufSize];
    if (m_stream.first == nullptr) {
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    m_stream.second = codedbufSize;
    return ENC_TURBO_SUCCESS;
}

EncTurboCode VaEncoderAmd::EncVaQueryConfigEntrypoints()
{
    int numEntrypoints = vaMaxNumEntrypoints(m_vaContext.vaDpy);
    if (numEntrypoints <= 0 || numEntrypoints > MAX_ENTRYPOINTS_NUM) {
        ERR("Invalid num of Entrypoints!");
        return ENC_TURBO_ERROR_CONFIG;
    }
    VAEntrypoint entrypoints[numEntrypoints];
    VAStatus vaStatus = vaQueryConfigEntrypoints(m_vaContext.vaDpy,
                                                 m_vaContext.enc.profile, entrypoints, &numEntrypoints);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("vaQueryConfigEntrypoints failed : %#x", vaStatus);
        return ENC_TURBO_ERROR_CONFIG;
    }
    for (int i = 0; i < numEntrypoints; i++) {
        if (entrypoints[i] == m_vaContext.enc.selectEntrypoint) {
            return ENC_TURBO_SUCCESS;
        }
    }
    ERR("EncVaQueryConfigEntrypoints failed : %#x", vaStatus);
    return ENC_TURBO_ERROR_CONFIG;
}

EncTurboCode VaEncoderAmd::EncVaCreateContext()
{
    constexpr int numAttrib = 2;
    VAConfigAttrib attribs[numAttrib];
    attribs[0].type = VAConfigAttribRTFormat;
    attribs[1].type = VAConfigAttribRateControl;
    VAStatus vaStatus = vaGetConfigAttributes(m_vaContext.vaDpy, m_vaContext.enc.profile,
                                              m_vaContext.enc.selectEntrypoint, attribs, numAttrib);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("EncVaCreateContext vaGetConfigAttributes failed : %#x", vaStatus);
        return ENC_TURBO_ERROR_CONFIG;
    }
    // 检查用户设置的format是否在支持列表内(VA_RT_FORMAT_YUV420)
    if ((attribs[0].value & m_vaContext.enc.rtFormat) == 0)  {
        ERR("rt format not support !");
        return ENC_TURBO_ERROR_CONFIG;
    }
    // 检查用户设置的rate control是否支持列表内(VBR/CBR)
    if ((attribs[1].value & m_vaContext.enc.rateControlMethod) == 0) {
        ERR("rc control not support !");
        return ENC_TURBO_ERROR_CONFIG;
    }
    attribs[0].value = m_vaContext.enc.rtFormat; // set to desired RT format
    attribs[1].value = m_vaContext.enc.rateControlMethod; // set to desired RC mode
    vaStatus = vaCreateConfig(m_vaContext.vaDpy, m_vaContext.enc.profile, m_vaContext.enc.selectEntrypoint,
                              attribs, numAttrib, &(m_vaContext.enc.configID));
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("EncVaCreateContext vaCreateConfig failed : %#x", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    vaStatus = vaCreateContext(m_vaContext.vaDpy, m_vaContext.enc.configID,
                               m_vaContext.enc.pictureWidth, m_vaContext.enc.pictureHeight,
                               VA_PROGRESSIVE, 0, 0, &(m_vaContext.enc.contextID));
    if (vaStatus != VA_STATUS_SUCCESS) {
        EncodeDestroyConfig();
        ERR("EncVaCreateContext vaCreateContext failed : %#x", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    
    return ENC_TURBO_SUCCESS;
}

EncTurboCode VaEncoderAmd::VppOpen()
{
    INFO("Vpp open...");
    if (m_vaContext.vaDpy == nullptr) {
        ERR("Vpp open failed, dpy is null.");
        return ENC_TURBO_ERROR_CONTEXT_HANDLE;
    }
    EncTurboCode encStatus = VppCreateConfig();
    if (encStatus != ENC_TURBO_SUCCESS) {
        return encStatus;
    }

    encStatus = VppCreateContext();
    if (encStatus != ENC_TURBO_SUCCESS) {
        VppDestroyConfig();
        return encStatus;
    }

    // 创建RGB转YUV中设定RGB显存地址与转换参数的buffer
    VAStatus vaStatus = vaCreateBuffer(m_vaContext.vaDpy, m_vaContext.vpp.contextID, VAProcPipelineParameterBufferType,
                                       sizeof(VAProcPipelineParameterBuffer), 1, nullptr,
                                       &(m_vaContext.vpp.pipelineBuf));
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("Vpp open create pipelineBuf failed: %#x", vaStatus);
        VppDestroyContext();
        VppDestroyConfig();
        return ENC_TURBO_ERROR_VA_STATUS;
    }

    INFO("Vpp open success.");
    return ENC_TURBO_SUCCESS;
}

EncTurboCode VaEncoderAmd::VppCreateConfig()
{
    VAStatus vaStatus = vaCreateConfig(m_vaContext.vaDpy, VAProfileNone, VAEntrypointVideoProc,
                                       nullptr, 0, &(m_vaContext.vpp.configID));
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("Vpp open vaCreateConfig failed: %#x", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    return ENC_TURBO_SUCCESS;
}

EncTurboCode VaEncoderAmd::VppCreateContext()
{
    VAStatus vaStatus = vaCreateContext(m_vaContext.vaDpy, m_vaContext.vpp.configID, m_vaContext.vpp.width,
                                        m_vaContext.vpp.height, 0, nullptr, 0, &(m_vaContext.vpp.contextID));
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("Vpp open vaCreateContext failed: %#x", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    return ENC_TURBO_SUCCESS;
}

EncTurboCode VaEncoderAmd::VppDestroyConfig()
{
    VAStatus vaStatus = vaDestroyConfig(m_vaContext.vaDpy, m_vaContext.vpp.configID);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("Vpp close vaDestroyConfig failed: %#x", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    return ENC_TURBO_SUCCESS;
}

EncTurboCode VaEncoderAmd::VppDestroyContext()
{
    VAStatus vaStatus = vaDestroyContext(m_vaContext.vaDpy, m_vaContext.vpp.contextID);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("Vpp close vaDestroyContext failed: %#x", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    return ENC_TURBO_SUCCESS;
}

EncTurboCode VaEncoderAmd::VppClose()
{
    INFO("Vpp close...");
    if (m_vaContext.vaDpy == nullptr) {
        ERR("Vpp open failed, dpy is null.");
        return ENC_TURBO_ERROR_CONTEXT_HANDLE;
    }

    EncTurboCode encStatus = ENC_TURBO_SUCCESS;
    VAStatus vaStatus = vaDestroyBuffer(m_vaContext.vaDpy, m_vaContext.vpp.pipelineBuf);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("Vpp Close vaDestroyBuffer failed: %#x", vaStatus);
        encStatus = ENC_TURBO_ERROR_VA_STATUS;
    }

    EncTurboCode error = ENC_TURBO_SUCCESS;
    error = VppDestroyContext();
    if (error != ENC_TURBO_SUCCESS) {
        encStatus = error;
    }
    error = VppDestroyConfig();
    if (error != ENC_TURBO_SUCCESS) {
        encStatus = error;
    }

    return encStatus;
}

EncTurboCode VaEncoderAmd::EncodeClose()
{
    INFO("Encode Close...");
    if (m_vaContext.vaDpy == nullptr) {
        ERR("Encode close failed, dpy is null");
        return ENC_TURBO_ERROR_CONTEXT_HANDLE;
    }
    EncTurboCode encStatus = ENC_TURBO_SUCCESS;
    EncTurboCode error = ENC_TURBO_SUCCESS;
    error = EncodeDestroyContext();
    if (error != ENC_TURBO_SUCCESS) {
        encStatus = error;
    }

    error = EncodeDestroyConfig();
    if (error != ENC_TURBO_SUCCESS) {
        encStatus = error;
    }

    if (m_vaContext.outputFormat == CAP_VA_ENCODE_H264 || m_vaContext.outputFormat == CAP_VA_ENCODE_HEVC) {
        VAStatus vaStatus = vaDestroySurfaces(m_vaContext.vaDpy, m_vaContext.enc.refSurface, REF_SURFACE_NUM);
        if (vaStatus != VA_STATUS_SUCCESS) {
            ERR("Encode close vaDestroySurfaces failed: %#x", vaStatus);
            encStatus = ENC_TURBO_ERROR_VA_STATUS;
        }
    }

    if (m_stream.first != nullptr) {
        delete[] m_stream.first;
        m_stream.first = nullptr;
        m_stream.second = 0;
    }

    return encStatus;
}

EncTurboCode VaEncoderAmd::EncodeDestroyConfig()
{
    VAStatus vaStatus = vaDestroyConfig(m_vaContext.vaDpy, m_vaContext.enc.configID);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("Encode close vaDestroyContext failed: %#x", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    return ENC_TURBO_SUCCESS;
}

EncTurboCode VaEncoderAmd::EncodeDestroyContext()
{
    VAStatus vaStatus = vaDestroyContext(m_vaContext.vaDpy, m_vaContext.enc.contextID);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("Encode close vaDestroyConfig failed: %#x", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    return ENC_TURBO_SUCCESS;
}

EncTurboCode VaEncoderAmd::EncodeDestroy()
{
    INFO("encode destroy...");
    // uninit编码驱动
    EncTurboCode encStatus = EncVaUninit();
    if (encStatus != ENC_TURBO_SUCCESS) {
        ERR("Encode destroy failed: %#x", encStatus);
    }
    // 关闭显卡设备节点
    DisplayDrmClose();
    return encStatus;
}

void VaEncoderAmd::DisplayDrmClose()
{
    if (m_vaContext.drmFd > 0) {
        (void)close(m_vaContext.drmFd);
        m_vaContext.drmFd = -1;
    }
}

EncTurboCode VaEncoderAmd::EncVaUninit()
{
    INFO("uninit enc");
    if (m_vaContext.vaDpy == nullptr) {
        ERR("enc va uninit dpy is null");
        return ENC_TURBO_ERROR_CONTEXT_HANDLE;
    }
    VAStatus vaStatus = vaTerminate(m_vaContext.vaDpy);
    m_vaContext.vaDpy = nullptr; // 驱动vaTerminate后，需要把va_dpy赋NULL，否则会变成野指针
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("EncVaUninit vaTerminate failed : %#x", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    return ENC_TURBO_SUCCESS;
}

void VaEncoderAmd::ContextDestroy()
{
    (void)std::fill_n(reinterpret_cast<uint8_t*>(&m_vaContext), sizeof(VaEncoderContext), 0);
}

// 调用层确保handle不为nullptr
EncTurboCode VaEncoderAmd::ImportRgbSurface(uint64_t handle, VASurfaceID &surfaceId)
{
    if (m_vaContext.vaDpy == nullptr) {
        ERR("Import rgb surface failed, dpy is nulll");
        return ENC_TURBO_ERROR_CONTEXT_HANDLE;
    }
    constexpr int attribsNum = 3;
    VASurfaceAttrib attribs[attribsNum];
    VASurfaceAttribExternalBuffers external;
    (void)std::fill_n(reinterpret_cast<uint8_t*>(&external), sizeof(VASurfaceAttribExternalBuffers), 0);
    external.pixel_format = m_vaContext.vpp.pixelFormat;
    external.width = m_vaContext.vpp.width;
    external.height = m_vaContext.vpp.height;
    // m_vaContext.vpp.width与height设定时有上限值限制
    external.data_size = external.width * external.height * RGB_BYTE_ONE_PIXEL;
    external.num_planes = 1;
    // m_vaContext.vpp.stride设定时有上限值限制
    external.pitches[0] = m_vaContext.vpp.stride * RGB_BYTE_ONE_PIXEL;
    external.buffers = reinterpret_cast<uintptr_t*>(&handle);
    external.num_buffers = 1;
    int i = 0;
    attribs[i].flags = VA_SURFACE_ATTRIB_SETTABLE;
    attribs[i].type = VASurfaceAttribMemoryType;
    attribs[i].value.type = VAGenericValueTypeInteger;
    attribs[i].value.value.i = VA_SURFACE_ATTRIB_MEM_TYPE_KERNEL_DRM; // RGB数据类型为内核空间的地址
    i++;
    attribs[i].flags = VA_SURFACE_ATTRIB_SETTABLE;
    attribs[i].type = VASurfaceAttribExternalBufferDescriptor;
    attribs[i].value.type = VAGenericValueTypePointer;
    attribs[i].value.value.p = &external;
    i++;
    attribs[i].flags = VA_SURFACE_ATTRIB_SETTABLE;
    attribs[i].type = VASurfaceAttribPixelFormat;
    attribs[i].value.type = VAGenericValueTypeInteger;
    attribs[i].value.value.i = m_vaContext.fourcc;
    VAStatus vaStatus = vaCreateSurfaces(m_vaContext.vaDpy, VA_RT_FORMAT_RGB32, m_vaContext.vpp.width,
                                         m_vaContext.vpp.height, &surfaceId, 1, attribs, attribsNum);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("Import rgb surface failed: %#x", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }

    return ENC_TURBO_SUCCESS;
}

EncTurboCode VaEncoderAmd::ReleaseRgbSurface(VASurfaceID &rgbSurfaceId)
{
    if (m_vaContext.vaDpy == nullptr) {
        ERR("Release rgb surface failed, dpy is null");
        return ENC_TURBO_ERROR_CONTEXT_HANDLE;
    }
    VAStatus vaStatus = vaDestroySurfaces(m_vaContext.vaDpy, &rgbSurfaceId, 1);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("Release rgb surface vaDestroySurfaces failed: %#x", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }

    return ENC_TURBO_SUCCESS;
}

EncTurboCode VaEncoderAmd::CreateYuvSurface(VASurfaceID &surfaceId)
{
    if (m_vaContext.vaDpy == nullptr) {
        ERR("Create yuv surface failed, dpy is null");
        return ENC_TURBO_ERROR_CONTEXT_HANDLE;
    }
    constexpr int surfaceAttribsNum = 1;
    VASurfaceAttrib surfaceAttribs[surfaceAttribsNum];
    surfaceAttribs[0].flags = VA_SURFACE_ATTRIB_SETTABLE;
    surfaceAttribs[0].type = VASurfaceAttribPixelFormat;
    surfaceAttribs[0].value.type = VAGenericValueTypeInteger;
    surfaceAttribs[0].value.value.i = m_vaContext.fourcc;
    VAStatus vaStatus = vaCreateSurfaces(m_vaContext.vaDpy, m_vaContext.enc.rtFormat,
                                         m_vaContext.enc.pictureWidth, m_vaContext.enc.pictureHeight,
                                         &surfaceId, 1, surfaceAttribs, surfaceAttribsNum);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("Create yuv surface vaCreateSurfaces failed: %#x", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }

    return ENC_TURBO_SUCCESS;
}

EncTurboCode VaEncoderAmd::ReleaseYuvSurface(VASurfaceID &surfaceId)
{
    if (m_vaContext.vaDpy == nullptr) {
        ERR("Release yuv surface failed, dpy is null");
        return ENC_TURBO_ERROR_CONTEXT_HANDLE;
    }
    VAStatus vaStatus = vaDestroySurfaces(m_vaContext.vaDpy, &surfaceId, 1);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("Release yuv surface vaDestroySurfaces failed: %#x", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }

    return ENC_TURBO_SUCCESS;
}

EncTurboCode VaEncoderAmd::CreateYuvImage(VAImage &yuvImage)
{
    if (m_vaContext.vaDpy == nullptr) {
        ERR("Create yuv image failed, dpy is null");
        return ENC_TURBO_ERROR_CONTEXT_HANDLE;
    }
    VAImageFormat imageFormat;
    (void)std::fill_n(reinterpret_cast<uint8_t*>(&imageFormat), sizeof(VAImageFormat), 0);
    imageFormat.fourcc = m_vaContext.downloadYuvFourcc;
    VAStatus vaStatus = vaCreateImage(m_vaContext.vaDpy, &imageFormat, m_vaContext.enc.pictureWidth,
                                      m_vaContext.enc.pictureHeight, &yuvImage);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("Create yuv image vaCreateImage failed: %#x", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    return ENC_TURBO_SUCCESS;
}

EncTurboCode VaEncoderAmd::ReleaseYuvImage(VAImage &outImage)
{
    if (m_vaContext.vaDpy == nullptr) {
        ERR("Release yuv image failed, dpy is null");
        return ENC_TURBO_ERROR_CONTEXT_HANDLE;
    }
    VAStatus vaStatus = vaDestroyImage(m_vaContext.vaDpy, outImage.image_id);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("Release yuv Image vaDestroyImage failed: %#x", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    return ENC_TURBO_SUCCESS;
}

EncTurboCode VaEncoderAmd::CopyYuvSurfaceToImage(VASurfaceID yuvSurfaceId, VAImage outImage)
{
    if (m_vaContext.vaDpy == nullptr) {
        ERR("Copy yuv data from surface to image failed, dpy is null");
        return ENC_TURBO_ERROR_CONTEXT_HANDLE;
    }
    VAStatus vaStatus = vaGetImage(m_vaContext.vaDpy, yuvSurfaceId, 0, 0, m_vaContext.enc.pictureWidth,
                                   m_vaContext.enc.pictureHeight, outImage.image_id);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("Copy yuv to Image vaGetImage failed: %#x", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    return ENC_TURBO_SUCCESS;
}

EncTurboCode VaEncoderAmd::MapImage(VAImage outImage, ConvertDataOutput &dataOutput)
{
    if (m_vaContext.vaDpy == nullptr) {
        ERR("Yuv image map failed, dpy is null");
        return ENC_TURBO_ERROR_CONTEXT_HANDLE;
    }
    VAStatus vaStatus = vaMapBuffer(m_vaContext.vaDpy, outImage.buf, (void**)&dataOutput.data);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("Yuv image map vaMapBuffer failed: %#x", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    dataOutput.dataLen = ClcYuvSize(m_vaContext.enc.pictureWidth * m_vaContext.enc.pictureHeight);
    return ENC_TURBO_SUCCESS;
}

EncTurboCode VaEncoderAmd::UnmapImage(VAImage outImage)
{
    if (m_vaContext.vaDpy == nullptr) {
        ERR("Unmap image failed, dpy is null");
        return ENC_TURBO_ERROR_CONTEXT_HANDLE;
    }
    VAStatus vaStatus = vaUnmapBuffer(m_vaContext.vaDpy, outImage.buf);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("Yuv image umap vaUnmapBuffer failed:%#x", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    return ENC_TURBO_SUCCESS;
}

EncTurboCode VaEncoderAmd::CreateCodedBuffer(VABufferID &bufferId)
{
    if (m_vaContext.vaDpy == nullptr) {
        ERR("Create coded buffer failed, dpy is null");
        return ENC_TURBO_ERROR_CONTEXT_HANDLE;
    }
    int32_t codedbufSize = ClcYuvSize(m_vaContext.enc.pictureWidth * m_vaContext.enc.pictureHeight);
    VAStatus vaStatus = vaCreateBuffer(m_vaContext.vaDpy, m_vaContext.enc.contextID, VAEncCodedBufferType,
                                       codedbufSize, 1, nullptr, &bufferId);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("Create coded buffer vaCreateBuffer failed:%#x", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }

    return ENC_TURBO_SUCCESS;
}

EncTurboCode VaEncoderAmd::ReleaseCodedBuffer(VABufferID &bufferId)
{
    if (bufferId == VA_INVALID_ID) {
        ERR("Release coded buffer failed, bufferid is invalid");
        return ENC_TURBO_ERROR_INVALID_PARAMETER;
    }
    if (m_vaContext.vaDpy == nullptr) {
        ERR("Release coded buffer failed, dpy is null");
        return ENC_TURBO_ERROR_CONTEXT_HANDLE;
    }
    VAStatus vaStatus = vaDestroyBuffer(m_vaContext.vaDpy, bufferId);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("Release coded buffer vaDestroyBuffer failed:%#x", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    bufferId = VA_INVALID_ID;
    return ENC_TURBO_SUCCESS;
}

EncTurboCode VaEncoderAmd::RgbConvertToYuv(VASurfaceID &rgbSurfaceId, VASurfaceID &yuvSurfaceId)
{
    if (m_vaContext.vaDpy == nullptr) {
        ERR("Rgb convert to yuv failed, dpy is null");
        return ENC_TURBO_ERROR_CONTEXT_HANDLE;
    }
    VAProcPipelineParameterBuffer *pipelineParam = nullptr;
    VAStatus vaStatus = vaMapBuffer(m_vaContext.vaDpy, m_vaContext.vpp.pipelineBuf, (void **) &pipelineParam);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("vpp rgb to yuv vaMapBuffer failed : %#x", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    (void)std::fill_n(reinterpret_cast<uint8_t*>(pipelineParam), sizeof(VAProcPipelineParameterBuffer), 0);
    pipelineParam->surface = rgbSurfaceId; // 存储RGB的surface_id
    pipelineParam->surface_color_standard = VAProcColorStandardNone;
    pipelineParam->output_background_color = 0xff000000; // 0xff000000 纯黑
    pipelineParam->output_color_standard = VAProcColorStandardNone;
    vaStatus = vaUnmapBuffer(m_vaContext.vaDpy, m_vaContext.vpp.pipelineBuf);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("vpp rgb to yuv vaUnmapBuffer failed : %#x", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    // 转换准备：指定输出的YUV surface
    vaStatus = vaBeginPicture(m_vaContext.vaDpy, m_vaContext.vpp.contextID,
                              yuvSurfaceId);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("vpp rgb to yuv vaBeginPicture failed : %#x", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    // 把待转换的RGB数据指定到vpp.contextID，trans rgb surface to yuv surface
    vaStatus = vaRenderPicture(m_vaContext.vaDpy, m_vaContext.vpp.contextID, &(m_vaContext.vpp.pipelineBuf), 1);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("va render picture failed : %#x", vaStatus);
        if (vaEndPicture(m_vaContext.vaDpy, m_vaContext.vpp.contextID) != VA_STATUS_SUCCESS) {
            ERR("va end picture failed : %#x", vaStatus);
        }
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    // 开始转换RGB数据到YUV
    vaStatus = vaEndPicture(m_vaContext.vaDpy, m_vaContext.vpp.contextID);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("vpp rgb to yuv vaEndPicture failed : %#x", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    return ENC_TURBO_SUCCESS;
}

EncTurboCode VaEncoderAmd::MapStreamBuffer(VABufferID codedbufBufId, StreamDataOutput &data)
{
    if (m_vaContext.vaDpy == nullptr) {
        ERR("Enc StreamBuffer map failed, dpy is null");
        return ENC_TURBO_ERROR_CONTEXT_HANDLE;
    }

    VAStatus vaStatus = vaMapBuffer(m_vaContext.vaDpy, codedbufBufId, (void**)&data.codedBufferSegment);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("Enc CodedStreamdBuffer Map StreamBuffer failed : %#x", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    if (data.codedBufferSegment->status & VA_CODED_BUF_STATUS_SLICE_OVERFLOW_MASK) {
        return ENC_TURBO_ERROR_VA_SYNC_STATUS;
    }

    data.handle = nullptr;
    uint32_t bufNum = GetListNodeNums(data);
    if (bufNum == 0) {
        ERR("MapStreamBuffer failed, ListNode length: %u", bufNum);
        return ENC_TURBO_ERROR_INTERNAL_STATUS;
    } else if (bufNum == 1) {
        data.handle = static_cast<uint8_t*>(data.codedBufferSegment->buf);
    }
    // change sps stream only for frame type-I
    if (m_vaContext.outputFormat == CAP_VA_ENCODE_HEVC &&
        m_vaContext.enc.currentFrameType == FRAME_TYPE_I) {
        if (bufNum > 1) {
            data.handle = std::make_unique<uint8_t[]>(data.dataLen).release();
            if (data.handle == nullptr) {
                return ENC_TURBO_ERROR_INTERNAL_STATUS;
            }
            RetriveBufferData(data.codedBufferSegment, data.handle, data.dataLen);
        }
        uint8_t* recordPtr = data.handle;
        // copy to m_stream
        EncTurboCode encStatus = ChangeSpsStreamH265(data);
        if (encStatus != ENC_TURBO_SUCCESS && bufNum > 1) {
            data.handle = nullptr;
        }
        // release temporary memory if Listnode length > 1
        if (bufNum > 1) {
            free(recordPtr);
        }
    }
    return ENC_TURBO_SUCCESS;
}

// 获取链表节点数目
uint32_t VaEncoderAmd::GetListNodeNums(StreamDataOutput &data)
{
    VACodedBufferSegment *bufList = data.codedBufferSegment;
    uint32_t streamSize = 0;
    uint32_t bufNum = 0;
    while (bufList != nullptr) {
        if (bufList->buf == nullptr) {
            WARN("Stream data is null");
            break;
        }
        streamSize += bufList->size;
        ++bufNum;
        bufList = reinterpret_cast<VACodedBufferSegment *>(bufList->next);
    }
    data.dataLen = streamSize;
    return bufNum;
}

void VaEncoderAmd::RetriveBufferData(VACodedBufferSegment *codedBufferSegment, uint8_t *data,
                                     uint32_t &dataLen)
{
    dataLen = 0;
    VACodedBufferSegment *bufList = codedBufferSegment;
    while (bufList != nullptr) {
        if (bufList->buf == nullptr) {
            WARN("Stream data is null, skip data copy");
            break;
        }
        dataLen += bufList->size;
        std::copy_n(static_cast<uint8_t *>(bufList->buf), bufList->size, data);
        data += bufList->size;
        bufList = reinterpret_cast<VACodedBufferSegment *>(bufList->next);
    }
    return;
}

EncTurboCode VaEncoderAmd::UnmapStreamBuffer(VABufferID codedbufBufId)
{
    if (m_vaContext.vaDpy == nullptr) {
        ERR("Enc Unmap StreamBuffer failed, dpy is null");
        return ENC_TURBO_ERROR_CONTEXT_HANDLE;
    }
    VAStatus vaStatus = vaUnmapBuffer(m_vaContext.vaDpy, codedbufBufId);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("Enc CodedStreamBuffer UnMap StreamBuffer failed : %#x", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    return ENC_TURBO_SUCCESS;
}

/**
 * @brief: YUV数据编码为H.264码流
 * @param [in] yuvSurfaceId: 存储Yuv的surface空间
 * @param [out] codedbufBufId: 存放编码后的数据
 * @return EncTurboCode: 成功，返回ENC_TURBO_SUCCESS；失败，返回ENC_TURBO_ERROR_VA_STATUS
 */
EncTurboCode VaEncoderAmd::EncPictureEncodeH264(VASurfaceID &yuvSurfaceId, VABufferID &codedbufBufId)
{
    EncTurboCode encTurboCode;
    if (m_vaContext.enc.currentFrameType == FRAME_TYPE_I) {
        encTurboCode = EncUpdateSdquenceParaH264();
        if (encTurboCode != ENC_TURBO_SUCCESS) {
            return encTurboCode;
        }
    }
    // 在VBR模式下，bitrate大于0，设置bitrate的最大窗口值
    encTurboCode = EncUpdateMiscRateCtrl();
    if (encTurboCode != ENC_TURBO_SUCCESS) {
        return encTurboCode;
    }
    // 在VBR模式下，bitrate大于0，设置bitrate的最大buffer值
    encTurboCode = EncUpdateMiscHrd();
    if (encTurboCode != ENC_TURBO_SUCCESS) {
        return encTurboCode;
    }
    EncUpdateRefPicListH264();
    // pps: picture parameter set，更新PPS参数
    encTurboCode = EncUpdatePictureParamH264(codedbufBufId);
    if (encTurboCode != ENC_TURBO_SUCCESS) {
        return encTurboCode;
    }
    // slice parameter
    encTurboCode = EncUpdateSliceParamH264();
    if (encTurboCode != ENC_TURBO_SUCCESS) {
        return encTurboCode;
    }
    // 编码开始
    encTurboCode = EncEncodeStart(yuvSurfaceId);
    if (encTurboCode != ENC_TURBO_SUCCESS) {
        return encTurboCode;
    }
    EncUpdateRefFramesH264();
    // 释放编码资源
    return ENC_TURBO_SUCCESS;
}

/**
 * @brief: YUV数据编码为H.265码流
 * @param [in] yuvSurfaceId: 存储Yuv的surface空间
 * @param [out] codedbufBufId: 存放编码后的数据
 * @return EncTurboCode: 成功，返回ENC_TURBO_SUCCESS；失败，返回ENC_TURBO_ERROR_VA_STATUS
 */
EncTurboCode VaEncoderAmd::EncPictureEncodeH265(VASurfaceID &yuvSurfaceId, VABufferID &codedbufBufId)
{
    EncTurboCode encTurboCode;
    if (m_vaContext.enc.currentFrameType == FRAME_TYPE_I) {
        encTurboCode = EncUpdateSdquenceParaH265();
        if (encTurboCode != ENC_TURBO_SUCCESS) {
            return encTurboCode;
        }
    }
    // 在VBR模式下，bitrate大于0，设置bitrate的最大窗口值
    encTurboCode = EncUpdateMiscRateCtrl();
    if (encTurboCode != ENC_TURBO_SUCCESS) {
        return encTurboCode;
    }
    // 在VBR模式下，bitrate大于0，设置bitrate的最大buffer值
    encTurboCode = EncUpdateMiscHrd();
    if (encTurboCode != ENC_TURBO_SUCCESS) {
        return encTurboCode;
    }
    EncUpdateRefPicListH265();
    // pps: picture parameter set，更新PPS参数
    encTurboCode = EncUpdatePictureParamH265(codedbufBufId);
    if (encTurboCode != ENC_TURBO_SUCCESS) {
        return encTurboCode;
    }
    // slice parameter
    encTurboCode = EncUpdateSliceParamH265();
    if (encTurboCode != ENC_TURBO_SUCCESS) {
        return encTurboCode;
    }
    // 编码开始
    encTurboCode = EncEncodeStart(yuvSurfaceId);
    if (encTurboCode != ENC_TURBO_SUCCESS) {
        return encTurboCode;
    }
    EncUpdateRefFramesH265();
    // 释放编码资源
    return ENC_TURBO_SUCCESS;
}

void VaEncoderAmd::SetProfileIdcH264()
{
    m_vaContext.enc.constraintSetFlag = 0;
    if (m_vaContext.enc.profileIdc == PROFILE_IDC_BASELINE) {
        m_vaContext.enc.constraintSetFlag |= 1;        // 0 : set constraint_set0_flag to 1
        m_vaContext.enc.constraintSetFlag |= (1 << 1); // 1 : set constraint_set1_flag to 1
        m_vaContext.enc.profile = VAProfileH264ConstrainedBaseline;
    } else if (m_vaContext.enc.profileIdc == PROFILE_IDC_MAIN) {
        m_vaContext.enc.constraintSetFlag |= (1 << 1); // 1 : set constraint_set1_flag to 1
        m_vaContext.enc.profile = VAProfileH264Main;
    } else if (m_vaContext.enc.profileIdc == PROFILE_IDC_HIGH) {
        m_vaContext.enc.constraintSetFlag |= (1 << 3); // 3 : set constraint_set3_flag to 1
        m_vaContext.enc.profile = VAProfileH264High;
    }
}

/**
 * 功能说明：更新reference参考帧序列，进行排序
 */
void VaEncoderAmd::EncUpdateRefPicListH264()
{
    if (m_vaContext.enc.currentFrameType == FRAME_TYPE_P) {
        (void)std::copy_n(m_vaContext.enc.ReferenceFramesH264, REF_FRAMES_NUM, m_vaContext.enc.RefPicList0H264);
        if (m_vaContext.enc.numShortTerm == REF_FRAMES_NUM) {
            if (m_vaContext.enc.RefPicList0H264[0].frame_idx < m_vaContext.enc.RefPicList0H264[1].frame_idx) {
                VAPictureH264 tmp = m_vaContext.enc.RefPicList0H264[0];
                m_vaContext.enc.RefPicList0H264[0] = m_vaContext.enc.RefPicList0H264[1];
                m_vaContext.enc.RefPicList0H264[1] = tmp;
            }
        }
    }
}

/**
 * 功能说明；H264 更新reference参考帧序列
 */
void VaEncoderAmd::EncUpdateRefFramesH264()
{
    m_vaContext.enc.CurrentCurrPicH264.flags = VA_PICTURE_H264_SHORT_TERM_REFERENCE;
    m_vaContext.enc.numShortTerm++;
    m_vaContext.enc.numShortTerm = (m_vaContext.enc.numShortTerm > m_vaContext.enc.numRefFrames)
                                        ? m_vaContext.enc.numRefFrames
                                        : m_vaContext.enc.numShortTerm;
    for (int i = m_vaContext.enc.numShortTerm - 1; i > 0; i--) {
        m_vaContext.enc.ReferenceFramesH264[i] = m_vaContext.enc.ReferenceFramesH264[i - 1];
    }
    m_vaContext.enc.ReferenceFramesH264[0] = m_vaContext.enc.CurrentCurrPicH264;
    m_vaContext.enc.currentFrameNum = (m_vaContext.enc.currentFrameNum + 1) & MAX_FRAME_NUM_MASK;
    for (int i = 0; i < REF_SURFACE_NUM; i++) {
        m_vaContext.enc.useSlot[i] = 0;
        for (uint32_t j = 0; j < m_vaContext.enc.numShortTerm; j++) {
            if (m_vaContext.enc.refSurface[i] == m_vaContext.enc.ReferenceFramesH264[j].picture_id) {
                m_vaContext.enc.useSlot[i] = 1;
                break;
            }
        }
    }
    return;
}

uint8_t VaEncoderAmd::EncGuessLevelH264(int profileIdc, int64_t bitrate, int framerate, int maxDecFrameBuffering)
{
    if (profileIdc == PROFILE_IDC_HIGH) {
        return LEVEL_IDC_DEFAULT_VALUE; // 40 levelIdcDefaultValue
    }
    uint32_t mbsWidth = m_vaContext.enc.pictureWidthInMbs;
    uint32_t mbsHeight = m_vaContext.enc.pictureHeightInMbs;

    int br = (profileIdc == PROFILE_IDC_MAIN) ? CPB_BR_NAL_FACTOR_HIGH : CPB_BR_NAL_FACTOR_BASE;
    for (uint32_t i = 0; i < sizeof(g_h264Levels) / sizeof(g_h264Levels[0]); i++) {
        H264Level *level = &g_h264Levels[i];
        if (bitrate > static_cast<int64_t>(level->maxBr * br) || mbsWidth * mbsHeight > level->maxFs ||
            mbsWidth * mbsWidth > BIY_PER_BYTE * level->maxFs || mbsHeight * mbsHeight > BIY_PER_BYTE * level->maxFs) {
            continue;
        }
        int maxDpbFramesTmp = level->maxDpbMbs / (mbsWidth * mbsHeight);
        int maxDpbFrames = (maxDpbFramesTmp > MAX_DPB_FRAMES) ? MAX_DPB_FRAMES : maxDpbFramesTmp;
        int fr = static_cast<int>(level->maxMbps / (1 + mbsWidth * mbsHeight));
        if (maxDecFrameBuffering > maxDpbFrames || framerate > fr) {
            continue;
        }
        return level->levelIdc;
    }
    return 40; // 40 levelIdcDefaultValue
}

/**
 * 功能说明：SPS table参数初始化
 */
void VaEncoderAmd::EncSeqParamInitH264()
{
    VAEncSequenceParameterBufferH264 *seqParam = &(m_vaContext.enc.seqParamH264);
    // 根据bit rate / frame rate计算出level
    seqParam->seq_parameter_set_id = 0;
    seqParam->level_idc =
        EncGuessLevelH264(m_vaContext.enc.profileIdc, m_vaContext.enc.bitRate, m_vaContext.enc.frameRate, 1);
    seqParam->intra_period = m_vaContext.enc.intraPeriod; // I帧
    seqParam->intra_idr_period = seqParam->intra_period;   // IDR帧
    seqParam->ip_period = 1; // 1;this is mean how many b frames per P，此处默认B帧为0
    seqParam->bits_per_second = (m_vaContext.enc.bitRate > 0) ? m_vaContext.enc.bitRate : 0;
    seqParam->max_num_ref_frames = (seqParam->intra_period == 1) ? 0 : 1;
    seqParam->picture_width_in_mbs = m_vaContext.enc.pictureWidthInMbs;
    seqParam->picture_height_in_mbs = m_vaContext.enc.pictureHeightInMbs;
    seqParam->seq_fields.bits.chroma_format_idc = 1;
    seqParam->seq_fields.bits.frame_mbs_only_flag = 1;
    seqParam->seq_fields.bits.direct_8x8_inference_flag = 1;
    seqParam->seq_fields.bits.log2_max_frame_num_minus4 = LOG2_MAX_FRAME_NUM;
    seqParam->seq_fields.bits.log2_max_pic_order_cnt_lsb_minus4 = LOG2_MAX_PIC_ORDER_CNT_LSB;
    seqParam->seq_fields.bits.pic_order_cnt_type = 2; // type 2 影响第一帧出帧的速度,解码播放顺序一致
    if (m_vaContext.enc.pictureWidth != SIZE_ALIGN * m_vaContext.enc.pictureWidthInMbs ||
        m_vaContext.enc.pictureHeight != SIZE_ALIGN * m_vaContext.enc.pictureHeightInMbs) {
        seqParam->frame_cropping_flag = 1;
        seqParam->frame_crop_left_offset = 0;
        seqParam->frame_crop_right_offset =
            (SIZE_ALIGN * m_vaContext.enc.pictureWidthInMbs - m_vaContext.enc.pictureWidth) >> 1;
        seqParam->frame_crop_top_offset = 0;
        seqParam->frame_crop_bottom_offset =
            (SIZE_ALIGN * m_vaContext.enc.pictureHeightInMbs - m_vaContext.enc.pictureHeight) >> 1;
    }
    if (m_vaContext.enc.bitRate > 0) {
        seqParam->vui_parameters_present_flag = 1; // HRD info located in vui
        seqParam->vui_fields.bits.aspect_ratio_info_present_flag = 1;
        seqParam->vui_fields.bits.timing_info_present_flag = 1;
        seqParam->vui_fields.bits.bitstream_restriction_flag = 1;
        seqParam->vui_fields.bits.log2_max_mv_length_horizontal = 15; // log2MaxMvLength 15
        seqParam->vui_fields.bits.log2_max_mv_length_vertical = 15;   // log2MaxMvLength 15
        seqParam->vui_fields.bits.fixed_frame_rate_flag = 1;
        seqParam->aspect_ratio_idc = 1;
        seqParam->num_units_in_tick = 1;                       // is num_units_in_tick / time_sacle
        seqParam->time_scale = m_vaContext.enc.frameRate * 2; // n_frames drop method type 2
    }
}

/**
 * 功能说明：PPS table参数初始化
 */
void VaEncoderAmd::EncPicParamInitH264()
{
    VAEncPictureParameterBufferH264 *picParam = &(m_vaContext.enc.picParamH264);
    (void)std::fill_n(reinterpret_cast<uint8_t*>(picParam), sizeof(VAEncPictureParameterBufferH264), 0);
    picParam->CurrPic.picture_id = VA_INVALID_ID;
    picParam->CurrPic.flags = VA_PICTURE_H264_INVALID;
    picParam->coded_buf = VA_INVALID_ID;
    picParam->pic_init_qp = m_vaContext.enc.picInitQpMinus26 + QP_DEFAULT_VALUE;
    picParam->num_ref_idx_l0_active_minus1 = 1;
    picParam->pic_fields.bits.entropy_coding_mode_flag =
        (m_vaContext.enc.profile == VAProfileH264ConstrainedBaseline) ? ENTROPY_MODE_CAVLC : ENTROPY_MODE_CABAC;
    picParam->pic_fields.bits.transform_8x8_mode_flag = ((m_vaContext.enc.constraintSetFlag & 0x7) == 0) ? 1 : 0;
    (void)std::fill_n(reinterpret_cast<uint8_t*>(&picParam->ReferenceFrames), sizeof(picParam->ReferenceFrames), 0xff);
}

/**
 * 功能说明：填充H264pps table
 */
EncTurboCode VaEncoderAmd::EncUpdatePictureParamH264(VABufferID codedbufBufId)
{
    int reconIndex = UtilityGetFreeSlot();
    VAEncPictureParameterBufferH264 *picParam = &(m_vaContext.enc.picParamH264);
    picParam->CurrPic.picture_id = m_vaContext.enc.refSurface[reconIndex];
    picParam->CurrPic.frame_idx = m_vaContext.enc.currentFrameNum;
    picParam->CurrPic.flags = 0;
    picParam->CurrPic.TopFieldOrderCnt = m_vaContext.enc.currentPoc;
    picParam->CurrPic.BottomFieldOrderCnt = picParam->CurrPic.TopFieldOrderCnt;
    picParam->coded_buf = codedbufBufId; // 指定对应的编码buffer
    picParam->frame_num = m_vaContext.enc.currentFrameNum;
    picParam->pic_fields.bits.idr_pic_flag = (m_vaContext.enc.currentFrameType == FRAME_TYPE_I);
    picParam->pic_fields.bits.reference_pic_flag = (m_vaContext.enc.currentFrameType != FRAME_TYPE_B);
    m_vaContext.enc.CurrentCurrPicH264 = picParam->CurrPic;
    if (m_vaContext.enc.currentFrameType == FRAME_TYPE_P) {
        (void)std::fill_n(
            reinterpret_cast<uint8_t *>(&picParam->ReferenceFrames), sizeof(picParam->ReferenceFrames), 0xff);
        picParam->ReferenceFrames[0] = m_vaContext.enc.RefPicList0H264[0];
    }
    VAStatus vaStatus = vaCreateBuffer(m_vaContext.vaDpy, m_vaContext.enc.contextID, VAEncPictureParameterBufferType,
        sizeof(*picParam), 1, picParam, &(m_vaContext.enc.picParamBufID));
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("Enc UpdatePictureParam vaCreateBuffer failed : %#x", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    return ENC_TURBO_SUCCESS;
}

/**
 * 功能说明：更新/填充slice级编码的参数
 */
EncTurboCode VaEncoderAmd::EncUpdateSliceParamH264()
{
    VAEncSliceParameterBufferH264 *sliceParam = &(m_vaContext.enc.sliceParamH264);
    (void)std::fill_n(reinterpret_cast<uint8_t*>(sliceParam), sizeof(VAEncSliceParameterBufferH264), 0);
    sliceParam->num_macroblocks = m_vaContext.enc.pictureHeightInMbs * m_vaContext.enc.pictureWidthInMbs;
    sliceParam->slice_type = m_vaContext.enc.currentFrameType;
    sliceParam->direct_spatial_mv_pred_flag = 1;
    sliceParam->num_ref_idx_l0_active_minus1 = 1;
    sliceParam->slice_alpha_c0_offset_div2 = 2; // sliceAlphaC0OffsetDiv 2
    sliceParam->slice_beta_offset_div2 = 2;     // sliceBetaOffsetDiv 2
    sliceParam->direct_spatial_mv_pred_flag = 1;
    if (m_vaContext.enc.currentFrameType == FRAME_TYPE_P) {
        (void)std::fill_n(reinterpret_cast<uint8_t*>(sliceParam->RefPicList0), sizeof(sliceParam->RefPicList0), 0xff);
        sliceParam->RefPicList0[0] = m_vaContext.enc.RefPicList0H264[0];
    }
    VAStatus vaStatus = vaCreateBuffer(m_vaContext.vaDpy, m_vaContext.enc.contextID, VAEncSliceParameterBufferType,
                                       sizeof(*sliceParam), 1, sliceParam, &(m_vaContext.enc.sliceParamBufID));
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("Enc UpdateSliceParam vaCreateBuffer failed : %#x", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    return ENC_TURBO_SUCCESS;
}

EncTurboCode VaEncoderAmd::EncUpdateSdquenceParaH264()
{
    // 更新SPS编码参数
    VAEncSequenceParameterBufferH264 *seqParam = &(m_vaContext.enc.seqParamH264);
    VAStatus vaStatus = vaCreateBuffer(m_vaContext.vaDpy, m_vaContext.enc.contextID, VAEncSequenceParameterBufferType,
                                       sizeof(*seqParam), 1, seqParam, &(m_vaContext.enc.seqParamBufID));
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("EncPictureEncodeH264 EncUpdateSequenceParam failed : %#x", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    return ENC_TURBO_SUCCESS;
}

EncTurboCode VaEncoderAmd::ChangeSpsStreamH265(StreamDataOutput &data)
{
    if (data.dataLen <= 3) { // 3 pervent array bounds
        WARN("codebufSize size err");
        return ENC_TURBO_ERROR_INTERNAL_STATUS;
    }

    int fixPos = 56; // 56 :  57 fix pos
    int endPos;
    uint8_t *stream = data.handle;
    for (endPos = fixPos; endPos < data.dataLen - 3; endPos++) { // 3 pervent array bounds
        if (stream[endPos] == 0 && stream[endPos + 1] == 0 &&
            stream[endPos + 2] == 0 && stream[endPos + 3] == 1) { // 2 3 : pos
            break;
        }
    }
    if (endPos == data.dataLen - 3) { // 3 pervent array bounds
        WARN("find separator failed, sps header fix fail");
        return ENC_TURBO_ERROR_INTERNAL_STATUS;
    }

    std::pair<uint8_t *, uint32_t> newSpsStream = m_vaBitstream.GetBitstreamForSPS();
    if (newSpsStream.second == 0) {
        WARN("Fill sps header first");
        return ENC_TURBO_ERROR_INTERNAL_STATUS;
    }

    std::copy_n(stream, fixPos, m_stream.first);
    std::copy_n(newSpsStream.first, newSpsStream.second, m_stream.first + fixPos);
    std::copy_n(stream + endPos, data.dataLen - endPos, m_stream.first + fixPos + newSpsStream.second);

    data.handle = m_stream.first;
    data.dataLen = fixPos + newSpsStream.second + data.dataLen - endPos;
    return ENC_TURBO_SUCCESS;
}

void VaEncoderAmd::EncUpdateRefPicListH265()
{
    if (m_vaContext.enc.currentFrameType == FRAME_TYPE_P) {
        (void)std::copy_n(m_vaContext.enc.ReferenceFramesH265, REF_FRAMES_NUM, m_vaContext.enc.RefPicList0H265);
        if (m_vaContext.enc.numShortTerm == REF_FRAMES_NUM) {
            if (m_vaContext.enc.RefPicList0H265[0].pic_order_cnt <=
                m_vaContext.enc.RefPicList0H265[1].pic_order_cnt) {
                VAPictureHEVC tmp = m_vaContext.enc.RefPicList0H265[0];
                m_vaContext.enc.RefPicList0H265[0] = m_vaContext.enc.RefPicList0H265[1];
                m_vaContext.enc.RefPicList0H265[1] = tmp;
            }
        }
    }
}

/**
 * 功能说明：SPS table参数初始化
 */
void VaEncoderAmd::EncSeqParamInitH265()
{
    VAEncSequenceParameterBufferHEVC *seqParam = &(m_vaContext.enc.seqParamH265);
    (void)std::fill_n(reinterpret_cast<uint8_t*>(seqParam), sizeof(VAEncSequenceParameterBufferHEVC), 0);
    // 根据bit rate / frame rate计算出level
    seqParam->general_profile_idc = 1;
    seqParam->general_level_idc = 120;                     // para 120 (libvautil)
    seqParam->intra_period = m_vaContext.enc.intraPeriod; // I帧
    seqParam->intra_idr_period = seqParam->intra_period;   // IDR帧
    seqParam->ip_period = 1; // this is mean how many b frames per P，此处默认B帧为0
    seqParam->bits_per_second = (m_vaContext.enc.bitRate > 0) ? m_vaContext.enc.bitRate : 0;
    seqParam->pic_width_in_luma_samples = m_vaContext.enc.pictureWidthAlignedH265;
    seqParam->pic_height_in_luma_samples = m_vaContext.enc.pictureHeightAlignedH265;
    seqParam->seq_fields.bits.chroma_format_idc = 1; // 色度采样格式，1：420
    seqParam->seq_fields.bits.amp_enabled_flag = 1;
    seqParam->log2_diff_max_min_luma_coding_block_size = 3; // 3 : log2(LCU_SIZE) - 3
    seqParam->log2_diff_max_min_transform_block_size = 3;   // para 3
    seqParam->max_transform_hierarchy_depth_inter = 3;      // para 3
    seqParam->max_transform_hierarchy_depth_intra = 3;      // para 3
}

/**
 * 功能说明：PPS table参数初始化
 */
void VaEncoderAmd::EncPicParamInitH265()
{
    VAEncPictureParameterBufferHEVC *picParam = &(m_vaContext.enc.picParamH265);
    (void)std::fill_n(reinterpret_cast<uint8_t*>(picParam), sizeof(VAEncPictureParameterBufferHEVC), 0);
    picParam->coded_buf = VA_INVALID_ID;
    picParam->decoded_curr_pic.picture_id = VA_INVALID_ID;
    picParam->decoded_curr_pic.flags = VA_PICTURE_HEVC_INVALID;
    picParam->pic_init_qp = m_vaContext.enc.picInitQpMinus26 + QP_DEFAULT_VALUE;
}

/**
 * 功能说明：填充H265pps table
 */
EncTurboCode VaEncoderAmd::EncUpdatePictureParamH265(VABufferID codedbufBufId)
{
    VAEncPictureParameterBufferHEVC *picParam = &(m_vaContext.enc.picParamH265);
    int reconIndex = UtilityGetFreeSlot();
    picParam->last_picture = 0;
    picParam->pic_init_qp = QP_DEFAULT_VALUE - 1;
    picParam->coded_buf = codedbufBufId;
    picParam->decoded_curr_pic.picture_id = m_vaContext.enc.refSurface[reconIndex];
    picParam->decoded_curr_pic.pic_order_cnt = m_vaContext.enc.currentPoc;
    picParam->decoded_curr_pic.flags = 0;
    m_vaContext.enc.CurrentCurrPicH265 = picParam->decoded_curr_pic;
    if (m_vaContext.enc.currentFrameType == FRAME_TYPE_P) {
        picParam->pic_fields.bits.idr_pic_flag = 0;
        picParam->pic_fields.bits.coding_type = 2; // 2 : for p
        picParam->pic_fields.bits.reference_pic_flag = 1;
        picParam->nal_unit_type = 1;
        (void)std::fill_n(
            reinterpret_cast<uint8_t *>(&picParam->reference_frames), sizeof(picParam->reference_frames), 0xff);
        picParam->reference_frames[0] = m_vaContext.enc.RefPicList0H265[0];
    } else {
        picParam->pic_fields.bits.idr_pic_flag = 1;
        picParam->pic_fields.bits.coding_type = 1;
        picParam->pic_fields.bits.reference_pic_flag = 1;
        picParam->nal_unit_type = 19; // 19 : NALU_IDR_W_DLP
    }
    VAStatus vaStatus = vaCreateBuffer(m_vaContext.vaDpy, m_vaContext.enc.contextID, VAEncPictureParameterBufferType,
        sizeof(*picParam), 1, picParam, &(m_vaContext.enc.picParamBufID));
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("Enc UpdatePictureParam vaCreateBuffer failed : %#x", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }

    return ENC_TURBO_SUCCESS;
}

/**
 * 功能说明：更新/填充slice级编码的参数
 */
EncTurboCode VaEncoderAmd::EncUpdateSliceParamH265()
{
    VAEncSliceParameterBufferHEVC *sliceParam = &(m_vaContext.enc.sliceParamH265);
    (void)std::fill_n(reinterpret_cast<uint8_t*>(sliceParam), sizeof(VAEncSliceParameterBufferHEVC), 0);
    sliceParam->slice_segment_address = 0;
    int widthCtus = (m_vaContext.enc.pictureWidthAlignedH265 + LCU_SIZE - 1) / LCU_SIZE;
    int heightCtus = (m_vaContext.enc.pictureHeightAlignedH265 + LCU_SIZE - 1) / LCU_SIZE;
    sliceParam->num_ctu_in_slice = widthCtus * heightCtus;
    sliceParam->slice_type = (m_vaContext.enc.currentFrameType == FRAME_TYPE_P ? 1 : 2); // 0 - B , 1 - P , 2 - I
    sliceParam->num_ref_idx_l0_active_minus1 = m_vaContext.enc.picParamH265.num_ref_idx_l0_default_active_minus1;
    sliceParam->num_ref_idx_l1_active_minus1 = m_vaContext.enc.picParamH265.num_ref_idx_l1_default_active_minus1;
    sliceParam->max_num_merge_cand = 5; // [1,5]
    sliceParam->slice_cb_qp_offset = m_vaContext.enc.picParamH265.pps_cb_qp_offset;
    sliceParam->slice_cr_qp_offset = m_vaContext.enc.picParamH265.pps_cr_qp_offset;
    sliceParam->slice_beta_offset_div2 = 2; // sliceBetaOffsetDiv 2
    sliceParam->slice_fields.bits.slice_temporal_mvp_enabled_flag = 0;
    sliceParam->slice_fields.bits.last_slice_of_pic_flag = 1;

    if (m_vaContext.enc.currentFrameType == FRAME_TYPE_P) {
        (void)std::fill_n(
            reinterpret_cast<uint8_t *>(sliceParam->ref_pic_list0), sizeof(sliceParam->ref_pic_list0), 0xff);
        sliceParam->ref_pic_list0[0] = m_vaContext.enc.RefPicList0H265[0];
        sliceParam->slice_fields.bits.last_slice_of_pic_flag = 0;
        sliceParam->slice_fields.bits.slice_temporal_mvp_enabled_flag = 1;
    }

    VAStatus vaStatus = vaCreateBuffer(m_vaContext.vaDpy, m_vaContext.enc.contextID, VAEncSliceParameterBufferType,
                                       sizeof(*sliceParam), 1, sliceParam, &(m_vaContext.enc.sliceParamBufID));
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("Enc UpdateSliceParam vaCreateBuffer failed : %#x", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    return ENC_TURBO_SUCCESS;
}

EncTurboCode VaEncoderAmd::EncUpdateSdquenceParaH265()
{
    // 更新SPS编码参数
    VAEncSequenceParameterBufferHEVC *seqParam = &(m_vaContext.enc.seqParamH265);
    VAStatus vaStatus = vaCreateBuffer(m_vaContext.vaDpy, m_vaContext.enc.contextID, VAEncSequenceParameterBufferType,
        sizeof(*seqParam), 1, seqParam, &(m_vaContext.enc.seqParamBufID));
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("EncPictureEncodeH265 EncUpdateSequenceParam failed : %#x", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    return ENC_TURBO_SUCCESS;
}

/**
 * 功能说明；H265 更新reference参考帧序列
 */
void VaEncoderAmd::EncUpdateRefFramesH265()
{
    m_vaContext.enc.numShortTerm++;
    m_vaContext.enc.numShortTerm = (m_vaContext.enc.numShortTerm > m_vaContext.enc.numRefFrames)
                                        ? m_vaContext.enc.numRefFrames
                                        : m_vaContext.enc.numShortTerm;
    m_vaContext.enc.ReferenceFramesH265[0] = m_vaContext.enc.CurrentCurrPicH265;
    for (int i = 0; i < REF_SURFACE_NUM; i++) {
        m_vaContext.enc.useSlot[i] = 0;
        for (uint32_t j = 0; j < m_vaContext.enc.numShortTerm; j++) {
            if (m_vaContext.enc.refSurface[i] == m_vaContext.enc.ReferenceFramesH265[j].picture_id) {
                m_vaContext.enc.useSlot[i] = 1;
                break;
            }
        }
    }
    return;
}


/**
 * @brief: 获取空闲的slot
 * @return int: 如有空闲的slot，则返回空闲slot的编号；如无空闲的slot，则返回最后一个slot的编号
 */
int VaEncoderAmd::UtilityGetFreeSlot()
{
    for (int i = 0; i < REF_SURFACE_NUM; i++) {
        if (m_vaContext.enc.useSlot[i] == 0) {
            return i;
        }
    }
    WARN("No free slot to store the reconstructed frame");
    return REF_SURFACE_NUM - 1;
}

/**
 * @brief: 销毁指定ID的buffer缓冲区
 * @param [in] bufId: 指定销毁的buffer ID
 */
void VaEncoderAmd::EncBuffersDestroy(VABufferID &bufId)
{
    if (bufId == VA_INVALID_ID) {
        return;
    }
    VAStatus vaStatus = vaDestroyBuffer(m_vaContext.vaDpy, bufId);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("vaDestroyBuffer failed(ret:%d)", vaStatus);
    }
    bufId = VA_INVALID_ID;
}

/**
 * @brief: 一帧编码结束，释放资源
 */
void VaEncoderAmd::EncEncodeFinish()
{
    EncBuffersDestroy(m_vaContext.enc.picParamBufID); // EncUpdatePictureParam
    EncBuffersDestroy(m_vaContext.enc.sliceParamBufID); // EncUpdateSliceParam
    EncBuffersDestroy(m_vaContext.enc.miscHrdBufID); // EncUpdateMiscHrd
    EncBuffersDestroy(m_vaContext.enc.miscRateControlBufID); // EncUpdateMiscRateCtrl
    if (m_vaContext.enc.currentFrameType == FRAME_TYPE::FRAME_TYPE_I) {
        EncBuffersDestroy(m_vaContext.enc.seqParamBufID); // EncUpdateSequenceParam
    }
}

/**
 * @brief: CBR码控模式下更新IDR帧的RemoveTime
 */
void VaEncoderAmd::EncUpdateIDRRemovalTime()
{
    uint64_t frameInterval = m_vaContext.enc.encFrameNumber - m_vaContext.enc.idrFrameNum;
    m_vaContext.enc.currentCpbRemoval = m_vaContext.enc.prevIdrCpbRemoval + frameInterval * CPB_REMOVAL_DELAY;
    m_vaContext.enc.idrFrameNum = m_vaContext.enc.encFrameNumber;
    m_vaContext.enc.currentIdrCpbRemoval = m_vaContext.enc.currentCpbRemoval;
    m_vaContext.enc.currentDpbRemovalDelta = (m_vaContext.enc.ipPeriod + 1) * CURRENT_DPB_REMOVAL_DELTA;
}

/**
 * @brief: CBR码控模式下更新非IDR帧的RemoveTime
 */
void VaEncoderAmd::EncUpdateRemovalTime()
{
    uint64_t frameInterval = m_vaContext.enc.encFrameNumber - m_vaContext.enc.idrFrameNum;
    m_vaContext.enc.currentCpbRemoval = m_vaContext.enc.currentIdrCpbRemoval + frameInterval * CPB_REMOVAL_DELAY;
    m_vaContext.enc.currentDpbRemovalDelta = (m_vaContext.enc.ipPeriod + 1) * CURRENT_DPB_REMOVAL_DELTA;
}

/**
 * @brief: 更新码控参数，用于设置码率的最大值和波动窗口大小等
 * @return EncTurboCode: 成功，返回ENC_TURBO_SUCCESS；失败，返回ENC_TURBO_ERROR_VA_STATUS
 */
EncTurboCode VaEncoderAmd::EncUpdateMiscRateCtrl()
{
    if (m_vaContext.enc.bitRate == 0) {
        return ENC_TURBO_SUCCESS;
    }
    // 创建一块内存空间
    VAStatus vaStatus = vaCreateBuffer(m_vaContext.vaDpy, m_vaContext.enc.contextID, VAEncMiscParameterBufferType,
                                       sizeof(VAEncMiscParameterBuffer) + sizeof(VAEncMiscParameterRateControl), 1,
                                       nullptr, &(m_vaContext.enc.miscRateControlBufID));
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("Enc UpdateMiscRateCtrl vaCreateBuffer failed: %d", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    // 映射到用户空间地址
    VAEncMiscParameterBuffer *miscRateCtrlBuffer = nullptr;
    vaStatus = vaMapBuffer(m_vaContext.vaDpy, m_vaContext.enc.miscRateControlBufID, (void **)&miscRateCtrlBuffer);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("Enc UpdateMiscRateCtrl vaMapBuffer failed: %d", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }

    // 更新码控参数到显存，vaUnmapBuffer是实际拷贝数据到显存的动作
    miscRateCtrlBuffer->type = VAEncMiscParameterTypeRateControl;
    VAEncMiscParameterRateControl *miscRateCtrl =
        reinterpret_cast<VAEncMiscParameterRateControl *>(miscRateCtrlBuffer->data);
    miscRateCtrl->bits_per_second = m_vaContext.enc.maxBitRate;
    miscRateCtrl->target_percentage = (m_vaContext.enc.bitRate * 100) / m_vaContext.enc.maxBitRate;  // 100 percentage
    miscRateCtrl->window_size = (m_vaContext.enc.maxBitRate * 1000) / miscRateCtrl->bits_per_second;  // 1000 windows
    
    vaStatus = vaUnmapBuffer(m_vaContext.vaDpy, m_vaContext.enc.miscRateControlBufID);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("Enc UpdateMiscRateCtrl vaUnmapBuffer failed: %d", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    return ENC_TURBO_SUCCESS;
}

/**
 * @brief: 更新HRD参数，用于设置编码缓冲区容量和初始缓冲区大小
 * @return EncTurboCode: 成功，返回ENC_TURBO_SUCCESS；失败，返回ENC_TURBO_ERROR_VA_STATUS
 */
EncTurboCode VaEncoderAmd::EncUpdateMiscHrd()
{
    if (m_vaContext.enc.bitRate == 0) {
        return ENC_TURBO_SUCCESS;
    }
    // 创建一块内存空间
    VAStatus vaStatus = vaCreateBuffer(m_vaContext.vaDpy, m_vaContext.enc.contextID, VAEncMiscParameterBufferType,
                                       sizeof(VAEncMiscParameterBuffer) + sizeof(VAEncMiscParameterHRD), 1,
                                       nullptr, &(m_vaContext.enc.miscHrdBufID));
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("Enc UpdateMiscHrd vaCreateBuffer failed: %d", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    // 映射到用户空间地址
    VAEncMiscParameterBuffer *miscHRDBuffer;
    vaStatus = vaMapBuffer(m_vaContext.vaDpy, m_vaContext.enc.miscHrdBufID, (void **)&miscHRDBuffer);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("Enc UpdateMiscHrd vaMapBuffer failed: %d", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    // 更新hrd参数到显存，vaUnmapBuffer是实际拷贝数据到显存的动作
    miscHRDBuffer->type = VAEncMiscParameterTypeHRD;
    VAEncMiscParameterHRD *miscHRD = reinterpret_cast<VAEncMiscParameterHRD *>(miscHRDBuffer->data);
    miscHRD->initial_buffer_fullness = static_cast<uint32_t>(m_vaContext.enc.maxBitRate * 0.75);  // 0.75: 3 / 4
    miscHRD->buffer_size = m_vaContext.enc.maxBitRate;  // hrd_buffer_size must >= maxBitRate

    vaStatus = vaUnmapBuffer(m_vaContext.vaDpy, m_vaContext.enc.miscHrdBufID);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("Enc UpdateMiscHrd vaUnmapBuffer failed: %d", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    return ENC_TURBO_SUCCESS;
}

/**
 * @brief: 把YUV/sps/pps/slice/ratectrl/hrd各buffer的值送入显存并启动编码
 * @param [in] yuvSurfaceId: YUV数据的surface ID，可通过此ID访问显存中的YUV数据
 * @return EncTurboCode: 成功，返回ENC_TURBO_SUCCESS；失败，返回ENC_TURBO_ERROR_VA_STATUS
 */
EncTurboCode VaEncoderAmd::EncEncodeStart(VASurfaceID &yuvSurfaceId)
{
    // 获取YUV数据
    VAStatus vaStatus = vaSyncSurface(m_vaContext.vaDpy, yuvSurfaceId);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("Enc EncodeStart get yuv buffer vaSyncSurface failed: %d", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    VABufferID vaBuffers[MAX_VA_BUFFERS];
    uint32_t vaBuffersNum = 0;
    if (m_vaContext.enc.currentFrameType == FRAME_TYPE::FRAME_TYPE_I) {
        vaBuffers[vaBuffersNum++] = m_vaContext.enc.seqParamBufID;  // sps table
    }
    vaBuffers[vaBuffersNum++] = m_vaContext.enc.picParamBufID;  // pps table
    vaBuffers[vaBuffersNum++] = m_vaContext.enc.sliceParamBufID;   // slice para
    if (m_vaContext.enc.miscRateControlBufID != VA_INVALID_ID) {
        vaBuffers[vaBuffersNum++] = m_vaContext.enc.miscRateControlBufID;  // rate control buffer
    }
    if (m_vaContext.enc.miscHrdBufID != VA_INVALID_ID) {
        vaBuffers[vaBuffersNum++] = m_vaContext.enc.miscHrdBufID;  // hrd buffer
    }
    // YUV数据 surface绑定到编码器context_id
    vaStatus = vaBeginPicture(
        m_vaContext.vaDpy, m_vaContext.enc.contextID, yuvSurfaceId);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("Enc EncodeStart vaBeginPicture failed: %d", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    // sps/pps/slice/rateCtrl/hrd参数绑定到编码器context_id
    vaStatus = vaRenderPicture(m_vaContext.vaDpy, m_vaContext.enc.contextID, vaBuffers, vaBuffersNum);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("Enc EncodeStart vaRenderPicture failed: %d", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    // 把vaBeginPicture/vaRenderPicture中绑定的buffer内容拷贝到显存空间，并开始编码，编码模式为同步模式
    vaStatus = vaEndPicture(m_vaContext.vaDpy, m_vaContext.enc.contextID);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("Enc EncodeStart vaEndPicture failed: %d", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    return CheckEncodeEnd(yuvSurfaceId);
}

EncTurboCode VaEncoderAmd::CheckEncodeEnd(VASurfaceID &yuvSurfaceId)
{
    VASurfaceStatus surfaceStatus;
    // 检查编码是否结束
    VAStatus vaStatus = vaSyncSurface(m_vaContext.vaDpy, yuvSurfaceId);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("Enc ReceiveCodedBuffer vaSyncSurface failed : %#x", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    // 查询编码器编码状态，因为有几率存在编码不成功情况，即当创建的codedBuffer size小于实际编码的大小时，编码就会不成功
    // 当编码器状态为VA_CODED_BUF_STATUS_SLICE_OVERFLOW_MASK时，增大编码buffer size，重新编码此帧
    vaStatus = vaQuerySurfaceStatus(m_vaContext.vaDpy, yuvSurfaceId, &surfaceStatus);
    if (vaStatus != VA_STATUS_SUCCESS) {
        ERR("Enc vaQuerySurfaceStatus failed: %#x", vaStatus);
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    return ENC_TURBO_SUCCESS;
}

/**
 * @brief: YUV数据编码为视频码流
 * @param [in] yuvSurfaceId: 存储Yuv的surface空间
 * @param [out] codedbufBufId: 存放编码后的数据
 * @return EncTurboCode: ENC_TURBO_SUCCESS表示成功，其他错误码表示失败
 */
EncTurboCode VaEncoderAmd::Encode(VASurfaceID yuvSurfaceId, VABufferID codedbufBufId)
{
    m_vaContext.enc.encFrameNumber = m_frameIndex;
    m_vaContext.enc.currentFrameDisplay = m_frameIndex;
    if ((m_vaContext.enc.encFrameNumber % m_vaContext.enc.intraPeriod == 0) || m_needKeyFrame) {
        m_needKeyFrame = false;
        m_vaContext.enc.currentFrameType = FRAME_TYPE::FRAME_TYPE_I;
        m_vaContext.enc.numShortTerm = 0;
        m_vaContext.enc.currentFrameNum = 0;
        (void)std::fill_n(reinterpret_cast<uint8_t *>(&m_vaContext.enc.useSlot), sizeof(m_vaContext.enc.useSlot), 0);
        m_vaContext.enc.currentIDRdisplay = m_vaContext.enc.currentFrameDisplay;
        if (m_vaContext.enc.rateControlMethod == VA_RC_CBR) {
            EncUpdateIDRRemovalTime();
        }
    } else {
        m_vaContext.enc.currentFrameType = FRAME_TYPE::FRAME_TYPE_P;
        if (m_vaContext.enc.rateControlMethod == VA_RC_CBR) {
            EncUpdateRemovalTime();
        }
    }
    m_vaContext.enc.currentPoc =
        ((m_vaContext.enc.currentFrameDisplay - m_vaContext.enc.currentIDRdisplay) % m_vaContext.enc.intraPeriod);
    // 开始编码
    EncTurboCode encTurboCode;
    if (m_vaContext.outputFormat == CAP_VA_ENCODE_H264) {
        encTurboCode = EncPictureEncodeH264(yuvSurfaceId, codedbufBufId);
    } else {
        encTurboCode = EncPictureEncodeH265(yuvSurfaceId, codedbufBufId);
    }
    EncEncodeFinish();
    if (encTurboCode != ENC_TURBO_SUCCESS) {
        ERR("EncPictureEncode failed: %#x", encTurboCode);
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    if ((m_vaContext.enc.currentFrameType == FRAME_TYPE::FRAME_TYPE_I) &&
        (m_vaContext.enc.rateControlMethod == VA_RC_CBR)) {
        m_vaContext.enc.prevIdrCpbRemoval = m_vaContext.enc.currentIdrCpbRemoval;
    }
    m_frameIndex++;
    return ENC_TURBO_SUCCESS;
}

uint32_t VaEncoderAmd::ClcYuvSize(uint32_t size) const
{
    return size + (size + 3) / 4 * 2;  // mul 2 , Usize eq Vsize eq (Ysize + 3) / 4
}

EncTurboCode VaEncoderAmd::CheckConvertStatus(VASurfaceID &yuvSurfaceId)
{
    if (m_vaContext.vaDpy == nullptr) {
        ERR("Check convert status failed, dpy is null");
        return ENC_TURBO_ERROR_CONTEXT_HANDLE;
    }

    VAStatus vaStatus = vaSyncSurface(m_vaContext.vaDpy, yuvSurfaceId);
    if (vaStatus != VA_STATUS_SUCCESS) {
        return ENC_TURBO_ERROR_VA_STATUS;
    }
    return ENC_TURBO_SUCCESS;
}

void VaEncoderAmd::SetNewValue()
{
    if (m_vaContext.outputFormat == CAP_VA_ENCODE_H264) {
        SetProfileIdcH264();
        VAEncSequenceParameterBufferH264 *seqParam = &(m_vaContext.enc.seqParamH264);
        seqParam->level_idc =
            EncGuessLevelH264(m_vaContext.enc.profileIdc, m_vaContext.enc.bitRate, m_vaContext.enc.frameRate, 1);
        seqParam->intra_period = m_vaContext.enc.intraPeriod;  // I帧
        seqParam->bits_per_second = (m_vaContext.enc.bitRate > 0) ? m_vaContext.enc.bitRate : 0;
        seqParam->intra_idr_period = seqParam->intra_period;    // IDR帧
        seqParam->time_scale = m_vaContext.enc.frameRate * 2;  // n_frames drop method type 2
        VAEncPictureParameterBufferH264 *picParam = &(m_vaContext.enc.picParamH264);
        picParam->pic_fields.bits.entropy_coding_mode_flag =
            (m_vaContext.enc.profile == VAProfileH264ConstrainedBaseline) ? ENTROPY_MODE_CAVLC : ENTROPY_MODE_CABAC;
        picParam->pic_fields.bits.transform_8x8_mode_flag = ((m_vaContext.enc.constraintSetFlag & 0x7) == 0) ? 1 : 0;
    } else {
        m_vaContext.enc.profile = VAProfileHEVCMain;
        VAEncSequenceParameterBufferHEVC *seqParam = &(m_vaContext.enc.seqParamH265);
        seqParam->intra_period = m_vaContext.enc.intraPeriod;  // I帧
        seqParam->bits_per_second = (m_vaContext.enc.bitRate > 0) ? m_vaContext.enc.bitRate : 0;
        seqParam->intra_idr_period = seqParam->intra_period;    // IDR帧
        m_vaBitstream.FillBitstreamForSPS(
            m_vaContext.enc.seqParamH265, m_vaContext.enc.pictureWidth, m_vaContext.enc.pictureHeight);
    }
    m_needKeyFrame = true;
    return;
}