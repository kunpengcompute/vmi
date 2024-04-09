/*
 * Copyright (c) 2007-2009 Intel Corporation. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
 * IN NO EVENT SHALL INTEL AND/OR ITS SUPPLIERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
/*
 * Video Acceleration (VA) API Specification
 *
 * Rev. 0.30
 * <jonathan.bian@intel.com>
 *
 * Revision History:
 * rev 0.10 (12/10/2006 Jonathan Bian) - Initial draft
 * rev 0.11 (12/15/2006 Jonathan Bian) - Fixed some errors
 * rev 0.12 (02/05/2007 Jonathan Bian) - Added VC-1 data structures for slice level decode
 * rev 0.13 (02/28/2007 Jonathan Bian) - Added GetDisplay()
 * rev 0.14 (04/13/2007 Jonathan Bian) - Fixed MPEG-2 PictureParameter structure, cleaned up a few funcs.
 * rev 0.15 (04/20/2007 Jonathan Bian) - Overhauled buffer management
 * rev 0.16 (05/02/2007 Jonathan Bian) - Added error codes and fixed some issues with configuration
 * rev 0.17 (05/07/2007 Jonathan Bian) - Added H.264/AVC data structures for slice level decode.
 * rev 0.18 (05/14/2007 Jonathan Bian) - Added data structures for MPEG-4 slice level decode 
 *                                       and MPEG-2 motion compensation.
 * rev 0.19 (08/06/2007 Jonathan Bian) - Removed extra type for bitplane data.
 * rev 0.20 (08/08/2007 Jonathan Bian) - Added missing fields to VC-1 PictureParameter structure.
 * rev 0.21 (08/20/2007 Jonathan Bian) - Added image and subpicture support.
 * rev 0.22 (08/27/2007 Jonathan Bian) - Added support for chroma-keying and global alpha.
 * rev 0.23 (09/11/2007 Jonathan Bian) - Fixed some issues with images and subpictures.
 * rev 0.24 (09/18/2007 Jonathan Bian) - Added display attributes.
 * rev 0.25 (10/18/2007 Jonathan Bian) - Changed to use IDs only for some types.
 * rev 0.26 (11/07/2007 Waldo Bastian) - Change vastCreateBuffer semantics
 * rev 0.27 (11/19/2007 Matt Sottek)   - Added DeriveImage
 * rev 0.28 (12/06/2007 Jonathan Bian) - Added new versions of PutImage and AssociateSubpicture 
 *                                       to enable scaling
 * rev 0.29 (02/07/2008 Jonathan Bian) - VC1 parameter fixes,
 *                                       added VAST_STATUS_ERROR_RESOLUTION_NOT_SUPPORTED
 * rev 0.30 (03/01/2009 Jonathan Bian) - Added encoding support for H.264 BP and MPEG-4 SP and fixes
 *                                       for ISO C conformance.
 * rev 0.31 (09/02/2009 Gwenole Beauchesne) - VC-1/H264 fields change for VDPAU and XvBA backend
 *                                       Application needs to relink with the new library.
 *
 * rev 0.31.1 (03/29/2009)              - Data structure for JPEG encode
 * rev 0.31.2 (01/13/2011 Anthony Pabon)- Added a flag to indicate Subpicture coordinates are screen
 *                                        screen relative rather than source video relative.
 * rev 0.32.0 (01/13/2011 Xiang Haihao) - Add profile into VASTPictureParameterBufferVC1
 *                                        update VAAPI to 0.32.0
 *
 * Acknowledgements:
 *  Some concepts borrowed from XvMC and XvImage.
 *  Waldo Bastian (Intel), Matt Sottek (Intel),  Austin Yuan (Intel), and Gwenole Beauchesne (SDS)
 *  contributed to various aspects of the API.
 */

/**
 * \file va.h
 * \brief The Core API
 *
 * This file contains the \ref api_core "Core API".
 */

#ifndef _VASTVA_H_
#define _VASTVA_H_

#include <stddef.h>
#include <stdint.h>
#include <vastva/va_version.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __GNUC__
#define va_deprecated __attribute__((deprecated))
#if __GNUC__ >= 6
#define va_deprecated_enum va_deprecated
#else
#define va_deprecated_enum
#endif
#else
#define va_deprecated
#define va_deprecated_enum
#endif

/**
 * \mainpage Video Acceleration (VA) API
 *
 * \section intro Introduction
 *
 * The main motivation for VA-API (Video Acceleration API) is to
 * enable hardware accelerated video decode and encode at various
 * entry-points (VLD, IDCT, Motion Compensation etc.) for the
 * prevailing coding standards today (MPEG-2, MPEG-4 ASP/H.263, MPEG-4
 * AVC/H.264, VC-1/VMW3, and JPEG, HEVC/H265, VP8, VP9) and video pre/post
 * processing
 *
 * VA-API is split into several modules:
 * - \ref api_core
 * - Encoder (H264, HEVC, JPEG, MPEG2, VP8, VP9)
 * 	- \ref api_enc_h264
 * 	- \ref api_enc_hevc
 * 	- \ref api_enc_jpeg
 * 	- \ref api_enc_mpeg2
 * 	- \ref api_enc_vp8
 * 	- \ref api_enc_vp9
 * - Decoder (HEVC, JPEG, VP8, VP9)
 *      - \ref api_dec_hevc
 *      - \ref api_dec_jpeg
 *      - \ref api_dec_vp8
 *      - \ref api_dec_vp9
 * - \ref api_vpp
 * - FEI (H264, HEVC)
 * 	- \ref api_fei
 * 	- \ref api_fei_h264
 * 	- \ref api_fei_hevc
 */

/**
 * \defgroup api_core Core API
 *
 * @{
 */

/**
Overview 

The VA API is intended to provide an interface between a video decode/encode/processing
application (client) and a hardware accelerator (server), to off-load 
video decode/encode/processing operations from the host to the hardware accelerator at various
entry-points.

The basic operation steps are:

- Negotiate a mutually acceptable configuration with the server to lock
  down profile, entrypoints, and other attributes that will not change on 
  a frame-by-frame basis.
- Create a video decode, encode or processing context which represents a
  "virtualized" hardware device
- Get and fill the render buffers with the corresponding data (depending on
  profiles and entrypoints)
- Pass the render buffers to the server to handle the current frame

Initialization & Configuration Management 

- Find out supported profiles
- Find out entrypoints for a given profile
- Find out configuration attributes for a given profile/entrypoint pair
- Create a configuration for use by the application

*/

#define MAX_SEI_COUNT  8

typedef void* VASTDisplay;	/* window system dependent */

typedef int VASTStatus;	/** Return status type from functions */
/** Values for the return status */
#define VAST_STATUS_SUCCESS			0x00000000
#define VAST_STATUS_ERROR_OPERATION_FAILED	0x00000001
#define VAST_STATUS_ERROR_ALLOCATION_FAILED	0x00000002
#define VAST_STATUS_ERROR_INVALID_DISPLAY		0x00000003
#define VAST_STATUS_ERROR_INVALID_CONFIG		0x00000004
#define VAST_STATUS_ERROR_INVALID_CONTEXT		0x00000005
#define VAST_STATUS_ERROR_INVALID_SURFACE		0x00000006
#define VAST_STATUS_ERROR_INVALID_BUFFER		0x00000007
#define VAST_STATUS_ERROR_INVALID_IMAGE		0x00000008
#define VAST_STATUS_ERROR_INVALID_SUBPICTURE	0x00000009
#define VAST_STATUS_ERROR_ATTR_NOT_SUPPORTED	0x0000000a
#define VAST_STATUS_ERROR_MAX_NUM_EXCEEDED	0x0000000b
#define VAST_STATUS_ERROR_UNSUPPORTED_PROFILE	0x0000000c
#define VAST_STATUS_ERROR_UNSUPPORTED_ENTRYPOINT	0x0000000d
#define VAST_STATUS_ERROR_UNSUPPORTED_RT_FORMAT	0x0000000e
#define VAST_STATUS_ERROR_UNSUPPORTED_BUFFERTYPE	0x0000000f
#define VAST_STATUS_ERROR_SURFACE_BUSY		0x00000010
#define VAST_STATUS_ERROR_FLAG_NOT_SUPPORTED      0x00000011
#define VAST_STATUS_ERROR_INVALID_PARAMETER	0x00000012
#define VAST_STATUS_ERROR_RESOLUTION_NOT_SUPPORTED 0x00000013
#define VAST_STATUS_ERROR_UNIMPLEMENTED           0x00000014
#define VAST_STATUS_ERROR_SURFACE_IN_DISPLAYING   0x00000015
#define VAST_STATUS_ERROR_INVALID_IMAGE_FORMAT    0x00000016
#define VAST_STATUS_ERROR_DECODING_ERROR          0x00000017
#define VAST_STATUS_ERROR_ENCODING_ERROR          0x00000018
/**
 * \brief An invalid/unsupported value was supplied.
 *
 * This is a catch-all error code for invalid or unsupported values.
 * e.g. value exceeding the valid range, invalid type in the context
 * of generic attribute values.
 */
#define VAST_STATUS_ERROR_INVALID_VALUE           0x00000019
/** \brief An unsupported filter was supplied. */
#define VAST_STATUS_ERROR_UNSUPPORTED_FILTER      0x00000020
/** \brief An invalid filter chain was supplied. */
#define VAST_STATUS_ERROR_INVALID_FILTER_CHAIN    0x00000021
/** \brief Indicate HW busy (e.g. run multiple encoding simultaneously). */
#define VAST_STATUS_ERROR_HW_BUSY	                0x00000022
/** \brief An unsupported memory type was supplied. */
#define VAST_STATUS_ERROR_UNSUPPORTED_MEMORY_TYPE 0x00000024
#define VAST_STATUS_ERROR_INSTANCE_MISMATCH  0x00000025
#define VAST_STATUS_ERROR_UNKNOWN			0xFFFFFFFF

/** De-interlacing flags for vastPutSurface() */
#define VAST_FRAME_PICTURE        0x00000000 
#define VAST_TOP_FIELD            0x00000001
#define VAST_BOTTOM_FIELD         0x00000002

/**
 * Enabled the positioning/cropping/blending feature:
 * 1, specify the video playback position in the isurface
 * 2, specify the cropping info for video playback
 * 3, encoded video will blend with background color
 */
#define VAST_ENABLE_BLEND         0x00000004 /* video area blend with the constant color */ 
    
/**
 * Clears the drawable with background color.
 * for hardware overlay based implementation this flag
 * can be used to turn off the overlay
 */
#define VAST_CLEAR_DRAWABLE       0x00000008

/** Color space conversion flags for vastPutSurface() */
#define VAST_SRC_COLOR_MASK       0x000000f0
#define VAST_SRC_BT601            0x00000010
#define VAST_SRC_BT709            0x00000020
#define VAST_SRC_SMPTE_240        0x00000040

/** Scaling flags for vastPutSurface() */
#define VAST_FILTER_SCALING_DEFAULT       0x00000000
#define VAST_FILTER_SCALING_FAST          0x00000100
#define VAST_FILTER_SCALING_HQ            0x00000200
#define VAST_FILTER_SCALING_NL_ANAMORPHIC 0x00000300
#define VAST_FILTER_SCALING_MASK          0x00000f00

/** Padding size in 4-bytes */
#define VAST_PADDING_LOW          4
#define VAST_PADDING_MEDIUM       8
#define VAST_PADDING_HIGH         16
#define VAST_PADDING_LARGE        32

/**
 * Returns a short english description of error_status
 */
const char *vastErrorStr(VASTStatus error_status);

typedef struct _VASTRectangle
{
    int16_t x;
    int16_t y;
    uint16_t width;
    uint16_t height;
} VASTRectangle;

/** \brief Generic motion vector data structure. */
typedef struct _VASTMotionVector {
    /** \mv0[0]: horizontal motion vector for past reference */
    /** \mv0[1]: vertical motion vector for past reference */
    /** \mv1[0]: horizontal motion vector for future reference */
    /** \mv1[1]: vertical motion vector for future reference */
    int16_t  mv0[2];  /* past reference */
    int16_t  mv1[2];  /* future reference */
} VASTMotionVector;

/** Type of a message callback, used for both error and info log. */
typedef void (*VASTMessageCallback)(void *user_context, const char *message);

/**
 * Set the callback for error messages, or NULL for no logging.
 * Returns the previous one, or NULL if it was disabled.
 */
VASTMessageCallback vastSetErrorCallback(VASTDisplay dpy, VASTMessageCallback callback, void *user_context);

/**
 * Set the callback for info messages, or NULL for no logging.
 * Returns the previous one, or NULL if it was disabled.
 */
VASTMessageCallback vastSetInfoCallback(VASTDisplay dpy, VASTMessageCallback callback, void *user_context);

/**
 * Initialization:
 * A display must be obtained by calling vastGetDisplay() before calling
 * vastInitialize() and other functions. This connects the API to the 
 * native window system.
 * For X Windows, native_dpy would be from XOpenDisplay()
 */
typedef void* VASTNativeDisplay;	/* window system dependent */

int vastDisplayIsValid(VASTDisplay dpy);

/**
 *  Set the override driver name instead of queried driver driver.
 */
VASTStatus vastSetDriverName(VASTDisplay dpy,
                         char *driver_name
);

/**
 * Initialize the library 
 */
VASTStatus vastInitialize (
    VASTDisplay dpy,
    int *major_version,	 /* out */
    int *minor_version 	 /* out */
);

/**
 * After this call, all library internal resources will be cleaned up
 */ 
VASTStatus vastTerminate (
    VASTDisplay dpy
);

/**
 * vastQueryVendorString returns a pointer to a zero-terminated string
 * describing some aspects of the VA implemenation on a specific    
 * hardware accelerator. The format of the returned string is vendor
 * specific and at the discretion of the implementer.
 * e.g. for the Intel GMA500 implementation, an example would be:
 * "Intel GMA500 - 2.0.0.32L.0005"
 */
const char *vastQueryVendorString (
    VASTDisplay dpy
);

typedef int (*VAPrivFunc)(void);

/**
 * Return a function pointer given a function name in the library.
 * This allows private interfaces into the library
 */ 
VAPrivFunc vastGetLibFunc (
    VASTDisplay dpy,
    const char *func
);

/** Currently defined profiles */
typedef enum
{
    /** \brief Profile ID used for video processing. */
    VASTProfileNone                       = -1,
    VASTProfileMPEG2Simple		= 0,
    VASTProfileMPEG2Main			= 1,
    VASTProfileMPEG4Simple		= 2,
    VASTProfileMPEG4AdvancedSimple	= 3,
    VASTProfileMPEG4Main			= 4,
    VASTProfileH264Baseline = 5,
    VASTProfileH264Main			= 6,
    VASTProfileH264High			= 7,
    VASTProfileVC1Simple			= 8,
    VASTProfileVC1Main			= 9,
    VASTProfileVC1Advanced		= 10,
    VASTProfileH263Baseline		= 11,
    VASTProfileJPEGBaseline               = 12,
    VASTProfileH264ConstrainedBaseline    = 13,
    VASTProfileVP8Version0_3              = 14,
    VASTProfileH264MultiviewHigh          = 15,
    VASTProfileH264StereoHigh             = 16,
    VASTProfileHEVCMain                   = 17,
    VASTProfileHEVCMain10                 = 18,
    VASTProfileVP9Profile0                = 19,
    VASTProfileVP9Profile1                = 20,
    VASTProfileVP9Profile2                = 21,
    VASTProfileVP9Profile3                = 22,
    VASTProfileAV1Main                    = 23
} VASTProfile;

/**
 *  Currently defined entrypoints 
 */
typedef enum
{
    VASTEntrypointVLD		= 1,
    VASTEntrypointIZZ		= 2,
    VASTEntrypointIDCT		= 3,
    VASTEntrypointMoComp		= 4,
    VASTEntrypointDeblocking	= 5,
    VASTEntrypointEncSlice	= 6,	/* slice level encode */
    VASTEntrypointEncPicture 	= 7,	/* pictuer encode, JPEG, etc */
    /*
     * For an implementation that supports a low power/high performance variant
     * for slice level encode, it can choose to expose the
     * VASTEntrypointEncSliceLP entrypoint. Certain encoding tools may not be
     * available with this entrypoint (e.g. interlace, MBAFF) and the
     * application can query the encoding configuration attributes to find
     * out more details if this entrypoint is supported.
     */
    VASTEntrypointEncSliceLP 	= 8,
    VASTEntrypointVideoProc       = 10,   /**< Video pre/post-processing. */
    /**
     * \brief VASTEntrypointFEI
     *
     * The purpose of FEI (Flexible Encoding Infrastructure) is to allow applications to
     * have more controls and trade off quality for speed with their own IPs.
     * The application can optionally provide input to ENC for extra encode control
     * and get the output from ENC. Application can chose to modify the ENC
     * output/PAK input during encoding, but the performance impact is significant.
     *
     * On top of the existing buffers for normal encode, there will be
     * one extra input buffer (VAEncMiscParameterFEIFrameControl) and
     * three extra output buffers (VASTEncFEIMVBufferType, VAEncFEIMBModeBufferType
     * and VASTEncFEIDistortionBufferType) for VASTEntrypointFEI entry function.
     * If separate PAK is set, two extra input buffers
     * (VASTEncFEIMVBufferType, VAEncFEIMBModeBufferType) are needed for PAK input.
     **/
    VASTEntrypointFEI         = 11,
    /**
     * \brief VASTEntrypointStats
     *
     * A pre-processing function for getting some statistics and motion vectors is added,
     * and some extra controls for Encode pipeline are provided. The application can
     * optionally call the statistics function to get motion vectors and statistics like
     * variances, distortions before calling Encode function via this entry point.
     *
     * Checking whether Statistics is supported can be performed with vastQueryConfigEntrypoints().
     * If Statistics entry point is supported, then the list of returned entry-points will
     * include #VASTEntrypointStats. Supported pixel format, maximum resolution and statistics
     * specific attributes can be obtained via normal attribute query. One input buffer
     * (VASTStatsStatisticsParameterBufferType) and one or two output buffers
     * (VASTStatsStatisticsBufferType, VASTStatsStatisticsBottomFieldBufferType (for interlace only)
     * and VASTStatsMVBufferType) are needed for this entry point.
     **/
    VASTEntrypointStats       = 12,
} VASTEntrypoint;

/** Currently defined configuration attribute types */
typedef enum
{
    VASTConfigAttribRTFormat		= 0,
    VASTConfigAttribSpatialResidual	= 1,
    VASTConfigAttribSpatialClipping	= 2,
    VASTConfigAttribIntraResidual		= 3,
    VASTConfigAttribEncryption		= 4,
    VASTConfigAttribRateControl		= 5,

    /** @name Attributes for decoding */
    /**@{*/
    /**
     * \brief Slice Decoding mode. Read/write.
     *
     * This attribute determines what mode the driver supports for slice
     * decoding, through vastGetConfigAttributes(); and what mode the user
     * will be providing to the driver, through vastCreateConfig(), if the
     * driver supports those. If this attribute is not set by the user then
     * it is assumed that VAST_DEC_SLICE_MODE_NORMAL mode is used.
     *
     * See \c VA_DEC_SLICE_MODE_xxx for the list of slice decoding modes.
     */
    VASTConfigAttribDecSliceMode		= 6,
   /**
     * \brief JPEG decoding attribute. Read-only.
     *
     * This attribute exposes a number of capabilities of the underlying
     * JPEG implementation. The attribute value is partitioned into fields as defined in the
     * VASTConfigAttribValDecJPEG union.
     */
    VASTConfigAttribDecJPEG             = 7,
    /**
     * \brief Decode processing support. Read/write.
     *
     * This attribute determines if the driver supports video processing
     * with decoding using the decoding context in a single call, through
     * vastGetConfigAttributes(); and if the user may use this feature,
     * through vastCreateConfig(), if the driver supports the user scenario.
     * The user will essentially create a regular decode VAContext.  Therefore,
     * the parameters of vastCreateContext() such as picture_width, picture_height
     * and render_targets are in relation to the decode output parameters
     * (not processing output parameters) as normal.
     * If this attribute is not set by the user then it is assumed that no
     * extra processing is done after decoding for this decode context.
     *
     * Since essentially the application is creating a decoder config and context,
     * all function calls that take in the config (e.g. vastQuerySurfaceAttributes())
     * or context are in relation to the decoder, except those video processing
     * function specified in the next paragraph.
     *
     * Once the decode config and context are created, the user must further
     * query the supported processing filters using vastQueryVideoProcFilters(),
     * vastQueryVideoProcFilterCaps(), vastQueryVideoProcPipelineCaps() by specifying
     * the created decode context.  The user must provide processing information
     * and extra processing output surfaces as "additional_outputs" to the driver
     * through VASTProcPipelineParameterBufferType.  The render_target specified
     * at vastBeginPicture() time refers to the decode output surface.  The
     * target surface for the output of processing needs to be a different
     * surface since the decode process requires the original reconstructed buffer.
     * The “surface” member of VASTProcPipelineParameterBuffer should be set to the
     * same as “render_target” set in vastBeginPicture(), but the driver may choose
     * to ignore this parameter.
     */
    VASTConfigAttribDecProcessing		= 8,
    /** @name Attributes for encoding */
    /**@{*/
    /**
     * \brief Packed headers mode. Read/write.
     *
     * This attribute determines what packed headers the driver supports,
     * through vastGetConfigAttributes(); and what packed headers the user
     * will be providing to the driver, through vastCreateConfig(), if the
     * driver supports those.
     *
     * See \c VA_ENC_PACKED_HEADER_xxx for the list of packed headers.
     */
    VASTConfigAttribEncPackedHeaders      = 10,
    /**
     * \brief Interlaced mode. Read/write.
     *
     * This attribute determines what kind of interlaced encoding mode
     * the driver supports.
     *
     * See \c VA_ENC_INTERLACED_xxx for the list of interlaced modes.
     */
    VASTConfigAttribEncInterlaced         = 11,
    /**
     * \brief Maximum number of reference frames. Read-only.
     *
     * This attribute determines the maximum number of reference
     * frames supported for encoding.
     *
     * Note: for H.264 encoding, the value represents the maximum number
     * of reference frames for both the reference picture list 0 (bottom
     * 16 bits) and the reference picture list 1 (top 16 bits).
     */
    VASTConfigAttribEncMaxRefFrames       = 13,
    /**
     * \brief Maximum number of slices per frame. Read-only.
     *
     * This attribute determines the maximum number of slices the
     * driver can support to encode a single frame.
     */
    VASTConfigAttribEncMaxSlices          = 14,
    /**
     * \brief Slice structure. Read-only.
     *
     * This attribute determines slice structures supported by the
     * driver for encoding. This attribute is a hint to the user so
     * that he can choose a suitable surface size and how to arrange
     * the encoding process of multiple slices per frame.
     *
     * More specifically, for H.264 encoding, this attribute
     * determines the range of accepted values to
     * VASTEncSliceParameterBufferH264::macroblock_address and
     * VASTEncSliceParameterBufferH264::num_macroblocks.
     *
     * See \c VA_ENC_SLICE_STRUCTURE_xxx for the supported slice
     * structure types.
     */
    VASTConfigAttribEncSliceStructure     = 15,
    /**
     * \brief Macroblock information. Read-only.
     *
     * This attribute determines whether the driver supports extra
     * encoding information per-macroblock. e.g. QP.
     *
     * More specifically, for H.264 encoding, if the driver returns a non-zero
     * value for this attribute, this means the application can create
     * additional #VASTEncMacroblockParameterBufferH264 buffers referenced
     * through VASTEncSliceParameterBufferH264::macroblock_info.
     */
    VASTConfigAttribEncMacroblockInfo     = 16,
    /**
     * \brief Maximum picture width. Read-only.
     *
     * This attribute determines the maximum picture width the driver supports
     * for a given configuration.
     */
    VASTConfigAttribMaxPictureWidth     = 18,
    /**
     * \brief Maximum picture height. Read-only.
     *
     * This attribute determines the maximum picture height the driver supports
     * for a given configuration.
     */
    VASTConfigAttribMaxPictureHeight    = 19,
    /**
     * \brief JPEG encoding attribute. Read-only.
     *
     * This attribute exposes a number of capabilities of the underlying
     * JPEG implementation. The attribute value is partitioned into fields as defined in the 
     * VAConfigAttribValEncJPEG_VAST union.
     */
    VASTConfigAttribEncJPEG             = 20,
    /**
     * \brief Encoding quality range attribute. Read-only.
     *
     * This attribute conveys whether the driver supports different quality level settings
     * for encoding. A value less than or equal to 1 means that the encoder only has a single
     * quality setting, and a value greater than 1 represents the number of quality levels 
     * that can be configured. e.g. a value of 2 means there are two distinct quality levels. 
     */
    VASTConfigAttribEncQualityRange     = 21,
    /**
     * \brief Encoding quantization attribute. Read-only.
     *
     * This attribute conveys whether the driver supports certain types of quantization methods
     * for encoding (e.g. trellis). See \c VA_ENC_QUANTIZATION_xxx for the list of quantization methods
     */
    VASTConfigAttribEncQuantization     = 22,
    /**
     * \brief Encoding intra refresh attribute. Read-only.
     *
     * This attribute conveys whether the driver supports certain types of intra refresh methods
     * for encoding (e.g. adaptive intra refresh or rolling intra refresh).
     * See \c VA_ENC_INTRA_REFRESH_xxx for intra refresh methods
     */
    VASTConfigAttribEncIntraRefresh     = 23,
    /**
     * \brief Encoding skip frame attribute. Read-only.
     *
     * This attribute conveys whether the driver supports sending skip frame parameters 
     * (VASTEncMiscParameterTypeSkipFrame) to the encoder's rate control, when the user has 
     * externally skipped frames. 
     */
    VASTConfigAttribEncSkipFrame        = 24,
    /**
     * \brief Encoding region-of-interest (ROI) attribute. Read-only.
     *
     * This attribute conveys whether the driver supports region-of-interest (ROI) encoding,
     * based on user provided ROI rectangles.  The attribute value is partitioned into fields
     * as defined in the VASTConfigAttribValEncROI union.
     *
     * If ROI encoding is supported, the ROI information is passed to the driver using
     * VASTEncMiscParameterTypeROI.
     */
    VASTConfigAttribEncROI              = 25,
    /**
     * \brief Encoding extended rate control attribute. Read-only.
     *
     * This attribute conveys whether the driver supports any extended rate control features
     * The attribute value is partitioned into fields as defined in the
     * VASTConfigAttribValEncRateControlExt union.
     */
    VASTConfigAttribEncRateControlExt   = 26,
    /**
     * \brief Processing rate reporting attribute. Read-only.
     *
     * This attribute conveys whether the driver supports reporting of
     * encode/decode processing rate based on certain set of parameters
     * (i.e. levels, I frame internvals) for a given configuration.
     * If this is supported, vastQueryProcessingRate() can be used to get
     * encode or decode processing rate.
     * See \c VA_PROCESSING_RATE_xxx for encode/decode processing rate
     */
    VASTConfigAttribProcessingRate    = 27,
    /**
     * \brief Encoding dirty rectangle. Read-only.
     *
     * This attribute conveys whether the driver supports dirty rectangle.
     * encoding, based on user provided ROI rectangles which indicate the rectangular areas
     * where the content has changed as compared to the previous picture.  The regions of the
     * picture that are not covered by dirty rect rectangles are assumed to have not changed
     * compared to the previous picture.  The encoder may do some optimizations based on
     * this information.  The attribute value returned indicates the number of regions that
     * are supported.  e.g. A value of 0 means dirty rect encoding is not supported.  If dirty
     * rect encoding is supported, the ROI information is passed to the driver using
     * VASTEncMiscParameterTypeDirtyRect.
     */
     VASTConfigAttribEncDirtyRect       = 28,
    /**
     * \brief Parallel Rate Control (hierachical B) attribute. Read-only.
     *
     * This attribute conveys whether the encoder supports parallel rate control.
     * It is a integer value 0 - unsupported, > 0 - maximum layer supported.
     * This is the way when hireachical B frames are encoded, multiple independent B frames
     * on the same layer may be processed at same time. If supported, app may enable it by
     * setting enable_parallel_brc in VASTEncMiscParameterRateControl,and the number of B frames
     * per layer per GOP will be passed to driver through VASTEncMiscParameterParallelRateControl
     * structure.Currently three layers are defined.
     */
     VASTConfigAttribEncParallelRateControl   = 29,
     /**
     * \brief Dynamic Scaling Attribute. Read-only.
     *
     * This attribute conveys whether encoder is capable to determine dynamic frame
     * resolutions adaptive to bandwidth utilization and processing power, etc.
     * It is a boolean value 0 - unsupported, 1 - supported.
     * If it is supported,for VP9, suggested frame resolution can be retrieved from VASTCodedBufferVP9Status.
     */
     VASTConfigAttribEncDynamicScaling        = 30,
     /**
     * \brief frame size tolerance support
     * it indicates the tolerance of frame size
     */
     VASTConfigAttribFrameSizeToleranceSupport = 31,
     /**
     * \brief Encode function type for FEI.
     *
     * This attribute conveys whether the driver supports different function types for encode.
     * It can be VAST_FEI_FUNCTION_ENC, VAST_FEI_FUNCTION_PAK, or VAST_FEI_FUNCTION_ENC_PAK. Currently
     * it is for FEI entry point only.
     * Default is VAST_FEI_FUNCTION_ENC_PAK.
     */
     VASTConfigAttribFEIFunctionType     = 32,
    /**
     * \brief Maximum number of FEI MV predictors. Read-only.
     *
     * This attribute determines the maximum number of MV predictors the driver
     * can support to encode a single frame. 0 means no MV predictor is supported.
     * Currently it is for FEI entry point only.
     */
    VASTConfigAttribFEIMVPredictors     = 33,
    /**
     * \brief Statistics attribute. Read-only.
     *
     * This attribute exposes a number of capabilities of the VASTEntrypointStats entry
     * point. The attribute value is partitioned into fields as defined in the
     * VASTConfigAttribValStats union. Currently it is for VASTEntrypointStats only.
     */
    VASTConfigAttribStats               = 34,
     /**
     * \brief Tile Support Attribute. Read-only.
     *
     * This attribute conveys whether encoder is capable to support tiles.
     * If not supported, the tile related parameters sent to encoder, such as
     * tiling structure, should be ignored. 0 - unsupported, 1 - supported.
     */
     VASTConfigAttribEncTileSupport        = 35,
    /**
     * \brief whether accept rouding setting from application. Read-only.
     * This attribute is for encode quality, if it is report,
     * application can change the rounding setting by VASTEncMiscParameterTypeCustomRoundingControl
     */
    VASTConfigAttribCustomRoundingControl = 36,
    /**
     * \brief Encoding QP info block size attribute. Read-only.
     * This attribute conveys the block sizes that underlying driver
     * support for QP info for buffer #VAEncQpBuffer.
     */
    VASTConfigAttribQPBlockSize            = 37,
    VASTConfigAttribGPUNodes               = 38,
    /**@}*/
    VASTConfigAttribTypeMax
} VASTConfigAttribType;

/** Extension of VASTBufferType */
typedef enum {

    VASTHANTRODecEmbeddedPostprocessParameterBufferType = 100,
    VASTHANTRODecMiscParameterBufferType,
    VASTHANTROEncROIMapBufferType,
    VASTHANTROEncCuCtrlBufferType,
    VASTHANTROEncCuCtrlIndexBufferType,
    VASTHANTROEncMBCUOutputInfoBufferType,
    VASTHANTROEncPSNROutputInfoBufferType,
    VASTHANTROEncUserDataBufferType,
    VASTHANTROEncAdaptiveGOPDecisionBufferType

} VASTHANTROBufferType;

/**
 * Configuration attributes
 * If there is more than one value for an attribute, a default
 * value will be assigned to the attribute if the client does not
 * specify the attribute when creating a configuration
 */
typedef struct _VASTConfigAttrib {
    VASTConfigAttribType type;
    uint32_t value; /* OR'd flags (bits) for this attribute */
} VASTConfigAttrib;

/** attribute value for VASTConfigAttribRTFormat */
#define VAST_RT_FORMAT_YUV420	0x00000001	
#define VAST_RT_FORMAT_YUV422	0x00000002
#define VAST_RT_FORMAT_YUV444	0x00000004
#define VAST_RT_FORMAT_YUV411	0x00000008
#define VAST_RT_FORMAT_YUV400	0x00000010
/** YUV formats with more than 8 bpp */
#define VAST_RT_FORMAT_YUV420_10BPP	0x00000100
/** RGB formats */
#define VAST_RT_FORMAT_RGB16	0x00010000
#define VAST_RT_FORMAT_RGB32	0x00020000
/* RGBP covers RGBP and BGRP fourcc */ 
#define VAST_RT_FORMAT_RGBP	0x00100000
/**
 * RGB 10-bit packed format with upper 2 bits as alpha channel.
 * The existing pre-defined fourcc codes can be used to signal
 * the position of each component for this RT format.
 */
#define VAST_RT_FORMAT_RGB32_10BPP 0x00200000
#define VAST_RT_FORMAT_PROTECTED	0x80000000

/** @name Attribute values for VASTConfigAttribRateControl */
/**@{*/
/** \brief Driver does not support any form of rate control. */
#define VAST_RC_NONE                      0x00000001
/** \brief Constant bitrate. */
#define VAST_RC_CBR                       0x00000002
/** \brief Variable bitrate. */
#define VAST_RC_VBR                       0x00000004
/** \brief Video conference mode. */
#define VAST_RC_VCM                       0x00000008
/** \brief Constant QP. */
#define VAST_RC_CQP                       0x00000010
/** \brief Variable bitrate with peak rate higher than average bitrate. */
#define VAST_RC_VBR_CONSTRAINED           0x00000020
/** \brief Intelligent Constant Quality. Provided an initial ICQ_quality_factor,
 *  adjusts QP at a frame and MB level based on motion to improve subjective quality. */
#define VAST_RC_ICQ			0x00000040
/** \brief Macroblock based rate control.  Per MB control is decided
 *  internally in the encoder. It may be combined with other RC modes, except CQP. */
#define VAST_RC_MB                        0x00000080
/** \brief Constant Frame Size, it is used for small tolerent  */
#define VAST_RC_CFS                       0x00000100
/** \brief Parallel BRC, for hierachical B.
 *
 *  For hierachical B, B frames can be refered by other B frames.
 *  Currently three layers of hierachy are defined:
 *  B0 - regular B, no reference to other B frames.
 *  B1 - reference to only I, P and regular B0 frames.
 *  B2 - reference to any other frames, including B1.
 *  In Hierachical B structure, B frames on the same layer can be processed
 *  simultaneously. And BRC would adjust accordingly. This is so called
 *  Parallel BRC. */
#define VAST_RC_PARALLEL                  0x00000200

/**@}*/

/** @name Attribute values for VASTConfigAttribDecSliceMode */
/**@{*/
/** \brief Driver supports normal mode for slice decoding */
#define VAST_DEC_SLICE_MODE_NORMAL       0x00000001
/** \brief Driver supports base mode for slice decoding */
#define VAST_DEC_SLICE_MODE_BASE         0x00000002

/** @name Attribute values for VASTConfigAttribDecJPEG */
/**@{*/
typedef union _VASTConfigAttribValDecJPEG {
    struct{
    /** \brief Set to (1 << VA_ROTATION_xxx) for supported rotation angles. */
    uint32_t rotation : 4;
    /** \brief Reserved for future use. */
    uint32_t reserved : 28;
    }bits;
    uint32_t value;
    uint32_t va_reserved[VAST_PADDING_LOW];
} VASTConfigAttribValDecJPEG;
/** @name Attribute values for VASTConfigAttribDecProcessing */
/**@{*/
/** \brief No decoding + processing in a single decoding call. */
#define VAST_DEC_PROCESSING_NONE     0x00000000
/** \brief Decode + processing in a single decoding call. */
#define VAST_DEC_PROCESSING          0x00000001
/**@}*/

/** @name Attribute values for VASTConfigAttribEncPackedHeaders */
/**@{*/
/** \brief Driver does not support any packed headers mode. */
#define VAST_ENC_PACKED_HEADER_NONE       0x00000000
/**
 * \brief Driver supports packed sequence headers. e.g. SPS for H.264.
 *
 * Application must provide it to driver once this flag is returned through
 * vastGetConfigAttributes()
 */
#define VAST_ENC_PACKED_HEADER_SEQUENCE   0x00000001
/**
 * \brief Driver supports packed picture headers. e.g. PPS for H.264.
 *
 * Application must provide it to driver once this falg is returned through
 * vastGetConfigAttributes()
 */
#define VAST_ENC_PACKED_HEADER_PICTURE    0x00000002
/**
 * \brief Driver supports packed slice headers. e.g. slice_header() for H.264.
 *
 * Application must provide it to driver once this flag is returned through
 * vastGetConfigAttributes()
 */
#define VAST_ENC_PACKED_HEADER_SLICE      0x00000004
/**
 * \brief Driver supports misc packed headers. e.g. SEI for H.264.
 *
 * @deprecated
 * This is a deprecated packed header flag, All applications can use
 * \c VAST_ENC_PACKED_HEADER_RAW_DATA to pass the corresponding packed
 * header data buffer to the driver
 */
#define VAST_ENC_PACKED_HEADER_MISC       0x00000008
/** \brief Driver supports raw packed header, see VASTEncPackedHeaderRawData */
#define VAST_ENC_PACKED_HEADER_RAW_DATA   0x00000010
/**@}*/

/** @name Attribute values for VASTConfigAttribEncInterlaced */
/**@{*/
/** \brief Driver does not support interlaced coding. */
#define VAST_ENC_INTERLACED_NONE          0x00000000
/** \brief Driver supports interlaced frame coding. */
#define VAST_ENC_INTERLACED_FRAME         0x00000001
/** \brief Driver supports interlaced field coding. */
#define VAST_ENC_INTERLACED_FIELD         0x00000002
/** \brief Driver supports macroblock adaptive frame field coding. */
#define VAST_ENC_INTERLACED_MBAFF         0x00000004
/** \brief Driver supports picture adaptive frame field coding. */
#define VAST_ENC_INTERLACED_PAFF          0x00000008
/**@}*/

/** @name Attribute values for VASTConfigAttribEncSliceStructure */
/**@{*/
/** \brief Driver supports a power-of-two number of rows per slice. */
#define VAST_ENC_SLICE_STRUCTURE_POWER_OF_TWO_ROWS        0x00000001
/** \brief Driver supports an arbitrary number of macroblocks per slice. */
#define VAST_ENC_SLICE_STRUCTURE_ARBITRARY_MACROBLOCKS    0x00000002
/** \brief Dirver support 1 rows  per slice */
#define VAST_ENC_SLICE_STRUCTURE_EQUAL_ROWS               0x00000004
/** \brief Dirver support max encoded slice size per slice */
#define VAST_ENC_SLICE_STRUCTURE_MAX_SLICE_SIZE           0x00000008
/** \brief Driver supports an arbitrary number of rows per slice. */
#define VAST_ENC_SLICE_STRUCTURE_ARBITRARY_ROWS           0x00000010
/**@}*/

/** \brief Attribute value for VASTConfigAttribEncJPEG */
typedef union _VASTConfigAttribValEncJPEG {
    struct {
        /** \brief set to 1 for arithmatic coding. */
        uint32_t arithmatic_coding_mode : 1;
        /** \brief set to 1 for progressive dct. */
        uint32_t progressive_dct_mode : 1;
        /** \brief set to 1 for non-interleaved. */
        uint32_t non_interleaved_mode : 1;
        /** \brief set to 1 for differential. */
        uint32_t differential_mode : 1;
        uint32_t max_num_components : 3;
        uint32_t max_num_scans : 4;
        uint32_t max_num_huffman_tables : 3;
        uint32_t max_num_quantization_tables : 3;
    } bits;
    uint32_t value;
} VASTConfigAttribValEncJPEG;

/** @name Attribute values for VASTConfigAttribEncQuantization */
/**@{*/
/** \brief Driver does not support special types of quantization */
#define VAST_ENC_QUANTIZATION_NONE                        0x00000000
/** \brief Driver supports trellis quantization */
#define VAST_ENC_QUANTIZATION_TRELLIS_SUPPORTED           0x00000001
/**@}*/

/** @name Attribute values for VASTConfigAttribEncIntraRefresh */
/**@{*/
/** \brief Driver does not support intra refresh */
#define VAST_ENC_INTRA_REFRESH_NONE                       0x00000000
/** \brief Driver supports column based rolling intra refresh */
#define VAST_ENC_INTRA_REFRESH_ROLLING_COLUMN             0x00000001
/** \brief Driver supports row based rolling intra refresh */
#define VAST_ENC_INTRA_REFRESH_ROLLING_ROW                0x00000002
/** \brief Driver supports adaptive intra refresh */
#define VAST_ENC_INTRA_REFRESH_ADAPTIVE                   0x00000010
/** \brief Driver supports cyclic intra refresh */
#define VAST_ENC_INTRA_REFRESH_CYCLIC                     0x00000020
/** \brief Driver supports intra refresh of P frame*/
#define VAST_ENC_INTRA_REFRESH_P_FRAME                    0x00010000
/** \brief Driver supports intra refresh of B frame */
#define VAST_ENC_INTRA_REFRESH_B_FRAME                    0x00020000
/** \brief Driver supports intra refresh of multiple reference encoder */
#define VAST_ENC_INTRA_REFRESH_MULTI_REF                  0x00040000

/**@}*/

/** \brief Attribute value for VASTConfigAttribEncROI */
typedef union _VASTConfigAttribValEncROI {
    struct {
        /** \brief The number of ROI regions supported, 0 if ROI is not supported. */
        uint32_t num_roi_regions 		: 8;
        /**
         * \brief A flag indicates whether ROI priority is supported
         *
         * \ref roi_rc_priority_support equal to 1 specifies the underlying driver supports
         * ROI priority when VASTConfigAttribRateControl != VAST_RC_CQP , user can use \c roi_value
         * in #VASTEncROI to set ROI priority. \ref roi_rc_priority_support equal to 0 specifies
         * the underlying driver doesn't support ROI priority.
         *
         * User should ignore \ref roi_rc_priority_support when VASTConfigAttribRateControl == VAST_RC_CQP
         * because ROI delta QP is always required when VASTConfigAttribRateControl == VAST_RC_CQP .
         */
        uint32_t roi_rc_priority_support	: 1;
        /**
         * \brief A flag indicates whether ROI delta QP is supported
         *
         * \ref roi_rc_qp_delta_support equal to 1 specifies the underlying driver supports
         * ROI delta QP when VASTConfigAttribRateControl != VAST_RC_CQP , user can use \c roi_value
         * in #VASTEncROI to set ROI delta QP. \ref roi_rc_qp_delta_support equal to 0 specifies
         * the underlying driver doesn't support ROI delta QP.
         *
         * User should ignore \ref roi_rc_qp_delta_support when VASTConfigAttribRateControl == VAST_RC_CQP
         * because ROI delta QP is always required when VASTConfigAttribRateControl == VAST_RC_CQP .
         */
        uint32_t roi_rc_qp_delta_support    : 1;
        uint32_t reserved                   : 22;
     } bits;
     uint32_t value;
} VASTConfigAttribValEncROI;

/** \brief Attribute value for VASTConfigAttribEncRateControlExt */
typedef union _VASTConfigAttribValEncRateControlExt {
    struct {
        /**
         * \brief The maximum number of temporal layers minus 1
         *
         * \ref max_num_temporal_layers_minus1 plus 1 specifies the maximum number of temporal
         * layers that supported by the underlying driver. \ref max_num_temporal_layers_minus1
         * equal to 0 implies the underlying driver doesn't support encoding with temporal layer.
         */
        uint32_t max_num_temporal_layers_minus1      : 8;

        /**
         * /brief support temporal layer bit-rate control flag
         *
         * \ref temporal_layer_bitrate_control_flag equal to 1 specifies the underlying driver
         * can support bit-rate control per temporal layer when (#VASTConfigAttribRateControl == #VAST_RC_CBR ||
         * #VASTConfigAttribRateControl == #VAST_RC_VBR).
         *
         * The underlying driver must set \ref temporal_layer_bitrate_control_flag to 0 when
         * \c max_num_temporal_layers_minus1 is equal to 0
         *
         * To use bit-rate control per temporal layer, an application must send the right layer
         * structure via #VASTEncMiscParameterTemporalLayerStructure at the beginning of a coded sequence
         * and then followed by #VASTEncMiscParameterRateControl and #VASTEncMiscParameterFrameRate structures
         * for each layer, using the \c temporal_id field as the layer identifier. Otherwise
         * the driver doesn't use bitrate control per temporal layer if an application doesn't send the
         * layer structure via #VASTEncMiscParameterTemporalLayerStructure to the driver. The driver returns
         * VAST_STATUS_ERROR_INVALID_PARAMETER if an application sends a wrong layer structure or doesn't send
         * #VASTEncMiscParameterRateControl and #VASTEncMiscParameterFrameRate for each layer.
         *
         * The driver will ignore #VASTEncMiscParameterTemporalLayerStructure and the \c temporal_id field
         * in #VASTEncMiscParameterRateControl and #VASTEncMiscParameterFrameRate if
         * \ref temporal_layer_bitrate_control_flag is equal to 0 or #VASTConfigAttribRateControl == #VAST_RC_CQP
         */
        uint32_t temporal_layer_bitrate_control_flag : 1;
        uint32_t reserved                            : 23;
    } bits;
    uint32_t value;
} VASTConfigAttribValEncRateControlExt;

/** @name Attribute values for VASTConfigAttribProcessingRate. */
/**@{*/
/** \brief Driver does not support processing rate report */
#define VAST_PROCESSING_RATE_NONE                       0x00000000
/** \brief Driver supports encode processing rate report  */
#define VAST_PROCESSING_RATE_ENCODE                     0x00000001
/** \brief Driver supports decode processing rate report  */
#define VAST_PROCESSING_RATE_DECODE                     0x00000002
/**@}*/
/**
 * if an attribute is not applicable for a given
 * profile/entrypoint pair, then set the value to the following
 */
#define VAST_ATTRIB_NOT_SUPPORTED 0x80000000

/** Get maximum number of profiles supported by the implementation */
int vastMaxNumProfiles (
    VASTDisplay dpy
);

/** Get maximum number of entrypoints supported by the implementation */
int vastMaxNumEntrypoints (
    VASTDisplay dpy
);

/** Get maximum number of attributs supported by the implementation */
int vastMaxNumConfigAttributes (
    VASTDisplay dpy
);

/**
 * Query supported profiles
 * The caller must provide a "profile_list" array that can hold at
 * least vaMaxNumProfile() entries. The actual number of profiles
 * returned in "profile_list" is returned in "num_profile".
 */
VASTStatus vastQueryConfigProfiles (
    VASTDisplay dpy,
    VASTProfile *profile_list,	/* out */
    int *num_profiles		/* out */
);

/**
 * Query supported entrypoints for a given profile
 * The caller must provide an "entrypoint_list" array that can hold at
 * least vastMaxNumEntrypoints() entries. The actual number of entrypoints
 * returned in "entrypoint_list" is returned in "num_entrypoints".
 */
VASTStatus vastQueryConfigEntrypoints (
    VASTDisplay dpy,
    VASTProfile profile,
    VASTEntrypoint *entrypoint_list,	/* out */
    int *num_entrypoints		/* out */
);

/**
 * Get attributes for a given profile/entrypoint pair
 * The caller must provide an "attrib_list" with all attributes to be
 * retrieved.  Upon return, the attributes in "attrib_list" have been
 * updated with their value.  Unknown attributes or attributes that are
 * not supported for the given profile/entrypoint pair will have their
 * value set to VAST_ATTRIB_NOT_SUPPORTED
 */
VASTStatus vastGetConfigAttributes (
    VASTDisplay dpy,
    VASTProfile profile,
    VASTEntrypoint entrypoint,
    VASTConfigAttrib *attrib_list, /* in/out */
    int num_attribs
);

/** Generic ID type, can be re-typed for specific implementation */
typedef unsigned int VASTGenericID;

typedef VASTGenericID VASTConfigID;

/**
 * Create a configuration for the video decode/encode/processing pipeline
 * it passes in the attribute list that specifies the attributes it cares
 * about, with the rest taking default values.
 */
VASTStatus vastCreateConfig (
    VASTDisplay dpy,
    VASTProfile profile,
    VASTEntrypoint entrypoint,
    VASTConfigAttrib *attrib_list,
    int num_attribs,
    VASTConfigID *config_id /* out */
);

/**
 * Free resources associdated with a given config
 */
VASTStatus vastDestroyConfig (
    VASTDisplay dpy,
    VASTConfigID config_id
);

/**
 * Query all attributes for a given configuration
 * The profile of the configuration is returned in "profile"
 * The entrypoint of the configuration is returned in "entrypoint"
 * The caller must provide an "attrib_list" array that can hold at least
 * vastMaxNumConfigAttributes() entries. The actual number of attributes
 * returned in "attrib_list" is returned in "num_attribs"
 */
VASTStatus vastQueryConfigAttributes (
    VASTDisplay dpy,
    VASTConfigID config_id,
    VASTProfile *profile, 	/* out */
    VASTEntrypoint *entrypoint, 	/* out */
    VASTConfigAttrib *attrib_list,/* out */
    int *num_attribs 		/* out */
);


/**
 * Contexts and Surfaces
 *
 * Context represents a "virtual" video decode, encode or video processing
 * pipeline. Surfaces are render targets for a given context. The data in the
 * surfaces are not accessible to the client except if derived image is supported
 * and the internal data format of the surface is implementation specific.
 *
 * Surfaces are provided as a hint of what surfaces will be used when the context
 * is created through vastCreateContext(). A surface may be used by different contexts
 * at the same time as soon as application can make sure the operations are synchronized
 * between different contexts, e.g. a surface is used as the output of a decode context
 * and the input of a video process context. Surfaces can only be destroyed after all
 * contexts using these surfaces have been destroyed.
 *
 * Both contexts and surfaces are identified by unique IDs and its
 * implementation specific internals are kept opaque to the clients
 */

typedef VASTGenericID VASTContextID;

typedef VASTGenericID VASTSurfaceID;

#define VAST_INVALID_ID		0xffffffff
#define VAST_INVALID_SURFACE	VAST_INVALID_ID

/** \brief Generic value types. */
typedef enum  {
    VASTGenericValueTypeInteger = 1,      /**< 32-bit signed integer. */
    VASTGenericValueTypeFloat,            /**< 32-bit floating-point value. */
    VASTGenericValueTypePointer,          /**< Generic pointer type */
    VASTGenericValueTypeFunc              /**< Pointer to function */
} VASTGenericValueType;

/** \brief Generic function type. */
typedef void (*VASTGenericFunc)(void);

/** \brief Generic value. */
typedef struct _VASTGenericValue {
    /** \brief Value type. See #VASTGenericValueType. */
    VASTGenericValueType  type;
    /** \brief Value holder. */
    union {
        /** \brief 32-bit signed integer. */
        int32_t             i;
        /** \brief 32-bit float. */
        float           f;
        /** \brief Generic pointer. */
        void           *p;
        /** \brief Pointer to function. */
        VASTGenericFunc   fn;
    }                   value;
} VASTGenericValue;

/** @name Surface attribute flags */
/**@{*/
/** \brief Surface attribute is not supported. */
#define VAST_SURFACE_ATTRIB_NOT_SUPPORTED 0x00000000
/** \brief Surface attribute can be got through vastQuerySurfaceAttributes(). */
#define VAST_SURFACE_ATTRIB_GETTABLE      0x00000001
/** \brief Surface attribute can be set through vastCreateSurfaces(). */
#define VAST_SURFACE_ATTRIB_SETTABLE      0x00000002
/**@}*/

/** \brief Surface attribute types. */
typedef enum {
    VASTSurfaceAttribNone = 0,
    /**
     * \brief Pixel format (fourcc).
     *
     * The value is meaningful as input to vastQuerySurfaceAttributes().
     * If zero, the driver returns the optimal pixel format for the
     * specified config. Otherwise, if non-zero, the value represents
     * a pixel format (FOURCC) that is kept as is on output, if the
     * driver supports it. Otherwise, the driver sets the value to
     * zero and drops the \c VAST_SURFACE_ATTRIB_SETTABLE flag.
     */
    VASTSurfaceAttribPixelFormat,
    /** \brief Minimal width in pixels (int, read-only). */
    VASTSurfaceAttribMinWidth,
    /** \brief Maximal width in pixels (int, read-only). */
    VASTSurfaceAttribMaxWidth,
    /** \brief Minimal height in pixels (int, read-only). */
    VASTSurfaceAttribMinHeight,
    /** \brief Maximal height in pixels (int, read-only). */
    VASTSurfaceAttribMaxHeight,
    /** \brief Surface memory type expressed in bit fields (int, read/write). */
    VASTSurfaceAttribMemoryType,
    /** \brief External buffer descriptor (pointer, write). */
    VASTSurfaceAttribExternalBufferDescriptor,
    /** \brief Surface usage hint, gives the driver a hint of intended usage
     *  to optimize allocation (e.g. tiling) (int, read/write). */
    VASTSurfaceAttribUsageHint,
    /** \brief this attrib info surface belong to which slice, should be added by intel. */
    VASTSurfaceAttribGPUNodes,
    /** \brief Number of surface attributes. */
    VASTSurfaceAttribCount
} VASTSurfaceAttribType;

/** \brief Surface attribute. */
typedef struct _VASTSurfaceAttrib {
    /** \brief Type. */
    VASTSurfaceAttribType type;
    /** \brief Flags. See "Surface attribute flags". */
    uint32_t        flags;
    /** \brief Value. See "Surface attribute types" for the expected types. */
    VASTGenericValue      value;
} VASTSurfaceAttrib;

/**
 * @name VASTSurfaceAttribMemoryType values in bit fields.
 * Bit 0:7 are reserved for generic types, Bit 31:28 are reserved for
 * Linux DRM, Bit 23:20 are reserved for Android. DRM and Android specific
 * types are defined in DRM and Android header files.
 */
/**@{*/
/** \brief VA memory type (default) is supported. */
#define VAST_SURFACE_ATTRIB_MEM_TYPE_VA			0x00000001
/** \brief V4L2 buffer memory type is supported. */
#define VAST_SURFACE_ATTRIB_MEM_TYPE_V4L2			0x00000002
/** \brief User pointer memory type is supported. */
#define VAST_SURFACE_ATTRIB_MEM_TYPE_USER_PTR		0x00000004
/**@}*/

/**
 * \brief VASTSurfaceAttribExternalBuffers structure for
 * the VASTSurfaceAttribExternalBufferDescriptor attribute.
 */
typedef struct _VASTSurfaceAttribExternalBuffers {
    /** \brief pixel format in fourcc. */
    uint32_t pixel_format;
    /** \brief width in pixels. */
    uint32_t width;
    /** \brief height in pixels. */
    uint32_t height;
    /** \brief total size of the buffer in bytes. */
    uint32_t data_size;
    /** \brief number of planes for planar layout */
    uint32_t num_planes;
    /** \brief pitch for each plane in bytes */
    uint32_t pitches[4];
    /** \brief offset for each plane in bytes */
    uint32_t offsets[4];
    /** \brief buffer handles or user pointers */
    uintptr_t *buffers;
    /** \brief number of elements in the "buffers" array */
    uint32_t num_buffers;
    /** \brief flags. See "Surface external buffer descriptor flags". */
    uint32_t flags;
    /** \brief reserved for passing private data */
    void *private_data;
} VASTSurfaceAttribExternalBuffers;

/** @name VASTSurfaceAttribExternalBuffers flags */
/**@{*/
/** \brief Enable memory tiling */
#define VAST_SURFACE_EXTBUF_DESC_ENABLE_TILING	0x00000001
/** \brief Memory is cacheable */
#define VAST_SURFACE_EXTBUF_DESC_CACHED		0x00000002
/** \brief Memory is non-cacheable */
#define VAST_SURFACE_EXTBUF_DESC_UNCACHED		0x00000004
/** \brief Memory is write-combined */
#define VAST_SURFACE_EXTBUF_DESC_WC		0x00000008
/** \brief Memory is protected */
#define VAST_SURFACE_EXTBUF_DESC_PROTECTED        0x80000000

/** @name VASTSurfaceAttribUsageHint attribute usage hint flags */
/**@{*/
/** \brief Surface usage not indicated. */
#define VAST_SURFACE_ATTRIB_USAGE_HINT_GENERIC 	0x00000000
/** \brief Surface used by video decoder. */
#define VAST_SURFACE_ATTRIB_USAGE_HINT_DECODER 	0x00000001
/** \brief Surface used by video encoder. */
#define VAST_SURFACE_ATTRIB_USAGE_HINT_ENCODER 	0x00000002
/** \brief Surface read by video post-processing. */
#define VAST_SURFACE_ATTRIB_USAGE_HINT_VPP_READ 	0x00000004
/** \brief Surface written by video post-processing. */
#define VAST_SURFACE_ATTRIB_USAGE_HINT_VPP_WRITE 	0x00000008
/** \brief Surface used for display. */
#define VAST_SURFACE_ATTRIB_USAGE_HINT_DISPLAY 	0x00000010

/**@}*/

/**
 * \brief Queries surface attributes for the supplied config.
 *
 * This function queries for all supported attributes for the
 * supplied VA @config. In particular, if the underlying hardware
 * supports the creation of VA surfaces in various formats, then
 * this function will enumerate all pixel formats that are supported.
 *
 * The \c attrib_list array is allocated by the user and \c
 * num_attribs shall be initialized to the number of allocated
 * elements in that array. Upon successful return, the actual number
 * of attributes will be overwritten into \c num_attribs. Otherwise,
 * \c VAST_STATUS_ERROR_MAX_NUM_EXCEEDED is returned and \c num_attribs
 * is adjusted to the number of elements that would be returned if
 * enough space was available.
 *
 * Note: it is perfectly valid to pass NULL to the \c attrib_list
 * argument when vastQuerySurfaceAttributes() is used to determine the
 * actual number of elements that need to be allocated.
 *
 * @param[in] dpy               the VA display
 * @param[in] config            the config identifying a codec or a video
 *     processing pipeline
 * @param[out] attrib_list      the output array of #VASTSurfaceAttrib elements
 * @param[in,out] num_attribs   the number of elements allocated on
 *      input, the number of elements actually filled in output
 */
VASTStatus
vastQuerySurfaceAttributes(
    VASTDisplay           dpy,
    VASTConfigID          config,
    VASTSurfaceAttrib    *attrib_list,
    unsigned int       *num_attribs
);

/**
 * \brief Creates an array of surfaces
 *
 * Creates an array of surfaces. The optional list of attributes shall
 * be constructed based on what the underlying hardware could expose
 * through vastQuerySurfaceAttributes().
 *
 * @param[in] dpy               the VA display
 * @param[in] format            the desired surface format. See \c VA_RT_FORMAT_*
 * @param[in] width             the surface width
 * @param[in] height            the surface height
 * @param[out] surfaces         the array of newly created surfaces
 * @param[in] num_surfaces      the number of surfaces to create
 * @param[in] attrib_list       the list of (optional) attributes, or \c NULL
 * @param[in] num_attribs       the number of attributes supplied in
 *     \c attrib_list, or zero
 */
VASTStatus
vastCreateSurfaces(
    VASTDisplay           dpy,
    unsigned int        format,
    unsigned int        width,
    unsigned int        height,
    VASTSurfaceID        *surfaces,
    unsigned int        num_surfaces,
    VASTSurfaceAttrib    *attrib_list,
    unsigned int        num_attribs
);

/**
 * vastDestroySurfaces - Destroy resources associated with surfaces.
 *  Surfaces can only be destroyed after all contexts using these surfaces have been
 *  destroyed.
 *  dpy: display
 *  surfaces: array of surfaces to destroy
 *  num_surfaces: number of surfaces in the array to be destroyed.
 */
VASTStatus vastDestroySurfaces (
    VASTDisplay dpy,
    VASTSurfaceID *surfaces,
    int num_surfaces
);

#define VA_PROGRESSIVE 0x1
/**
 * vastCreateContext - Create a context
 *  dpy: display
 *  config_id: configuration for the context
 *  picture_width: coded picture width
 *  picture_height: coded picture height
 *  flag: any combination of the following:
 *    VA_PROGRESSIVE (only progressive frame pictures in the sequence when set)
 *  render_targets: a hint for render targets (surfaces) tied to the context
 *  num_render_targets: number of render targets in the above array
 *  context: created context id upon return
 */
VASTStatus vastCreateContext (
    VASTDisplay dpy,
    VASTConfigID config_id,
    int picture_width,
    int picture_height,
    int flag,
    VASTSurfaceID *render_targets,
    int num_render_targets,
    VASTContextID *context		/* out */
);

/**
 * vastDestroyContext - Destroy a context
 *  dpy: display
 *  context: context to be destroyed
 */
VASTStatus vastDestroyContext (
    VASTDisplay dpy,
    VASTContextID context
);

//Multi-frame context
typedef VASTGenericID VASTMFContextID;
/**
 * vastCreateMFContext - Create a multi-frame context
 *  interface encapsulating common for all streams memory objects and structures
 *  required for single GPU task submission from several VASTContextID's.
 *  Allocation: This call only creates an instance, doesn't allocate any additional memory.
 *  Support identification: Application can identify multi-frame feature support by ability
 *  to create multi-frame context. If driver supports multi-frame - call successful,
 *  mf_context != NULL and VASTStatus = VAST_STATUS_SUCCESS, otherwise if multi-frame processing
 *  not supported driver returns VAST_STATUS_ERROR_UNIMPLEMENTED and mf_context = NULL.
 *  return values:
 *  VAST_STATUS_SUCCESS - operation successful.
 *  VAST_STATUS_ERROR_UNIMPLEMENTED - no support for multi-frame.
 *  dpy: display adapter.
 *  mf_context: Multi-Frame context encapsulating all associated context
 *  for multi-frame submission.
 */
VASTStatus vastCreateMFContext (
    VASTDisplay dpy,
    VASTMFContextID *mf_context    /* out */
);

/**
 * vastMFAddContext - Provide ability to associate each context used for
 *  Multi-Frame submission and common Multi-Frame context.
 *  Try to add context to understand if it is supported.
 *  Allocation: this call allocates and/or reallocates all memory objects
 *  common for all contexts associated with particular Multi-Frame context.
 *  All memory required for each context(pixel buffers, internal driver
 *  buffers required for processing) allocated during standard vastCreateContext call for each context.
 *  Runtime dependency - if current implementation doesn't allow to run different entry points/profile,
 *  first context added will set entry point/profile for whole Multi-Frame context,
 *  all other entry points and profiles can be rejected to be added.
 *  Return values:
 *  VAST_STATUS_SUCCESS - operation successful, context was added.
 *  VAST_STATUS_ERROR_OPERATION_FAILED - something unexpected happened - application have to close
 *  current mf_context and associated contexts and start working with new ones.
 *  VAST_STATUS_ERROR_INVALID_CONTEXT - ContextID is invalid, means:
 *  1 - mf_context is not valid context or
 *  2 - driver can't suport different VASTEntrypoint or VASTProfile simultaneosly
 *  and current context contradicts with previously added, application can continue with current mf_context
 *  and other contexts passed this call, rejected context can continue work in stand-alone
 *  mode or other mf_context.
 *  VAST_STATUS_ERROR_UNSUPPORTED_ENTRYPOINT - particular context being added was created with with
 *  unsupported VASTEntrypoint. Application can continue with current mf_context
 *  and other contexts passed this call, rejected context can continue work in stand-alone
 *  mode.
 *  VAST_STATUS_ERROR_UNSUPPORTED_PROFILE - Current context with Particular VASTEntrypoint is supported
 *  but VASTProfile is not supported. Application can continue with current mf_context
 *  and other contexts passed this call, rejected context can continue work in stand-alone
 *  mode.
 *  dpy: display adapter.
 *  context: context being associated with Multi-Frame context.
 *  mf_context: - multi-frame context used to associate contexts for multi-frame submission.
 */
VASTStatus vastMFAddContext (
    VASTDisplay dpy,
    VASTMFContextID mf_context,
    VASTContextID context
);

/**
 * vastMFReleaseContext - Removes context from multi-frame and
 *  association with multi-frame context.
 *  After association removed vastEndPicture will submit tasks, but not vastMFSubmit.
 *  Return values:
 *  VAST_STATUS_SUCCESS - operation successful, context was removed.
 *  VAST_STATUS_ERROR_OPERATION_FAILED - something unexpected happened.
 *  application need to destroy this VASTMFContextID and all assotiated VASTContextID
 *  dpy: display
 *  mf_context: VASTMFContextID where context is added
 *  context: VASTContextID to be added
 */
VASTStatus vastMFReleaseContext (
    VASTDisplay dpy,
    VASTMFContextID mf_context,
    VASTContextID context
);

/**
 * Buffers
 * Buffers are used to pass various types of data from the
 * client to the server. The server maintains a data store
 * for each buffer created, and the client idenfies a buffer
 * through a unique buffer id assigned by the server.
 */

typedef VASTGenericID VASTBufferID;

typedef enum
{
    VASTPictureParameterBufferType	= 0,
    VASTIQMatrixBufferType		= 1,
    VASTBitPlaneBufferType		= 2,
    VASTSliceGroupMapBufferType		= 3,
    VASTSliceParameterBufferType		= 4,
    VASTSliceDataBufferType		= 5,
    VASTMacroblockParameterBufferType	= 6,
    VASTResidualDataBufferType		= 7,
    VASTDeblockingParameterBufferType	= 8,
    VASTImageBufferType			= 9,
    VASTProtectedSliceDataBufferType	= 10,
    VASTQMatrixBufferType                 = 11,
    VASTHuffmanTableBufferType            = 12,
    VASTProbabilityBufferType             = 13,

/* Following are encode buffer types */
    VASTEncCodedBufferType		= 21,
    VASTEncSequenceParameterBufferType	= 22,
    VASTEncPictureParameterBufferType	= 23,
    VASTEncSliceParameterBufferType	= 24,
    VASTEncPackedHeaderParameterBufferType = 25,
    VASTEncPackedHeaderDataBufferType     = 26,
    VASTEncMiscParameterBufferType	= 27,
    VASTEncMacroblockParameterBufferType	= 28,
    VASTEncMacroblockMapBufferType        = 29,

    /**
     * \brief Encoding QP buffer
     *
     * This buffer contains QP per MB for encoding. Currently
     * VASTEncQPBufferH264 is defined for H.264 encoding, see
     * #VASTEncQPBufferH264 for details
     */
    VASTEncQPBufferType                   = 30,
/* Following are video processing buffer types */
    /**
     * \brief Video processing pipeline parameter buffer.
     *
     * This buffer describes the video processing pipeline. See
     * #VASTProcPipelineParameterBuffer for details.
     */
    VASTProcPipelineParameterBufferType   = 41,
    /**
     * \brief Video filter parameter buffer.
     *
     * This buffer describes the video filter parameters. All buffers
     * inherit from #VASTProcFilterParameterBufferBase, thus including
     * a unique filter buffer type.
     *
     * The default buffer used by most filters is #VASTProcFilterParameterBuffer.
     * Filters requiring advanced parameters include, but are not limited to,
     * deinterlacing (#VASTProcFilterParameterBufferDeinterlacing),
     * color balance (#VASTProcFilterParameterBufferColorBalance), etc.
     */
    VASTProcFilterParameterBufferType     = 42,
    /**
     * \brief FEI specific buffer types
     */
    VASTEncFEIMVBufferType                = 43,
    VASTEncFEIMBCodeBufferType            = 44,
    VASTEncFEIDistortionBufferType        = 45,
    VASTEncFEIMBControlBufferType         = 46,
    VASTEncFEIMVPredictorBufferType       = 47,
    VASTStatsStatisticsParameterBufferType = 48,
    /** \brief Statistics output for VASTEntrypointStats progressive and top field of interlaced case*/
    VASTStatsStatisticsBufferType         = 49,
    /** \brief Statistics output for VASTEntrypointStats bottom field of interlaced case*/
    VASTStatsStatisticsBottomFieldBufferType = 50,
    VASTStatsMVBufferType                 = 51,
    VASTStatsMVPredictorBufferType        = 52,
    /** Force MB's to be non skip for encode.it's per-mb control buffer, The width of the MB map
     * Surface is (width of the Picture in MB unit) * 1 byte, multiple of 64 bytes.
     * The height is (height of the picture in MB unit). The picture is either
     * frame or non-interleaved top or bottom field.  If the application provides this
     *surface, it will override the "skipCheckDisable" setting in VASTEncMiscParameterEncQuality.
     */
    VASTEncMacroblockDisableSkipMapBufferType = 53,
    /**
     * \brief HEVC FEI CTB level cmd buffer
     * it is CTB level information for future usage.
     */
    VASTEncFEICTBCmdBufferType            = 54,
    /**
     * \brief HEVC FEI CU level data buffer
     * it's CTB level information for future usage
     */
    VASTEncFEICURecordBufferType          = 55,
    /** decode stream out buffer, intermedia data of decode, it may include MV, MB mode etc.
      * it can be used to detect motion and analyze the frame contain  */
    VASTDecodeStreamoutBufferType             = 56,
    VASTBufferTypeMax
} VASTBufferType;

/**
 * Processing rate parameter for encode.
 */
typedef struct _VASTProcessingRateParameterEnc {
    /** \brief Profile level */
    uint8_t         level_idc;
    uint8_t         reserved[3];
    /** \brief quality level. When set to 0, default quality
     * level is used.
     */
    uint32_t        quality_level;
    /** \brief Period between I frames. */
    uint32_t        intra_period;
    /** \brief Period between I/P frames. */
    uint32_t        ip_period;
} VASTProcessingRateParameterEnc;

/**
 * Processing rate parameter for decode.
 */
typedef struct _VASTProcessingRateParameterDec {
    /** \brief Profile level */
    uint8_t         level_idc;
    uint8_t         reserved0[3];
    uint32_t        reserved;
} VASTProcessingRateParameterDec;

typedef struct _VASTProcessingRateParameter {
    union {
        VASTProcessingRateParameterEnc proc_buf_enc;
        VASTProcessingRateParameterDec proc_buf_dec;
    };
} VASTProcessingRateParameter;

/**
 * \brief Queries processing rate for the supplied config.
 *
 * This function queries the processing rate based on parameters in
 * \c proc_buf for the given \c config. Upon successful return, the processing
 * rate value will be stored in \c processing_rate. Processing rate is
 * specified as the number of macroblocks/CTU per second.
 *
 * If NULL is passed to the \c proc_buf, the default processing rate for the
 * given configuration will be returned.
 *
 * @param[in] dpy               the VA display
 * @param[in] config            the config identifying a codec or a video
 *     processing pipeline
 * @param[in] proc_buf       the buffer that contains the parameters for
        either the encode or decode processing rate
 * @param[out] processing_rate  processing rate in number of macroblocks per
        second constrained by parameters specified in proc_buf
 *
 */
VASTStatus
vastQueryProcessingRate(
    VASTDisplay           dpy,
    VASTConfigID          config,
    VASTProcessingRateParameter *proc_buf,
    unsigned int       *processing_rate
);

typedef enum
{
    VASTEncMiscParameterTypeFrameRate 	= 0,
    VASTEncMiscParameterTypeRateControl  	= 1,
    VASTEncMiscParameterTypeMaxSliceSize	= 2,
    VASTEncMiscParameterTypeAIR    	= 3,
    /** \brief Buffer type used to express a maximum frame size (in bits). */
    VASTEncMiscParameterTypeMaxFrameSize  = 4,
    /** \brief Buffer type used for HRD parameters. */
    VASTEncMiscParameterTypeHRD           = 5,
    VASTEncMiscParameterTypeQualityLevel  = 6,
    /** \brief Buffer type used for Rolling intra refresh */
    VASTEncMiscParameterTypeRIR           = 7,
    /** \brief Buffer type used for quantization parameters, it's per-sequence parameter*/
    VASTEncMiscParameterTypeQuantization  = 8,
    /** \brief Buffer type used for sending skip frame parameters to the encoder's
      * rate control, when the user has externally skipped frames. */
    VASTEncMiscParameterTypeSkipFrame     = 9,
    /** \brief Buffer type used for region-of-interest (ROI) parameters. */
    VASTEncMiscParameterTypeROI           = 10,
    /** \brief Buffer type used for temporal layer structure */
    VASTEncMiscParameterTypeTemporalLayerStructure   = 12,
    /** \brief Buffer type used for dirty region-of-interest (ROI) parameters. */
    VASTEncMiscParameterTypeDirtyRect      = 13,
    /** \brief Buffer type used for parallel BRC parameters. */
    VASTEncMiscParameterTypeParallelBRC   = 14,
    /** \brief Set MB partion mode mask and Half-pel/Quant-pel motion search */
    VASTEncMiscParameterTypeSubMbPartPel = 15,
    /** \brief set encode quality tuning */
    VASTEncMiscParameterTypeEncQuality = 16,
    /** \brief Buffer type used for encoder rounding offset parameters. */
    VASTEncMiscParameterTypeCustomRoundingControl = 17,
    /** \brief Buffer type used for FEI input frame level parameters */
    VASTEncMiscParameterTypeFEIFrameControl = 18,
    /** \brief encode extension buffer, ect. MPEG2 Sequence extenstion data */
    VASTEncMiscParameterTypeExtensionData = 19,

    VASTSTEncMiscParameterType =20

} VASTEncMiscParameterTypeai;

/** \brief Packed header type. */
typedef enum {
    /** \brief Packed sequence header. */
    VASTEncPackedHeaderSequence   = 1,
    /** \brief Packed picture header. */
    VASTEncPackedHeaderPicture    = 2,
    /** \brief Packed slice header. */
    VASTEncPackedHeaderSlice      = 3,
    /**
     * \brief Packed raw header.
     *
     * Packed raw data header can be used by the client to insert a header
     * into the bitstream data buffer at the point it is passed, the driver
     * will handle the raw packed header based on "has_emulation_bytes" field
     * in the packed header parameter structure.
     */
    VASTEncPackedHeaderRawData    = 4,
    /**
     * \brief Misc packed header. See codec-specific definitions.
     *
     * @deprecated
     * This is a deprecated packed header type. All applications can use
     * \c VASTEncPackedHeaderRawData to insert a codec-specific packed header
     */
    VASTEncPackedHeaderMiscMask  = 0x80000000,
} VASTEncPackedHeaderType;

/** \brief Packed header parameter. */
typedef struct _VASTEncPackedHeaderParameterBuffer {
    /** Type of the packed header buffer. See #VASTEncPackedHeaderType. */
    uint32_t                type;
    /** \brief Size of the #VAEncPackedHeaderDataBuffer in bits. */
    uint32_t                bit_length;
    /** \brief Flag: buffer contains start code emulation prevention bytes? */
    uint8_t               has_emulation_bytes;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTEncPackedHeaderParameterBuffer;

/**
 *  For application, e.g. set a new bitrate
 *    VASTBufferID buf_id;
 *    VASTEncMiscParameterBuffer *misc_param;
 *    VASTEncMiscParameterRateControl *misc_rate_ctrl;
 *
 *    vastCreateBuffer(dpy, context, VASTEncMiscParameterBufferType,
 *              sizeof(VASTEncMiscParameterBuffer) + sizeof(VASTEncMiscParameterRateControl),
 *              1, NULL, &buf_id);
 *
 *    vastMapBuffer(dpy,buf_id,(void **)&misc_param);
 *    misc_param->type = VASTEncMiscParameterTypeRateControl;
 *    misc_rate_ctrl= (VASTEncMiscParameterRateControl *)misc_param->data;
 *    misc_rate_ctrl->bits_per_second = 6400000;
 *    vastUnmapBuffer(dpy, buf_id);
 *    vastRenderPicture(dpy, context, &buf_id, 1);
 */
typedef struct _VASTEncMiscParameterBuffer
{
    VASTEncMiscParameterTypeai type;
    uint32_t data[];
} VASTEncMiscParameterBuffer;

/** \brief Temporal layer Structure*/
typedef struct _VASTEncMiscParameterTemporalLayerStructure
{
    /** \brief The number of temporal layers */
    uint32_t number_of_layers;
    /** \brief The length of the array defining frame layer membership. Should be 1-32 */
    uint32_t periodicity;
    /**
     * \brief The array indicating the layer id for each frame
     *
     * The layer id for the first frame in a coded sequence is always 0, so layer_id[] specifies the layer
     * ids for frames starting from the 2nd frame.
     */
    uint32_t layer_id[32];

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTEncMiscParameterTemporalLayerStructure;


/** \brief Rate control parameters */
typedef struct _VASTEncMiscParameterRateControl
{
    /* this is the maximum bit-rate to be constrained by the rate control implementation */
    uint32_t bits_per_second;
    /* this is the bit-rate the rate control is targeting, as a percentage of the maximum
     * bit-rate for example if target_percentage is 95 then the rate control will target
     * a bit-rate that is 95% of the maximum bit-rate
     */
    uint32_t target_percentage;
    /* windows size in milliseconds. For example if this is set to 500,
     * then the rate control will guarantee the target bit-rate over a 500 ms window
     */
    uint32_t window_size;
    /* initial QP at I frames */
    uint32_t initial_qp;
    uint32_t min_qp;
    uint32_t basic_unit_size;
    union
    {
        struct
        {
            uint32_t reset : 1;
            uint32_t disable_frame_skip : 1; /* Disable frame skip in rate control mode */
            uint32_t disable_bit_stuffing : 1; /* Disable bit stuffing in rate control mode */
            uint32_t mb_rate_control : 4; /* Control VA_RC_MB 0: default, 1: enable, 2: disable, other: reserved*/
            /*
             * The temporal layer that the rate control parameters are specified for.
             */
            uint32_t temporal_id : 8;
            uint32_t cfs_I_frames : 1; /* I frame also follows CFS */
            uint32_t enable_parallel_brc    : 1;
            uint32_t enable_dynamic_scaling : 1;
             /**  \brief Frame Tolerance Mode
             *  Indicates the tolerance the application has to variations in the frame size.
             *  For example, wireless display scenarios may require very steady bit rate to
             *  reduce buffering time. It affects the rate control algorithm used,
             *  but may or may not have an effect based on the combination of other BRC
             *  parameters.  Only valid when the driver reports support for
             *  #VASTConfigAttribFrameSizeToleranceSupport.
             *
             *  equals 0    -- normal mode;
             *  equals 1    -- maps to sliding window;
             *  equals 2    -- maps to low delay mode;
             *  other       -- invalid.
             */
            uint32_t frame_tolerance_mode   : 2;
            uint32_t reserved               : 12;
        } bits;
        uint32_t value;
    } rc_flags;
    uint32_t ICQ_quality_factor; /* Initial ICQ quality factor: 1-51. */
    /** \brief Reserved bytes for future use, must be zero */
    uint32_t max_qp;
    uint32_t va_reserved[VAST_PADDING_MEDIUM - 2];
} VASTEncMiscParameterRateControl;

typedef struct _VASTEncMiscParameterFrameRate
{
    /*
     * The framerate is specified as a number of frames per second, as a
     * fraction.  The denominator of the fraction is given in the top half
     * (the high two bytes) of the framerate field, and the numerator is
     * given in the bottom half (the low two bytes).
     *
     * That is:
     * denominator = framerate >> 16 & 0xffff;
     * numerator   = framerate & 0xffff;
     * fps         = numerator / denominator;
     *
     * For example, if framerate is set to (100 << 16 | 750), this is
     * 750 / 100, hence 7.5fps.
     *
     * If the denominator is zero (the high two bytes are both zero) then
     * it takes the value one instead, so the framerate is just the integer
     * in the low 2 bytes.
     */
    uint32_t framerate;
    union
    {
        struct
        {
            /*
             * The temporal id the framerate parameters are specified for.
             */
            uint32_t temporal_id : 8;
            uint32_t reserved : 24;
         } bits;
         uint32_t value;
     } framerate_flags;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTEncMiscParameterFrameRate;

/**
 * Allow a maximum slice size to be specified (in bits).
 * The encoder will attempt to make sure that individual slices do not exceed this size
 * Or to signal applicate if the slice size exceed this size, see "status" of VASTCodedBufferSegment
 */
typedef struct _VASTEncMiscParameterMaxSliceSize
{
    uint32_t max_slice_size;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTEncMiscParameterMaxSliceSize;

typedef struct _VASTEncMiscParameterAIR
{
    uint32_t air_num_mbs;
    uint32_t air_threshold;
    uint32_t air_auto; /* if set to 1 then hardware auto-tune the AIR threshold */

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTEncMiscParameterAIR;

/*
 * \brief Rolling intra refresh data structure for encoding.
 */
typedef struct _VASTEncMiscParameterRIR
{
    union
    {
        struct
	/**
	 * \brief Indicate if intra refresh is enabled in column/row.
	 *
	 * App should query VASTConfigAttribEncIntraRefresh to confirm RIR support
	 * by the driver before sending this structure.
         */
        {
	    /* \brief enable RIR in column */
            uint32_t enable_rir_column : 1;
	    /* \brief enable RIR in row */
            uint32_t enable_rir_row : 1;
	    uint32_t reserved : 30;
        } bits;
        uint32_t value;
    } rir_flags;
    /**
     * \brief Indicates the column or row location in MB. It is ignored if
     * rir_flags is 0.
     */
    uint16_t intra_insertion_location;
    /**
     * \brief Indicates the number of columns or rows in MB. It is ignored if
     * rir_flags is 0.
     */
    uint16_t intra_insert_size;
    /**
     * \brief indicates the Qp difference for inserted intra columns or rows.
     * App can use this to adjust intra Qp based on bitrate & max frame size.
     */
    uint8_t  qp_delta_for_inserted_intra;
    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTEncMiscParameterRIR;

typedef struct _VASTEncMiscParameterHRD
{
    uint32_t initial_buffer_fullness;       /* in bits */
    uint32_t buffer_size;                   /* in bits */

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTEncMiscParameterHRD;

/**
 * \brief Defines a maximum frame size (in bits).
 *
 * This misc parameter buffer defines the maximum size of a frame (in
 * bits). The encoder will try to make sure that each frame does not
 * exceed this size. Otherwise, if the frame size exceeds this size,
 * the \c status flag of #VASTCodedBufferSegment will contain
 * #VA_CODED_BUF_STATUS_FRAME_SIZE_OVERFLOW.
 */
typedef struct _VASTEncMiscParameterBufferMaxFrameSize {
    /** \brief Type. Shall be set to #VASTEncMiscParameterTypeMaxFrameSize. */
    VASTEncMiscParameterTypeai      type;
    /** \brief Maximum size of a frame (in bits). */
    uint32_t                max_frame_size;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTEncMiscParameterBufferMaxFrameSize;

/**
 * \brief Encoding quality level.
 *
 * The encoding quality could be set through this structure, if the implementation
 * supports multiple quality levels. The quality level set through this structure is
 * persistent over the entire coded sequence, or until a new structure is being sent.
 * The quality level range can be queried through the VASTConfigAttribEncQualityRange
 * attribute. A lower value means higher quality, and a value of 1 represents the highest
 * quality. The quality level setting is used as a trade-off between quality and speed/power
 * consumption, with higher quality corresponds to lower speed and higher power consumption.
 */
typedef struct _VASTEncMiscParameterBufferQualityLevel {
    /** \brief Encoding quality level setting. When set to 0, default quality
     * level is used.
     */
    uint32_t                quality_level;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTEncMiscParameterBufferQualityLevel;

/**
 * \brief Quantization settings for encoding.
 *
 * Some encoders support special types of quantization such as trellis, and this structure
 * can be used by the app to control these special types of quantization by the encoder.
 */
typedef struct _VASTEncMiscParameterQuantization
{
    union
    {
    /* if no flags is set then quantization is determined by the driver */
        struct
        {
	    /* \brief disable trellis for all frames/fields */
            uint64_t disable_trellis : 1;
	    /* \brief enable trellis for I frames/fields */
            uint64_t enable_trellis_I : 1;
	    /* \brief enable trellis for P frames/fields */
            uint64_t enable_trellis_P : 1;
	    /* \brief enable trellis for B frames/fields */
            uint64_t enable_trellis_B : 1;
            uint64_t reserved : 28;
        } bits;
        uint64_t value;
    } quantization_flags;
} VASTEncMiscParameterQuantization;

/**
 * \brief Encoding skip frame.
 *
 * The application may choose to skip frames externally to the encoder (e.g. drop completely or
 * code as all skip's). For rate control purposes the encoder will need to know the size and number
 * of skipped frames.  Skip frame(s) indicated through this structure is applicable only to the
 * current frame.  It is allowed for the application to still send in packed headers for the driver to
 * pack, although no frame will be encoded (e.g. for HW to encrypt the frame).
 */
typedef struct _VASTEncMiscParameterSkipFrame {
    /** \brief Indicates skip frames as below.
      * 0: Encode as normal, no skip.
      * 1: One or more frames were skipped prior to the current frame, encode the current frame as normal.
      * 2: The current frame is to be skipped, do not encode it but pack/encrypt the packed header contents
      *    (all except VASTEncPackedHeaderSlice) which could contain actual frame contents (e.g. pack the frame
      *    in VASTEncPackedHeaderPicture).  */
    uint8_t               skip_frame_flag;
    /** \brief The number of frames skipped prior to the current frame.  Valid when skip_frame_flag = 1. */
    uint8_t               num_skip_frames;
    /** \brief When skip_frame_flag = 1, the size of the skipped frames in bits.   When skip_frame_flag = 2,
      * the size of the current skipped frame that is to be packed/encrypted in bits. */
    uint32_t                size_skip_frames;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTEncMiscParameterSkipFrame;

/**
 * \brief Encoding region-of-interest (ROI).
 *
 * The encoding ROI can be set through VASTEncMiscParameterBufferROI, if the implementation
 * supports ROI input. The ROI set through this structure is applicable only to the
 * current frame or field, so must be sent every frame or field to be applied.  The number of
 * supported ROIs can be queried through the VASTConfigAttribEncROI.  The encoder will use the
 * ROI information to adjust the QP values of the MB's that fall within the ROIs.
 */
typedef struct _VASTEncROI
{
        /** \brief Defines the ROI boundary in pixels, the driver will map it to appropriate
         *  codec coding units.  It is relative to frame coordinates for the frame case and
         *  to field coordinates for the field case. */
        VASTRectangle     roi_rectangle;
        /**
         * \brief ROI value
         *
         * \ref roi_value specifies ROI delta QP or ROI priority.
         * --  ROI delta QP is the value that will be added on top of the frame level QP.
         * --  ROI priority specifies the priority of a region, it can be positive (more important)
         * or negative (less important) values and is compared with non-ROI region (taken as value 0),
         * E.g. ROI region with \ref roi_value -3 is less important than the non-ROI region (\ref roi_value
         * implied to be 0) which is less important than ROI region with roi_value +2. For overlapping
         * regions, the roi_value that is first in the ROI array will have priority.
         *
         * \ref roi_value always specifes ROI delta QP when VASTConfigAttribRateControl == VAST_RC_CQP , no matter
         * the value of \c roi_value_is_qp_delta in #VASTEncMiscParameterBufferROI.
         *
         * \ref roi_value depends on \c roi_value_is_qp_delta in #VASTEncMiscParameterBufferROI when
         * VASTConfigAttribRateControl != VAST_RC_CQP . \ref roi_value specifies ROI_delta QP if \c roi_value_is_qp_delta
         * in VASTEncMiscParameterBufferROI is 1, otherwise \ref roi_value specifies ROI priority.
         */
        int32_t            roi_value;//vastai fix bug
} VASTEncROI;

typedef struct _VASTEncMiscParameterBufferROI {
    /** \brief Number of ROIs being sent.*/
    uint32_t        num_roi;

    /** \brief Valid when VASTConfigAttribRateControl != VAST_RC_CQP , then the encoder's
     *  rate control will determine actual delta QPs.  Specifies the max/min allowed delta
     *  QPs. */
    int8_t                max_delta_qp;
    int8_t                min_delta_qp;

   /** \brief Pointer to a VASTEncROI array with num_roi elements.  It is relative to frame
     *  coordinates for the frame case and to field coordinates for the field case.*/
    VASTEncROI            *roi;
    union {
        struct {
            /**
             * \brief An indication for roi value.
             *
             * \ref roi_value_is_qp_delta equal to 1 indicates \c roi_value in #VASTEncROI should
             * be used as ROI delta QP. \ref roi_value_is_qp_delta equal to 0 indicates \c roi_value
             * in #VASTEncROI should be used as ROI priority.
             *
             * \ref roi_value_is_qp_delta is only available when VASTConfigAttribRateControl != VAST_RC_CQP ,
             * the setting must comply with \c roi_rc_priority_support and \c roi_rc_qp_delta_support in
             * #VASTConfigAttribValEncROI. The underlying driver should ignore this field
             * when VASTConfigAttribRateControl == VAST_RC_CQP .
             */
            uint32_t  roi_value_is_qp_delta    : 1;
            uint32_t  reserved                 : 31;
        } bits;
        uint32_t value;
    } roi_flags;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTEncMiscParameterBufferROI;
/*
 * \brief Dirty rectangle data structure for encoding.
 *
 * The encoding dirty rect can be set through VASTEncMiscParameterBufferDirtyRect, if the
 * implementation supports dirty rect input. The rect set through this structure is applicable
 * only to the current frame or field, so must be sent every frame or field to be applied.
 * The number of supported rects can be queried through the VASTConfigAttribEncDirtyRect.  The
 * encoder will use the rect information to know those rectangle areas have changed while the
 * areas not covered by dirty rect rectangles are assumed to have not changed compared to the
 * previous picture.  The encoder may do some internal optimizations.
 */
typedef struct _VASTEncMiscParameterBufferDirtyRect
{
    /** \brief Number of Rectangle being sent.*/
    uint32_t    num_roi_rectangle;

    /** \brief Pointer to a VASTRectangle array with num_roi_rectangle elements.*/
     VASTRectangle    *roi_rectangle;
} VASTEncMiscParameterBufferDirtyRect;

/** \brief Attribute value for VASTConfigAttribEncParallelRateControl */
typedef struct _VASTEncMiscParameterParallelRateControl {
    /** brief Number of layers*/
    uint32_t num_layers;
    /** brief Number of B frames per layer per GOP.
     *
     * it should be allocated by application, and the is num_layers.
     *  num_b_in_gop[0] is the number of regular B which refers to only I or P frames. */
    uint32_t *num_b_in_gop;
} VASTEncMiscParameterParallelRateControl;

/** per frame encoder quality controls, once set they will persist for all future frames
  *till it is updated again. */
typedef struct _VASTEncMiscParameterEncQuality
{
    union
    {
        struct
        {
            /** Use raw frames for reference instead of reconstructed frames.
              * it only impact motion estimation (ME)  stage, and will not impact MC stage
              * so the reconstruct picture will can match with decode side */
            uint32_t useRawPicForRef                    : 1;
            /**  Disables skip check for ME stage, it will increase the bistream size
              * but will improve the qulity */
            uint32_t skipCheckDisable                   : 1;
            /**  Indicates app will override default driver FTQ settings using FTQEnable.
              *  FTQ is forward transform quantization */
            uint32_t FTQOverride                        : 1;
            /** Enables/disables FTQ. */
            uint32_t FTQEnable                          : 1;
            /** Indicates the app will provide the Skip Threshold LUT to use when FTQ is
              * enabled (FTQSkipThresholdLUT), else default driver thresholds will be used. */
            uint32_t FTQSkipThresholdLUTInput           : 1;
            /** Indicates the app will provide the Skip Threshold LUT to use when FTQ is
              * disabled (NonFTQSkipThresholdLUT), else default driver thresholds will be used. */
            uint32_t NonFTQSkipThresholdLUTInput        : 1;
            uint32_t ReservedBit                        : 1;
            /** Control to enable the ME mode decision algorithm to bias to fewer B Direct/Skip types.
              * Applies only to B frames, all other frames will ignore this setting.  */
            uint32_t directBiasAdjustmentEnable         : 1;
            /** Enables global motion bias. global motion also is called HME (Heirarchical Motion Estimation )
              * HME is used to handle large motions and avoiding local minima in the video encoding process
              * down scaled the input and reference picture, then do ME. the result will be a predictor to next level HME or ME
              * current interface divide the HME to 3 level. UltraHME , SuperHME, and HME, result of UltraHME will be input of SurperHME,
              * result of superHME will be a input for HME. HME result will be input of ME. it is a switch for HMEMVCostScalingFactor
              * can change the HME bias inside RDO stage*/
            uint32_t globalMotionBiasAdjustmentEnable   : 1;
            /** MV cost scaling ratio for HME ( predictors.  It is used when
              * globalMotionBiasAdjustmentEnable == 1, else it is ignored.  Values are:
              *     0: set MV cost to be 0 for HME predictor.
              *     1: scale MV cost to be 1/2 of the default value for HME predictor.
              *     2: scale MV cost to be 1/4 of the default value for HME predictor.
              *     3: scale MV cost to be 1/8 of the default value for HME predictor. */
            uint32_t HMEMVCostScalingFactor             : 2;
            /**disable HME, if it is disabled. Super*ultraHME should also be disabled  */
            uint32_t HMEDisable                         : 1;
            /**disable Super HME, if it is disabled, ultraHME should be disabled */
            uint32_t SuperHMEDisable                    : 1;
            /** disable Ultra HME */
            uint32_t UltraHMEDisable                    : 1;
            /** disable panic mode. Panic mode happened when there are extreme BRC (bit rate control) requirement
              * frame size cant achieve the target of BRC.  when Panic mode is triggered, Coefficients will
              *  be set to zero. disable panic mode will improve quality but will impact BRC */
            uint32_t PanicModeDisable                   : 1;
            /** Force RepartitionCheck
             *  0: DEFAULT - follow driver default settings.
             *  1: FORCE_ENABLE - enable this feature totally for all cases.
             *  2: FORCE_DISABLE - disable this feature totally for all cases. */
            uint32_t ForceRepartitionCheck              : 2;

        };
        uint32_t encControls;
    };

    /** Maps QP to skip thresholds when FTQ is enabled.  Valid range is 0-255. */
    uint8_t FTQSkipThresholdLUT[52];
    /** Maps QP to skip thresholds when FTQ is disabled.  Valid range is 0-65535. */
    uint16_t NonFTQSkipThresholdLUT[52];

    uint32_t reserved[VAST_PADDING_HIGH];  // Reserved for future use.

} VASTEncMiscParameterEncQuality;

/**
 *  \brief Custom Encoder Rounding Offset Control.
 *  Application may use this structure to set customized rounding
 *  offset parameters for quantization.
 *  Valid when \c VASTConfigAttribCustomRoundingControl equals 1.
 */
typedef struct _VASTEncMiscParameterCustomRoundingControl
{
    union {
        struct {
            /** \brief Enable customized rounding offset for intra blocks.
             *  If 0, default value would be taken by driver for intra
             *  rounding offset.
             */
            uint32_t    enable_custom_rouding_intra     : 1 ;

            /** \brief Intra rounding offset
             *  Ignored if \c enable_custom_rouding_intra equals 0.
             */
            uint32_t    rounding_offset_intra           : 7;

            /** \brief Enable customized rounding offset for inter blocks.
             *  If 0, default value would be taken by driver for inter
             *  rounding offset.
             */
            uint32_t    enable_custom_rounding_inter    : 1 ;

            /** \brief Inter rounding offset
             *  Ignored if \c enable_custom_rouding_inter equals 0.
             */
            uint32_t    rounding_offset_inter           : 7;

           /* Reserved */
            uint32_t    reserved                        :16;
        }  bits;
        uint32_t    value;
    }   rounding_offset_setting;
} VASTEncMiscParameterCustomRoundingControl;
/**
 * There will be cases where the bitstream buffer will not have enough room to hold
 * the data for the entire slice, and the following flags will be used in the slice
 * parameter to signal to the server for the possible cases.
 * If a slice parameter buffer and slice data buffer pair is sent to the server with
 * the slice data partially in the slice data buffer (BEGIN and MIDDLE cases below),
 * then a slice parameter and data buffer needs to be sent again to complete this slice.
 */
#define VAST_SLICE_DATA_FLAG_ALL		0x00	/* whole slice is in the buffer */
#define VAST_SLICE_DATA_FLAG_BEGIN	0x01	/* The beginning of the slice is in the buffer but the end if not */
#define VAST_SLICE_DATA_FLAG_MIDDLE	0x02	/* Neither beginning nor end of the slice is in the buffer */
#define VAST_SLICE_DATA_FLAG_END		0x04	/* end of the slice is in the buffer */

/* Codec-independent Slice Parameter Buffer base */
typedef struct _VASTSliceParameterBufferBase
{
    uint32_t slice_data_size;	/* number of bytes in the slice data buffer for this slice */
    uint32_t slice_data_offset;	/* the offset to the first byte of slice data */
    uint32_t slice_data_flag;	/* see VA_SLICE_DATA_FLAG_XXX definitions */
} VASTSliceParameterBufferBase;

/**********************************
 * JPEG common  data structures
 **********************************/
/**
 * \brief Huffman table for JPEG decoding.
 *
 * This structure holds the complete Huffman tables. This is an
 * aggregation of all Huffman table (DHT) segments maintained by the
 * application. i.e. up to 2 Huffman tables are stored in there for
 * baseline profile.
 *
 * The #load_huffman_table array can be used as a hint to notify the
 * VA driver implementation about which table(s) actually changed
 * since the last submission of this buffer.
 */
typedef struct _VASTHuffmanTableBufferJPEGBaseline {
    /** \brief Specifies which #huffman_table is valid. */
    uint8_t       load_huffman_table[2];
    /** \brief Huffman tables indexed by table identifier (Th). */
    struct {
        /** @name DC table (up to 12 categories) */
        /**@{*/
        /** \brief Number of Huffman codes of length i + 1 (Li). */
        uint8_t   num_dc_codes[16];
        /** \brief Value associated with each Huffman code (Vij). */
        uint8_t   dc_values[12];
        /**@}*/
        /** @name AC table (2 special codes + up to 16 * 10 codes) */
        /**@{*/
        /** \brief Number of Huffman codes of length i + 1 (Li). */
        uint8_t   num_ac_codes[16];
        /** \brief Value associated with each Huffman code (Vij). */
        uint8_t   ac_values[162];
        /** \brief Padding to 4-byte boundaries. Must be set to zero. */
        uint8_t   pad[2];
        /**@}*/
    }                   huffman_table[2];

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTHuffmanTableBufferJPEGBaseline;

/****************************
 * MPEG-2 data structures
 ****************************/

/* MPEG-2 Picture Parameter Buffer */
/*
 * For each frame or field, and before any slice data, a single
 * picture parameter buffer must be send.
 */
typedef struct _VASTPictureParameterBufferMPEG2
{
    uint16_t horizontal_size;
    uint16_t vertical_size;
    VASTSurfaceID forward_reference_picture;
    VASTSurfaceID backward_reference_picture;
    /* meanings of the following fields are the same as in the standard */
    int32_t picture_coding_type;
    int32_t f_code; /* pack all four fcode into this */
    union {
        struct {
            uint32_t intra_dc_precision		: 2;
            uint32_t picture_structure		: 2;
            uint32_t top_field_first		: 1;
            uint32_t frame_pred_frame_dct		: 1;
            uint32_t concealment_motion_vectors	: 1;
            uint32_t q_scale_type			: 1;
            uint32_t intra_vlc_format		: 1;
            uint32_t alternate_scan			: 1;
            uint32_t repeat_first_field		: 1;
            uint32_t progressive_frame		: 1;
            uint32_t is_first_field			: 1; /* indicate whether the current field
                                                              * is the first field for field picture
                                                              */
        } bits;
        uint32_t value;
    } picture_coding_extension;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTPictureParameterBufferMPEG2;

/** MPEG-2 Inverse Quantization Matrix Buffer */
typedef struct _VASTIQMatrixBufferMPEG2
{
    /** \brief Same as the MPEG-2 bitstream syntax element. */
    int32_t load_intra_quantiser_matrix;
    /** \brief Same as the MPEG-2 bitstream syntax element. */
    int32_t load_non_intra_quantiser_matrix;
    /** \brief Same as the MPEG-2 bitstream syntax element. */
    int32_t load_chroma_intra_quantiser_matrix;
    /** \brief Same as the MPEG-2 bitstream syntax element. */
    int32_t load_chroma_non_intra_quantiser_matrix;
    /** \brief Luminance intra matrix, in zig-zag scan order. */
    uint8_t intra_quantiser_matrix[64];
    /** \brief Luminance non-intra matrix, in zig-zag scan order. */
    uint8_t non_intra_quantiser_matrix[64];
    /** \brief Chroma intra matrix, in zig-zag scan order. */
    uint8_t chroma_intra_quantiser_matrix[64];
    /** \brief Chroma non-intra matrix, in zig-zag scan order. */
    uint8_t chroma_non_intra_quantiser_matrix[64];

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTIQMatrixBufferMPEG2;

/** MPEG-2 Slice Parameter Buffer */
typedef struct _VASTSliceParameterBufferMPEG2
{
    uint32_t slice_data_size;/* number of bytes in the slice data buffer for this slice */
    uint32_t slice_data_offset;/* the offset to the first byte of slice data */
    uint32_t slice_data_flag; /* see VA_SLICE_DATA_FLAG_XXX defintions */
    uint32_t macroblock_offset;/* the offset to the first bit of MB from the first byte of slice data */
    uint32_t slice_horizontal_position;
    uint32_t slice_vertical_position;
    int32_t quantiser_scale_code;
    int32_t intra_slice_flag;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTSliceParameterBufferMPEG2;

/** MPEG-2 Macroblock Parameter Buffer */
typedef struct _VASTMacroblockParameterBufferMPEG2
{
    uint16_t macroblock_address;
    /*
     * macroblock_address (in raster scan order)
     * top-left: 0
     * bottom-right: picture-height-in-mb*picture-width-in-mb - 1
     */
    uint8_t macroblock_type;  /* see definition below */
    union {
        struct {
            uint32_t frame_motion_type		: 2;
            uint32_t field_motion_type		: 2;
            uint32_t dct_type			: 1;
        } bits;
        uint32_t value;
    } macroblock_modes;
    uint8_t motion_vertical_field_select;
    /*
     * motion_vertical_field_select:
     * see section 6.3.17.2 in the spec
     * only the lower 4 bits are used
     * bit 0: first vector forward
     * bit 1: first vector backward
     * bit 2: second vector forward
     * bit 3: second vector backward
     */
    int16_t PMV[2][2][2]; /* see Table 7-7 in the spec */
    uint16_t coded_block_pattern;
    /*
     * The bitplanes for coded_block_pattern are described
     * in Figure 6.10-12 in the spec
     */

    /* Number of skipped macroblocks after this macroblock */
    uint16_t num_skipped_macroblocks;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTMacroblockParameterBufferMPEG2;

/*
 * OR'd flags for macroblock_type (section 6.3.17.1 in the spec)
 */
#define VAST_MB_TYPE_MOTION_FORWARD	0x02
#define VAST_MB_TYPE_MOTION_BACKWARD	0x04
#define VAST_MB_TYPE_MOTION_PATTERN	0x08
#define VAST_MB_TYPE_MOTION_INTRA		0x10

/**
 * MPEG-2 Residual Data Buffer
 * For each macroblock, there wil be 64 shorts (16-bit) in the
 * residual data buffer
 */

/****************************
 * MPEG-4 Part 2 data structures
 ****************************/

/* MPEG-4 Picture Parameter Buffer */
/*
 * For each frame or field, and before any slice data, a single
 * picture parameter buffer must be send.
 */
typedef struct _VASTPictureParameterBufferMPEG4
{
    uint16_t vop_width;
    uint16_t vop_height;
    VASTSurfaceID forward_reference_picture;
    VASTSurfaceID backward_reference_picture;
    union {
        struct {
            uint32_t short_video_header		: 1;
            uint32_t chroma_format			: 2;
            uint32_t interlaced			: 1;
            uint32_t obmc_disable			: 1;
            uint32_t sprite_enable			: 2;
            uint32_t sprite_warping_accuracy	: 2;
            uint32_t quant_type			: 1;
            uint32_t quarter_sample			: 1;
            uint32_t data_partitioned		: 1;
            uint32_t reversible_vlc			: 1;
            uint32_t resync_marker_disable		: 1;
        } bits;
        uint32_t value;
    } vol_fields;
    uint8_t no_of_sprite_warping_points;
    int16_t sprite_trajectory_du[3];
    int16_t sprite_trajectory_dv[3];
    uint8_t quant_precision;
    union {
        struct {
            uint32_t vop_coding_type		: 2;
            uint32_t backward_reference_vop_coding_type	: 2;
            uint32_t vop_rounding_type		: 1;
            uint32_t intra_dc_vlc_thr		: 3;
            uint32_t top_field_first		: 1;
            uint32_t alternate_vertical_scan_flag	: 1;
        } bits;
        uint32_t value;
    } vop_fields;
    uint8_t vop_fcode_forward;
    uint8_t vop_fcode_backward;
    uint16_t vop_time_increment_resolution;
    /* short header related */
    uint8_t num_gobs_in_vop;
    uint8_t num_macroblocks_in_gob;
    /* for direct mode prediction */
    int16_t TRB;
    int16_t TRD;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTPictureParameterBufferMPEG4;

/** MPEG-4 Inverse Quantization Matrix Buffer */
typedef struct _VASTIQMatrixBufferMPEG4
{
    /** Same as the MPEG-4:2 bitstream syntax element. */
    int32_t load_intra_quant_mat;
    /** Same as the MPEG-4:2 bitstream syntax element. */
    int32_t load_non_intra_quant_mat;
    /** The matrix for intra blocks, in zig-zag scan order. */
    uint8_t intra_quant_mat[64];
    /** The matrix for non-intra blocks, in zig-zag scan order. */
    uint8_t non_intra_quant_mat[64];

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTIQMatrixBufferMPEG4;

/** MPEG-4 Slice Parameter Buffer */
typedef struct _VASTSliceParameterBufferMPEG4
{
    uint32_t slice_data_size;/* number of bytes in the slice data buffer for this slice */
    uint32_t slice_data_offset;/* the offset to the first byte of slice data */
    uint32_t slice_data_flag; /* see VA_SLICE_DATA_FLAG_XXX defintions */
    uint32_t macroblock_offset;/* the offset to the first bit of MB from the first byte of slice data */
    uint32_t macroblock_number;
    int32_t quant_scale;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTSliceParameterBufferMPEG4;

/**
 VC-1 data structures
*/

typedef enum   /* see 7.1.1.32 */
{
    VASTMvMode1Mv                        = 0,
    VASTMvMode1MvHalfPel                 = 1,
    VASTMvMode1MvHalfPelBilinear         = 2,
    VASTMvModeMixedMv                    = 3,
    VASTMvModeIntensityCompensation      = 4
} VASTMvModeVC1;

/** VC-1 Picture Parameter Buffer */
/*
 * For each picture, and before any slice data, a picture parameter
 * buffer must be send. Multiple picture parameter buffers may be
 * sent for a single picture. In that case picture parameters will
 * apply to all slice data that follow it until a new picture
 * parameter buffer is sent.
 *
 * Notes:
 *   pic_quantizer_type should be set to the applicable quantizer
 *   type as defined by QUANTIZER (J.1.19) and either
 *   PQUANTIZER (7.1.1.8) or PQINDEX (7.1.1.6)
 */
typedef struct _VASTPictureParameterBufferVC1
{
    VASTSurfaceID forward_reference_picture;
    VASTSurfaceID backward_reference_picture;
    /* if out-of-loop post-processing is done on the render
       target, then we need to keep the in-loop decoded
       picture as a reference picture */
    VASTSurfaceID inloop_decoded_picture;

    /* sequence layer for AP or meta data for SP and MP */
    union {
        struct {
            uint32_t pulldown	: 1; /* SEQUENCE_LAYER::PULLDOWN */
            uint32_t interlace	: 1; /* SEQUENCE_LAYER::INTERLACE */
            uint32_t tfcntrflag	: 1; /* SEQUENCE_LAYER::TFCNTRFLAG */
            uint32_t finterpflag	: 1; /* SEQUENCE_LAYER::FINTERPFLAG */
            uint32_t psf		: 1; /* SEQUENCE_LAYER::PSF */
            uint32_t multires	: 1; /* METADATA::MULTIRES */
            uint32_t overlap	: 1; /* METADATA::OVERLAP */
            uint32_t syncmarker	: 1; /* METADATA::SYNCMARKER */
            uint32_t rangered	: 1; /* METADATA::RANGERED */
            uint32_t max_b_frames	: 3; /* METADATA::MAXBFRAMES */
            uint32_t profile	: 2; /* SEQUENCE_LAYER::PROFILE or The MSB of METADATA::PROFILE */
        } bits;
        uint32_t value;
    } sequence_fields;

    uint16_t coded_width;		/* ENTRY_POINT_LAYER::CODED_WIDTH */
    uint16_t coded_height;	/* ENTRY_POINT_LAYER::CODED_HEIGHT */
    union {
	struct {
            uint32_t broken_link	: 1; /* ENTRY_POINT_LAYER::BROKEN_LINK */
            uint32_t closed_entry	: 1; /* ENTRY_POINT_LAYER::CLOSED_ENTRY */
            uint32_t panscan_flag	: 1; /* ENTRY_POINT_LAYER::PANSCAN_FLAG */
            uint32_t loopfilter	: 1; /* ENTRY_POINT_LAYER::LOOPFILTER */
	} bits;
	uint32_t value;
    } entrypoint_fields;
    uint8_t conditional_overlap_flag; /* ENTRY_POINT_LAYER::CONDOVER */
    uint8_t fast_uvmc_flag;	/* ENTRY_POINT_LAYER::FASTUVMC */
    union {
        struct {
            uint32_t luma_flag	: 1; /* ENTRY_POINT_LAYER::RANGE_MAPY_FLAG */
            uint32_t luma		: 3; /* ENTRY_POINT_LAYER::RANGE_MAPY */
            uint32_t chroma_flag	: 1; /* ENTRY_POINT_LAYER::RANGE_MAPUV_FLAG */
            uint32_t chroma		: 3; /* ENTRY_POINT_LAYER::RANGE_MAPUV */
        } bits;
        uint32_t value;
    } range_mapping_fields;

    uint8_t b_picture_fraction;	/* Index for PICTURE_LAYER::BFRACTION value in Table 40 (7.1.1.14) */
    uint8_t cbp_table;		/* PICTURE_LAYER::CBPTAB/ICBPTAB */
    uint8_t mb_mode_table;	/* PICTURE_LAYER::MBMODETAB */
    uint8_t range_reduction_frame;/* PICTURE_LAYER::RANGEREDFRM */
    uint8_t rounding_control;	/* PICTURE_LAYER::RNDCTRL */
    uint8_t post_processing;	/* PICTURE_LAYER::POSTPROC */
    uint8_t picture_resolution_index;	/* PICTURE_LAYER::RESPIC */
    uint8_t luma_scale;		/* PICTURE_LAYER::LUMSCALE */
    uint8_t luma_shift;		/* PICTURE_LAYER::LUMSHIFT */

    union {
        struct {
            uint32_t picture_type		: 3; /* PICTURE_LAYER::PTYPE */
            uint32_t frame_coding_mode	: 3; /* PICTURE_LAYER::FCM */
            uint32_t top_field_first	: 1; /* PICTURE_LAYER::TFF */
            uint32_t is_first_field		: 1; /* set to 1 if it is the first field */
            uint32_t intensity_compensation	: 1; /* PICTURE_LAYER::INTCOMP */
        } bits;
        uint32_t value;
    } picture_fields;
    union {
        struct {
            uint32_t mv_type_mb	: 1; 	/* PICTURE::MVTYPEMB */
            uint32_t direct_mb	: 1; 	/* PICTURE::DIRECTMB */
            uint32_t skip_mb	: 1; 	/* PICTURE::SKIPMB */
            uint32_t field_tx	: 1; 	/* PICTURE::FIELDTX */
            uint32_t forward_mb	: 1;	/* PICTURE::FORWARDMB */
            uint32_t ac_pred	: 1;	/* PICTURE::ACPRED */
            uint32_t overflags	: 1;	/* PICTURE::OVERFLAGS */
        } flags;
        uint32_t value;
    } raw_coding;
    union {
        struct {
            uint32_t bp_mv_type_mb   : 1;    /* PICTURE::MVTYPEMB */
            uint32_t bp_direct_mb    : 1;    /* PICTURE::DIRECTMB */
            uint32_t bp_skip_mb      : 1;    /* PICTURE::SKIPMB */
            uint32_t bp_field_tx     : 1;    /* PICTURE::FIELDTX */
            uint32_t bp_forward_mb   : 1;    /* PICTURE::FORWARDMB */
            uint32_t bp_ac_pred      : 1;    /* PICTURE::ACPRED */
            uint32_t bp_overflags    : 1;    /* PICTURE::OVERFLAGS */
        } flags;
        uint32_t value;
    } bitplane_present; /* signal what bitplane is being passed via the bitplane buffer */
    union {
        struct {
            uint32_t reference_distance_flag : 1;/* PICTURE_LAYER::REFDIST_FLAG */
            uint32_t reference_distance	: 5;/* PICTURE_LAYER::REFDIST */
            uint32_t num_reference_pictures: 1;/* PICTURE_LAYER::NUMREF */
            uint32_t reference_field_pic_indicator	: 1;/* PICTURE_LAYER::REFFIELD */
        } bits;
        uint32_t value;
    } reference_fields;
    union {
        struct {
            uint32_t mv_mode		: 3; /* PICTURE_LAYER::MVMODE */
            uint32_t mv_mode2		: 3; /* PICTURE_LAYER::MVMODE2 */
            uint32_t mv_table		: 3; /* PICTURE_LAYER::MVTAB/IMVTAB */
            uint32_t two_mv_block_pattern_table: 2; /* PICTURE_LAYER::2MVBPTAB */
            uint32_t four_mv_switch		: 1; /* PICTURE_LAYER::4MVSWITCH */
            uint32_t four_mv_block_pattern_table : 2; /* PICTURE_LAYER::4MVBPTAB */
            uint32_t extended_mv_flag	: 1; /* ENTRY_POINT_LAYER::EXTENDED_MV */
            uint32_t extended_mv_range	: 2; /* PICTURE_LAYER::MVRANGE */
            uint32_t extended_dmv_flag	: 1; /* ENTRY_POINT_LAYER::EXTENDED_DMV */
            uint32_t extended_dmv_range	: 2; /* PICTURE_LAYER::DMVRANGE */
        } bits;
        uint32_t value;
    } mv_fields;
    union {
        struct {
            uint32_t dquant	: 2; 	/* ENTRY_POINT_LAYER::DQUANT */
            uint32_t quantizer     : 2; 	/* ENTRY_POINT_LAYER::QUANTIZER */
            uint32_t half_qp	: 1; 	/* PICTURE_LAYER::HALFQP */
            uint32_t pic_quantizer_scale : 5;/* PICTURE_LAYER::PQUANT */
            uint32_t pic_quantizer_type : 1;/* PICTURE_LAYER::PQUANTIZER */
            uint32_t dq_frame	: 1; 	/* VOPDQUANT::DQUANTFRM */
            uint32_t dq_profile	: 2; 	/* VOPDQUANT::DQPROFILE */
            uint32_t dq_sb_edge	: 2; 	/* VOPDQUANT::DQSBEDGE */
            uint32_t dq_db_edge 	: 2; 	/* VOPDQUANT::DQDBEDGE */
            uint32_t dq_binary_level : 1; 	/* VOPDQUANT::DQBILEVEL */
            uint32_t alt_pic_quantizer : 5;/* VOPDQUANT::ALTPQUANT */
        } bits;
        uint32_t value;
    } pic_quantizer_fields;
    union {
        struct {
            uint32_t variable_sized_transform_flag	: 1;/* ENTRY_POINT_LAYER::VSTRANSFORM */
            uint32_t mb_level_transform_type_flag	: 1;/* PICTURE_LAYER::TTMBF */
            uint32_t frame_level_transform_type	: 2;/* PICTURE_LAYER::TTFRM */
            uint32_t transform_ac_codingset_idx1	: 2;/* PICTURE_LAYER::TRANSACFRM */
            uint32_t transform_ac_codingset_idx2	: 2;/* PICTURE_LAYER::TRANSACFRM2 */
            uint32_t intra_transform_dc_table	: 1;/* PICTURE_LAYER::TRANSDCTAB */
        } bits;
        uint32_t value;
    } transform_fields;

    uint8_t luma_scale2;                  /* PICTURE_LAYER::LUMSCALE2 */
    uint8_t luma_shift2;                  /* PICTURE_LAYER::LUMSHIFT2 */
    uint8_t intensity_compensation_field; /* Index for PICTURE_LAYER::INTCOMPFIELD value in Table 109 (9.1.1.48) */

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_MEDIUM - 1];
} VASTPictureParameterBufferVC1;

/** VC-1 Bitplane Buffer
There will be at most three bitplanes coded in any picture header. To send
the bitplane data more efficiently, each byte is divided in two nibbles, with
each nibble carrying three bitplanes for one macroblock.  The following table
shows the bitplane data arrangement within each nibble based on the picture
type.

Picture Type	Bit3		Bit2		Bit1		Bit0
I or BI				OVERFLAGS	ACPRED		FIELDTX
P				MYTYPEMB	SKIPMB		DIRECTMB
B				FORWARDMB	SKIPMB		DIRECTMB

Within each byte, the lower nibble is for the first MB and the upper nibble is
for the second MB.  E.g. the lower nibble of the first byte in the bitplane
buffer is for Macroblock #1 and the upper nibble of the first byte is for
Macroblock #2 in the first row.
*/

/* VC-1 Slice Parameter Buffer */
typedef struct _VASTSliceParameterBufferVC1
{
    uint32_t slice_data_size;/* number of bytes in the slice data buffer for this slice */
    uint32_t slice_data_offset;/* the offset to the first byte of slice data */
    uint32_t slice_data_flag; /* see VA_SLICE_DATA_FLAG_XXX defintions */
    uint32_t macroblock_offset;/* the offset to the first bit of MB from the first byte of slice data */
    uint32_t slice_vertical_position;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTSliceParameterBufferVC1;

/* VC-1 Slice Data Buffer */
/*
This is simplely a buffer containing raw bit-stream bytes
*/

/****************************
 * H.264/AVC data structures
 ****************************/

typedef struct _VASTPictureH264
{
    VASTSurfaceID picture_id;
    uint32_t frame_idx;
    uint32_t flags;
    int32_t TopFieldOrderCnt;
    int32_t BottomFieldOrderCnt;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTPictureH264;
/* flags in VASTPictureH264 could be OR of the following */
#define VAST_PICTURE_H264_INVALID			0x00000001
#define VAST_PICTURE_H264_TOP_FIELD		0x00000002
#define VAST_PICTURE_H264_BOTTOM_FIELD		0x00000004
#define VAST_PICTURE_H264_SHORT_TERM_REFERENCE	0x00000008
#define VAST_PICTURE_H264_LONG_TERM_REFERENCE	0x00000010

/** H.264 Picture Parameter Buffer */
/*
 * For each picture, and before any slice data, a single
 * picture parameter buffer must be send.
 */
typedef struct _VASTPictureParameterBufferH264
{
    VASTPictureH264 CurrPic;
    VASTPictureH264 ReferenceFrames[16];	/* in DPB */
    uint16_t picture_width_in_mbs_minus1;
    uint16_t picture_height_in_mbs_minus1;
    uint8_t bit_depth_luma_minus8;
    uint8_t bit_depth_chroma_minus8;
    uint8_t num_ref_frames;
    union {
        struct {
            uint32_t chroma_format_idc			: 2;
            uint32_t residual_colour_transform_flag		: 1; /* Renamed to separate_colour_plane_flag in newer standard versions. */
            uint32_t gaps_in_frame_num_value_allowed_flag	: 1;
            uint32_t frame_mbs_only_flag			: 1;
            uint32_t mb_adaptive_frame_field_flag		: 1;
            uint32_t direct_8x8_inference_flag		: 1;
            uint32_t MinLumaBiPredSize8x8			: 1; /* see A.3.3.2 */
            uint32_t log2_max_frame_num_minus4		: 4;
            uint32_t pic_order_cnt_type			: 2;
            uint32_t log2_max_pic_order_cnt_lsb_minus4	: 4;
            uint32_t delta_pic_order_always_zero_flag	: 1;
        } bits;
        uint32_t value;
    } seq_fields;
    // FMO is not supported.
    va_deprecated uint8_t num_slice_groups_minus1;
    va_deprecated uint8_t slice_group_map_type;
    va_deprecated uint16_t slice_group_change_rate_minus1;
    int8_t pic_init_qp_minus26;
    int8_t pic_init_qs_minus26;
    int8_t chroma_qp_index_offset;
    int8_t second_chroma_qp_index_offset;
    union {
        struct {
            uint32_t entropy_coding_mode_flag	: 1;
            uint32_t weighted_pred_flag		: 1;
            uint32_t weighted_bipred_idc		: 2;
            uint32_t transform_8x8_mode_flag	: 1;
            uint32_t field_pic_flag			: 1;
            uint32_t constrained_intra_pred_flag	: 1;
            uint32_t pic_order_present_flag			: 1; /* Renamed to bottom_field_pic_order_in_frame_present_flag in newer standard versions. */
            uint32_t deblocking_filter_control_present_flag : 1;
            uint32_t redundant_pic_cnt_present_flag		: 1;
            uint32_t reference_pic_flag			: 1; /* nal_ref_idc != 0 */
        } bits;
        uint32_t value;
    } pic_fields;
    uint16_t frame_num;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_MEDIUM];
} VASTPictureParameterBufferH264;

/** H.264 Inverse Quantization Matrix Buffer */
typedef struct _VASTIQMatrixBufferH264
{
    /** \brief 4x4 scaling list, in raster scan order. */
    uint8_t ScalingList4x4[6][16];
    /** \brief 8x8 scaling list, in raster scan order. */
    uint8_t ScalingList8x8[2][64];

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTIQMatrixBufferH264;

/** H.264 Slice Parameter Buffer */
typedef struct _VASTSliceParameterBufferH264
{
    uint32_t slice_data_size;/* number of bytes in the slice data buffer for this slice */
    /** \brief Byte offset to the NAL Header Unit for this slice. */
    uint32_t slice_data_offset;
    uint32_t slice_data_flag; /* see VA_SLICE_DATA_FLAG_XXX defintions */
    /**
     * \brief Bit offset from NAL Header Unit to the begining of slice_data().
     *
     * This bit offset is relative to and includes the NAL unit byte
     * and represents the number of bits parsed in the slice_header()
     * after the removal of any emulation prevention bytes in
     * there. However, the slice data buffer passed to the hardware is
     * the original bitstream, thus including any emulation prevention
     * bytes.
     */
    uint16_t slice_data_bit_offset;
    uint16_t first_mb_in_slice;
    uint8_t slice_type;
    uint8_t direct_spatial_mv_pred_flag;
    /**
     * H264/AVC syntax element
     *
     * if num_ref_idx_active_override_flag equals 0, host decoder should
     * set its value to num_ref_idx_l0_default_active_minus1.
     */
    uint8_t num_ref_idx_l0_active_minus1;
    /**
     * H264/AVC syntax element
     *
     * if num_ref_idx_active_override_flag equals 0, host decoder should
     * set its value to num_ref_idx_l1_default_active_minus1.
     */
    uint8_t num_ref_idx_l1_active_minus1;
    uint8_t cabac_init_idc;
    int8_t slice_qp_delta;
    uint8_t disable_deblocking_filter_idc;
    int8_t slice_alpha_c0_offset_div2;
    int8_t slice_beta_offset_div2;
    VASTPictureH264 RefPicList0[32];	/* See 8.2.4.2 */
    VASTPictureH264 RefPicList1[32];	/* See 8.2.4.2 */
    uint8_t luma_log2_weight_denom;
    uint8_t chroma_log2_weight_denom;
    uint8_t luma_weight_l0_flag;
    int16_t luma_weight_l0[32];
    int16_t luma_offset_l0[32];
    uint8_t chroma_weight_l0_flag;
    int16_t chroma_weight_l0[32][2];
    int16_t chroma_offset_l0[32][2];
    uint8_t luma_weight_l1_flag;
    int16_t luma_weight_l1[32];
    int16_t luma_offset_l1[32];
    uint8_t chroma_weight_l1_flag;
    int16_t chroma_weight_l1[32][2];
    int16_t chroma_offset_l1[32][2];

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTSliceParameterBufferH264;

/****************************
 * Common encode data structures
 ****************************/
typedef enum
{
    VASTEncPictureTypeIntra		= 0,
    VASTEncPictureTypePredictive		= 1,
    VASTEncPictureTypeBidirectional	= 2,
} VASTEncPictureType;

/**
 * \brief Encode Slice Parameter Buffer.
 *
 * @deprecated
 * This is a deprecated encode slice parameter buffer, All applications
 * \c can use VAEncSliceParameterBufferXXX (XXX = MPEG2, HEVC, H264, JPEG)
 */
typedef struct _VASTEncSliceParameterBuffer
{
    uint32_t start_row_number;	/* starting MB row number for this slice */
    uint32_t slice_height;	/* slice height measured in MB */
    union {
        struct {
            uint32_t is_intra	: 1;
            uint32_t disable_deblocking_filter_idc : 2;
            uint32_t uses_long_term_ref		:1;
            uint32_t is_long_term_ref		:1;
        } bits;
        uint32_t value;
    } slice_flags;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTEncSliceParameterBuffer;


/****************************
 * H.263 specific encode data structures
 ****************************/

typedef struct _VASTEncSequenceParameterBufferH263
{
    uint32_t intra_period;
    uint32_t bits_per_second;
    uint32_t frame_rate;
    uint32_t initial_qp;
    uint32_t min_qp;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTEncSequenceParameterBufferH263;

typedef struct _VASTEncPictureParameterBufferH263
{
    VASTSurfaceID reference_picture;
    VASTSurfaceID reconstructed_picture;
    VASTBufferID coded_buf;
    uint16_t picture_width;
    uint16_t picture_height;
    VASTEncPictureType picture_type;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTEncPictureParameterBufferH263;

/****************************
 * MPEG-4 specific encode data structures
 ****************************/

typedef struct _VASTEncSequenceParameterBufferMPEG4
{
    uint8_t profile_and_level_indication;
    uint32_t intra_period;
    uint32_t video_object_layer_width;
    uint32_t video_object_layer_height;
    uint32_t vop_time_increment_resolution;
    uint32_t fixed_vop_rate;
    uint32_t fixed_vop_time_increment;
    uint32_t bits_per_second;
    uint32_t frame_rate;
    uint32_t initial_qp;
    uint32_t min_qp;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTEncSequenceParameterBufferMPEG4;

typedef struct _VASTEncPictureParameterBufferMPEG4
{
    VASTSurfaceID reference_picture;
    VASTSurfaceID reconstructed_picture;
    VASTBufferID coded_buf;
    uint16_t picture_width;
    uint16_t picture_height;
    uint32_t modulo_time_base; /* number of 1s */
    uint32_t vop_time_increment;
    VASTEncPictureType picture_type;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTEncPictureParameterBufferMPEG4;



/** Buffer functions */

/**
 * Creates a buffer for "num_elements" elements of "size" bytes and
 * initalize with "data".
 * if "data" is null, then the contents of the buffer data store
 * are undefined.
 * Basically there are two ways to get buffer data to the server side. One is
 * to call vastCreateBuffer() with a non-null "data", which results the data being
 * copied to the data store on the server side.  A different method that
 * eliminates this copy is to pass null as "data" when calling vastCreateBuffer(),
 * and then use vastMapBuffer() to map the data store from the server side to the
 * client address space for access.
 * The user must call vastDestroyBuffer() to destroy a buffer.
 *  Note: image buffers are created by the library, not the client. Please see
 *        vastCreateImage on how image buffers are managed.
 */
VASTStatus vastCreateBuffer (
    VASTDisplay dpy,
    VASTContextID context,
    VASTBufferType type,	/* in */
    unsigned int size,	/* in */
    unsigned int num_elements, /* in */
    void *data,		/* in */
    VASTBufferID *buf_id	/* out */
);

/**
 * Create a buffer for given width & height get unit_size, pitch, buf_id for 2D buffer
 * for permb qp buffer, it will return unit_size for one MB or LCU and the pitch for alignments
 * can call vastMapBuffer with this Buffer ID to get virtual address.
 * e.g. AVC 1080P encode, 1920x1088, the size in MB is 120x68,but inside driver,
 * maybe it should align with 256, and one byte present one Qp.so, call the function.
 * then get unit_size = 1, pitch = 256. call vastMapBuffer to get the virtual address (pBuf).
 * then read write the memory like 2D. the size is 256x68, application can only use 120x68
 * pBuf + 256 is the start of next line.
 * different driver implementation maybe return different unit_size and pitch
 */
VASTStatus vastCreateBuffer2(
    VASTDisplay dpy,
    VASTContextID context,
    VASTBufferType type,
    unsigned int width,
    unsigned int height,
    unsigned int *unit_size,
    unsigned int *pitch,
    VASTBufferID *buf_id
);

/**
 * Convey to the server how many valid elements are in the buffer.
 * e.g. if multiple slice parameters are being held in a single buffer,
 * this will communicate to the server the number of slice parameters
 * that are valid in the buffer.
 */
VASTStatus vastBufferSetNumElements (
    VASTDisplay dpy,
    VASTBufferID buf_id,	/* in */
    unsigned int num_elements /* in */
);


/**
 * device independent data structure for codedbuffer
 */

/*
 * FICTURE_AVE_QP(bit7-0): The average Qp value used during this frame
 * LARGE_SLICE(bit8):At least one slice in the current frame was large
 *              enough for the encoder to attempt to limit its size.
 * SLICE_OVERFLOW(bit9): At least one slice in the current frame has
 *              exceeded the maximum slice size specified.
 * BITRATE_OVERFLOW(bit10): The peak bitrate was exceeded for this frame.
 * BITRATE_HIGH(bit11): The frame size got within the safety margin of the maximum size (VCM only)
 * AIR_MB_OVER_THRESHOLD: the number of MBs adapted to Intra MB
 */
#define VAST_CODED_BUF_STATUS_PICTURE_AVE_QP_MASK         0xff
#define VAST_CODED_BUF_STATUS_LARGE_SLICE_MASK            0x100
#define VAST_CODED_BUF_STATUS_SLICE_OVERFLOW_MASK         0x200
#define VAST_CODED_BUF_STATUS_BITRATE_OVERFLOW		0x400
#define VAST_CODED_BUF_STATUS_BITRATE_HIGH		0x800
/**
 * \brief The frame has exceeded the maximum requested size.
 *
 * This flag indicates that the encoded frame size exceeds the value
 * specified through a misc parameter buffer of type
 * #VASTEncMiscParameterTypeMaxFrameSize.
 */
#define VAST_CODED_BUF_STATUS_FRAME_SIZE_OVERFLOW         0x1000
/**
 * \brief the bitstream is bad or corrupt.
 */
#define VAST_CODED_BUF_STATUS_BAD_BITSTREAM               0x8000
#define VAST_CODED_BUF_STATUS_AIR_MB_OVER_THRESHOLD	0xff0000
#define VAST_CODED_BUF_STATUS_EXEPTION	0x2000

/**
 * \brief The coded buffer segment status contains frame encoding passes number
 *
 * This is the mask to get the number of encoding passes from the coded
 * buffer segment status.
 * NUMBER_PASS(bit24~bit27): the number for encoding passes executed for the coded frame.
 *
 */
#define VAST_CODED_BUF_STATUS_NUMBER_PASSES_MASK          0xf000000

/**
 * \brief The coded buffer segment contains a single NAL unit.
 *
 * This flag indicates that the coded buffer segment contains a
 * single NAL unit. This flag might be useful to the user for
 * processing the coded buffer.
 */
#define VAST_CODED_BUF_STATUS_SINGLE_NALU                 0x10000000

/**
 * \brief Coded buffer segment.
 *
 * #VASTCodedBufferSegment is an element of a linked list describing
 * some information on the coded buffer. The coded buffer segment
 * could contain either a single NAL unit, or more than one NAL unit.
 * It is recommended (but not required) to return a single NAL unit
 * in a coded buffer segment, and the implementation should set the
 * VA_CODED_BUF_STATUS_SINGLE_NALU status flag if that is the case.
 */
typedef  struct _VASTCodedBufferSegment  {
    /**
     * \brief Size of the data buffer in this segment (in bytes).
     */
    uint32_t        size;
    /** \brief Bit offset into the data buffer where the video data starts. */
    uint32_t        bit_offset;
    /** \brief Status set by the driver. See \c VA_CODED_BUF_STATUS_*. */
    uint32_t        status;
    /** \brief Reserved for future use. */
    uint32_t        reserved;
    /** \brief Pointer to the start of the data buffer. */
    void               *buf;
    /**
     * \brief Pointer to the next #VASTCodedBufferSegment element,
     * or \c NULL if there is none.
     */
    void               *next;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTCodedBufferSegment;

/**
 * Map data store of the buffer into the client's address space
 * vastCreateBuffer() needs to be called with "data" set to NULL before
 * calling vastMapBuffer()
 *
 * if buffer type is VASTEncCodedBufferType, pbuf points to link-list of
 * VASTCodedBufferSegment, and the list is terminated if "next" is NULL
 */
VASTStatus vastMapBuffer (
    VASTDisplay dpy,
    VASTBufferID buf_id,	/* in */
    void **pbuf 	/* out */
);

/**
 * After client making changes to a mapped data store, it needs to
 * "Unmap" it to let the server know that the data is ready to be
 * consumed by the server
 */
VASTStatus vastUnmapBuffer (
    VASTDisplay dpy,
    VASTBufferID buf_id	/* in */
);

/**
 * After this call, the buffer is deleted and this buffer_id is no longer valid
 *
 * A buffer can be re-used and sent to the server by another Begin/Render/End
 * sequence if vastDestroyBuffer() is not called with this buffer.
 *
 * Note re-using a shared buffer (e.g. a slice data buffer) between the host and the
 * hardware accelerator can result in performance dropping.
 */
VASTStatus vastDestroyBuffer (
    VASTDisplay dpy,
    VASTBufferID buffer_id
);

/** \brief VA buffer information */
typedef struct {
    /** \brief Buffer handle */
    uintptr_t           handle;
    /** \brief Buffer type (See \ref VASTBufferType). */
    uint32_t            type;
    /**
     * \brief Buffer memory type (See \ref VASTSurfaceAttribMemoryType).
     *
     * On input to vastAcquireBufferHandle(), this field can serve as a hint
     * to specify the set of memory types the caller is interested in.
     * On successful return from vastAcquireBufferHandle(), the field is
     * updated with the best matching memory type.
     */
    uint32_t            mem_type;
    /** \brief Size of the underlying buffer. */
    size_t              mem_size;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTBufferInfo;

/**
 * \brief Acquires buffer handle for external API usage
 *
 * Locks the VA buffer object \ref buf_id for external API usage like
 * EGL or OpenCL (OCL). This function is a synchronization point. This
 * means that any pending operation is guaranteed to be completed
 * prior to returning from the function.
 *
 * If the referenced VA buffer object is the backing store of a VA
 * surface, then this function acts as if vastSyncSurface() on the
 * parent surface was called first.
 *
 * The \ref VASTBufferInfo argument shall be zero'ed on input. On
 * successful output, the data structure is filled in with all the
 * necessary buffer level implementation details like handle, type,
 * memory type and memory size.
 *
 * Note: the external API implementation, or the application, can
 * express the memory types it is interested in by filling in the \ref
 * mem_type field accordingly. On successful output, the memory type
 * that fits best the request and that was used is updated in the \ref
 * VASTBufferInfo data structure. If none of the supplied memory types
 * is supported, then a \ref VAST_STATUS_ERROR_UNSUPPORTED_MEMORY_TYPE
 * error is returned.
 *
 * The \ref VASTBufferInfo data is valid until vastReleaseBufferHandle()
 * is called. Besides, no additional operation is allowed on any of
 * the buffer parent object until vastReleaseBufferHandle() is called.
 * e.g. decoding into a VA surface backed with the supplied VA buffer
 * object \ref buf_id would fail with a \ref VAST_STATUS_ERROR_SURFACE_BUSY
 * error.
 *
 * Possible errors:
 * - \ref VAST_STATUS_ERROR_UNIMPLEMENTED: the VA driver implementation
 *   does not support this interface
 * - \ref VAST_STATUS_ERROR_INVALID_DISPLAY: an invalid display was supplied
 * - \ref VAST_STATUS_ERROR_INVALID_BUFFER: an invalid buffer was supplied
 * - \ref VAST_STATUS_ERROR_UNSUPPORTED_BUFFERTYPE: the implementation
 *   does not support exporting buffers of the specified type
 * - \ref VAST_STATUS_ERROR_UNSUPPORTED_MEMORY_TYPE: none of the requested
 *   memory types in \ref VASTBufferInfo.mem_type was supported
 *
 * @param[in] dpy               the VA display
 * @param[in] buf_id            the VA buffer
 * @param[in,out] buf_info      the associated VA buffer information
 * @return VAST_STATUS_SUCCESS if successful
 */
VASTStatus
vastAcquireBufferHandle(VASTDisplay dpy, VASTBufferID buf_id, VASTBufferInfo *buf_info);

/**
 * \brief Releases buffer after usage from external API
 *
 * Unlocks the VA buffer object \ref buf_id from external API usage like
 * EGL or OpenCL (OCL). This function is a synchronization point. This
 * means that any pending operation is guaranteed to be completed
 * prior to returning from the function.
 *
 * The \ref VASTBufferInfo argument shall point to the original data
 * structure that was obtained from vastAcquireBufferHandle(), unaltered.
 * This is necessary so that the VA driver implementation could
 * deallocate any resources that were needed.
 *
 * In any case, returning from this function invalidates any contents
 * in \ref VASTBufferInfo. i.e. the underlyng buffer handle is no longer
 * valid. Therefore, VA driver implementations are free to reset this
 * data structure to safe defaults.
 *
 * Possible errors:
 * - \ref VAST_STATUS_ERROR_UNIMPLEMENTED: the VA driver implementation
 *   does not support this interface
 * - \ref VAST_STATUS_ERROR_INVALID_DISPLAY: an invalid display was supplied
 * - \ref VAST_STATUS_ERROR_INVALID_BUFFER: an invalid buffer was supplied
 * - \ref VAST_STATUS_ERROR_UNSUPPORTED_BUFFERTYPE: the implementation
 *   does not support exporting buffers of the specified type
 *
 * @param[in] dpy               the VA display
 * @param[in] buf_id            the VA buffer
 * @return VAST_STATUS_SUCCESS if successful
 */
VASTStatus
vastReleaseBufferHandle(VASTDisplay dpy, VASTBufferID buf_id);

/** @name vastExportSurfaceHandle() flags
 *
 * @{
 */
/** Export surface to be read by external API. */
#define VAST_EXPORT_SURFACE_READ_ONLY        0x0001
/** Export surface to be written by external API. */
#define VAST_EXPORT_SURFACE_WRITE_ONLY       0x0002
/** Export surface to be both read and written by external API. */
#define VAST_EXPORT_SURFACE_READ_WRITE       0x0003
/** Export surface with separate layers.
 *
 * For example, NV12 surfaces should be exported as two separate
 * planes for luma and chroma.
 */
#define VAST_EXPORT_SURFACE_SEPARATE_LAYERS  0x0004
/** Export surface with composed layers.
 *
 * For example, NV12 surfaces should be exported as a single NV12
 * composed object.
 */
#define VAST_EXPORT_SURFACE_COMPOSED_LAYERS  0x0008

/** @} */

/**
 * \brief Export a handle to a surface for use with an external API
 *
 * The exported handles are owned by the caller, and the caller is
 * responsible for freeing them when no longer needed (e.g. by closing
 * DRM PRIME file descriptors).
 *
 * This does not perform any synchronisation.  If the contents of the
 * surface will be read, vastSyncSurface() must be called before doing so.
 * If the contents of the surface are written, then all operations must
 * be completed externally before using the surface again by via VA-API
 * functions.
 *
 * @param[in] dpy          VA display.
 * @param[in] surface_id   Surface to export.
 * @param[in] mem_type     Memory type to export to.
 * @param[in] flags        Combination of flags to apply
 *   (VA_EXPORT_SURFACE_*).
 * @param[out] descriptor  Pointer to the descriptor structure to fill
 *   with the handle details.  The type of this structure depends on
 *   the value of mem_type.
 *
 * @return Status code:
 * - VAST_STATUS_SUCCESS:    Success.
 * - VAST_STATUS_ERROR_INVALID_DISPLAY:  The display is not valid.
 * - VAST_STATUS_ERROR_UNIMPLEMENTED:  The driver does not implement
 *     this interface.
 * - VAST_STATUS_ERROR_INVALID_SURFACE:  The surface is not valid, or
 *     the surface is not exportable in the specified way.
 * - VAST_STATUS_ERROR_UNSUPPORTED_MEMORY_TYPE:  The driver does not
 *     support exporting surfaces to the specified memory type.
 */
VASTStatus vastExportSurfaceHandle(VASTDisplay dpy,
                               VASTSurfaceID surface_id,
                               uint32_t mem_type, uint32_t flags,
                               void *descriptor);

/**
 * Render (Video Decode/Encode/Processing) Pictures
 *
 * A picture represents either a frame or a field.
 *
 * The Begin/Render/End sequence sends the video decode/encode/processing buffers
 * to the server
 */

/**
 * Get ready for a video pipeline
 * - decode a picture to a target surface
 * - encode a picture from a target surface
 * - process a picture to a target surface
 */
VASTStatus vastBeginPicture (
    VASTDisplay dpy,
    VASTContextID context,
    VASTSurfaceID render_target
);

/**
 * Send video decode, encode or processing buffers to the server.
 */
VASTStatus vastRenderPicture (
    VASTDisplay dpy,
    VASTContextID context,
    VASTBufferID *buffers,
    int num_buffers
);

/**
 * Make the end of rendering for a picture.
 * The server should start processing all pending operations for this
 * surface. This call is non-blocking. The client can start another
 * Begin/Render/End sequence on a different render target.
 * if VASTContextID used in this function previously successfully passed
 * vastMFAddContext call, real processing will be started during vastMFSubmit
 */
VASTStatus vastEndPicture (
    VASTDisplay dpy,
    VASTContextID context
);

/**
 * Make the end of rendering for a pictures in contexts passed with submission.
 * The server should start processing all pending operations for contexts.
 * All contexts passed should be associated through vastMFAddContext
 * and call sequence Begin/Render/End performed.
 * This call is non-blocking. The client can start another
 * Begin/Render/End/vastMFSubmit sequence on a different render targets.
 * Return values:
 * VAST_STATUS_SUCCESS - operation successful, context was removed.
 * VAST_STATUS_ERROR_INVALID_CONTEXT - mf_context or one of contexts are invalid
 * due to mf_context not created or one of contexts not assotiated with mf_context
 * through vaAddContext.
 * VAST_STATUS_ERROR_INVALID_PARAMETER - one of context has not submitted it's frame
 * through vastBeginPicture vastRenderPicture vastEndPicture call sequence.
 * dpy: display
 * mf_context: Multi-Frame context
 * contexts: list of contexts submitting their tasks for multi-frame operation.
 * num_contexts: number of passed contexts.
 */
VASTStatus vastMFSubmit (
    VASTDisplay dpy,
    VASTMFContextID mf_context,
    VASTContextID * contexts,
    int num_contexts
);

/*

Synchronization

*/

/**
 * This function blocks until all pending operations on the render target
 * have been completed.  Upon return it is safe to use the render target for a
 * different picture.
 */
VASTStatus vastSyncSurface (
    VASTDisplay dpy,
    VASTSurfaceID render_target
);

typedef enum
{
    VASTSurfaceRendering	= 1, /* Rendering in progress */
    VASTSurfaceDisplaying	= 2, /* Displaying in progress (not safe to render into it) */
                             /* this status is useful if surface is used as the source */
                             /* of an overlay */
    VASTSurfaceReady	= 4, /* not being rendered or displayed */
    VASTSurfaceSkipped	= 8  /* Indicate a skipped frame during encode */
} VASTSurfaceStatus;

/**
 * Find out any pending ops on the render target
 */
VASTStatus vastQuerySurfaceStatus (
    VASTDisplay dpy,
    VASTSurfaceID render_target,
    VASTSurfaceStatus *status	/* out */
);

typedef enum
{
    VASTDecodeSliceMissing            = 0,
    VASTDecodeMBError                 = 1,
} VASTDecodeErrorType;

/**
 * Client calls vastQuerySurfaceError with VAST_STATUS_ERROR_DECODING_ERROR, server side returns
 * an array of structure VASTSurfaceDecodeMBErrors, and the array is terminated by setting status=-1
*/
typedef struct _VASTSurfaceDecodeMBErrors
{
    int32_t status; /* 1 if hardware has returned detailed info below, -1 means this record is invalid */
    uint32_t start_mb; /* start mb address with errors */
    uint32_t end_mb;  /* end mb address with errors */
    VASTDecodeErrorType decode_error_type;
    uint32_t num_mb;   /* number of mbs with errors */
    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW - 1];
} VASTSurfaceDecodeMBErrors;

/**
 * After the application gets VAST_STATUS_ERROR_DECODING_ERROR after calling vastSyncSurface(),
 * it can call vastQuerySurfaceError to find out further details on the particular error.
 * VAST_STATUS_ERROR_DECODING_ERROR should be passed in as "error_status",
 * upon the return, error_info will point to an array of _VASurfaceDecodeMBErrors structure,
 * which is allocated and filled by libVA with detailed information on the missing or error macroblocks.
 * The array is terminated if "status==-1" is detected.
 */
VASTStatus vastQuerySurfaceError(
    VASTDisplay dpy,
    VASTSurfaceID surface,
    VASTStatus error_status,
    void **error_info
);

/**
 * Images and Subpictures
 * VASTImage is used to either get the surface data to client memory, or
 * to copy image data in client memory to a surface.
 * Both images, subpictures and surfaces follow the same 2D coordinate system where origin
 * is at the upper left corner with positive X to the right and positive Y down
 */
#define VAST_FOURCC(ch0, ch1, ch2, ch3) \
    ((unsigned long)(unsigned char) (ch0) | ((unsigned long)(unsigned char) (ch1) << 8) | \
    ((unsigned long)(unsigned char) (ch2) << 16) | ((unsigned long)(unsigned char) (ch3) << 24 ))

/*
 * Pre-defined fourcc codes
 */
#define VAST_FOURCC_NV12		0x3231564E
#define VAST_FOURCC_NV21		0x3132564E
#define VAST_FOURCC_AI44		0x34344149
#define VAST_FOURCC_RGBA		0x41424752
#define VAST_FOURCC_RGBX		0x58424752
#define VAST_FOURCC_BGRA		0x41524742
#define VAST_FOURCC_BGRX		0x58524742
#define VAST_FOURCC_RGB565        0x36314752
#define VAST_FOURCC_BGR565        0x36314742
#define VAST_FOURCC_ARGB		0x42475241
#define VAST_FOURCC_XRGB		0x42475258
#define VAST_FOURCC_ABGR          0x52474241
#define VAST_FOURCC_XBGR          0x52474258
#define VAST_FOURCC_UYVY          0x59565955
#define VAST_FOURCC_YUY2          0x32595559
#define VAST_FOURCC_AYUV          0x56555941
#define VAST_FOURCC_NV11          0x3131564e
#define VAST_FOURCC_YV12          0x32315659
#define VAST_FOURCC_P208          0x38303250
/* IYUV same as I420, but most user perfer I420, will deprecate it */
#define VAST_FOURCC_IYUV          0x56555949
#define VAST_FOURCC_I420          0x30323449
#define VAST_FOURCC_YV24          0x34325659
#define VAST_FOURCC_YV32          0x32335659
#define VAST_FOURCC_Y800          0x30303859
#define VAST_FOURCC_IMC3          0x33434D49
#define VAST_FOURCC_411P          0x50313134
#define VAST_FOURCC_422H          0x48323234
#define VAST_FOURCC_422V          0x56323234
#define VAST_FOURCC_444P          0x50343434
#define VAST_FOURCC_RGBP          0x50424752
#define VAST_FOURCC_BGRP          0x50524742
#define VAST_FOURCC_411R          0x52313134 /* rotated 411P */
/**
 * Planar YUV 4:2:2.
 * 8-bit Y plane, followed by 8-bit 2x1 subsampled V and U planes
 */
#define VAST_FOURCC_YV16          0x36315659
/**
 * 10-bit and 16-bit Planar YUV 4:2:0.
 */
#define VAST_FOURCC_P010          0x30313050
#define VAST_FOURCC_P016          0x36313050

/**
 * 10-bit Planar YUV 420 and occupy the lower 10-bit.
 */
#define VAST_FOURCC_I010          0x30313049

/* byte order */
#define VAST_LSB_FIRST		1
#define VAST_MSB_FIRST		2

typedef struct _VASTImageFormat
{
    uint32_t	fourcc;
    uint32_t	byte_order; /* VAST_LSB_FIRST, VAST_MSB_FIRST */
    uint32_t	bits_per_pixel;
    /* for RGB formats */
    uint32_t	depth; /* significant bits per pixel */
    uint32_t	red_mask;
    uint32_t	green_mask;
    uint32_t	blue_mask;
    uint32_t	alpha_mask;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTImageFormat;

typedef VASTGenericID VASTImageID;

typedef struct _VASTImage
{
    VASTImageID		image_id; /* uniquely identify this image */
    VASTImageFormat	format;
    VASTBufferID		buf;	/* image data buffer */
    /*
     * Image data will be stored in a buffer of type VASTImageBufferType to facilitate
     * data store on the server side for optimal performance. The buffer will be
     * created by the CreateImage function, and proper storage allocated based on the image
     * size and format. This buffer is managed by the library implementation, and
     * accessed by the client through the buffer Map/Unmap functions.
     */
    uint16_t	width;
    uint16_t	height;
    uint32_t	data_size;
    uint32_t	num_planes;	/* can not be greater than 3 */
    /*
     * An array indicating the scanline pitch in bytes for each plane.
     * Each plane may have a different pitch. Maximum 3 planes for planar formats
     */
    uint32_t	pitches[3];
    /*
     * An array indicating the byte offset from the beginning of the image data
     * to the start of each plane.
     */
    uint32_t	offsets[3];

    /* The following fields are only needed for paletted formats */
    int32_t num_palette_entries;   /* set to zero for non-palette images */
    /*
     * Each component is one byte and entry_bytes indicates the number of components in
     * each entry (eg. 3 for YUV palette entries). set to zero for non-palette images
     */
    int32_t entry_bytes;
    /*
     * An array of ascii characters describing the order of the components within the bytes.
     * Only entry_bytes characters of the string are used.
     */
    int8_t component_order[4];

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTImage;

/** Get maximum number of image formats supported by the implementation */
int vastMaxNumImageFormats (
    VASTDisplay dpy
);

/**
 * Query supported image formats
 * The caller must provide a "format_list" array that can hold at
 * least vastMaxNumImageFormats() entries. The actual number of formats
 * returned in "format_list" is returned in "num_formats".
 */
VASTStatus vastQueryImageFormats (
    VASTDisplay dpy,
    VASTImageFormat *format_list,	/* out */
    int *num_formats		/* out */
);

/**
 * Create a VASTImage structure
 * The width and height fields returned in the VASTImage structure may get
 * enlarged for some YUV formats. Upon return from this function,
 * image->buf has been created and proper storage allocated by the library.
 * The client can access the image through the Map/Unmap calls.
 */
VASTStatus vastCreateImage (
    VASTDisplay dpy,
    VASTImageFormat *format,
    int width,
    int height,
    VASTImage *image	/* out */
);

/**
 * Should call DestroyImage before destroying the surface it is bound to
 */
VASTStatus vastDestroyImage (
    VASTDisplay dpy,
    VASTImageID image
);

VASTStatus vastSetImagePalette (
    VASTDisplay dpy,
    VASTImageID image,
    /*
     * pointer to an array holding the palette data.  The size of the array is
     * num_palette_entries * entry_bytes in size.  The order of the components
     * in the palette is described by the component_order in VASTImage struct
     */
    unsigned char *palette
);

/**
 * Retrive surface data into a VASTImage
 * Image must be in a format supported by the implementation
 */
VASTStatus vastGetImage (
    VASTDisplay dpy,
    VASTSurfaceID surface,
    int x,	/* coordinates of the upper left source pixel */
    int y,
    unsigned int width, /* width and height of the region */
    unsigned int height,
    VASTImageID image
);

/**
 * Copy data from a VASTImage to a surface
 * Image must be in a format supported by the implementation
 * Returns a VAST_STATUS_ERROR_SURFACE_BUSY if the surface
 * shouldn't be rendered into when this is called
 */
VASTStatus vastPutImage (
    VASTDisplay dpy,
    VASTSurfaceID surface,
    VASTImageID image,
    int src_x,
    int src_y,
    unsigned int src_width,
    unsigned int src_height,
    int dest_x,
    int dest_y,
    unsigned int dest_width,
    unsigned int dest_height
);

/**
 * Derive an VASTImage from an existing surface.
 * This interface will derive a VASTImage and corresponding image buffer from
 * an existing VA Surface. The image buffer can then be mapped/unmapped for
 * direct CPU access. This operation is only possible on implementations with
 * direct rendering capabilities and internal surface formats that can be
 * represented with a VASTImage. When the operation is not possible this interface
 * will return VAST_STATUS_ERROR_OPERATION_FAILED. Clients should then fall back
 * to using vastCreateImage + vastPutImage to accomplish the same task in an
 * indirect manner.
 *
 * Implementations should only return success when the resulting image buffer
 * would be useable with vaMap/Unmap.
 *
 * When directly accessing a surface special care must be taken to insure
 * proper synchronization with the graphics hardware. Clients should call
 * vastQuerySurfaceStatus to insure that a surface is not the target of concurrent
 * rendering or currently being displayed by an overlay.
 *
 * Additionally nothing about the contents of a surface should be assumed
 * following a vastPutSurface. Implementations are free to modify the surface for
 * scaling or subpicture blending within a call to vastPutImage.
 *
 * Calls to vastPutImage or vastGetImage using the same surface from which the image
 * has been derived will return VAST_STATUS_ERROR_SURFACE_BUSY. vastPutImage or
 * vastGetImage with other surfaces is supported.
 *
 * An image created with vastDeriveImage should be freed with vastDestroyImage. The
 * image and image buffer structures will be destroyed; however, the underlying
 * surface will remain unchanged until freed with vastDestroySurfaces.
 */
VASTStatus vastDeriveImage (
    VASTDisplay dpy,
    VASTSurfaceID surface,
    VASTImage *image	/* out */
);

VASTStatus vastGetSurfaceIdAddress (
    VASTDisplay dpy,
    VASTSurfaceID surface,
    unsigned long long *address	/* out */
);

VASTStatus vastSetSurfaceIdAddress (
    VASTDisplay dpy,
    VASTSurfaceID surface,
    unsigned long long address	/* out */
);

VASTStatus vastGetDieinfo (
    VASTDisplay dpy,
    int* die_id	/* out */
);


VASTStatus vastCreateDmaHandle(
    VASTDisplay dpy,
    int die_id /* in */,
    unsigned int dmabuff_size,
    void *dma_handle
);

VASTStatus vastDestroyDmaHandle(
    VASTDisplay dpy,
    void *dma_handle
);

VASTStatus vastDmaWriteBuf(
    VASTDisplay dpy,
    unsigned long dst_soc_addr,
    int buf_size,
    void *dma_handle
);

VASTStatus vaQueWriteDmaBufSg(
        VASTDisplay dpy,
        void *channel,
        int channel_num,
        unsigned long axi_addr,
        unsigned int die_index
);

VASTStatus vastDmaWriteExternalBuf(
    VASTDisplay dpy,
    int fd,
    int die_id,
    uint64_t dmabuffer_addr,
    uint64_t axi_addr,
    unsigned int dmabuff_size,
    int src_type
);

/**
 * Subpictures
 * Subpicture is a special type of image that can be blended
 * with a surface during vastPutSurface(). Subpicture can be used to render
 * DVD sub-titles or closed captioning text etc.
 */

typedef VASTGenericID VASTSubpictureID;

/** Get maximum number of subpicture formats supported by the implementation */
int vastMaxNumSubpictureFormats (
    VASTDisplay dpy
);

/** flags for subpictures */
#define VAST_SUBPICTURE_CHROMA_KEYING			0x0001
#define VAST_SUBPICTURE_GLOBAL_ALPHA			0x0002
#define VAST_SUBPICTURE_DESTINATION_IS_SCREEN_COORD	0x0004
/**
 * Query supported subpicture formats
 * The caller must provide a "format_list" array that can hold at
 * least vastMaxNumSubpictureFormats() entries. The flags arrary holds the flag
 * for each format to indicate additional capabilities for that format. The actual
 * number of formats returned in "format_list" is returned in "num_formats".
 *  flags: returned value to indicate addtional capabilities
 *         VAST_SUBPICTURE_CHROMA_KEYING - supports chroma-keying
 *         VAST_SUBPICTURE_GLOBAL_ALPHA - supports global alpha
 * 	   VAST_SUBPICTURE_DESTINATION_IS_SCREEN_COORD - supports unscaled screen relative subpictures for On Screen Display
 */

VASTStatus vastQuerySubpictureFormats (
    VASTDisplay dpy,
    VASTImageFormat *format_list,	/* out */
    unsigned int *flags,	/* out */
    unsigned int *num_formats	/* out */
);

/**
 * Subpictures are created with an image associated.
 */
VASTStatus vastCreateSubpicture (
    VASTDisplay dpy,
    VASTImageID image,
    VASTSubpictureID *subpicture	/* out */
);

/**
 * Destroy the subpicture before destroying the image it is assocated to
 */
VASTStatus vastDestroySubpicture (
    VASTDisplay dpy,
    VASTSubpictureID subpicture
);

/**
 * Bind an image to the subpicture. This image will now be associated with
 * the subpicture instead of the one at creation.
 */
VASTStatus vastSetSubpictureImage (
    VASTDisplay dpy,
    VASTSubpictureID subpicture,
    VASTImageID image
);

/**
 * If chromakey is enabled, then the area where the source value falls within
 * the chromakey [min, max] range is transparent
 * The chromakey component format is the following:
 *  For RGB: [0:7] Red [8:15] Blue [16:23] Green
 *  For YUV: [0:7] V [8:15] U [16:23] Y
 * The chromakey mask can be used to mask out certain components for chromakey
 * comparision
 */
VASTStatus vastSetSubpictureChromakey (
    VASTDisplay dpy,
    VASTSubpictureID subpicture,
    unsigned int chromakey_min,
    unsigned int chromakey_max,
    unsigned int chromakey_mask
);

/**
 * Global alpha value is between 0 and 1. A value of 1 means fully opaque and
 * a value of 0 means fully transparent. If per-pixel alpha is also specified then
 * the overall alpha is per-pixel alpha multiplied by the global alpha
 */
VASTStatus vastSetSubpictureGlobalAlpha (
    VASTDisplay dpy,
    VASTSubpictureID subpicture,
    float global_alpha
);

/**
 * vastAssociateSubpicture associates the subpicture with target_surfaces.
 * It defines the region mapping between the subpicture and the target
 * surfaces through source and destination rectangles (with the same width and height).
 * Both will be displayed at the next call to vastPutSurface.  Additional
 * associations before the call to vastPutSurface simply overrides the association.
 */
VASTStatus vastAssociateSubpicture (
    VASTDisplay dpy,
    VASTSubpictureID subpicture,
    VASTSurfaceID *target_surfaces,
    int num_surfaces,
    int16_t src_x, /* upper left offset in subpicture */
    int16_t src_y,
    uint16_t src_width,
    uint16_t src_height,
    int16_t dest_x, /* upper left offset in surface */
    int16_t dest_y,
    uint16_t dest_width,
    uint16_t dest_height,
    /*
     * whether to enable chroma-keying, global-alpha, or screen relative mode
     * see VA_SUBPICTURE_XXX values
     */
    uint32_t flags
);

/**
 * vastDeassociateSubpicture removes the association of the subpicture with target_surfaces.
 */
VASTStatus vastDeassociateSubpicture (
    VASTDisplay dpy,
    VASTSubpictureID subpicture,
    VASTSurfaceID *target_surfaces,
    int num_surfaces
);

/**
 * Display attributes
 * Display attributes are used to control things such as contrast, hue, saturation,
 * brightness etc. in the rendering process.  The application can query what
 * attributes are supported by the driver, and then set the appropriate attributes
 * before calling vastPutSurface()
 */
/* PowerVR IEP Lite attributes */
typedef enum
{
    VASTDISPLAYATTRIB_BLE_OFF              = 0x00,
    VASTDISPLAYATTRIB_BLE_LOW,
    VASTDISPLAYATTRIB_BLE_MEDIUM,
    VASTDISPLAYATTRIB_BLE_HIGH,
    VASTDISPLAYATTRIB_BLE_NONE,
} VASTDisplayAttribBLEMode;

/** attribute value for VADisplayAttribRotation   */
#define VAST_ROTATION_NONE        0x00000000
#define VAST_ROTATION_90          0x00000001
#define VAST_ROTATION_180         0x00000002
#define VAST_ROTATION_270         0x00000003
/**@}*/

/**
 * @name Mirroring directions
 *
 * Those values could be used for VADisplayAttribMirror attribute or
 * VASTProcPipelineParameterBuffer::mirror_state.

 */
/**@{*/
/** \brief No Mirroring. */
#define VAST_MIRROR_NONE              0x00000000
/** \brief Horizontal Mirroring. */
#define VAST_MIRROR_HORIZONTAL        0x00000001
/** \brief Vertical Mirroring. */
#define VAST_MIRROR_VERTICAL          0x00000002
/**@}*/

/** attribute value for VADisplayAttribOutOfLoopDeblock */
#define VAST_OOL_DEBLOCKING_FALSE 0x00000000
#define VAST_OOL_DEBLOCKING_TRUE  0x00000001

/** Render mode */
#define VAST_RENDER_MODE_UNDEFINED           0
#define VAST_RENDER_MODE_LOCAL_OVERLAY       1
#define VAST_RENDER_MODE_LOCAL_GPU           2
#define VAST_RENDER_MODE_EXTERNAL_OVERLAY    4
#define VAST_RENDER_MODE_EXTERNAL_GPU        8

/** Render device */
#define VAST_RENDER_DEVICE_UNDEFINED  0
#define VAST_RENDER_DEVICE_LOCAL      1
#define VAST_RENDER_DEVICE_EXTERNAL   2

/** Currently defined display attribute types */
typedef enum
{
    VASTDisplayAttribBrightness		= 0,
    VASTDisplayAttribContrast		= 1,
    VASTDisplayAttribHue			= 2,
    VASTDisplayAttribSaturation		= 3,
    /* client can specifiy a background color for the target window
     * the new feature of video conference,
     * the uncovered area of the surface is filled by this color
     * also it will blend with the decoded video color
     */
    VASTDisplayAttribBackgroundColor      = 4,
    /*
     * this is a gettable only attribute. For some implementations that use the
     * hardware overlay, after PutSurface is called, the surface can not be
     * re-used until after the subsequent PutSurface call. If this is the case
     * then the value for this attribute will be set to 1 so that the client
     * will not attempt to re-use the surface right after returning from a call
     * to PutSurface.
     *
     * Don't use it, use flag VASTSurfaceDisplaying of vastQuerySurfaceStatus since
     * driver may use overlay or GPU alternatively
     */
    VASTDisplayAttribDirectSurface       = 5,
    VASTDisplayAttribRotation            = 6,
    VASTDisplayAttribOutofLoopDeblock    = 7,

    /* PowerVR IEP Lite specific attributes */
    VASTDisplayAttribBLEBlackMode        = 8,
    VASTDisplayAttribBLEWhiteMode        = 9,
    VASTDisplayAttribBlueStretch         = 10,
    VASTDisplayAttribSkinColorCorrection = 11,
    /*
     * For type VADisplayAttribCSCMatrix, "value" field is a pointer to the color
     * conversion matrix. Each element in the matrix is float-point
     */
    VASTDisplayAttribCSCMatrix           = 12,
    /* specify the constant color used to blend with video surface
     * Cd = Cv*Cc*Ac + Cb *(1 - Ac) C means the constant RGB
     *      d: the final color to overwrite into the frame buffer
     *      v: decoded video after color conversion,
     *      c: video color specified by VADisplayAttribBlendColor
     *      b: background color of the drawable
     */
    VASTDisplayAttribBlendColor          = 13,
    /*
     * Indicate driver to skip painting color key or not.
     * only applicable if the render is overlay
     */
    VASTDisplayAttribOverlayAutoPaintColorKey   = 14,
    /*
     * customized overlay color key, the format is RGB888
     * [23:16] = Red, [15:08] = Green, [07:00] = Blue.
     */
    VASTDisplayAttribOverlayColorKey	= 15,
    /*
     * The hint for the implementation of vastPutSurface
     * normally, the driver could use an overlay or GPU to render the surface on the screen
     * this flag provides APP the flexibity to switch the render dynamically
     */
    VASTDisplayAttribRenderMode           = 16,
    /*
     * specify if vastPutSurface needs to render into specified monitors
     * one example is that one external monitor (e.g. HDMI) is enabled,
     * but the window manager is not aware of it, and there is no associated drawable
     */
    VASTDisplayAttribRenderDevice        = 17,
    /*
     * specify vastPutSurface render area if there is no drawable on the monitor
     */
    VASTDisplayAttribRenderRect          = 18,
} VASTDisplayAttribType;

/* flags for VASTDisplayAttribute */
#define VAST_DISPLAY_ATTRIB_NOT_SUPPORTED	0x0000
#define VAST_DISPLAY_ATTRIB_GETTABLE	0x0001
#define VAST_DISPLAY_ATTRIB_SETTABLE	0x0002

typedef struct _VASTDisplayAttribute
{
    VASTDisplayAttribType type;
    int32_t min_value;
    int32_t max_value;
    int32_t value;	/* used by the set/get attribute functions */
/* flags can be VA_DISPLAY_ATTRIB_GETTABLE or VA_DISPLAY_ATTRIB_SETTABLE or OR'd together */
    uint32_t flags;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTDisplayAttribute;

/** Get maximum number of display attributs supported by the implementation */
int vastMaxNumDisplayAttributes (
    VASTDisplay dpy
);

/**
 * Query display attributes
 * The caller must provide a "attr_list" array that can hold at
 * least vastMaxNumDisplayAttributes() entries. The actual number of attributes
 * returned in "attr_list" is returned in "num_attributes".
 */
VASTStatus vastQueryDisplayAttributes (
    VASTDisplay dpy,
    VASTDisplayAttribute *attr_list,	/* out */
    int *num_attributes			/* out */
);

/**
 * Get display attributes
 * This function returns the current attribute values in "attr_list".
 * Only attributes returned with VA_DISPLAY_ATTRIB_GETTABLE set in the "flags" field
 * from vastQueryDisplayAttributes() can have their values retrieved.
 */
VASTStatus vastGetDisplayAttributes (
    VASTDisplay dpy,
    VASTDisplayAttribute *attr_list,	/* in/out */
    int num_attributes
);

/**
 * Set display attributes
 * Only attributes returned with VA_DISPLAY_ATTRIB_SETTABLE set in the "flags" field
 * from vastQueryDisplayAttributes() can be set.  If the attribute is not settable or
 * the value is out of range, the function returns VAST_STATUS_ERROR_ATTR_NOT_SUPPORTED
 */
VASTStatus vastSetDisplayAttributes (
    VASTDisplay dpy,
    VASTDisplayAttribute *attr_list,
    int num_attributes
);

/****************************
 * HEVC data structures
 ****************************/
/**
 * \brief Description of picture properties of those in DPB surfaces.
 *
 * If only progressive scan is supported, each surface contains one whole
 * frame picture.
 * Otherwise, each surface contains two fields of whole picture.
 * In this case, two entries of ReferenceFrames[] may share same picture_id
 * value.
 */
typedef struct _VASTPictureHEVC
{
    /** \brief reconstructed picture buffer surface index
     * invalid when taking value VAST_INVALID_SURFACE.
     */
    VASTSurfaceID             picture_id;
    /** \brief picture order count.
     * in HEVC, POCs for top and bottom fields of same picture should
     * take different values.
     */
    int32_t                 pic_order_cnt;
    /* described below */
    uint32_t                flags;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTPictureHEVC;

typedef struct _VAPictureAV1
{
    /** \brief reconstructed picture buffer surface index
     * invalid when taking value VA_INVALID_SURFACE.
     */
    VASTSurfaceID             picture_id;
    /** \brief picture order count.
     * in HEVC, POCs for top and bottom fields of same picture should
     * take different values.
     */
    int32_t                 pic_order_cnt;
    /* described below */
    uint32_t                flags;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VAPictureAV1;

#define DEFAULT_VAST -255
#define MAX_TEMPORAL_LAYERS_VAST         4
#define MAX_SCENE_CHANGE_VAST 20
#define HANTRO_MAX_NUM_ROI_REGIONS_VAST                     8
#define HANTRO_MAX_NUM_IPCM_REGIONS_VAST                    8
#define HANTRO_MAX_NUM_INTRA_AREA_REGIONS_VAST              1
#define HANTRO_MAX_SUBPIC_SUM_VAST                     8

  /* HDR10 */
  typedef struct _Hdr10DisplaySeiVast
  {
	  uint8_t hdr10_display_enable;
	  uint16_t hdr10_dx0;
	  uint16_t hdr10_dy0;
	  uint16_t hdr10_dx1;
	  uint16_t hdr10_dy1;
	  uint16_t hdr10_dx2;
	  uint16_t hdr10_dy2;
	  uint16_t hdr10_wx;
	  uint16_t hdr10_wy;
	  uint32_t hdr10_maxluma;
	  uint32_t hdr10_minluma;
  }VASTHdr10DisplaySei;

  typedef struct _Hdr10LightLevelSeiVast
  {
	  uint8_t  hdr10_lightlevel_enable;
	  uint16_t hdr10_maxlight;
	  uint16_t hdr10_avglight;
  }VASTHdr10LightLevelSei;
  typedef struct _VuiColorDescriptionVast
  {
	  uint8_t vuiColorDescripPresentFlag;                        /* color description present in the vui.0- not present, 1- present */
	  uint8_t vuiColorPrimaries;                                 /* Color's Primaries */
	  uint8_t vuiTransferCharacteristics;                        /* Transfer Characteristics */
	  uint8_t vuiMatrixCoefficients;                             /* Matrix Coefficients */
  }VASTVuiColorDescription;

typedef struct _VAEncMiscParameter
{
    int use_vast_params;
    unsigned int rate_control_mode;
    unsigned int quality_level;
    unsigned int quality_range;
    unsigned int num_frames_in_sequence;
    unsigned int frame_width_in_pixel;
    unsigned int frame_height_in_pixel;
    unsigned int max_slice_or_seg_num;

    unsigned int recon_frame_compression_mode;
    unsigned int enable_P010_ref;

    struct {
        unsigned int num_layers;
        unsigned int size_frame_layer_ids;
        unsigned int frame_layer_ids[32];
        unsigned int curr_frame_layer_id;
    } layer;

    struct {
        unsigned short gop_size;
        unsigned short num_iframes_in_gop;
        unsigned short num_pframes_in_gop;
        unsigned short num_bframes_in_gop;
        unsigned int bits_per_second[MAX_TEMPORAL_LAYERS_VAST];

        int32_t inputRateNumer;
        int32_t inputRateDenom;
        int32_t outputRateNumer;
        int32_t outputRateDenom;
        unsigned int mb_rate_control[MAX_TEMPORAL_LAYERS_VAST];
        unsigned int target_percentage[MAX_TEMPORAL_LAYERS_VAST];
        unsigned int hrd_buffer_size;
        unsigned int hrd_initial_buffer_fullness;
        unsigned int window_size;
        unsigned int initial_qp;
        unsigned int min_qp;
        unsigned int need_reset;
        unsigned int need_reset_new_sps;

        unsigned int smooth_psnr_in_GOP;
        int          intra_qp_delta;
        unsigned int fixedIntraQp;
        unsigned int basic_unit_size;
        unsigned int disable_frame_skip;

        unsigned int num_roi;
        unsigned int roi_max_delta_qp;
        unsigned int roi_min_delta_qp;
        unsigned int roi_value_is_qp_delta;
        VASTEncROI roi[HANTRO_MAX_NUM_ROI_REGIONS_VAST];

        unsigned int roimap_is_enabled;
        unsigned int roimap_block_unit;
        unsigned int roi_map_version;
        VASTBufferID roimap_ipcmmap_buf_id;

        unsigned int cu_ctrl_version;


        unsigned int cu_ctrl_index_enable;


        unsigned int num_ipcm;
        VASTRectangle ipcm[HANTRO_MAX_NUM_IPCM_REGIONS_VAST];

        unsigned int num_intra_area;
        VASTRectangle intra_area[HANTRO_MAX_NUM_INTRA_AREA_REGIONS_VAST];

        unsigned int cir_start;
        unsigned int cir_interval;

        unsigned int gdr_duration;

        unsigned int video_full_range;

        unsigned int ipcmmap_is_enabled;
        unsigned int skipmap_is_enabled;
        unsigned int pcm_loop_filter_disabled_flag;

        unsigned int mb_cu_encoding_info_output_is_enabled;


        int use_extend_rc_params;
        int tolMovingBitRate;
        int monitorFrames;
        int bitVarRangeI;
        int bitVarRangeP;
        int bitVarRangeB;
        unsigned int u32StaticSceneIbitPercent;
        unsigned int rcQpDeltaRange;
        unsigned int rcBaseMBComplexity;
        float tolCtbRcInter;
        float tolCtbRcIntra;
        float maxFrameSizeMultiple; /**< maximum multiple to average target frame size */
        int picQpDeltaMin;
        int picQpDeltaMax;
        unsigned int hrd;
        unsigned int bitrateWindow;
        unsigned int pictureSkip;
        int qpHdr;
        unsigned int qpMax;

        unsigned int psnr_info_output_is_enabled;
        VASTBufferID psnr_info_buf_id;
        int32_t cqp;
        int32_t ctbRcRowQpStep;
        uint32_t longTermGap;
        uint32_t longTermGapOffset;
        uint32_t ltrInterval;
        int32_t longTermQpDelta;
        int32_t gopLowdelay;
        unsigned int qpMaxI;
        unsigned int qpMinI;
        int32_t bFrameQpDelta;
        int32_t vbr;
        char * gopCfg;
        char * roiMapDeltaQpFile;
        char * roiMapDeltaQpBinFile;
        char * ipcmMapFile;
        char * roiMapInfoBinFile;
        char * RoimapCuCtrlInfoBinFile;
        char * RoimapCuCtrlIndexBinFile;
        uint32_t skipMapBlockUnit;
        char * skipMapFile;
        int adaptiveIntraQpDelta;
    } brc;

    struct {
        int use_extend_coding_ctrl_params;

        unsigned int cabacInitFlag;
        unsigned int enableCabac;
        unsigned int disableDeblockingFilter;
        unsigned int enableSao;
        int tc_Offset;
        int beta_Offset;
        unsigned int enableDeblockOverride;
        unsigned int deblockOverride;
        int32_t tiles_enabled_flag;
        int num_tile_columns;
        int num_tile_rows;
        int loop_filter_across_tiles_enabled_flag;
        unsigned int enableScalingList;
        unsigned int RpsInSliceHeader;
        int chroma_qp_offset;

        uint32_t streamMultiSegmentMode;
        uint32_t streamMultiSegmentAmount;

        uint32_t noiseReductionEnable;
        uint32_t noiseLow;
        uint32_t firstFrameSigma;
        unsigned int fieldOrder;
        int32_t smartModeEnable;
        /* dynamic rdo params */
        uint32_t dynamicRdoEnable;
        uint32_t dynamicRdoCu16Bias;
        uint32_t dynamicRdoCu16Factor;
        uint32_t dynamicRdoCu32Bias;
        uint32_t dynamicRdoCu32Factor;

    } coding_ctrl;

    //line buffer
    struct {
        unsigned int inputLineBufMode;
        unsigned int inputLineBufDepth;
        unsigned int amountPerLoopBack;
    } low_latency;

    struct {
        int use_gmv_params;
        int16_t  mv0[2];  /* past reference */
        int16_t  mv1[2];  /* future reference */
        char * gmvFileName[2];
    } gmv;

    struct {
        /** \brief cropping offset x, in pixel unit, in original picture*/
        unsigned int        cropping_offset_x;
        /** \brief cropping offset y,in pixel unit, in original picture*/
        unsigned int        cropping_offset_y;

        /** \brief cropped width in pixel unit, in original picture. if 0, equal to input surface orig_width*/
        unsigned int        cropped_width;
        /** \brief cropped height in pixel unit, in original picture. if 0, equal to input surface orig_height*/
        unsigned int        cropped_height;


        /** \brief rotation. VA_PREPROCESS_ROTATION_NONE - none, VA_PREPROCESS_ROTATION_270 - 270, VA_PREPROCESS_ROTATION_90 - 90, VA_PREPROCESS_ROTATION_180 - 180 */
        unsigned int        rotation;
        /** \brief mirror. 0 - none, 1 - mirror */
        unsigned int        mirror;
        /** \brief constant chroma enabled. */
        unsigned int        preprocess_constant_chroma_is_enabled;
        /** \brief constant Cb value. */
        unsigned int        constCb;
        /** \brief constant Cr value. */
        unsigned int        constCr;
        /** \brief colorConversion value. */
        unsigned int        colorConversion;
        /** \brief down-scaled width value.
            *Optional down-scaled output picture width,multiple of 4. 0=disabled. [16..width] */
        unsigned int        scaledWidth;
        /** \brief down-scaled height value.
            * Optional down-scaled output picture height,multiple of 2. [96..height] */
        unsigned int        scaledHeight;
        unsigned int interlacedFrame;
        unsigned int codedChromaIdc;
        unsigned int scaledOutputFormat;
    } preprocess;
    /**packed header output control**/
    struct {
        unsigned int sps_reoutput_enable;
        unsigned int pps_reoutput_enable;
        unsigned int sei_output_enable;
        unsigned int aud_output_enable;
    } phoc;


    void* vsi_private_context;
    //VASTProfile profile;

    unsigned int is_tmp_id: 1;
    unsigned int low_power_mode: 1;
    unsigned int soft_batch_force: 1;
    unsigned int context_roi: 1;
    unsigned int is_new_sequence: 1; /* Currently only valid for H.264, TODO for other codecs */

    int temporal_id;

    unsigned char special_size;
    unsigned char ltrcnt;
    int skipFramePOC;
    unsigned int enableOutputCuInfo;
    unsigned int hashtype;
    unsigned int ssim;
    unsigned int enableVuiTimingInfo;
    unsigned int verbose;

    /* for HDR10 */
    unsigned int use_hdr10_params;
    VASTHdr10DisplaySei    Hdr10Display;
    VASTHdr10LightLevelSei Hdr10LightLevel;
    VASTVuiColorDescription      Hdr10Color;

    /**used for JPEG Lossless. */
    unsigned int losslessEn;
    unsigned int predictMode;
    unsigned int ptransValue;

    struct {
        int codingWidth;
        int codingHeight;
        int lumWidthSrc;
        int lumHeightSrc;
        int inputFormat;
        int horOffsetSrc;
        int verOffsetSrc;
        int input_alignment;
        int ref_alignment;
        int ref_ch_alignment;
        int aqInfoAlignment;
    } image_info;

    unsigned int flushRemainingFrames;
    unsigned int extDSRatio;
    unsigned int frames2Encode;
    unsigned int encodedFramesCnt;
    unsigned int lookaheadDepth;

    VASTBufferID adaptive_gop_decision_buf_id;
    uint32_t adaptive_gop_buffer;
    unsigned int adaptiveGOPEn;

    unsigned int numberMultiCore;
    unsigned int *dualBufMultiCore[2];

    unsigned int has_dec400;
    unsigned int intraPicRate;
    int32_t sceneChange[MAX_SCENE_CHANGE_VAST];
    unsigned int tier;
    uint32_t cpbMaxRate;
    int32_t outReconFrame;
    int32_t formatCustomizedType;
    int32_t multimode;
    uint32_t bitDepthLuma;
    uint32_t bitDepthChroma;
    int32_t cuInfoVersion;
    uint32_t rasterscan;
    char * halfDsInput;
    unsigned int streamBufChain;
    unsigned int MEVertRange;
    uint32_t picOrderCntType;
    uint32_t log2MaxPicOrderCntLsb;
    uint32_t log2MaxFrameNum;
    uint32_t preset;
    uint32_t tune;
    uint32_t firstPic;
    uint32_t lastPic;
    uint32_t lastVastFrame;
    uint32_t lastCodingType;
    int32_t crf;
    uint32_t userCoreID;
    int32_t insertIDR;
    int32_t P2B;
    int32_t bBPyramid; // 0: non-referece B Frames 1: reference B Frames
    int32_t sei_buf_count;
    VASTBufferID sei_buf_id[MAX_SEI_COUNT];
    int32_t llRc;
    int32_t ctbRc;
    int32_t gopChangeIdr;//for insertIDR
    int32_t psnrType;//for psnr,411 or 611
    int32_t speedLimit;//for multi-mode speed limit;
    unsigned int maxBFrames;
} VAEncMiscParameter;


typedef enum {
    VA_SINGLE_CORE_MODE,    //one stream encode on one encoder core
    VA_MULTI_CORE_MODE,     //one stream encode on multi encoder core
    VA_WORK_MODE_MAX,
}va_encoder_workmode_t;

#define VA_PICTURE_AV1_INVALID                  0x00000001

/* flags in VASTPictureHEVC could be OR of the following */
#define VAST_PICTURE_HEVC_INVALID                 0x00000001
/** \brief indication of interlace scan picture.
 * should take same value for all the pictures in sequence.
 */
#define VAST_PICTURE_HEVC_FIELD_PIC               0x00000002
/** \brief polarity of the field picture.
 * top field takes even lines of buffer surface.
 * bottom field takes odd lines of buffer surface.
 */
#define VAST_PICTURE_HEVC_BOTTOM_FIELD            0x00000004
/** \brief Long term reference picture */
#define VAST_PICTURE_HEVC_LONG_TERM_REFERENCE     0x00000008
/**
 * VAST_PICTURE_HEVC_RPS_ST_CURR_BEFORE, VAST_PICTURE_HEVC_RPS_ST_CURR_AFTER
 * and VAST_PICTURE_HEVC_RPS_LT_CURR of any picture in ReferenceFrames[] should
 * be exclusive. No more than one of them can be set for any picture.
 * Sum of NumPocStCurrBefore, NumPocStCurrAfter and NumPocLtCurr
 * equals NumPocTotalCurr, which should be equal to or smaller than 8.
 * Application should provide valid values for both short format and long format.
 * The pictures in DPB with any of these three flags turned on are referred by
 * the current picture.
 */
/** \brief RefPicSetStCurrBefore of HEVC spec variable
 * Number of ReferenceFrames[] entries with this bit set equals
 * NumPocStCurrBefore.
 */
#define VAST_PICTURE_HEVC_RPS_ST_CURR_BEFORE      0x00000010
/** \brief RefPicSetStCurrAfter of HEVC spec variable
 * Number of ReferenceFrames[] entries with this bit set equals
 * NumPocStCurrAfter.
 */
#define VAST_PICTURE_HEVC_RPS_ST_CURR_AFTER       0x00000020
/** \brief RefPicSetLtCurr of HEVC spec variable
 * Number of ReferenceFrames[] entries with this bit set equals
 * NumPocLtCurr.
 */
#define VAST_PICTURE_HEVC_RPS_LT_CURR             0x00000040

#include <vastva/va_dec_hevc.h>
#include <vastva/va_dec_jpeg.h>
#include <vastva/va_dec_vp8.h>
#include <vastva/va_dec_vp9.h>
#include <vastva/va_enc_hevc.h>
#include <vastva/va_fei_hevc.h>
#include <vastva/va_enc_h264.h>
#include <vastva/va_enc_jpeg.h>
#include <vastva/va_enc_mpeg2.h>
#include <vastva/va_enc_vp8.h>
#include <vastva/va_enc_vp9.h>
#include <vastva/va_fei.h>
#include <vastva/va_fei_h264.h>
#include <vastva/va_vpp.h>

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* _VA_H_ */