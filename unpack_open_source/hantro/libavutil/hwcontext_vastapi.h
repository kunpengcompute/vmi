/*
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef AVUTIL_HWCONTEXT_VASTAPI_H
#define AVUTIL_HWCONTEXT_VASTAPI_H

#include <vastva/va.h>

#include <va_hantro/va_hantro.h> //add by vastai to support hantro external function

#define VASTAI_DMA_BUFFER_MAX  128

/**
 * @file
 * API-specific header for AV_HWDEVICE_TYPE_VASTAPI.
 *
 * Dynamic frame pools are supported, but note that any pool used as a render
 * target is required to be of fixed size in order to be be usable as an
 * argument to vastCreateContext().
 *
 * For user-allocated pools, AVHWFramesContext.pool must return AVBufferRefs
 * with the data pointer set to a VASTSurfaceID.
 */

enum {
    /**
     * The quirks field has been set by the user and should not be detected
     * automatically by av_hwdevice_ctx_init().
     */
    AV_VASTAPI_DRIVER_QUIRK_USER_SET = (1 << 0),
    /**
     * The driver does not destroy parameter buffers when they are used by
     * vastRenderPicture().  Additional code will be required to destroy them
     * separately afterwards.
     */
    AV_VASTAPI_DRIVER_QUIRK_RENDER_PARAM_BUFFERS = (1 << 1),

    /**
     * The driver does not support the VASTSurfaceAttribMemoryType attribute,
     * so the surface allocation code will not try to use it.
     */
    AV_VASTAPI_DRIVER_QUIRK_ATTRIB_MEMTYPE = (1 << 2),

    /**
     * The driver does not support surface attributes at all.
     * The surface allocation code will never pass them to surface allocation,
     * and the results of the vastQuerySurfaceAttributes() call will be faked.
     */
    AV_VASTAPI_DRIVER_QUIRK_SURFACE_ATTRIBUTES = (1 << 3),
};

/**
 * VASTAPI connection details.
 *
 * Allocated as AVHWDeviceContext.hwctx
 */
typedef struct AVVASTAPIDeviceContext {
    /**
     * The VASTDisplay handle, to be filled by the user.
     */
    VASTDisplay display;
    /**
     * Driver quirks to apply - this is filled by av_hwdevice_ctx_init(),
     * with reference to a table of known drivers, unless the
     * AV_VASTAPI_DRIVER_QUIRK_USER_SET bit is already present.  The user
     * may need to refer to this field when performing any later
     * operations using VASTAPI with the same VASTDisplay.
     */
    unsigned int driver_quirks;
} AVVASTAPIDeviceContext;

/**
 * VASTAPI-specific data associated with a frame pool.
 *
 * Allocated as AVHWFramesContext.hwctx.
 */
typedef struct AVVASTAPIFramesContext {
    /**
     * Set by the user to apply surface attributes to all surfaces in
     * the frame pool.  If null, default settings are used.
     */
    VASTSurfaceAttrib *attributes;
    int           nb_attributes;
    /**
     * The surfaces IDs of all surfaces in the pool after creation.
     * Only valid if AVHWFramesContext.initial_pool_size was positive.
     * These are intended to be used as the render_targets arguments to
     * vastCreateContext().
     */
    VASTSurfaceID     *surface_ids;
    int           nb_surfaces;
} AVVASTAPIFramesContext;

/**
 * VASTAPI hardware pipeline configuration details.
 *
 * Allocated with av_hwdevice_hwconfig_alloc().
 */
typedef struct AVVASTAPIHWConfig {
    /**
     * ID of a VASTAPI pipeline configuration.
     */
    VASTConfigID config_id;
} AVVASTAPIHWConfig;

#endif /* AVUTIL_HWCONTEXT_VASTAPI_H */
