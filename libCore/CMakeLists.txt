cmake_minimum_required(VERSION 2.8)

project(libCore CXX)

#if(NOT CMAKE_BUILD_TYPE)
#  set(CMAKE_BUILD_TYPE "Debug")
#endif()
 
set(CXX_FLAGS
 -g
 # -DVALGRINDz
 -DLOG2FILE_WITH_C
 -DHAVE_CLOCK_GETTIME
 -DCHECK_PTHREAD_RETURN_VALUE
 -D_FILE_OFFSET_BITS=64
 -Wno-write-strings
 #-Wall
 -Wextra
 -march=native
 #-lpthread
 # -MMD
 # -std=c++0x
 -rdynamic
 -Wmissing-field-initializers
 )
if(CMAKE_BUILD_BITS EQUAL 32)
  list(APPEND CXX_FLAGS "-m32")
endif()
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -finline-limit=1000 -DNDEBUG")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)



#include
message(STATUS ${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/include/comm)
include_directories(${PROJECT_SOURCE_DIR}/include/event/include)
include_directories(${PROJECT_SOURCE_DIR}/include/zmq/include)
include_directories(${PROJECT_SOURCE_DIR}/include/lua/include)
include_directories(${PROJECT_SOURCE_DIR}/../Lib/linux_event/include)
include_directories(/usr/include)


#lib
message(STATUS "start link_directories")
link_directories(/usr/lib)
link_directories(/usr/local/lib)
link_directories(${PROJECT_SOURCE_DIR}/../lib/linux/.libs)

#string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
#message(STATUS "CXX_FLAGS = " ${CMAKE_CXX_FLAGS} " " ${CMAKE_CXX_FLAGS_${BUILD_TYPE}})

add_subdirectory(src)


