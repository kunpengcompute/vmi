#pragma once
#include <functional>
#include <ui/GraphicBuffer.h>

/* 
    PresentBufferCallback should be realized by user to connect with themselves stream solution
    when DisplayServer Mode is:
    1. Sync: GraphicBuffer could only be used during callback function registered in DisplayServer::Init()
    2. Async: GraphicBuffer could be used during ( DisplayServer::AcquireBuffer(), DisplayServer::ReleaseBuffer() )
    @input: android::GraphicBuffer is native struct in android, reference to $(ANDROID_ROOT)/frameworks/native/libs/ui/include/ui/GraphicBuffer.h
    @return: on success == 0, on error < 0
*/
#define PresentBufferCallback std::function<int(android::GraphicBuffer*)>

// functions to get GraphicBuffer property which are set by Vastai driver
uint64_t getPaddr(buffer_handle_t h);
uint32_t getWidth(buffer_handle_t h);
uint32_t getHeight(buffer_handle_t h);
int32_t getFormat(buffer_handle_t h);
uint32_t getStride(buffer_handle_t h);
uint32_t getStrideV(buffer_handle_t h); // vertical stride
uint64_t getBufferID(buffer_handle_t h);

struct RefreshReq {};

class DisplayServer {
    void* m_Impl;
public:
    enum Mode {Sync = 1, Async = 2};
    DisplayServer(Mode mode = Mode::Sync);
    ~DisplayServer();
    
    void Run();             // start the server, should be called after Init()
    bool Stop();            // stop the server, will be called automatically when DisplayServer is destoried

    /* During DisplayServer::Run(), callback will be called after a GraphicBuffer is rendered 
        @callback: User could use the rendered GraphicBuffer in callback, such as encode, dump and so on
        @maxInterval: The max interval between two callbacks in sync mode, even if screen does not change.
                      The unit is millisecond. Note that valid maxInterval should be greater than 1000/fps + 10. 
                      -1 means that callback function will not be called until screen change happen.
    */
    bool Init(PresentBufferCallback callback, int maxInterval = -1);		
    bool RefreshDisplay(RefreshReq);    		// refresh display, it will send invalidate signal, which causes a refresh at next vsync
    bool GetDisplayInfo(unsigned int *width, unsigned int *height, unsigned int *density, unsigned int *fps);

    // for async mode, not recommend to use
    void AcquireBuffer(android::GraphicBuffer *buffer, int* acquireFence = nullptr, int* releaseFence = nullptr);
    void ReleaseBuffer(android::GraphicBuffer *buffer);
    /* wait for first callback success returned in async mode, should be called after Run. -1 means infinite timeout
       @return: false when timeout is reached, true when callback successfully returned before timeout */
    bool WaitForFirstCallback(int timeout_ms = -1);  
};