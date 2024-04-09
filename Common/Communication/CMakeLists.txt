cmake_minimum_required(VERSION 3.6)
project(CommonCommunication)

## options
set(CMAKE_VERBOSE_MAKEFILE ON)
set(TargetName "VmiCommonCommunication")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined" )
set(NORMAL_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fPIC -fstack-protector-all" )
set(CMAKE_SKIP_RPATH TRUE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS "${NORMAL_CXX_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")

include_directories(${LIBS_COMMON_DIR}/Include)
include_directories(${LIBS_COMMON_DIR}/Utils)
include_directories(${LIBS_COMMON_DIR}/Log)
include_directories(${LIBS_COMMON_DIR}/Communication)
include_directories(${LIBS_COMMON_DIR}/Connection)
include(env.cmake) # defines LIBS_COMMON_DIR
include(ExternalProject)

add_compile_options(-DVMICLIENT=1)

function(build_vmi_source arg)
    ExternalProject_Add(
        ${ARGV0}
        SOURCE_DIR ${REPO_ROOT_DIR}/${ARGV1}
        BINARY_DIR ${CMAKE_BINARY_DIR}/${ARGV1}
        CMAKE_COMMAND ${CMAKE_COMMAND}
        CMAKE_ARGC -DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM} -DANDROID_ABI=${ANDROID_ABI} -DANDROID_PLATFORM=android-23 -DANDROID_TOOLCHAIN=clang -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE} -DANDROID_ARM_NEON=TRUE -DANDROID_STL=c++_static  -DDECOMPRESS_DEBUG=1 -DVMI_OUTPUT_DIR=${VMI_OUTPUT_DIR}  -DREVERSE_OPCODE_IN_DECODER=1 -DSAVE_DECOMPRESSED_STREAM=0 -DSQLITE_SYNC_PERIOD=20 -DVMICLIENT=1
        BUILD_COMMAND ${CMAKE_MAKE_PROGRAM}
        INSTALL_COMMAND ""
      )
    link_directories(${CMAKE_BINARY_DIR}/${ARGV1})
endfunction()

function(build_vmi_source_linux arg)
    ExternalProject_Add(
        ${ARGV0}
        SOURCE_DIR     ${REPO_ROOT_DIR}/${ARGV1}
        BINARY_DIR ${CMAKE_BINARY_DIR}/${ARGV1}
        INSTALL_COMMAND ""
      )
    link_directories(${CMAKE_BINARY_DIR}/${ARGV1})
endfunction()
link_directories(${CMAKE_BINARY_DIR}/..)
link_directories(${VMI_OUTPUT_DIR}/${ANDROID_ABI})

set(Srcs
    Heartbeat/Heartbeat.cpp
    MsgFragment/MsgFragment.cpp
    MsgFragment/MsgReassemble.cpp
    NetComm/NetComm.cpp
    PacketHandle/PacketHandle.cpp
    PacketManager/PacketManager.cpp
    Socket/VmiSocket.cpp
    StreamParse/StreamParser.cpp
    Connection/ServerConnection.cpp
    Connection/ClientConnection.cpp
    Connection/ConnectionSocket.cpp
)

add_library(${TargetName} SHARED ${Srcs}
)
target_compile_options(${TargetName}  PRIVATE
    -Wall -fstack-protector --param ssp-buffer-size=4 -Wformat -D_FORTIFY_SOURCE=2 -O2
)
if(VMI_OUTPUT_DIR)
set_target_properties(${TargetName} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${VMI_OUTPUT_DIR}/${ANDROID_ABI}
    )
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
    target_link_libraries(${TargetName} PUBLIC
        DemoUtils
        Communication
        log
        -Wl,--build-id=none -Wl,-z,relro  -Wl,-z,now -Wformat
        -static-libgcc
        -static-libstdc++
        -ldl
    )
else()
    target_link_libraries(${TargetName} PUBLIC
        -Wl,--build-id=none -Wl,-s -Wl,-z,relro  -Wl,-z,now -Wformat
        -static-libgcc
        -static-libstdc++
        -ldl
        pthread
        DemoUtils
        Communication
    )
endif()

target_include_directories(${TargetName} PUBLIC
    ${LIBS_COMMON_DIR}/Include
    ${LIBS_COMMON_DIR}/Utils
    ${LIBS_COMMON_DIR}/Communication
    ${LIBS_COMMON_DIR}/Log
    ${LIBS_COMMON_DIR}/Connection
    )
set_target_properties(${TargetName} PROPERTIES POSITION_INDEPENDENT_CODE ON)


include(CMakePrintHelpers)
cmake_print_properties(
    TARGETS ${TargetName}
    PROPERTIES INCLUDE_DIRECTORIES
    )

