
cmake_minimum_required(VERSION 3.4.1)
project(audioplay)

add_compile_options(-DANDROID)
#use Fillp lib
add_compile_options(-Werror)

add_compile_options(-D__OPTIMIZE__) # for opus

set(CLIENT_BUILD ON)

#set(CMAKE_VERBOSE_MAKEFILE ON)
set(ANDROID_EABI arm64-v8a)

# modify repo dir depend on the dir of this CMakeLists.txt
set(REPO_REL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../)
set(ARMNATIVE_REL_DIR ${REPO_REL_DIR}/ArmNative)

# use armnative/ThirdParty include while use repo_dir/ThirdParty when compile src
set(THIRDPARTY_REL_DIR ${REPO_REL_DIR}/unpack_open_source)
set(CPP_REL_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp)

get_filename_component(REPO_ROOT_DIR ${REPO_REL_DIR} ABSOLUTE)
get_filename_component(THIRDPARTY_DIR ${THIRDPARTY_REL_DIR} ABSOLUTE)
get_filename_component(CPP_ROOT ${CPP_REL_ROOT} ABSOLUTE)

include_directories(${THIRDPARTY_DIR}/)
include_directories(${REPO_ROOT_DIR}/Common/Include)
include_directories(${REPO_ROOT_DIR}/Common/Utils)
include_directories(${REPO_ROOT_DIR}/Common/Log)
include_directories(${REPO_ROOT_DIR}/unpack_open_source/opus)
include_directories(${REPO_ROOT_DIR}/unpack_open_source/opus/codes/include)
include_directories(${CPP_ROOT}/)

include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
list(APPEND open_sources opus )

foreach(open_source IN LISTS open_sources)
link_directories(${CMAKE_BINARY_DIR}/${open_source})
endforeach(open_source IN LISTS open_sources)

link_directories(${CMAKE_BINARY_DIR}/Common/Utils)
link_directories(${VMI_BINARY_DIR}/arm64-v8a)

find_library(android-lib android)

add_subdirectory(${CPP_ROOT}/)
