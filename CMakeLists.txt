cmake_minimum_required(VERSION 3.15)
project(silentloop)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wconversion -Wformat-overflow=2 -Wformat=2 -pedantic")

include_directories(src)

add_definitions(-D_GNU_SOURCE)

add_executable(silentloop
        main.cpp
        src/EventLoop.h
        src/EventLoop.cpp
        src/Events/core/Event.h
        src/Events/core/Event.cpp
        src/EventLoopBase.h
        src/EventLoopBase.cpp
        src/Utils.h
        src/Events/Tineout.h
        src/Events/Tineout.cpp
        src/IOMux/IOMuxBase.h
        src/IOMux/IOMux_epoll.h
        src/IOMux/IOMux_epoll.cpp
        src/IOMux/IOMux.h
        src/Log.h
        src/Log.cpp
        src/Events/core/IOEvent.h
        src/Events/core/IOEvent.cpp
        src/Events/core/Error.h
        src/Events/core/Error.cpp
        src/Events/stream/Stream.h
        src/Events/stream/Stream.cpp
        src/Events/net/Socket.h
        src/Events/net/Socket.cpp
        src/Events/net/Server.h
        src/Events/net/Server.cpp)
