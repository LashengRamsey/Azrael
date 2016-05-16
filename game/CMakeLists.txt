cmake_minimum_required(VERSION 2.8)
project(game CXX)

message(STATUS "start cmakes")

set(SRCS_SERVER
 main.cpp
 game.cpp
)

set(HEADERS_SERVER
 game.h
)

 set(CXX_FLAGS
 -g
 # -DVALGRINDz
 -DLOG2FILE_WITH_C
 -DCHECK_PTHREAD_RETURN_VALUE
 -D_FILE_OFFSET_BITS=64
 -Wno-write-strings
 #-Wall
 -Wextra
 -march=native
 #-lpthread
 # -MMD
 # -std=c++0x
 -levent
 -levent_extra
 -levent_core
 -rdynamic
 -Wmissing-field-initializers
 )


string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

set(CMAKE_CXX_COMPILER "g++")
#set(CMAKE_CXX_FLAGS_DEBUG "-O0")
#set(CMAKE_CXX_FLAGS_RELEASE "-O2 -finline-limit=1000 -DNDEBUG")

#头文件目录
include_directories(/usr/include)
include_directories(${PROJECT_SOURCE_DIR}/../libCore/include/comm)
include_directories(${PROJECT_SOURCE_DIR}/../libCore/include/event/include_linux/event2)
include_directories(${PROJECT_SOURCE_DIR}/../libCore/include/event/include_linux)
include_directories(${PROJECT_SOURCE_DIR}/../libCore/include/zmq/include)
include_directories(${PROJECT_SOURCE_DIR}/../libCore/include/lua/include)

#库目录
message(STATUS "start link_directories")
link_directories(/lib)
link_directories(/usr/lib)
link_directories(/usr/local/lib)

message(STATUS "PROJECT_SOURCE_DIR")
message(STATUS ${PROJECT_SOURCE_DIR}/../lib/linux)
link_directories(${PROJECT_SOURCE_DIR}/../lib/linux)

#执行
add_executable(game ${SRCS_SERVER})


#库
target_link_libraries(game -lpthread)
target_link_libraries(game dl)
target_link_libraries(game rt)

message(STATUS "target_link_libraries:event")
#target_link_libraries(game libevent.a)
#target_link_libraries(game libevent_extra.a)
#target_link_libraries(game libevent_core.a)
#target_link_libraries(game libevent_pthreads.a)

message(STATUS "target_link_libraries:libCore")
target_link_libraries(game Core.a)

#message(STATUS "target_link_libraries:zmq")
target_link_libraries(game zmq.a)

#message(STATUS "target_link_libraries:lua")
target_link_libraries(game lua.a)

#输出目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINRARY_DIR}/lib)

message(STATUS "end cmakes")

