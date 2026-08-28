#ifndef IFBC_API_H_
#define IFBC_API_H_

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    IFBC_ERR_NONE            = 0,
    IFBC_ERR_COMMON          = 1,
    IFBC_ERR_INVALID_PARAM   = 2,
    IFBC_ERR_OUT_OF_MEMORY   = 3,
    IFBC_ERR_CREATE_IMAGE_FD = 4  // create image failed with fd
} ifbc_result_e;

typedef enum {
    FORMAT_XRGB8888    = 0x00,  // dma buff fd
    FORMAT_ARGB8888    = 0x01,
    FORMAT_XBGR8888    = 0x02,
    FORMAT_ABGR8888    = 0x03,
    FORMAT_BGRX8888    = 0x04,
    FORMAT_BGRA8888    = 0x05,
    FORMAT_RGBX8888    = 0x06,
    FORMAT_RGBA8888    = 0x07,
    FORMAT_RGB565      = 0x08,
    FORMAT_ABGR2101010 = 0x09,

    FORMAT_NV12   = 0x10,  // YUV420 2Plane(YUV420SP-NV12)
    FORMAT_NV21   = 0x11,  // YVU420 2Plane(YUV420SP-NV12)
    FORMAT_YUV420 = 0x12,  // YUV420 3Plane(YUV420P-YU12/I420)
    FORMAT_YVU420 = 0x13,  // YVU420 3Plane(YUV420P-YV12)

    FORMAT_YUYV = 0x14,  // YUV422 1Plane
    FORMAT_YVYU = 0x15,  // YVU422 1Plane
    FORMAT_UYVY = 0x16,  // YUV422 1Plane  only cpu support
    // FORMAT_VYUY   = 0x17,  // YVU422 1Plane
    FORMAT_NV16   = 0x18,  // YUV422 2Plane(YUV422SP-NV16)
    FORMAT_NV61   = 0x19,  // YUV422 2Plane(YUV422SP-NV61)
    FORMAT_YUV422 = 0x1A,  // YUV422 3Plane(YUV422P) only cpu support
    // FORMAT_YVU422 = 0x1B,  // YVU422 3Plane

    // FORMAT_NV24   = 0x1C, //YUV444 2Plane(YUV444SP-NV24)
    // FORMAT_NV42   = 0x1D, //YUV444 2Plane(YUV444SP-NV42)
    FORMAT_YUV444 = 0x1E,  // YUV444 3Plane(YUV444P)
    // FORMAT_YVU444 = 0x1F, //YVU444 3Plane

    // FORMAT_TEX_XRGB8888 = 0x30,  // texture id(GL_TEXTURE_EXTERNAL_OES)
    // FORMAT_TEX_ARGB8888 = 0x31,
    // FORMAT_TEX_XBGR8888 = 0x32,
    // FORMAT_TEX_ABGR8888 = 0x33,

    // FORMAT_2D_TEX_XRGB8888 = 0x40,  // texture id(GL_TEXTURE_2D)
    // FORMAT_2D_TEX_ARGB8888 = 0x41,
    // FORMAT_2D_TEX_XBGR8888 = 0x42,
    // FORMAT_2D_TEX_ABGR8888 = 0x43,

    // FORMAT_MEM_XRGB8888 = 0x50,  // memory point
    // FORMAT_MEM_ARGB8888 = 0x51,
    // FORMAT_MEM_XBGR8888 = 0x52,
    // FORMAT_MEM_ABGR8888 = 0x53,

    FORMAT_ANATIVE_XRGB8888 = 0x60,  // ANativeWindowBuffer
    FORMAT_ANATIVE_ARGB8888 = 0x61,
    FORMAT_ANATIVE_XBGR8888 = 0x62,
    FORMAT_ANATIVE_ABGR8888 = 0x63,
    FORMAT_ANATIVE_PVRIC_NV12 = 0x64,

    FORMAT_INVALID = 0xFF
} ifbc_pixel_format_e;

typedef void* ifbc_capture_t;

typedef struct {
    int                 x;  // the upper-left vertex X coordinates of the specified area
    int                 y;  // the upper-left vertex X coordinates of the specified area
    int                 w;  // the width of the specified area
    int                 h;  // the height of the specified area
    int                 width;
    int                 height;
    unsigned int        stride;  // android requires 32 pixel alignment.
    unsigned int        stride_h;
    ifbc_pixel_format_e pixel_format;
    int                 fd;
    void*               buf;
} ifbc_frame_t;

typedef struct {
    int           version;
    int           num;    // frame num
    ifbc_frame_t* frame;  // frame(1(screen) + n(overlayer))
    void*         res;
} ifbc_frame_desc_t;

typedef enum {
    ROTATION_0 = 0,
    ROTATION_90,
    ROTATION_180,
    ROTATION_270,
} ifbc_screen_rotation_e;

typedef struct {
    int                     width;
    int                     height;
    int                     fps;
    int                     fd;  // device fd;
    unsigned int            plane_id;
    ifbc_screen_rotation_e  orientation;
} ifbc_screen_info_t;

typedef enum {
    CAPTURE_FEATURE_ID_MODE = 0,    // the default is FEATURE_MODULE_VD
    CAPTURE_FEATURE_ID_GRAB,        // the default is FEATURE_GRAB_ASYNC
    CAPTURE_FEATURE_ID_ORIENTATION, // the default is FEATURE_ORIENTATION_MAIN

    CAPTURE_FEATURE_ID_MAX,
    CAPTURE_FEATURE_ID_NONE = UINT32_MAX
} ifbc_capture_feature;

// CAPTURE_FEATURE_ID_MODE
typedef enum {
    FEATURE_MODE_VD = 0,   // use virtual display to capture
    FEATURE_MODE_HWC,      // use hwc to capture
    FEATURE_MODE_DRM,      // use drm to capture
    FEATURE_MODE_VD_PVRIC  // use virtual display to capture PVRIC
} ifbc_capture_feature_module;

// CAPTURE_FEATURE_ID_GRAB
typedef enum {
    FEATURE_GRAB_ASYNC = 1,  // async capture and need call ifbc_release_frame or
                             // ifbc_release_frame_by_fd to release frame
} ifbc_capture_feature_grab;

// CAPTURE_FEATURE_ID_ORIENTATION
typedef enum {
    FEATURE_ORIENTATION_MAIN = 0,  // eg.main display:1280x720, virtual display:1280x720; main display:720x1280, virtual display:720x1280
    FEATURE_ORIENTATION_VERTICAL   // eg.main display:1280x720, virtual display:720x1280; main display:720x1280, virtual display:720x1280
} ifbc_capture_feature_orientation;

/*!
 * \brief Init capture.
 * \param[in] attribs: attribs NULL for default
 *  example:
 *      uint32_t attribs[]={
 *           CAPTURE_FEATURE_ID_MODE, FEATURE_MODE_VD,
 *           CAPTURE_FEATURE_ID_GRAB, FEATURE_GRAB_ASYNC,
 *           CAPTURE_FEATURE_ID_NONE
 *       };
 * \return NULL:fail, others \see ifbc_capture_t
 */
ifbc_capture_t ifbc_capture_init(const uint32_t* attribs);

/*!
 * \brief Deinit capture.
 * \param[in] handle: \see ifbc_capture_t
 */
void ifbc_capture_deinit(ifbc_capture_t handle);

/*!
 * \brief Grab frame.
 * \param[in] handle: \see ifbc_capture_t
 * \param[out] frame: the data of captured frame \see ifbc_frame_desc_t
 * \param[in] timeout_usec: only android is supoorted
 * \return \see ifbc_result_e
 */
int ifbc_capture_grab(ifbc_capture_t handle, ifbc_frame_desc_t* frame, uint32_t timeout_usec);

/*!
 * \brief Release frame to video mem
 * \param[in] handle: \see ifbc_capture_t
 * \param[in] desc_frame: the data of captured frame
 * \return \see ifbc_result_e
 */
int ifbc_release_frame(ifbc_capture_t handle, const ifbc_frame_desc_t* desc_frame);

#ifdef __cplusplus
}
#endif

#endif  // IFBC_API_H_
