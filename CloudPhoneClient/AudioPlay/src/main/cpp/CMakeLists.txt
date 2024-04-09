cmake_minimum_required(VERSION 3.4.1)
project(audioplay)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_VERBOSE_MAKEFILEON ON)

add_library(audioplay SHARED JNIOpusWrapper.cpp)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC ")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fstack-protector-strong --param ssp-buffer-size=4 -Wformat -D_FORTIFY_SOURCE=2 -O2 ")
message("CMAKE_C_FLAGS ========= " ${CMAKE_C_FLAGS})

if(${ENABLE_ASAN})
    message("${PROJECT_NAME} ENABLE ASAN")
    target_compile_options(${PROJECT_NAME} PUBLIC
        -Wall -fstack-protector-strong --param ssp-buffer-size=4 -Wformat -fsanitize=address -fsanitize-recover=address,all -O0
    )
    target_link_libraries(audioplay
        DemoUtils
        opus
        -Wl,-s -Wl,--build-id=none -Wl,-z,relro -fPIE -Wl,-z,now -Wl,-z,noexecstack -Wformat -fsanitize=address
    )
else()
    message("${PROJECT_NAME} ENABLE FORTIFY SOURCE")
    target_compile_options(${PROJECT_NAME} PUBLIC
        -Wall -fstack-protector-strong --param ssp-buffer-size=4 -Wformat -D_FORTIFY_SOURCE=2 -O2
    )
    target_link_libraries(audioplay
        DemoUtils
        opus
        -Wl,-s -Wl,--build-id=none -Wl,-z,relro -fPIE -Wl,-z,now -Wl,-z,noexecstack -Wformat
    )
endif()

