/*
 * 版权所有 (c) 华为技术有限公司 2023-2023
 * 功能描述：ffmpeg编解码接口包裹层
 */

#ifndef AVCODEC_WRAPPER
#define AVCODEC_WRAPPER

#include <string>
#include <functional>
#include <dlfcn.h>
#include "logging.h"

namespace {
    /* 类型列表辅助类，用于根据函数ID自动获取函数类型 */
    template<typename... Elements>
    class TypeList {};

    /* 用于获取类型列表头的类型 */
    template<typename List>
    class FrontT;

    template<typename Head, typename... Tail>
    class FrontT<TypeList<Head, Tail...>>
    {
        public:
        using Type = Head;
    };

    template<typename List>
    using Front = typename FrontT<List>::Type;

    /* 用于弹出类型列表头 */
    template<typename List>
    class FrontPopT;

    template<typename Head, typename... Tail>
    class FrontPopT<TypeList<Head, Tail...>>
    {
        public:
        using Type = TypeList<Tail...>;
    };

    template<typename List>
    using FrontPop = typename FrontPopT<List>::Type;

    /* 用于获取类型列表中第N个类型 */
    template<typename List, unsigned N>
    class NthElementT : public NthElementT<FrontPop<List>, N - 1> {};

    template<typename List>
    class NthElementT<List, 0> : public FrontT<List> {};

    template <typename List, unsigned N>
    using NthElement = typename NthElementT<List, N>::Type;
}

namespace Vmi {
enum AvcodecFunction : uint32_t {
    AVCODEC_FIND_ENCODER_BY_NAME = 0,
    AVCODEC_OPEN2,
    AVCODEC_ALLOC_CONTEXT3,
    AVCODEC_FREE_CONTEXT,
    AVCODEC_SEND_FRAME,
    AV_PACKET_ALLOC,
    AVCODEC_RECEIVE_PACKET,
    AV_PACKET_UNREF,
    AV_PACKET_FREE,
    AV_HWFRAME_SET_ADDR_TO_SURFACEID,
    AV_STRERROR,
    AV_FRAME_FREE,
    AV_FRAME_ALLOC,
    AV_HWDEVICE_CTX_CREATE,
    AV_BUFFER_REF,
    AV_BUFFER_UNREF,
    AV_OPT_SET,
    AV_HWFRAME_CTX_ALLOC,
    AV_HWFRAME_CTX_INIT,
    AV_HWFRAME_GET_BUFFER,
    AV_FRAME_REMOVE_SIDE_DATA,
    AV_FRAME_NEW_SIDE_DATA,
    AV_FUNCS_NUM
};

class AvcodecWrapper {
public:
    using AVCODEC_FIND_ENCODER_BY_NAME_FUNC = AVCodec *(*)(const char *);
    using AVCODEC_OPEN2_FUNC = int(*)(AVCodecContext *, const AVCodec *, AVDictionary **);
    using AVCODEC_ALLOC_CONTEXT3_FUNC = AVCodecContext *(*)(const AVCodec *);
    using AVCODEC_FREE_CONTEXT_FUNC = void(*)(AVCodecContext **);
    using AVCODEC_SEND_FRAME_FUNC = int(*)(AVCodecContext *, const AVFrame *);
    using AV_PACKET_ALLOC_FUNC = AVPacket *(*)();
    using AVCODEC_RECEIVE_PACKET_FUNC = int(*)(AVCodecContext *, AVPacket *);
    using AV_PACKET_UNREF_FUNC = void(*)(AVPacket *);
    using AV_PACKET_FREE_FUNC = void(*)(AVPacket **);
    using AV_HWFRAME_SET_ADDR_TO_SURFACEID_FUNC = int(*)(const AVFrame *, uint64_t);
    using AV_STRERROR_FUNC = int(*)(int, char *, size_t);
    using AV_FRAME_FREE_FUNC = void(*)(AVFrame **);
    using AV_FRAME_ALLOC_FUNC = AVFrame *(*)();
    using AV_HWDEVICE_CTX_CREATE_FUNC = int(*)(AVBufferRef **, enum AVHWDeviceType, const char *, AVDictionary *, int);
    using AV_BUFFER_REF_FUNC = AVBufferRef *(*)(AVBufferRef *);
    using AV_BUFFER_UNREF_FUNC = void(*)(AVBufferRef **);
    using AV_OPT_SET_FUNC = int(*)(void *, const char *, const char *, int);
    using AV_HWFRAME_CTX_ALLOC_FUNC = AVBufferRef *(*)(AVBufferRef *);
    using AV_HWFRAME_CTX_INIT_FUNC = int(*)(AVBufferRef *);
    using AV_HWFRAME_GET_BUFFER_FUNC = int(*)(AVBufferRef *, AVFrame *, int);
    using AV_FRAME_REMOVE_SIDE_DATA_FUNC = AVFrameSideData*(*)(AVFrame *, enum AVFrameSideDataType);
    using AV_FRAME_NEW_SIDE_DATA_FUNC = AVFrameSideData*(*)(AVFrame *, enum AVFrameSideDataType, int);

    using FUNC_LIST = TypeList<
        AVCODEC_FIND_ENCODER_BY_NAME_FUNC,
        AVCODEC_OPEN2_FUNC,
        AVCODEC_ALLOC_CONTEXT3_FUNC,
        AVCODEC_FREE_CONTEXT_FUNC,
        AVCODEC_SEND_FRAME_FUNC,
        AV_PACKET_ALLOC_FUNC,
        AVCODEC_RECEIVE_PACKET_FUNC,
        AV_PACKET_UNREF_FUNC,
        AV_PACKET_FREE_FUNC,
        AV_HWFRAME_SET_ADDR_TO_SURFACEID_FUNC,
        AV_STRERROR_FUNC,
        AV_FRAME_FREE_FUNC,
        AV_FRAME_ALLOC_FUNC,
        AV_HWDEVICE_CTX_CREATE_FUNC,
        AV_BUFFER_REF_FUNC,
        AV_BUFFER_UNREF_FUNC,
        AV_OPT_SET_FUNC,
        AV_HWFRAME_CTX_ALLOC_FUNC,
        AV_HWFRAME_CTX_INIT_FUNC,
        AV_HWFRAME_GET_BUFFER_FUNC,
        AV_FRAME_REMOVE_SIDE_DATA_FUNC,
        AV_FRAME_NEW_SIDE_DATA_FUNC
    >;

    /* 根据传入的函数ID以及参数直接调用对应的函数 */
    template <AvcodecFunction funcCode, typename ...T>
    auto Call(T&&... args)
    {
        auto func = reinterpret_cast<NthElement<FUNC_LIST, funcCode>>(m_funcsMap[funcCode].ptr);
        return func(std::forward<T>(args)...);
    }

    bool Init()
    {
        for (uint32_t i = 0; i < AV_FUNCS_NUM; ++i) {
            auto libCode = m_funcsMap[i].libCode;
            if (m_libsMap[libCode].ptr == nullptr) {
                m_libsMap[libCode].ptr = dlopen(m_libsMap[libCode].name, RTLD_NOW);
            }
            if (m_libsMap[libCode].ptr == nullptr) {
                ERR("Fail to open lib %s, err: %s.", m_libsMap[libCode].name, dlerror());
                return false;
            }

            m_funcsMap[i].ptr = dlsym(m_libsMap[libCode].ptr, m_funcsMap[i].name);
            if (m_funcsMap[i].ptr == nullptr) {
                ERR("Fail to load func %s lib %s, err: %s.", m_funcsMap[i].name, m_libsMap[libCode].name, dlerror());
                return false;
            }
        }
        return true;
    }

    static AvcodecWrapper &Get()
    {
        static AvcodecWrapper instance;
        return instance;
    }

private:
    enum AvcodecLib : uint32_t {
        LIB_AVCODEC,
        LIB_AVUTIL,
        LIBS_NUM,
    };

    struct LibInfo {
        const char *name;
        void *ptr;
    };

    LibInfo m_libsMap[LIBS_NUM] = {
        {"/system/vendor/lib64/libavcodec.so", nullptr},
        {"/system/vendor/lib64/libavutil.so", nullptr},
    };

    struct FunctionInfo {
        AvcodecLib libCode;
        const char *name;
        void *ptr;
    };

    FunctionInfo m_funcsMap[AV_FUNCS_NUM] = {
        {LIB_AVCODEC, "avcodec_find_encoder_by_name", nullptr},
        {LIB_AVCODEC, "avcodec_open2", nullptr},
        {LIB_AVCODEC, "avcodec_alloc_context3", nullptr},
        {LIB_AVCODEC, "avcodec_free_context", nullptr},
        {LIB_AVCODEC, "avcodec_send_frame", nullptr},
        {LIB_AVCODEC, "av_packet_alloc", nullptr},
        {LIB_AVCODEC, "avcodec_receive_packet", nullptr},
        {LIB_AVCODEC, "av_packet_unref", nullptr},
        {LIB_AVCODEC, "av_packet_free", nullptr},
        {LIB_AVUTIL, "av_hwframe_set_addr_to_surfaceid", nullptr},
        {LIB_AVUTIL, "av_strerror", nullptr},
        {LIB_AVUTIL, "av_frame_free", nullptr},
        {LIB_AVUTIL, "av_frame_alloc", nullptr},
        {LIB_AVUTIL, "av_hwdevice_ctx_create", nullptr},
        {LIB_AVUTIL, "av_buffer_ref", nullptr},
        {LIB_AVUTIL, "av_buffer_unref", nullptr},
        {LIB_AVUTIL, "av_opt_set", nullptr},
        {LIB_AVUTIL, "av_hwframe_ctx_alloc", nullptr},
        {LIB_AVUTIL, "av_hwframe_ctx_init", nullptr},
        {LIB_AVUTIL, "av_hwframe_get_buffer", nullptr},
        {LIB_AVUTIL, "av_frame_remove_side_data", nullptr},
        {LIB_AVUTIL, "av_frame_new_side_data", nullptr},
    };
};
}
#endif