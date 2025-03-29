#ifndef DISPLAY_SERVER_WRAP_H
#define DISPLAY_SERVER_WRAP_H

#include <stdint.h>

extern "C" {

    typedef void* pVaGraphicBuffer;
    typedef const void* va_buffer_handle_wrap;

    /* 
        PresentBufferCallback should be realized by user to connect with themselves stream solution
        when DisplayServer Mode is:
        1. Sync: GraphicBuffer could only be used during callback function registered in DisplayServer::Init()
        2. Async: GraphicBuffer could be used during ( DisplayServer::AcquireBuffer(), DisplayServer::ReleaseBuffer() )
        @input: when VaGraphicBuffer is not defined, input is android::GraphicBuffer, which is a native struct in android, 
                    reference to $(ANDROID_ROOT)/frameworks/native/libs/ui/include/ui/GraphicBuffer.h. 
                when VaGraphicBuffer is defined, input is VaGraphicBuffer, 
                    and following functions could be used to get properties which are set by Va driver.
        @return: on success == 0, on error < 0
    */
    typedef int(*PresentBufferCallbackWrap)(pVaGraphicBuffer);

    // functions to get GraphicBuffer property which are set by Va driver
    va_buffer_handle_wrap getHandleWrap(pVaGraphicBuffer buffer);
    uint64_t getPaddrWrap(va_buffer_handle_wrap h);
    uint32_t getWidthWrap(va_buffer_handle_wrap h);
    uint32_t getHeightWrap(va_buffer_handle_wrap h);
    int32_t getFormatWrap(va_buffer_handle_wrap h);
    uint32_t getStrideWrap(va_buffer_handle_wrap h);
    uint32_t getStrideVWrap(va_buffer_handle_wrap h); // vertical stride
    uint64_t getBufferIDWrap(va_buffer_handle_wrap h);
    void* getAndroidGraphicBufferWrap(pVaGraphicBuffer buffer);

    // functions for DisplayServer 
    struct RefreshReqWrapper {unsigned int Type;};  // Currently, field Type is not used 
    typedef void* pDisplayServer;

    enum DisplayServerMode {Sync = 1, Async = 2};

    pDisplayServer CreateDisplayServer(DisplayServerMode mode);
    void DestroyDisplayServer(pDisplayServer w);
    void DisplayServerRun(pDisplayServer w);    // start the server, should be called after Init()
    bool DisplayServerStop(pDisplayServer w);   // stop the server, will be called automatically when DisplayServer is destoried

    /* During DisplayServer::Run(), callback will be called after a GraphicBuffer is rendered 
        @callback: User could use the rendered GraphicBuffer in callback, such as encode, dump and so on
        @maxInterval: The max interval between two callbacks in sync mode, even if screen does not change.
                        The unit is millisecond. Note that valid maxInterval should be greater than 1000/fps + 10. 
                        -1 means that callback function will not be called until screen change happen.
    */
    bool DisplayServerInit(pDisplayServer w, PresentBufferCallbackWrap callback, int maxInterval);

    bool DisplayServerSetWmSize(pDisplayServer w, unsigned int wmWidth, unsigned int wmHeight);	
    bool DisplayServerRefreshDisplay(pDisplayServer w, RefreshReqWrapper req);    		// refresh display, it will send invalidate signal, which causes a refresh at next vsync
    bool DisplayServerGetDisplayInfo(pDisplayServer w, unsigned int *width, unsigned int *height, unsigned int *density, unsigned int *fps);

    // for async mode, not recommend to use
    void DisplayServerAcquireBuffer(pDisplayServer w, pVaGraphicBuffer buffer, int* acquireFence = nullptr, int* releaseFence = nullptr);
    void DisplayServerReleaseBuffer(pDisplayServer w, pVaGraphicBuffer buffer);
    /* wait for first callback success returned in async mode, should be called after Run. -1 means infinite timeout
        @return: false when timeout is reached, true when callback successfully returned before timeout */
    bool DisplayServerWaitForFirstCallback(pDisplayServer w, int timeout_ms);  

    // fence 
    int DisplayServerCreateTimeline(pDisplayServer w, const char* name);
    int DisplayServerCloseTimeline(pDisplayServer w, int timeline); 
    int DisplayServerCreateFence(pDisplayServer w, int timeline, const char* name);
    int DisplayServerCloseFence(pDisplayServer w, int fence);
    bool DisplayServerTimelineInc(pDisplayServer w, int timeline);
    void DisplayServerDumpFenceInfo(pDisplayServer w, int fence, const char* tag);
    bool DisplayServerWaitFence(pDisplayServer w, int fence, int timeout);

}

#endif  // DISPLAY_SERVER_WRAP_H
