cmake_minimum_required(VERSION 3.6)
project(VideoEngineJni)
set(NORMAL_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14" )
set(CMAKE_SKIP_RPATH TRUE)
set(CMAKE_CXX_FLAGS "${NORMAL_CXX_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_FORTIFY_SOURCE=2 -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_FORTIFY_SOURCE=2 -O2")
set(DISTRIBUTION_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../jniLibs/arm64-v8a)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../../Common/Connection)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../../Common/Include)

add_library(VideoEngineJni SHARED
    VideoEngineJni.cpp)

add_library(VideoEngineClient
            SHARED
            IMPORTED)
set_target_properties(VideoEngineClient
                      PROPERTIES IMPORTED_LOCATION
                      ${DISTRIBUTION_DIR}/libVideoEngineClient.so)

add_library(Communication
            SHARED
            IMPORTED)
set_target_properties(Communication
                      PROPERTIES IMPORTED_LOCATION
                      ${DISTRIBUTION_DIR}/libCommunication.so)
find_library(android-lib android)

target_compile_options(VideoEngineJni PRIVATE
    #-Wpedantic -pedantic-errors -Wextra -Werror
    -Wpedantic -pedantic-errors -Wextra
    -Wno-zero-length-array
    -Wno-gnu-zero-variadic-macro-arguments
    -Wno-unused-command-line-argument
    -Wall -Wformat
    -Wno-undefined-var-template
    -D_FORTIFY_SOURCE=2 -O2
    )

target_link_libraries(VideoEngineJni
PRIVATE
    -static-libgcc
    -static-libstdc++
    -ldl
    android
    VideoEngineClient
    log
    Communication
    -Wl,--build-id=none -Wformat
    )
