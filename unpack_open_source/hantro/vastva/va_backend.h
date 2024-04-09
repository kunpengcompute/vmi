/*
 * Copyright (c) 2007 Intel Corporation. All Rights Reserved.
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
 * IN NO EVENT SHALL PRECISION INSIGHT AND/OR ITS SUPPLIERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * Video Decode Acceleration -Backend API
 */

#ifndef _VAST_BACKENDAI_H_
#define _VAST_BACKENDAI_H_

#include <vastva/va.h>
#include <linux/videodev2.h>

typedef struct VASTDriverContext *VASTDriverContextP;
typedef struct VASTDisplayContext *VASTDisplayContextP;

/** \brief VA display types. */
enum {
    /** \brief Mask to major identifier for VA display type. */
    VAST_DISPLAY_MAJOR_MASK = 0xf0,

    /** \brief VA/X11 API is used, through vastGetDisplay() entry-point. */
    VAST_DISPLAY_X11      = 0x10,
    /** \brief VA/GLX API is used, through vastGetDisplayGLX() entry-point. */
    VAST_DISPLAY_GLX      = (VAST_DISPLAY_X11 | (1 << 0)),
    /** \brief VA/Android API is used, through vastGetDisplay() entry-point. */
    VAST_DISPLAY_ANDROID  = 0x20,
    /** \brief VA/DRM API is used, through vastGetDisplayDRM() entry-point. */
    VAST_DISPLAY_DRM      = 0x30,
    /** \brief VA/DRM API is used, with a render-node device path */
    VAST_DISPLAY_DRM_RENDERNODES = (VAST_DISPLAY_DRM | (1 << 0)),
    /** \brief VA/Wayland API is used, through vastGetDisplayWl() entry-point. */
    VAST_DISPLAY_WAYLAND  = 0x40,
};

struct VASTDriverVTable
{
	VASTStatus (*vastTerminate) ( VASTDriverContextP ctx );

	VASTStatus (*vastQueryConfigProfiles) (
		VASTDriverContextP ctx,
		VASTProfile *profile_list,	/* out */
		int *num_profiles			/* out */
	);

	VASTStatus (*vastQueryConfigEntrypoints) (
		VASTDriverContextP ctx,
		VASTProfile profile,
		VASTEntrypoint  *entrypoint_list,	/* out */
		int *num_entrypoints			/* out */
	);

	VASTStatus (*vastGetConfigAttributes) (
		VASTDriverContextP ctx,
		VASTProfile profile,
		VASTEntrypoint entrypoint,
		VASTConfigAttrib *attrib_list,	/* in/out */
		int num_attribs
	);

	VASTStatus (*vastCreateConfig) (
		VASTDriverContextP ctx,
		VASTProfile profile, 
		VASTEntrypoint entrypoint, 
		VASTConfigAttrib *attrib_list,
		int num_attribs,
		VASTConfigID *config_id		/* out */
	);

	VASTStatus (*vastDestroyConfig) (
		VASTDriverContextP ctx,
		VASTConfigID config_id
	);

	VASTStatus (*vastQueryConfigAttributes) (
		VASTDriverContextP ctx,
		VASTConfigID config_id, 
		VASTProfile *profile,		/* out */
		VASTEntrypoint *entrypoint, 	/* out */
		VASTConfigAttrib *attrib_list,	/* out */
		int *num_attribs		/* out */
	);

	VASTStatus (*vastCreateSurfaces) (
		VASTDriverContextP ctx,
		int width,
		int height,
		int format,
		int num_surfaces,
		VASTSurfaceID *surfaces		/* out */
	);

	VASTStatus (*vastDestroySurfaces) (
		VASTDriverContextP ctx,
		VASTSurfaceID *surface_list,
		int num_surfaces
	);

	VASTStatus (*vastCreateContext) (
		VASTDriverContextP ctx,
		VASTConfigID config_id,
		int picture_width,
		int picture_height,
		int flag,
		VASTSurfaceID *render_targets,
		int num_render_targets,
		VASTContextID *context		/* out */
	);

	VASTStatus (*vastDestroyContext) (
		VASTDriverContextP ctx,
		VASTContextID context
	);

	VASTStatus (*vastCreateBuffer) (
		VASTDriverContextP ctx,
		VASTContextID context,		/* in */
		VASTBufferType type,		/* in */
		unsigned int size,		/* in */
		unsigned int num_elements,	/* in */
		void *data,			/* in */
                VASTBufferID *buf_id
	);

	VASTStatus (*vastBufferSetNumElements) (
		VASTDriverContextP ctx,
		VASTBufferID buf_id,	/* in */
		unsigned int num_elements	/* in */
	);

	VASTStatus (*vastMapBuffer) (
		VASTDriverContextP ctx,
		VASTBufferID buf_id,	/* in */
		void **pbuf         /* out */
	);

	VASTStatus (*vastUnmapBuffer) (
		VASTDriverContextP ctx,
		VASTBufferID buf_id	/* in */
	);

	VASTStatus (*vastDestroyBuffer) (
		VASTDriverContextP ctx,
		VASTBufferID buffer_id
	);

	VASTStatus (*vastBeginPicture) (
		VASTDriverContextP ctx,
		VASTContextID context,
		VASTSurfaceID render_target
	);

	VASTStatus (*vastRenderPicture) (
		VASTDriverContextP ctx,
		VASTContextID context,
		VASTBufferID *buffers,
		int num_buffers
	);

	VASTStatus (*vastEndPicture) (
		VASTDriverContextP ctx,
		VASTContextID context
	);

	VASTStatus (*vastSyncSurface) (
		VASTDriverContextP ctx,
		VASTSurfaceID render_target
	);

	VASTStatus (*vastQuerySurfaceStatus) (
		VASTDriverContextP ctx,
		VASTSurfaceID render_target,
		VASTSurfaceStatus *status	/* out */
	);

	VASTStatus (*vastQuerySurfaceError) (
		VASTDriverContextP ctx,
		VASTSurfaceID render_target,
                VASTStatus error_status,
                void **error_info /*out*/
	);

	VASTStatus (*vastPutSurface) (
    		VASTDriverContextP ctx,
		VASTSurfaceID surface,
		void* draw, /* Drawable of window system */
		short srcx,
		short srcy,
		unsigned short srcw,
		unsigned short srch,
		short destx,
		short desty,
		unsigned short destw,
		unsigned short desth,
		VASTRectangle *cliprects, /* client supplied clip list */
		unsigned int number_cliprects, /* number of clip rects in the clip list */
		unsigned int flags /* de-interlacing flags */
	);

	VASTStatus (*vastQueryImageFormats) (
		VASTDriverContextP ctx,
		VASTImageFormat *format_list,        /* out */
		int *num_formats           /* out */
	);

	VASTStatus (*vastCreateImage) (
		VASTDriverContextP ctx,
		VASTImageFormat *format,
		int width,
		int height,
		VASTImage *image     /* out */
	);

    VASTStatus (*vastGetSurfaceIdAddress)(
        VASTDriverContextP ctx,
        VASTDisplay dpy,
        VASTSurfaceID surface,
       unsigned long long *address /* out */
    );

    VASTStatus (*vastSetSurfaceIdAddress)(
        VASTDriverContextP ctx,
        VASTDisplay dpy,
        VASTSurfaceID surface,
       unsigned long long address
    );

    VASTStatus (*vastGetDieinfo)(
        VASTDriverContextP ctx,
        VASTDisplay dpy,
        int *die_id
    );

    
    VASTStatus (*vastCreateDmaHandle)(
        VASTDisplay dpy,
        int die_id /* in */,
        unsigned int dmabuff_size,
        void *dma_handle
    );

    VASTStatus (*vastDestroyDmaHandle)(
        VASTDisplay dpy,
        void *dma_handle
    );    

    VASTStatus (*vastDmaWriteBuf)(
        VASTDisplay dpy,
        unsigned long dst_soc_addr,
        int buf_size,
        void *dma_handle
    );      

    VASTStatus (*vastQueWriteDmaBufSg)(
        VASTDisplay dpy,
        void *channel,
        int channel_num,
        unsigned long axi_addr,
        unsigned int die_index
    );  

    VASTStatus (*vastDmaWriteExternalBuf)(
        VASTDisplay dpy,
        int fd,
        int die_id,
        uint64_t dmabuffer_addr,
        uint64_t axi_addr,
        unsigned int dmabuff_size,
        int src_type
    );

	VASTStatus (*vastDeriveImage) (
		VASTDriverContextP ctx,
		VASTSurfaceID surface,
		VASTImage *image     /* out */
	);

	VASTStatus (*vastDestroyImage) (
		VASTDriverContextP ctx,
		VASTImageID image
	);
	
	VASTStatus (*vastSetImagePalette) (
	        VASTDriverContextP ctx,
	        VASTImageID image,
	        /*
                 * pointer to an array holding the palette data.  The size of the array is
                 * num_palette_entries * entry_bytes in size.  The order of the components
                 * in the palette is described by the component_order in VASTImage struct
                 */
                unsigned char *palette
	);
	
	VASTStatus (*vastGetImage) (
		VASTDriverContextP ctx,
		VASTSurfaceID surface,
		int x,     /* coordinates of the upper left source pixel */
		int y,
		unsigned int width, /* width and height of the region */
		unsigned int height,
		VASTImageID image
	);

	VASTStatus (*vastPutImage) (
		VASTDriverContextP ctx,
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

	VASTStatus (*vastQuerySubpictureFormats) (
		VASTDriverContextP ctx,
		VASTImageFormat *format_list,        /* out */
		unsigned int *flags,       /* out */
		unsigned int *num_formats  /* out */
	);

	VASTStatus (*vastCreateSubpicture) (
		VASTDriverContextP ctx,
		VASTImageID image,
		VASTSubpictureID *subpicture   /* out */
	);

	VASTStatus (*vastDestroySubpicture) (
		VASTDriverContextP ctx,
		VASTSubpictureID subpicture
	);

        VASTStatus (*vastSetSubpictureImage) (
                VASTDriverContextP ctx,
                VASTSubpictureID subpicture,
                VASTImageID image
        );

	VASTStatus (*vastSetSubpictureChromakey) (
		VASTDriverContextP ctx,
		VASTSubpictureID subpicture,
		unsigned int chromakey_min,
		unsigned int chromakey_max,
		unsigned int chromakey_mask
	);

	VASTStatus (*vastSetSubpictureGlobalAlpha) (
		VASTDriverContextP ctx,
		VASTSubpictureID subpicture,
		float global_alpha 
	);

	VASTStatus (*vastAssociateSubpicture) (
		VASTDriverContextP ctx,
		VASTSubpictureID subpicture,
		VASTSurfaceID *target_surfaces,
		int num_surfaces,
		short src_x, /* upper left offset in subpicture */
		short src_y,
		unsigned short src_width,
		unsigned short src_height,
		short dest_x, /* upper left offset in surface */
		short dest_y,
		unsigned short dest_width,
		unsigned short dest_height,
		/*
		 * whether to enable chroma-keying or global-alpha
		 * see VA_SUBPICTURE_XXX values
		 */
		unsigned int flags
	);

	VASTStatus (*vastDeassociateSubpicture) (
		VASTDriverContextP ctx,
		VASTSubpictureID subpicture,
		VASTSurfaceID *target_surfaces,
		int num_surfaces
	);

	VASTStatus (*vastQueryDisplayAttributes) (
		VASTDriverContextP ctx,
		VASTDisplayAttribute *attr_list,	/* out */
		int *num_attributes		/* out */
        );

	VASTStatus (*vastGetDisplayAttributes) (
		VASTDriverContextP ctx,
		VASTDisplayAttribute *attr_list,	/* in/out */
		int num_attributes
        );
        
        VASTStatus (*vastSetDisplayAttributes) (
		VASTDriverContextP ctx,
                VASTDisplayAttribute *attr_list,
                int num_attributes
        );

        /* used by va trace */        
        VASTStatus (*vastBufferInfo) (
                   VASTDriverContextP ctx,      /* in */
                   VASTBufferID buf_id,         /* in */
                   VASTBufferType *type,        /* out */
                   unsigned int *size,        /* out */
                   unsigned int *num_elements /* out */
        );

        /* lock/unlock surface for external access */    
        VASTStatus (*vastLockSurface) (
		VASTDriverContextP ctx,
                VASTSurfaceID surface,
                unsigned int *fourcc, /* out  for follow argument */
                unsigned int *luma_stride,
                unsigned int *chroma_u_stride,
                unsigned int *chroma_v_stride,
                unsigned int *luma_offset,
                unsigned int *chroma_u_offset,
                unsigned int *chroma_v_offset,
                unsigned int *buffer_name, /* if it is not NULL, assign the low lever
                                            * surface buffer name
                                            */
                void **buffer /* if it is not NULL, map the surface buffer for
                                * CPU access
                                */
        );
    
        VASTStatus (*vastUnlockSurface) (
		VASTDriverContextP ctx,
                VASTSurfaceID surface
        );

        /* DEPRECATED */
        VASTStatus
        (*vastGetSurfaceAttributes)(
            VASTDriverContextP    dpy,
            VASTConfigID          config,
            VASTSurfaceAttrib    *attrib_list,
            unsigned int        num_attribs
        );

        VASTStatus
        (*vastCreateSurfaces2)(
            VASTDriverContextP    ctx,
            unsigned int        format,
            unsigned int        width,
            unsigned int        height,
            VASTSurfaceID        *surfaces,
            unsigned int        num_surfaces,
            VASTSurfaceAttrib    *attrib_list,
            unsigned int        num_attribs
        );

        VASTStatus
        (*vastQuerySurfaceAttributes)(
            VASTDriverContextP    dpy,
            VASTConfigID          config,
            VASTSurfaceAttrib    *attrib_list,
            unsigned int       *num_attribs
        );

        VASTStatus
        (*vastAcquireBufferHandle)(
            VASTDriverContextP    ctx,
            VASTBufferID          buf_id,         /* in */
            VASTBufferInfo *      buf_info        /* in/out */
        );

        VASTStatus
        (*vastReleaseBufferHandle)(
            VASTDriverContextP    ctx,
            VASTBufferID          buf_id          /* in */
        );

        VASTStatus (*vastCreateMFContext) (
            VASTDriverContextP ctx,
            VASTMFContextID *mfe_context    /* out */
        );

        VASTStatus (*vastMFAddContext) (
            VASTDriverContextP ctx,
            VASTMFContextID mf_context,
            VASTContextID context
        );

        VASTStatus (*vastMFReleaseContext) (
            VASTDriverContextP ctx,
            VASTMFContextID mf_context,
            VASTContextID context
        );

        VASTStatus (*vastMFSubmit) (
            VASTDriverContextP ctx,
            VASTMFContextID mf_context,
            VASTContextID *contexts,
            int num_contexts
        );
	VASTStatus (*vastCreateBuffer2) (
            VASTDriverContextP ctx,
            VASTContextID context,                /* in */
            VASTBufferType type,                  /* in */
            unsigned int width,                 /* in */
            unsigned int height,                /* in */
            unsigned int *unit_size,            /* out */
            unsigned int *pitch,                /* out */
            VASTBufferID *buf_id                  /* out */
	);

        VASTStatus (*vastQueryProcessingRate) (
            VASTDriverContextP ctx,               /* in */
            VASTConfigID config_id,               /* in */
            VASTProcessingRateParameter *proc_buf,/* in */
            unsigned int *processing_rate	/* out */
        );

        VASTStatus
        (*vastExportSurfaceHandle)(
            VASTDriverContextP    ctx,
            VASTSurfaceID         surface_id,     /* in */
            uint32_t            mem_type,       /* in */
            uint32_t            flags,          /* in */
            void               *descriptor      /* out */
        );

        /** \brief Reserved bytes for future use, must be zero */
        unsigned long reserved[57];
};

struct VASTDriverContext
{
    void *pDriverData;

    /**
     * The core VA implementation hooks.
     *
     * This structure is allocated from libva with calloc().
     */
    struct VASTDriverVTable *vtable;

    /**
     * The VA/GLX implementation hooks.
     *
     * This structure is intended for drivers that implement the
     * VA/GLX API. The driver implementation is responsible for the
     * allocation and deallocation of this structure.
     */
    struct VASTDriverVTableGLX *vtable_glx;

    /**
     * The VA/EGL implementation hooks.
     *
     * This structure is intended for drivers that implement the
     * VA/EGL API. The driver implementation is responsible for the
     * allocation and deallocation of this structure.
     */
    struct VADriverVTableEGL *vtable_egl;

    /**
     * The third-party/private implementation hooks.
     *
     * This structure is intended for drivers that implement the
     * private API. The driver implementation is responsible for the
     * allocation and deallocation of this structure.
     */
    void *vtable_tpi;

    void *native_dpy;
    int x11_screen;
    int version_major;
    int version_minor;
    int max_profiles;
    int max_entrypoints;
    int max_attributes;
    int max_image_formats;
    int max_subpic_formats;
    int max_display_attributes;
    const char *str_vendor;

    void *handle;			/* dlopen handle */

    /**
     * \brief DRM state.
     *
     * This field holds driver specific data for DRM-based
     * drivers. This structure is allocated from libva with
     * calloc(). Do not deallocate from within VA driver
     * implementations.
     *
     * All structures shall be derived from struct drm_state_vastai. So, for
     * instance, this field holds a dri_state_vastai structure for VA/X11
     * drivers that use the DRM protocol.
     */
    void *drm_state_vastai;

    void *glx;				/* opaque for GLX code */

    /** \brief VA display type. */
    unsigned long display_type;

    /**
     * The VA/Wayland implementation hooks.
     *
     * This structure is intended for drivers that implement the
     * VA/Wayland API. libVA allocates this structure with calloc()
     * and owns the resulting memory.
     */
    struct VASTDriverVTableWayland *vtable_wayland;

    /**
     * \brief The VA/VPP implementation hooks.
     *
     * This structure is allocated from libva with calloc().
     */
    struct VASTDriverVTableVPP *vtable_vpp;

    char *override_driver_name;

    void *pDisplayContext;

    /**
     * Error callback.
     *
     * This is set by libva when the driver is opened, and will not be
     * changed thereafter.  The driver can call it with an error message,
     * which will be propagated to the API user through their error
     * callbacks, or sent to a default output if no callback is available.
     *
     * It is expected that end users will always be able to see these
     * messages, so it should be called only for serious errors.  For
     * example, hardware problems or fatal configuration errors.
     *
     * @param pDriverContext  Pointer to the driver context structure
     *                        being used by the current driver.
     * @param message  Message to send to the API user.  This must be a
     *                 null-terminated string.
     */
    void (*error_callback)(VASTDriverContextP pDriverContext,
                           const char *message);
    /**
     * Info callback.
     *
     * This has the same behaviour as the error callback, but has its
     * own set of callbacks to the API user.
     *
     * It should be used for informational messages which may be useful
     * for an application programmer or for debugging.  For example, minor
     * configuration errors, or information about the reason when another
     * API call generates an error return.  It is not expected that end
     * users will see these messages.
     *
     * @param pDriverContext  Pointer to the driver context structure
     *                        being used by the current driver.
     * @param message  Message to send to the API user.  This must be a
     *                 null-terminated string.
     */
    void (*info_callback)(VASTDriverContextP pDriverContext,
                          const char *message);

    unsigned long reserved[38];         /* reserve for future add-ins, decrease the subscript accordingly */
};

#define VAST_DISPLAY_MAGIC 0x56414430 /* VAD0 */
struct VASTDisplayContext
{
    int vadpy_magic;
    
    VASTDisplayContextP pNext;
    VASTDriverContextP pDriverContext;

    int (*vaIsValid) (
	VASTDisplayContextP ctx
    );

    void (*vaDestroy) (
	VASTDisplayContextP ctx
    );

    VASTStatus (*vaGetDriverName) (
	VASTDisplayContextP ctx,
	char **driver_name
    );

    void *opaque; /* opaque for display extensions (e.g. GLX) */
    void *vatrace; /* opaque for VA trace context */
    void *vafool; /* opaque for VA fool context */

    VASTMessageCallback error_callback;
    void *error_callback_user_context;
    VASTMessageCallback info_callback;
    void *info_callback_user_context;

    /** \brief Reserved bytes for future use, must be zero */
    unsigned long reserved[32];
};

typedef VASTStatus (*VASTDriverInit) (
    VASTDriverContextP driver_context
);

#endif /* _VA_BACKEND_H_ */
