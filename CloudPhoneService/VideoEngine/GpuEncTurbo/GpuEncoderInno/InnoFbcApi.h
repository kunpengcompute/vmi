/*
 * 版权所有 (c) 华为技术有限公司 2023-2023
 * 功能描述：接入芯动FBC库头文件
 */
#ifndef INNO_FBC_API_H
#define INNO_FBC_API_H

#include <cstdint>

namespace Vmi {
namespace GpuEncoder {
enum IfbcResult : uint32_t {
    RESULT_SUCCESS = 0,
    RESULT_COMMON = 1,
    RESULT_INVALID_PARAM = 2,
    RESULT_OUT_OF_MEMORY = 3,
    RESULT_CREATE_IMAGE_FD = 4,
};

enum IfbcPixelFormat : uint32_t {
    FORMAT_XRGB8888 = 0x00,
    FORMAT_ARGB8888 = 0x01,

    FORMAT_NV12 = 0x10,
    FORMAT_NV21 = 0x11,
    FORMAT_YUV420 = 0x20,
    FORMAT_YVU420 = 0x21,

    FORMAT_TEX_XRGB8888 = 0x30,
    FORMAT_TEX_ARGB8888 = 0x31,

    FORMAT_2D_TEX_XRGB8888 = 0x40,
    FORMAT_2D_TEX_ARGB8888 = 0x41,

    FORMAT_MEM_XRGB8888 = 0x50,
    FORMAT_MEM_ARGB8888 = 0x51,

    FORMAT_ANATIVE_XRGB8888 = 0x60,
    FORMAT_ANATIVE_ARGB8888 = 0x61,

    FORMAT_INVALID = 0xFF,
};

struct IfbcFrame {
    int x;
    int y;
    int w;
    int h;
    int width;
    int height;
    int stride;
    IfbcPixelFormat pixelFormat;
    int fd;
    void* buf;
};

typedef enum {
    IENC_FORMAT_NV12 = 0,

    IENC_FORMAT_PVRIC_ARGB_8X8 = 0x10,
    IENC_FORMAT_PVRIC_NV12 = 0x20,
    IENC_FORMAT_MAX,
} ienc_fb_format_e;

typedef enum {
    IENC_AVC_BASELINE_PROFILE = 0x01,
    IENC_AVC_MAIN_PROFILE = 0x02,
    IENC_AVC_HIGH_PROFILE = 0x03,

    IENC_HEVC_MAIN_PROFILE = 0x10,
    IENC_HEVC_MAIN10_PROFILE = 0x11,

    IENC_PROFILE_MAX,
} ienc_profile_e;

typedef enum {
    IENC_CSC_MODE_NONE = 0,
    IENC_CSC_MODE_BT601 = 1,
    IENC_CSC_MODE_BT709 = 2,
    IENC_CSC_MODE_BT2020 = 3,
} ienc_csc_mode_e;

typedef enum {
    IENC_CSC_RANGE_LIMIT = 0,
	IENC_CSC_RANGE_FULL = 1,
} ienc_csc_range_e;

typedef struct {
    ienc_fb_format_e    src_format;      //源格式
    ienc_profile_e      profile;         //profile

    uint32_t            pic_width;       //编码宽
    uint32_t            pic_height;      //编码高

    uint32_t            b_frame_num;     //B帧个数(0,1,3)
    ienc_csc_mode_e     csc_mode;
    ienc_csc_range_e    csc_range;
} ienc_enc_attr_t;

typedef enum {
    IENC_RC_MODE_VBR = 1,  // 动态码率
    IENC_RC_MODE_CBR = 2,  // 固定码率
    IENC_RC_MODE_CQP = 3,  // 固定质量
} ienc_rc_mode_e;


typedef enum {
    IENC_ERR_NONE          = 0,
    IENC_ERR_INVALID_PARAM = -1,
    IENC_ERR_OPEN_ENCODER  = -2,
    IENC_ERR_CREATE_BUFFER = -3,
    IENC_ERR_ENCODE        = -4,
    IENC_ERR_UNKNOWN       = -5,
} ienc_err_e;

typedef struct {
    uint32_t            intra_period;           //i帧间隔(I帧和IDR帧重叠时,编码为IDR帧)
    uint32_t            intra_idr_period;       //idr帧间隔
    uint32_t            src_frame_rate;         //帧率
    uint32_t            max_bit_rate;           //最大码率
    uint32_t            init_qp;                //初始qp(编码第一帧的QP)
    uint32_t            min_qp;                 //最小qp
    uint32_t            max_qp;                 //最大qp
} ienc_vbr_attr_t;

typedef struct {
    uint32_t            intra_period;           //i帧间隔(I帧和IDR帧重叠时,编码为IDR帧)
    uint32_t            intra_idr_period;       //idr帧间隔
    uint32_t            src_frame_rate;         //帧率
    uint32_t            bit_rate;               //目标码率
    uint32_t            init_qp;                //初始qp(编码第一帧的QP)
} ienc_cbr_attr_t;

typedef struct {
    uint32_t            intra_period;           //i帧间隔(I帧和IDR帧重叠时,编码为IDR帧)
    uint32_t            intra_idr_period;       //idr帧间隔
    uint32_t            src_frame_rate;         //帧率
    uint32_t            qp;                     //qp
} ienc_cqp_attr_t;


typedef struct {
    ienc_rc_mode_e      rc_mode;
    union
    {
        ienc_vbr_attr_t     vbr_attr;
        ienc_cbr_attr_t     cbr_attr;
        ienc_cqp_attr_t     cqp_attr;
    };
} ienc_rc_attr_t;

typedef struct {
    ienc_enc_attr_t     enc_attr;       //编码属性
    ienc_rc_attr_t      rc_attr;        //码率属性
} ienc_attr_t;

typedef struct
{
    uint32_t    width;      //宽
    uint32_t    height;     //高

    int32_t     fd;         //句柄
    uint32_t    stride;     //步长
}ienc_frame_t;

typedef struct
{
    uint64_t    phy_addr;   //物理地址
    uint8_t*    addr;       //虚拟地址
    uint32_t    offset;     //偏移
    uint32_t    len;        //长度
}ienc_pack_t;

typedef struct
{
    ienc_pack_t*    pack;           //包
    uint32_t        pack_count;     //包数
}ienc_stream_t;

using InnoConvertHandle = void *;
using EglDisplpay = void *;
using EglContext = void *;
struct EglInfo {
    EglDisplpay eglDisplay;
    EglContext eglContext;
};

using EglInfoT = EglInfo *;
using IfbcFrameT = IfbcFrame *;
}
}

#endif
