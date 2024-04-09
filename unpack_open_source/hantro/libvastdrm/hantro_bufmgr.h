/*
 * Copyright(C) 2018 Verisilicon
 * All Rights Reserved.
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

#ifndef HANTRO_BUFMGR_H
#define HANTRO_BUFMGR_H

#include <stdio.h>
#include <stdint.h>
#include <stdio.h>
#include <pthread.h>
#include "drm.h"
#include "hantro_metadata.h"

#if defined(__cplusplus)
extern "C" {
#endif


#define DRIVER_NAME	"hantro"

enum {
    HANTRO_ERROR = 0,
    HANTRO_WARN,
    HANTRO_INFO,
    HANTRO_DEBUG,
    HANTRO_TRACE,
    HANTRO_COUNT
};
extern char hantrodrm_level[HANTRO_COUNT][20];

// int drm_log_level;
//#define DBG(l, ...) do {if (l <= drm_log_level) printf(__VA_ARGS__); } while (0)

#ifdef __FREERTOS__
//addr_t has been defined in basetype.h //Now the FreeRTOS mem need to support 64bit env
#elif defined(__linux__)
#undef  addr_t
#define addr_t ADDR_T_VCMD
typedef size_t addr_t;
#endif

typedef uint8_t  u8;
/* typedef int16_t   i16; */
typedef uint16_t  u16;
/* typedef int32_t   i32; */
typedef uint32_t  u32;



struct regsize_desc {
  __u32 slice; /* id of the slice */
  __u32 id; /* id of the subsystem */
  __u32 type; /* type of core to be written */
  __u32 size; /* iosize of the core */
};





struct config_parameter
{
  u16 module_type;                    //input vc8000e=0,cutree=1,vc8000d=2,jpege=3, jpegd=4
  u16 vcmd_core_num;                  //output, how many vcmd cores are there with corresponding module_type.
  u16 submodule_main_addr;            //output,if submodule addr == 0xffff, this submodule does not exist.
  u16 submodule_dec400_addr;          //output ,if submodule addr == 0xffff, this submodule does not exist.
  u16 submodule_L2Cache_addr;         //output,if submodule addr == 0xffff, this submodule does not exist.
  u16 submodule_MMU_addr;             //output,if submodule addr == 0xffff, this submodule does not exist.
  u16 submodule_MMUWrite_addr;        //output,if submodule addr == 0xffff, this submodule does not exist.
  u16 submodule_axife_addr;           //output,if submodule addr == 0xffff, this submodule does not exist.
  u16 config_status_cmdbuf_id;        // output , this status comdbuf save the all register values read in driver init.//used for analyse configuration in cwl.
  u32 vcmd_hw_version_id;
};

#ifdef VASTAI_DMA

#define VASTAI_MEM_WRITE 0
#define VASTAI_MEM_READ 1
#define LIB_VACCRT_PATH "libvaccrt.so"

typedef enum deviceRuntimeError {
    rtSuccess = 0,
    rtReady,
    /* -------- normal error -------- */
    rtErrMalloc,
    rtErrAssert,
    rtErrParamInvalid,
    rtErrOpenDevice,
    rtErrDevNotExist,
    rtErrDevInit,
    rtErrDlcNotExist,
    rtErrModelNameInvalid,
    rtErrModelCfgNotExist,
    rtErrModelMemCfgNotExist,
    rtErrMallocModel,
    rtErrAllocModelNotExist,
    rtErrAllocModelMemNotExist,
    rtErrSramNotExist,
    rtErrGetSramMem,
    rtErrMallocWeight,
    rtErrFreeWeight,
    rtErrMallocInOut,
    rtErrFreeInOut,
    rtErrMallocVideo,
    rtErrFreeVideo,
    rtErrMallocModelInfo,
    rtErrFreeModelInfo,
    rtErrMallocStreamInfo,
    rtErrFreeStreamInfo,
    rtErrMallocOperator,
    rtErrFreeOperator,
    rtErrGetDdrMem,
    rtErrMemcpyHostToDevice,
    rtErrMemcpyDeviceToHost,
    rtErrStorageInvalid,
    rtErrBatchIDInvalid,
    rtErrBatchSizeInvalid,
    // memory error
    // device management error
    // context management error
    // CSR parse error
    rtErrPipeSequence,
    rtErrInsType,      // type error
    rtErrInsParamLen,  // type and length conflict
    rtErrInsParam,     // the content of param error
    rtErrInsLayer,     // error instruction is inserted into one layer
    rtErrInsLayerSplit,// pipebegin and pipeend order error
    rtErrDevModelnameConflict,
    rtErrDevAdd,
    rtErrDevLoadModel,
    rtErrDevDestroyModel,
    rtErrDevRunModel,
    rtErrDevCreateStream,
    rtErrDevRunStream,
    rtErrDevSetinput,
    rtErrDevGetOutDesc,
    rtErrDevCopyDie2Die,
    rtErrOperatorInit,
    rtErrOperatorNotReady,
    rtErrOperatorNotExist,
    rtErrOperatorNameInvalid,
    // rtErrOperatorMalloc,
    rtErrOperatorLoad,
    rtErrModelInit,// model not initiallize or error in initiallize process
    rtErrModelNotReady,
    rtErrModelNotExist,// can not find mode node
    rtErrModelLayerlInit,
    rtErrModelLayerCopy,// failed to copy layers of model
    rtErrModelHWConfig,
    rtErrModelTieqParam,
    rtErrModelVdspNotInsert,
    rtErrModelVdspNotExist,
    rtErrSchedule,
    rtErrScheduleOdmaSRC,
    rtErrScheduleCdmaSRC,
    rtErrScheduleWdmaFW,
    rtErrScheduleWdmaLock,
    rtErrScheduleGroup,
    rtErrPthread,
    rtErrPthreadCreate,// failed to create a thread
    rtErrListFind,     // not find list node from list
    rtErrProNotExist,  // can not find process
    rtErrOthers,
    rtErrEnd
} rtError_t;

typedef struct ext_addr {
    void *soc_addr;// soc address (ddr address)
    void *bar_addr;// bar address
    void *vir_addr;// virtual address
} addr_ext_t;

typedef rtError_t (*vaccrt_init_t)(uint32_t dev_id);
typedef rtError_t (*vaccrt_malloc_video_t)(uint32_t dev_id, uint32_t align, size_t size, void **addr);
typedef rtError_t (*vaccrt_free_video_t)(uint32_t dev_id, void *addr);
typedef rtError_t (*vaccrt_get_current_pid_t)(uint32_t dev_id, uint32_t *pid);
typedef rtError_t (*vaccrt_get_process_status_t)(uint32_t dev_id, uint32_t pid, uint32_t *status);
typedef rtError_t (*vaccrt_get_video_reserver_ddr_t)(uint32_t dev_id, addr_ext_t *addr_ext);

typedef struct _runtime_func_t{
	vaccrt_init_t               	vaccrt_init;
	vaccrt_malloc_video_t       	vaccrt_malloc_video;
	vaccrt_free_video_t         	vaccrt_free_video;
	vaccrt_get_current_pid_t    	vaccrt_get_current_pid;
	vaccrt_get_process_status_t 	vaccrt_get_process_status;
	vaccrt_get_video_reserver_ddr_t vaccrt_get_video_reserver_ddr;
}runtime_func_t;

//used for sending data form host to device
typedef struct {
	uint32_t channel;
	void * stream_src_addr;         //stream buf addr on host user space
	unsigned long stream_dst_addr;  //stream buf phy addr on pcie device side
	u32 stream_size;                //stream data len
	void * vcmdbuf_src_addr;        //vcmd buf addr on host user space
	unsigned long vcmdbuf_dst_addr; //vcmd buf phy addr on pcie device side
	u32 vcmd_size;                  //vcmd data len
}vcmd_desc_t;

//used for receiving data form device to host
typedef struct {
	uint32_t channel;
	void * stream_dst_addr;         //stream buf addr on host user space
	unsigned long stream_src_addr;  //stream buf phy addr on pcie device side
	u32 stream_size;                //stream data len
}framebuf_desc_t;

typedef struct {
	u32 channel_id;
	u32 width;
	u32 height;
	u32 core_id;
	u32 die_id;
} channel_info_t;

typedef struct {
	char* src_url;
	int src_file_index;
	int src_stream_index;
	int src_width;
	int src_height;
}input_stream_info_t;

typedef struct {
	u16 vcmd_core_id;                   //input.
	u16 module_type;                    //input vc8000e=0,cutree=1,vc8000d=2,jpege=3, jpegd=4
	u32 decoderAsicId;                  // output , for main module: hardware build id;
	u32 hwBuildId;                      // output , hardware build id
	u32 l2CacheVersion;                 // output , l2cache version.
}vcmd_id_parameter;

typedef struct {
	u32 core_id;
	u32 cmdbuf_id;
	u16 status;
}waitcmd_parameter_t;

typedef struct {
	u8 dir; //transfer direction, host to device: 0, device to host: 1;
	unsigned long bus_addr; //device bus address;
	void* local_addr;//host virtual address;
	u32 len; //transfer data length;
}transfer_mem_param;

typedef struct {
	channel_info_t channel_info;                //channel id and core info
	input_stream_info_t input_stream_info;      //input stream info
}vastai_stream_info_t;

#endif

/*need to consider how many memory should be allocated for status.*/
struct exchange_parameter
{
  uint64_t executing_time;                 //input ;executing_time=encoded_image_size*(rdoLevel+1)*(rdoq+1);
  u16 module_type;                    //input input vc8000e=0,IM=1,vc8000d=2, jpege=3, jpegd=4
  u16 cmdbuf_size;                    //input, reserve is not used; link and run is input.
  u16 priority;                       //input,normal=0, high/live=1
  u16 cmdbuf_id;                      //output, it is unique in driver.
  u16 core_id;                        //just used for polling.
#ifdef VASTAI_DMA
  vcmd_desc_t vcmd_desc;
#endif
};



struct cmdbuf_mem_parameter
{
  u32 *virt_cmdbuf_addr;
  addr_t phy_cmdbuf_addr;             //cmdbuf pool base physical address
  u32 mmu_phy_cmdbuf_addr;             //cmdbuf pool base mmu mapping address
  u32 cmdbuf_total_size;              //cmdbuf pool total size in bytes.
  u16 cmdbuf_unit_size;               //one cmdbuf size in bytes. all cmdbuf have same size.
  u32 *virt_status_cmdbuf_addr;
  addr_t phy_status_cmdbuf_addr;      //status cmdbuf pool base physical address
  u32 mmu_phy_status_cmdbuf_addr;      //status cmdbuf pool base mmu mapping address
  u32 status_cmdbuf_total_size;       //status cmdbuf pool total size in bytes.
  u16 status_cmdbuf_unit_size;        //one status cmdbuf size in bytes. all status cmdbuf have same size.
  addr_t base_ddr_addr;               //for pcie interface, hw can only access phy_cmdbuf_addr-pcie_base_ddr_addr.
                                      //for other interface, this value should be 0?
#ifdef VASTAI_DMA
  addr_t vcmd_reg_mem_busAddress;     //start physical address of vcmd registers memory of  CMDBUF.
  u16 vcmd_core_id;                   //vcmd-core-id
#endif
};

struct CacheParams {
	uint64_t buf_size;
	u32 dpb_size;
};

/* the max meta data slots in one meta data buffer */
#define HANTRO_META_MAX_SLOTS     64
/* the default slice index of the memory to save the meta data */
#define HANTRO_META_SLICE_IDX     0

/*these definitions are identical to those in hantro.h*/
#define HANTRO_DOMAIN_NONE        0x00000
#define HANTRO_CPU_DOMAIN         0x00001
#define HANTRO_HEVC264_DOMAIN     0x00002
#define HANTRO_JPEG_DOMAIN        0x00004
#define HANTRO_DECODER0_DOMAIN    0x00008
#define HANTRO_DECODER1_DOMAIN    0x00010
#define HANTRO_DECODER2_DOMAIN    0x00020

#define CONFIG_HWDEC		(1 << 0)
#define CONFIG_HWENC		(1 << 1)
#define CONFIG_L2CACHE		(1 << 2)
#define CONFIG_DEC400		(1 << 3)

/* for CORE_ID will identify one core, it is combination of its slice_id and core index. */
#define KCORE(id) ((u32)(id)&0xff)
#define SLICE(id) ((u32)(id)>>16)
#define CORE_ID(slice, index) (((u32)(slice)<<16)|((u32)(index)&0xff))

/* for SUB_ID will dentify one peripheral, it is combination of its slice_id, parent node type and parent core index.
 * The peripheral can be dec400 or cache; 
 *   - slice index definition is unchanged. 
 *   - the node of the peripheral NODE(id) refers to its parent core number based on NODETYPE 
 *   - the node type is the parent node type of this periphearl, it can be NODE_TYPE_DEC or NODE_TYPE_ENC.  
 *     the NODETYPE is only for peripherals, it is unuseful for decoder or encoder core. 
 */
#define NODETYPE(id) (((u32)id >> 8) &0xff)
#define SUB_ID(slice, type, index) (((u32)(slice)<<16)|((u32)(type<<8)&0xff00)|((u32)(index)&0xff))
/* node type for NODETYPE(id), apply to be expanded */
#define NODE_TYPE_DEC	1
#define NODE_TYPE_ENC	2

/* Ioctl definitions */
/*hantro drm related */
#define HANTRO_IOCTL_START (DRM_COMMAND_BASE)
#define DRM_IOCTL_HANTRO_TESTCMD        DRM_IOWR(HANTRO_IOCTL_START, unsigned int)
#define DRM_IOCTL_HANTRO_GETPADDR       DRM_IOWR(HANTRO_IOCTL_START+1, struct hantro_addrmap)
#define DRM_IOCTL_HANTRO_HWCFG       DRM_IO(HANTRO_IOCTL_START+2)
#define DRM_IOCTL_HANTRO_TESTREADY      DRM_IOWR(HANTRO_IOCTL_START+3, struct hantro_fencecheck)
#define DRM_IOCTL_HANTRO_SETDOMAIN      DRM_IOWR(HANTRO_IOCTL_START+4, struct hantro_domainset)
#define DRM_IOCTL_HANTRO_ACQUIREBUF     DRM_IOWR(HANTRO_IOCTL_START+6, struct hantro_acquirebuf)
#define DRM_IOCTL_HANTRO_RELEASEBUF     DRM_IOWR(HANTRO_IOCTL_START+7, struct hantro_releasebuf)
#define DRM_IOCTL_HANTRO_GETPRIMEADDR     DRM_IOWR(HANTRO_IOCTL_START+8, unsigned long *)
#define DRM_IOCTL_HANTRO_PTR_PHYADDR     DRM_IOWR(HANTRO_IOCTL_START+9, unsigned long *)
#define DRM_IOCTL_HANTRO_QUERY_METADATA   DRM_IOWR(HANTRO_IOCTL_START+10, struct hantro_metainfo_params)
#define DRM_IOCTL_HANTRO_UPDATE_METADATA  DRM_IOWR(HANTRO_IOCTL_START+11, struct hantro_metainfo_params)

#define DRM_IOCTL_HANTRO_ADD_CLIENT       DRM_IOWR(HANTRO_IOCTL_START+12, struct hantro_client )
#define DRM_IOCTL_HANTRO_REMOVE_CLIENT    DRM_IOWR(HANTRO_IOCTL_START+13, struct hantro_client )

/* hantro enc related */
#define HX280ENC_IOC_START              DRM_IO(HANTRO_IOCTL_START + 17)
#define HX280ENC_IOCGHWOFFSET           DRM_IOR(HANTRO_IOCTL_START +  17, unsigned long long *)
#define HX280ENC_IOCGHWIOSIZE           DRM_IOWR(HANTRO_IOCTL_START + 18, unsigned long *)
#define HX280ENC_IOC_CLI                DRM_IO(HANTRO_IOCTL_START + 19)
#define HX280ENC_IOC_STI                DRM_IO(HANTRO_IOCTL_START + 20)
#define HX280ENC_IOCHARDRESET           DRM_IO(HANTRO_IOCTL_START + 21)   /* debugging tool */
#define HX280ENC_IOCGSRAMOFFSET         DRM_IOR(HANTRO_IOCTL_START +  22, unsigned long long *)
#define HX280ENC_IOCGSRAMEIOSIZE        DRM_IOR(HANTRO_IOCTL_START +  23, unsigned int *)
#define HX280ENC_IOCH_ENC_RESERVE       DRM_IOWR(HANTRO_IOCTL_START + 24, unsigned long *)
#define HX280ENC_IOCH_ENC_RELEASE       DRM_IOW(HANTRO_IOCTL_START + 25, unsigned long *)
#define HX280ENC_IOCG_CORE_NUM          DRM_IO(HANTRO_IOCTL_START + 26)
#define HX280ENC_IOCG_CORE_WAIT         DRM_IOWR(HANTRO_IOCTL_START + 27, unsigned int *)
//#define HX280ENC_IOCG_WRITE_REG         DRM_IOW( HANTRO_IOCTL_START+ 28, struct core_desc *)
//#define HX280ENC_IOCG_READ_REG          DRM_IOWR(HANTRO_IOCTL_START+ 29, struct core_desc *)
//#define HX280ENC_IOCG_PUSH_REG          DRM_IOW( HANTRO_IOCTL_START+ 30, struct core_desc *)
//#define HX280ENC_IOCG_PULL_REG          DRM_IOW( HANTRO_IOCTL_START+ 31, struct core_desc *)

#define HX280ENC_IOC_END                DRM_IO(HANTRO_IOCTL_START +  32)

/*slice related*/
#define HANTROSLICE_IOC_START             DRM_IO(HANTRO_IOCTL_START + 33)
#define DRM_IOCTL_HANTRO_GET_SLICENUM	DRM_IO(HANTRO_IOCTL_START+33)
#define HANTROSLICE_IOC_END             DRM_IO(HANTRO_IOCTL_START + 40)


/* hantro dec cmd */
/* Use 'k' as magic number */
#define HANTRODEC_IOC_MAGIC  'k'

/*
 * S means "Set" through a ptr,
 * T means "Tell" directly with the argument value
 * G means "Get": reply by setting through a pointer
 * Q means "Query": response is on the return value
 * X means "eXchange": G and S atomically
 * H means "sHift": T and Q atomically
 */

#define HANTRODEC_PP_INSTANCE       _IO(HANTRODEC_IOC_MAGIC, 1)
#define HANTRODEC_HW_PERFORMANCE    _IO(HANTRODEC_IOC_MAGIC, 2)
#define HANTRODEC_IOCGHWOFFSET      _IOR(HANTRODEC_IOC_MAGIC,  3, unsigned long *)
#define HANTRODEC_IOCGHWIOSIZE      _IOR(HANTRODEC_IOC_MAGIC,  4, struct regsize_desc *)

#define HANTRODEC_IOC_CLI           _IO(HANTRODEC_IOC_MAGIC,  5)
#define HANTRODEC_IOC_STI           _IO(HANTRODEC_IOC_MAGIC,  6)
#define HANTRODEC_IOC_MC_OFFSETS    _IOR(HANTRODEC_IOC_MAGIC, 7, unsigned long *)
#define HANTRODEC_IOC_MC_CORES      _IOR(HANTRODEC_IOC_MAGIC, 8, unsigned int *)


#define HANTRODEC_IOCS_DEC_PUSH_REG  _IOW(HANTRODEC_IOC_MAGIC, 9, struct core_desc *)
#define HANTRODEC_IOCS_PP_PUSH_REG   _IOW(HANTRODEC_IOC_MAGIC, 10, struct core_desc *)

#define HANTRODEC_IOCH_DEC_RESERVE   _IO(HANTRODEC_IOC_MAGIC, 11)
#define HANTRODEC_IOCT_DEC_RELEASE   _IO(HANTRODEC_IOC_MAGIC, 12)
#define HANTRODEC_IOCQ_PP_RESERVE    _IO(HANTRODEC_IOC_MAGIC, 13)
#define HANTRODEC_IOCT_PP_RELEASE    _IO(HANTRODEC_IOC_MAGIC, 14)

#define HANTRODEC_IOCX_DEC_WAIT      _IOWR(HANTRODEC_IOC_MAGIC, 15, struct core_desc *)
#define HANTRODEC_IOCX_PP_WAIT       _IOWR(HANTRODEC_IOC_MAGIC, 16, struct core_desc *)

#define HANTRODEC_IOCS_DEC_PULL_REG  _IOWR(HANTRODEC_IOC_MAGIC, 17, struct core_desc *)
#define HANTRODEC_IOCS_PP_PULL_REG   _IOWR(HANTRODEC_IOC_MAGIC, 18, struct core_desc *)

#define HANTRODEC_IOCG_CORE_WAIT     _IOR(HANTRODEC_IOC_MAGIC, 19, int *)

#define HANTRODEC_IOX_ASIC_ID        _IOWR(HANTRODEC_IOC_MAGIC, 20, struct core_param *)

#define HANTRODEC_IOCG_CORE_ID       _IOR(HANTRODEC_IOC_MAGIC, 21, unsigned long)

#define HANTRODEC_IOCS_DEC_WRITE_REG  _IOW(HANTRODEC_IOC_MAGIC, 22, struct core_desc *)

#define HANTRODEC_IOCS_DEC_READ_REG   _IOWR(HANTRODEC_IOC_MAGIC, 23, struct core_desc *)

#define HANTRODEC_IOX_ASIC_BUILD_ID   _IOWR(HANTRODEC_IOC_MAGIC, 24, __u32 *)

#define HANTRODEC_IOX_SUBSYS _IOWR(HANTRODEC_IOC_MAGIC, 25, struct subsys_desc *)

#define HANTRODEC_IOCX_POLL _IO(HANTRODEC_IOC_MAGIC, 26)

#define HANTRODEC_DEBUG_STATUS       _IO(HANTRODEC_IOC_MAGIC, 29)

#define HANTRODEC_IOCS_DEC_WRITE_APBFILTER_REG  _IOW(HANTRODEC_IOC_MAGIC, 30, struct core_desc *)

#define HANTRODEC_IOC_APBFILTER_CONFIG     _IOR(HANTRODEC_IOC_MAGIC,  31, struct apbfilter_cfg *)

#define HANTRODEC_IOC_AXIFE_CONFIG   _IOR(HANTRODEC_IOC_MAGIC,  32, struct axife_cfg *)

#define HANTRODEC_IOC_MAXNR 32

/* hantro vcmd */
/* Use 'v' as magic number for vcmd */
#define HANTRO_VCMD_IOC_MAGIC  'v'
/*
 * S means "Set" through a ptr,
 * T means "Tell" directly with the argument value
 * G means "Get": reply by setting through a pointer
 * Q means "Query": response is on the return value
 * X means "eXchange": G and S atomically
 * H means "sHift": T and Q atomically
 */
#define VASTAI_VCMD_IOCH_SET_DECODER                    _IOWR(HANTRO_VCMD_IOC_MAGIC, 19, u16)
#define HANTRO_VCMD_IOCH_GET_CMDBUF_PARAMETER           _IOWR(HANTRO_VCMD_IOC_MAGIC, 20,struct cmdbuf_mem_parameter *)
#define HANTRO_VCMD_IOCH_GET_CMDBUF_POOL_SIZE           _IOWR(HANTRO_VCMD_IOC_MAGIC, 21,unsigned long)
#define HANTRO_VCMD_IOCH_SET_CMDBUF_POOL_BASE           _IOWR(HANTRO_VCMD_IOC_MAGIC, 22,unsigned long)

#define HANTRO_VCMD_IOCH_GET_VCMD_PARAMETER             _IOWR(HANTRO_VCMD_IOC_MAGIC, 24, struct config_parameter *)

#define HANTRO_VCMD_IOCH_RESERVE_CMDBUF                 _IOWR(HANTRO_VCMD_IOC_MAGIC, 25,struct exchange_parameter *)
#define HANTRO_VCMD_IOCH_LINK_RUN_CMDBUF                _IOR(HANTRO_VCMD_IOC_MAGIC, 26, struct exchange_parameter *)
#ifdef VASTAI_DMA
#define HANTRO_VCMD_IOCH_WAIT_CMDBUF                    _IOWR(HANTRO_VCMD_IOC_MAGIC, 27, waitcmd_parameter_t *)
#define HANTRO_VCMD_IOCH_RELEASE_CMDBUF                 _IOWR(HANTRO_VCMD_IOC_MAGIC, 28, waitcmd_parameter_t *)
#else
#define HANTRO_VCMD_IOCH_WAIT_CMDBUF                    _IOR(HANTRO_VCMD_IOC_MAGIC, 27,u16 *)
#define HANTRO_VCMD_IOCH_RELEASE_CMDBUF                 _IOR(HANTRO_VCMD_IOC_MAGIC, 28,u16 *)
#endif

#define HANTRO_VCMD_IOCH_POLLING_CMDBUF                 _IOR(HANTRO_VCMD_IOC_MAGIC, 40,u16 *)
#define HANTRODEC_IOCX_GET_VCMDCFG						_IOR(HANTRO_VCMD_IOC_MAGIC, 41,u32 *)
#ifdef VASTAI_DMA
#define HANTRO_VCMD_IOCH_GET_FRAMEBUF                   _IOWR(HANTRO_VCMD_IOC_MAGIC, 42, framebuf_desc_t *)
#define HANTRO_VCMD_IOCH_GET_ID_PARAMETER               _IOWR(HANTRO_VCMD_IOC_MAGIC, 43, vcmd_id_parameter *)
#define HANTRO_VCMD_IOCH_READWRITE_DEVICE_MEM           _IOWR(HANTRO_VCMD_IOC_MAGIC, 44, transfer_mem_param *)
#define VASTAI_CONFIG_CORE_CHANNEL                      _IOWR(HANTRO_VCMD_IOC_MAGIC, 45, channel_info_t *)
#define VASTAI_DEC_GET_DIE_ID                           _IOWR(HANTRO_VCMD_IOC_MAGIC, 46, int *)
#define VASTAI_GET_DEC_WORK_MODE                        _IOWR(HANTRO_VCMD_IOC_MAGIC, 48, u32 *)
#endif
#define HANTRO_VCMD_IOC_MAXNR 50

/* hantro cache related */
#define HANTROCACHE_IOC_START               DRM_IO(HANTRO_IOCTL_START + 80)
#define CACHE_IOCGHWOFFSET      DRM_IOR(HANTRO_IOCTL_START + 80, unsigned long long *)
#define CACHE_IOCGHWIOSIZE      DRM_IO(HANTRO_IOCTL_START + 81)
#define CACHE_IOCHARDRESET      DRM_IO(HANTRO_IOCTL_START + 82)   /* debugging tool */
#define CACHE_IOCH_HW_RESERVE   DRM_IOW(HANTRO_IOCTL_START + 83, unsigned long long *)
#define CACHE_IOCH_HW_RELEASE   DRM_IO(HANTRO_IOCTL_START + 84)
#define CACHE_IOCG_CORE_NUM      DRM_IO(HANTRO_IOCTL_START + 85)
#define CACHE_IOCG_ABORT_WAIT     DRM_IO(HANTRO_IOCTL_START + 86)

#define HANTROCACHE_IOC_END               DRM_IO(HANTRO_IOCTL_START + 89)

#define HANTRODEC400_IOC_START             DRM_IO(HANTRO_IOCTL_START + 90)
#define DEC400_IOCGHWIOSIZE     			DRM_IO(HANTRO_IOCTL_START + 90)
#define DEC400_IOCS_DEC_WRITE_REG   	DRM_IOW(HANTRO_IOCTL_START + 91, struct core_desc *)
#define DEC400_IOCS_DEC_READ_REG    	DRM_IOWR(HANTRO_IOCTL_START + 92, struct core_desc *)
#define DEC400_IOCS_DEC_PUSH_REG    	DRM_IOW(HANTRO_IOCTL_START + 93, struct core_desc *)
#define DEC400_IOCGHWOFFSET			DRM_IOWR(HANTRO_IOCTL_START + 94, unsigned long long *)
#define HANTRODEC400_IOC_END               	DRM_IO(HANTRO_IOCTL_START + 99)

/*for l2cach|shaper*/
#define HANTRO_CACHE_IOCL_GET_IOCFG             DRM_IOWR(HANTRO_IOCTL_START + 75, struct regsize_desc *)

#define SURFACE_WITH_BUFFER     "vaapi surface buffer"
#define SURFACE_WITHOUT_BUFFER  "vaapi surface no buffer"
#define HANTRO_BUFMGR_BO_NO_BUFFER 		(1 << 0)

typedef size_t ptr_t;
typedef struct _drm_hantro_bo drm_hantro_bo;
typedef struct _drm_hantro_bufmgr drm_hantro_bufmgr;
typedef struct _drm_hantro_meta_node drm_hantro_meta_node;


#define VC8000E 0
#define DECODER_VC8000D_0 1
#define DECODER_VC8000D_1 2

typedef enum {
	/* VC8000E, */
	ENCODER_VC8000E,
	VC8000D_0,
	VC8000D_1,
	DECODER_G1_0,
	DECODER_G1_1,
	DECODER_G2_0,
	DECODER_G2_1,
} cache_client_type;

typedef enum {
	DIR_RD,
	DIR_WR,
	DIR_BI
} driver_cache_dir;

struct hantro_fencecheck {
	unsigned int handle;
	int ready;
};

struct hantro_domainset {
	unsigned int handle;
	unsigned int writedomain;
	unsigned int readdomain;
};

/** link list of meta data buffer */
struct _drm_hantro_meta_node {
	/** link list of next meta data buffer */
	drm_hantro_meta_node *next;
	/** the bo to save the meta data */
	drm_hantro_bo *meta_bo;
	/** slot number for meta data not occupied */
	int meta_free_slots;
	/** the export fd for the meta data bo */
	int meta_fd;
	/** the reference counter */
	int ref_cnt;
};

struct _drm_hantro_bufmgr {
	int fd;                    /*device file id opened*/
	int fd_mem;
	int fd_memalloc;
	int debug_enabled;
#ifdef VASTAI_DMA
	unsigned int die_id;
	int core_id;
	int channel_id;
	int vacc_init;
	uint32_t pid;

	void *runtimeHandle;
	vaccrt_init_t					vaccrt_init;
	vaccrt_malloc_video_t			vaccrt_malloc_video;
	vaccrt_free_video_t				vaccrt_free_video;
	vaccrt_get_current_pid_t		vaccrt_get_current_pid;
	vaccrt_get_process_status_t		vaccrt_get_process_status;
	vaccrt_get_video_reserver_ddr_t vaccrt_get_video_reserver_ddr;
#endif
	pthread_mutex_t lock;

	/* usd to record handles created by this bufmgr */
	void *handle_table;
	/**
	 * link list head of the meta data buffers, the head is the
	 * active node alway. the imported node will be put at tail.
	 */
	drm_hantro_meta_node *meta_node;
	/** lock when access the meta list */
	pthread_mutex_t meta_lock;
};

struct hantro_addrmap {
	unsigned int handle;
	unsigned long long vm_addr;
	unsigned long long phy_addr;
};

struct hantro_regtransfer {
	unsigned long coreid;
	unsigned long offset;
	unsigned long size;
	const void *data;
	int benc;                 /*encoder core or decoder core*/
	int direction;            /*0=read, 1=write*/
};

#define HANTRO_FENCE_WRITE  1
struct hantro_acquirebuf {
	unsigned long handle;
	unsigned long flags;
	unsigned long timeout;
	unsigned long fence_handle;
};

struct hantro_releasebuf {
	unsigned long fence_handle;
};

struct hantro_client {
	int clientid;
	int sliceid;
	unsigned long width;        /*buffer size*/
	unsigned long height;
	int profile;
	int codec;
};

struct _drm_hantro_bo {
	unsigned long size;        /*buffer size*/
	unsigned long align;
	int bimported;
#ifdef __cplusplus
	void *virt;
#else
	void *virtual;            /*logic addr*/
#endif
	ptr_t bus_addr;
	ptr_t translation_offset;

	drm_hantro_bufmgr *bufmgr;

	int handle;                /*buffer handle created by kernel */
	int fence_handle;

	int refcnt;
	int mmap_refcnt;

	int currentDomain;
	int domain_refcnt;
	int holddomain;

	pthread_mutex_t fencelock;
	/** the memta data location information get from the kernel. */
    struct dec_buf_info metainfo;
	/** the meta data buffer pointer */
	struct viv_vidmem_metadata * metadata;
	struct hantro_enc_dma_buf  enc_meta_output;
	int flag;
};

struct core_desc {
  __u32 id; /* id of the subsystem */
  __u32 type; /* type of core to be written */
  __u32 *regs; /* pointer to user registers */
  __u32 size; /* size of register space */
  __u32 reg_id; /* id of reigster to be read/written */
};

/* struct regsize_desc {
 *   __u32 slice; [> id of the slice <]
 *   __u32 id; [> id of the subsystem <]
 *   __u32 type; [> type of core to be written <]
 *   __u32 size; [> iosize of the core <]
 * }; */

struct core_param {
  __u32 slice; /* id of the slice */
  __u32 id; /* id of the subsystem */
  __u32 type; /* type of core to be written */
  __u32 size; /* iosize of the core */
  __u32 asic_id; /* asic id of the core */
};

/**
 * The memta data location information exchange IOCTRL parameters.
 */
struct hantro_metainfo_params {
	int handle;               /* the handle of current bo */
	struct dec_buf_info info; /* the meta data location information */
};

/************* some cache related defines   ************/
//#define PCI_BUS
/*Define Cache&Shaper Offset from common base*/
#define SHAPER_OFFSET                    (0x8<<2)
#define CACHE_ONLY_OFFSET                (0x8<<2)
#define CACHE_WITH_SHAPER_OFFSET         (0x80<<2)
/************* some cache related defines  end ************/

/* Buffer Manger API */
drm_hantro_bufmgr *drm_hantro_bufmgr_open(int fd);

int drm_hantro_bufmgr_destroy(drm_hantro_bufmgr *bufmgr);

drm_hantro_bo *drm_hantro_bo_gem_alloc(
	drm_hantro_bufmgr *bufmgr,
	const char *name, unsigned long size,
	unsigned int flag);

drm_hantro_bo *drm_hantro_bo_gem_create_from_name(
	drm_hantro_bufmgr *bufmgr,
	const char *name, unsigned int handle);

drm_hantro_bo *drm_hantro_bo_gem_create_from_prime(
	drm_hantro_bufmgr *bufmgr,
	int prime_fd,
	int size);

/* Buffer Object API */
int drm_hantro_bo_reference(drm_hantro_bo *bo);

int drm_hantro_bo_unreference(drm_hantro_bo *bo);

int drm_hantro_bo_map(drm_hantro_bo *buf, int write_enable);

int drm_hantro_bo_unmap(drm_hantro_bo *buf);

int drm_hantro_bo_enter_domain(drm_hantro_bo *bo, unsigned long domain, int bread);

int drm_hantro_bo_leave_domain(drm_hantro_bo *bo);

unsigned long drm_hantro_bo_fd2phys(drm_hantro_bo *bo, int fd);

unsigned long drm_hantro_bo_ptr2phys(drm_hantro_bo *bo, unsigned long vaddr);

unsigned int drm_hantro_get_hwcfg(int fd);

void drm_hantro_bufmgr_set_debug(drm_hantro_bufmgr *bufmgr, int debug_enabled);

int drm_hantro_bo_flink(drm_hantro_bo *bo, uint32_t *name);

int drm_hantro_bo_busy(drm_hantro_bo *bo);

int drm_hantro_bo_gem_export_to_prime(drm_hantro_bo *bo, int *prime_fd);

/* Encoder API */
int drm_hantro_enc_get_corenum(int fd, uint32_t sliceidx);
void *drm_hantro_enc_map_registers(
	drm_hantro_bufmgr *bufmgr,
	int core_id,
	unsigned long long *base,
	uint32_t *size);

int drm_hantro_enc_reserve_hw(drm_hantro_bufmgr *bufmgr, uint32_t *coreinfo);
int drm_hantro_enc_release_hw(drm_hantro_bufmgr *bufmgr, uint32_t *coreinfo);


/* Decoder API */
int drm_hantro_dec_get_corenum(int fd, uint32_t *cores);
int drm_hantro_dec_get_coreid(int fd,  uint32_t *client_type);
int drm_hantro_dec_push_hwreg(
	drm_hantro_bufmgr *bufmgr,
	unsigned int *reg_base,
	uint32_t core_id,
	uint32_t reg_size,
	uint32_t type);

#ifdef VASTAI_DMA
int drm_hantro_dec_get_framebuf(int fd, framebuf_desc_t *params);
int drm_hantro_dec_get_vcmd_idparams(int fd, vcmd_id_parameter *vcmd_params);
int drm_hantro_dec_readwrite_devicemem(int fd, transfer_mem_param *mem_params);
int drm_hantro_dec_config_core_channel(int fd, channel_info_t *channel_params);
int drm_hantro_dec_get_die_id(int fd, int *die_index);
int drm_hantro_dec_get_work_mode(int fd, int* work_mode);
#endif
int drm_hantro_dec_set_decoder(int fd);
int drm_hantro_dec_get_asicid(int fd,  uint32_t core_id, uint32_t type, int *hwid);
int drm_hantro_dec_get_core_hwbuildid(int fd,  uint32_t *build_id);
int drm_hantro_dec_get_multicore_base(int fd,  unsigned long *multicore_base);
int drm_hantro_dec_get_regsize(int fd, uint32_t core_id, uint32_t type);
int drm_hantro_dec_reserve_hw(drm_hantro_bufmgr *bufmgr, uint32_t client_type);
int drm_hantro_dec_release_hw(drm_hantro_bufmgr *bufmgr, uint32_t *core_id);
void drm_hantro_dec_write_reg(unsigned int *reg_base, uint32_t offset, uint32_t val);
uint32_t drm_hantro_dec_read_reg(unsigned int *reg_base, uint32_t offset);
int drm_hantro_dec_read_hwreg(
	drm_hantro_bufmgr *bufmgr,
	unsigned int *reg_base,
	uint32_t core_id,
	uint32_t reg_id,
	uint32_t reg_size,
	uint32_t type);
int drm_hantro_dec_pull_hwreg(
	drm_hantro_bufmgr *bufmgr,
	unsigned int *reg_base,
	uint32_t core_id,
	uint32_t reg_size,
	uint32_t type);
int drm_hantro_dec_wait_interrupt_and_pull_hwreg(
	drm_hantro_bufmgr *bufmgr,
	unsigned int *reg_base,
	uint32_t core_id,
	uint32_t reg_size,
	uint32_t type);
int drm_hantro_dec_wait_interrupt(drm_hantro_bufmgr *bufmgr, uint32_t *core_id);

int drm_hantro_cache_get_corenum(int fd, uint32_t sliceidx);
int drm_hantro_cache_get_multicore_base(int fd,  unsigned long long *multicore_base);
int drm_hantro_cache_get_regsize(int fd, uint32_t core_id);
int drm_hantro_cache_reserve_hw(drm_hantro_bufmgr *bufmgr, uint32_t client_type, uint32_t core_id);
int drm_hantro_cache_release_hw(drm_hantro_bufmgr *bufmgr, uint32_t core_id);
int drm_hantro_cache_abort_wait(drm_hantro_bufmgr *bufmgr, uint32_t core_id);

void *drm_hantro_dec400_map_registers(
	drm_hantro_bufmgr *bufmgr,
	int ssys_id,
	unsigned long long *base,
	uint32_t *size);
int drm_hantro_dec400_get_corebase(int fd, uint32_t ssys_id, unsigned long long *base);
int drm_hantro_dec400_get_iosize(int fd, uint32_t ssys_id, uint32_t *size);
int drm_hantro_dec400_writereg(
	int fd,
	uint32_t core_id,
	unsigned int *regs,
	unsigned int reg_id);
int drm_hantro_dec400_readreg(
	int fd,
	uint32_t core_id,
	unsigned int *regs,
	unsigned int reg_id);
int drm_hantro_dec400_pushreg(
	int fd,
	uint32_t core_id,
	unsigned int *regs);


int drm_hantro_get_slice_nums(int fd);
/** meta data operations */
int drm_hantro_bo_update_meta_data(drm_hantro_bo *bo, int meta_fd2,
                                   struct viv_vidmem_metadata *meta);
struct viv_vidmem_metadata *drm_hantro_bo_query_meta_data(drm_hantro_bo *bo);
int drm_hantro_bo_free_meta_data(drm_hantro_bo *bo, int meta_fd, int meta_offset);



int drm_hantro_add_client(
	drm_hantro_bufmgr *bufmgr,
	int clientid,
	int sliceid,
	uint32_t width,
	uint32_t height,
	uint32_t codec,
	uint32_t profile
	);

int drm_hantro_remove_client(
	drm_hantro_bufmgr *bufmgr,
	int clientid
	);

int drm_hantro_dec_poll_interrupt(drm_hantro_bufmgr *bufmgr,  uint32_t *core_id);

/*vcmd api*/
int drm_hantro_dec_get_vcmdcfg(int fd, uint32_t *vcmd_flag);
int drm_hantro_dec_get_vcmd_params(int fd, struct config_parameter *vcmd_param);
int	drm_hantro_dec_get_vcmdbuf_params(int fd, struct cmdbuf_mem_parameter *vcmd_mem_params);
int drm_hantro_dec_polling_vcmdbuf(int fd, u16 *cmdbuf_id);
int drm_hantro_dec_reserve_cmdbuf(int fd, struct exchange_parameter *exch_params);
int drm_hantro_dec_link_run_cmdbuf(int fd, struct exchange_parameter *exch_params);
#ifdef VASTAI_DMA
int drm_hantro_dec_wait_vcmdbuf(int fd, waitcmd_parameter_t *cmdbuf_id);
int drm_hantro_dec_release_vcmdbuf(int fd, waitcmd_parameter_t *vcmdbuf_id);
#else
int drm_hantro_dec_wait_vcmdbuf(int fd, u16 *cmdbuf_id);
int drm_hantro_dec_release_vcmdbuf(int fd, u16 *vcmdbuf_id);
#endif
/*l2cach|shaper api*/
int drm_hantro_dec_get_iocfg(int fd, struct regsize_desc *reg_decs);
int drm_hantro_dec_write_hwreg(int fd, struct core_desc *core_dev);


#if defined(__cplusplus)
}
#endif

#endif /* HANTRO_BUFMGR_H */
