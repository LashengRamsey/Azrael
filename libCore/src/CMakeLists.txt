set(Core_SRCS
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


add_library(Core ${Core_SRCS})

message(STATUS "target_link_libraries:pthread")
target_link_libraries(Core pthread rt)

message(STATUS "target_link_libraries:event")
target_link_libraries(Core event)
target_link_libraries(Core event_core)
target_link_libraries(Core event_extra)

message(STATUS "target_link_libraries:zmq")
target_link_libraries(Core zmq)


set(HEADERS
  luabit.h
  luaglobal.h
  luanetwork.h
  )


