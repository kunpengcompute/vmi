cmake_minimum_required(VERSION 3.6)
project(InstructionEngineJni)
set(NORMAL_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14" )
set(CMAKE_SKIP_RPATH TRUE)
set(CMAKE_CXX_FLAGS "${NORMAL_CXX_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_FORTIFY_SOURCE=2 -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_FORTIFY_SOURCE=2 -O2")
set(DISTRIBUTION_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../jniLibs/armeabi-v7a)

add_library(InstructionEngineJni SHARED
    InstructionEngineJni.cpp)

add_library(InstructionEngineClient
            SHARED
            IMPORTED)
set_target_properties(InstructionEngineClient
                      PROPERTIES IMPORTED_LOCATION
                      ${DISTRIBUTION_DIR}/libInstructionEngineClient.so)
find_library(android-lib android)

target_compile_options(InstructionEngineJni PRIVATE
    -Wpedantic -pedantic-errors -Wextra -Werror
    -Wno-zero-length-array
    -Wno-gnu-zero-variadic-macro-arguments
    -Wno-unused-command-line-argument
    -Wall -Wformat
    -Wno-undefined-var-template
    -D_FORTIFY_SOURCE=2 -O2
    )

target_link_libraries(InstructionEngineJni
PRIVATE
    -static-libgcc
    -static-libstdc++
    -ldl
    android
    InstructionEngineClient
    log
    -Wl,--build-id=none -Wformat
    )