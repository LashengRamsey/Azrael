set(libCore_SRCS
  app.cpp
  arch.cpp
  bevstream.cpp
  buf.cpp
  bufferstring.cpp
  Config.cpp
  connection.cpp
  log.cpp
  LogThread.cpp
  luabit.cpp
  luaglobal.cpp
  luanetwork.cpp
  luasvr.cpp
  lunar.cpp
  mqnet.cpp
  net.cpp
  timer.cpp
  )


add_library(libCore ${libCore_SRCS})

message(STATUS "target_link_libraries:pthread")
target_link_libraries(libCore pthread rt)

message(STATUS "target_link_libraries:event")
target_link_libraries(libCore event)
target_link_libraries(libCore event_core)
target_link_libraries(libCore event_extra)

target_link_libraries(libCore zmq)


set(HEADERS
  luabit.h
  luaglobal.h
  luanetwork.h
  )


