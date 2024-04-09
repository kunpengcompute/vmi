cmake_minimum_required(VERSION 3.4.1)
project(Communication)

set(NORMAL_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fPIC -fstack-protector-all -fexceptions")
set(CMAKE_CXX_FLAGS "${NORMAL_CXX_FLAGS}")

SET(REPO_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../)
include_directories(${CMAKE_CURRENT_LIST_DIR})
include_directories(Log)
include_directories(Communication)
include_directories(Include)
include_directories(Connection)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Libs/Client/include)

set(ConnectSrc
  Log/logging.cpp
  Log/LogInfo.cpp
  Connection/Connection.cpp
)

set(UtilsSources
  Log/logging.cpp
  Log/LogInfo.cpp
  Utils/EngineEventHandler.cpp
  Utils/Looper.cpp
  Utils/MurmurHash.cpp
  Utils/PacketQueue.cpp
  Utils/VersionCheck.cpp
  Utils/SystemProperty.cpp
)

math(EXPR BITS "${CMAKE_SIZEOF_VOID_P} * 8")
string(TOLOWER ${CMAKE_SYSTEM_PROCESSOR} CMAKE_SYSTEM_PROCESSOR_LC)
if(CMAKE_SYSTEM_PROCESSOR_LC MATCHES "x86_64" OR
  CMAKE_SYSTEM_PROCESSOR_LC MATCHES "amd64" OR
  CMAKE_SYSTEM_PROCESSOR_LC MATCHES "i[0-9]86" OR
  CMAKE_SYSTEM_PROCESSOR_LC MATCHES "x86" OR
  CMAKE_SYSTEM_PROCESSOR_LC MATCHES "ia32")
  if(BITS EQUAL 64)
    set(CPU_TYPE x86_64)
  else()
    set(CPU_TYPE i386)
  endif()
  if(NOT CMAKE_SYSTEM_PROCESSOR STREQUAL ${CPU_TYPE})
    set(CMAKE_SYSTEM_PROCESSOR ${CPU_TYPE})
  endif()
elseif(CMAKE_SYSTEM_PROCESSOR_LC STREQUAL "aarch64" OR
  CMAKE_SYSTEM_PROCESSOR_LC MATCHES "arm*64*")
  set(CPU_TYPE arm64)
elseif(CMAKE_SYSTEM_PROCESSOR_LC MATCHES "arm*")
  set(CPU_TYPE arm)
elseif(CMAKE_SYSTEM_PROCESSOR_LC MATCHES "ppc*" OR
  CMAKE_SYSTEM_PROCESSOR_LC MATCHES "powerpc*")
  set(CPU_TYPE powerpc)
else()
  set(CPU_TYPE ${CMAKE_SYSTEM_PROCESSOR_LC})
endif()
message(STATUS "${BITS}-bit build (${CPU_TYPE})")

if(ANDROID_ABI STREQUAL "arm64-v8a")
message(STATUS "build for android platform")
set(PLATFORM "android64")
elseif(CPU_TYPE STREQUAL "x86_64" OR CPU_TYPE STREQUAL "i386")
message(STATUS "build for x86 platform")
set(PLATFORM "x86")
else()
message(STATUS "build for arm platform")
set(PLATFORM "aarch64")
endif()

if(ANDROID_ABI STREQUAL "arm64-v8a")
add_library(Communication SHARED ${ConnectSrc})
target_link_libraries(Communication PUBLIC
    log
    -static-libgcc
    -static-libstdc++
    -ldl
)
add_library(DemoUtils SHARED ${UtilsSources})
target_link_libraries(DemoUtils PUBLIC
    log
    Communication
    -static-libgcc
    -static-libstdc++
    -ldl
)
else()
add_library(Communication SHARED ${ConnectSrc})
target_link_libraries(Communication PUBLIC
    -static-libgcc
    -static-libstdc++
    -ldl
    -Wl,-rpath=./,--build-id=none
)
add_library(DemoUtils SHARED ${UtilsSources})
target_link_libraries(DemoUtils PUBLIC
    Communication
    -static-libgcc
    -static-libstdc++
    -ldl
    -Wl,-rpath=./,--build-id=none
)
endif ()

target_include_directories(Communication PRIVATE ${COMMON_DIR})
