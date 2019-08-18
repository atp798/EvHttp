cmake_minimum_required(VERSION 2.8)
project(EvHttp)

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

#set(CMAKE_CXX_STANDARD 14) #This is only supported by cmake >3.1
#add_compile_options(-std=c++11)  #Use this together with above line


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall")

set(EVENT_INCLUDE "/usr/local/include/")
set(EVENT_LIB_DIR "/usr/local/lib/")
include_directories(.)
include_directories(Utilis)
include_directories(${EVENT_INCLUDE})

link_directories(${EVENT_LIB_DIR})

add_executable(EvHttp
        main.cpp
        Utilis/Defer.h
        Utilis/Exceptions.h
        Utilis/Logger.cpp
        Utilis/Logger.h
        Utilis/NonCopyable.h
        EvHttpResp.cpp
        EvHttpResp.h
        EvHttpServ.cpp
        EvHttpServ.h
        #EvHttpClient.cpp EvHttpClient.h Utilis/Initialize.h EvHttpBase.h EvHttpBase.cpp EvHttpConn.cpp EvHttpConn.h EvHttpReq.cpp EvHttpReq.h
        )

target_link_libraries(EvHttp -levent)


include_directories(${GTEST_INCLUDE_DIRS})

add_executable(AllTest
        test/MainTest.cpp
        test/EvHttpServTest.cpp
        test/EvHttpRequestTest.cpp
        )

target_link_libraries(AllTest ${GTEST_BOTH_LIBRARIES})
target_link_libraries(AllTest ${CMAKE_THREAD_LIBS_INIT})
add_test(Test AllTest)
enable_testing()