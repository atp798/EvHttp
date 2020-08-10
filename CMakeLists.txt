cmake_minimum_required(VERSION 3.1)

project(EvHttp CXX C)

# Build with -std=c++11
set(CMAKE_CXX_STANDARD  11)

find_package(Threads REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall")

set(THIRD_PARTY_PATH "${PROJECT_SOURCE_DIR}/third_party" CACHE STRING
    "A path setting third party libraries download directories.")

# ----- Build libevent -----
set(EVENT__DISABLE_BENCHMARK ON CACHE BOOL
    "Defines if Libevent should build without the benchmark executables, default: OFF")
set(EVENT__DISABLE_DEBUG_MODE ON CACHE BOOL
    "Define if Libevent should build without support for a debug mode, default: OFF")
set(EVENT__DISABLE_MM_REPLACEMENT OFF CACHE BOOL
    "Define if Libevent should not allow replacing the mm functions, default: OFF")
set(EVENT__DISABLE_OPENSSL OFF CACHE BOOL
    "Define if Libevent should build without support for OpenSSL encryption, default: OFF")
set(EVENT__DISABLE_REGRESS ON CACHE BOOL
    "Disable the regress tests, default: OFF")
set(EVENT__DISABLE_SAMPLES ON CACHE BOOL
    "Disable sample files, default: OFF")
set(EVENT__DISABLE_TESTS ON CACHE BOOL
    "If tests should be compiled or not, default: OFF")
set(EVENT__DISABLE_THREAD_SUPPORT OFF CACHE BOOL
    "Define if Libevent should not be compiled with thread support, default: OFF")
set(EVENT__ENABLE_VERBOSE_DEBUG OFF CACHE BOOL
    "Enables verbose debugging, default: OFF")
set(EVENT__DISABLE_MBEDTLS ON)

add_subdirectory(${THIRD_PARTY_PATH}/libevent)

# ----- Build EvHttp -----
include_directories(include)
include_directories(${LIBEVENT_INCLUDE_DIRS})

file(GLOB_RECURSE SRCS "${PROJECT_SOURCE_DIR}/include/*.h"
                       "${PROJECT_SOURCE_DIR}/src/*.cc")

add_executable(EvHttpExample1
        ${PROJECT_SOURCE_DIR}/example/example1.cc ${SRCS})
add_dependencies(EvHttpExample1 event_pthreads_static)
target_link_libraries(EvHttpExample1 event_static)

add_executable(EvHttpExample2
        ${PROJECT_SOURCE_DIR}/example/example2.cc ${SRCS})
add_dependencies(EvHttpExample2 event_pthreads_static)
target_link_libraries(EvHttpExample2 event_static)
