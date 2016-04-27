cmake_minimum_required(VERSION 2.8)

project(Core CXX)

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
 -DLOG2FILE_WHTH_C
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

#输出目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)


#include#头文件目录
message(STATUS ${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/include/comm)
include_directories(${PROJECT_SOURCE_DIR}/include/event/include_linux)
include_directories(${PROJECT_SOURCE_DIR}/include/event/include_linux/event2)
include_directories(${PROJECT_SOURCE_DIR}/include/zmq/include)
include_directories(${PROJECT_SOURCE_DIR}/include/lua/include)

include_directories(/usr/include)


#lib#库目录
message(STATUS "start link_directories")
link_directories(/usr/lib)
link_directories(/usr/local/lib)
link_directories(${PROJECT_SOURCE_DIR}/../lib/linux)

#string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
#message(STATUS "CXX_FLAGS = " ${CMAKE_CXX_FLAGS} " " ${CMAKE_CXX_FLAGS_${BUILD_TYPE}})

#子目录
#add_subdirectory(src)

set(Core_SRCS
  src/app.cpp
  src/arch.cpp
  src/bevstream.cpp
  src/buf.cpp
  src/bufferstring.cpp
  src/Config.cpp
  src/connection.cpp
  src/log.cpp
  src/LogThread.cpp
  src/luabit.cpp
  src/luaglobal.cpp
  src/luanetwork.cpp
  src/luasvr.cpp
  src/lunar.cpp
  src/mqnet.cpp
  src/net.cpp
  src/timer.cpp
  )


add_library(Core ${Core_SRCS})

#message(STATUS "target_link_libraries:pthread")
#target_link_libraries(Core pthread rt)

#message(STATUS "target_link_libraries:event")
#target_link_libraries(Core event)
#target_link_libraries(Core event_core)
#target_link_libraries(Core event_extra)

#message(STATUS "target_link_libraries:zmq")
#target_link_libraries(Core zmq)


#set(HEADERS
#  luabit.h
#  luaglobal.h
#  luanetwork.h
#  )


