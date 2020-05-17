cmake_minimum_required(VERSION 3.15)
project(silentloop)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wconversion -Wformat-overflow=2 -Wformat=2 -pedantic")

include_directories(src)

add_definitions(-D_GNU_SOURCE)

add_executable(silentloop
        main.cpp
        src/core/EventLoop.h
        src/core/EventLoop.cpp
        src/core/Event.h
        src/core/Event.cpp
        src/core/EventLoopBase.h
        src/core/EventLoopBase.cpp
        src/Utils.h
        src/core/Timer.h
        src/core/Timer.cpp
        src/core/io_mux/IOMuxBase.h
        src/core/io_mux/IOMux_epoll.h
        src/core/io_mux/IOMux_epoll.cpp
        src/core/io_mux/IOMux.h
        src/Log.h
        src/Log.cpp
        src/core/IOEvent.h
        src/core/IOEvent.cpp
        src/core/Error.h
        src/core/Error.cpp
        src/core/EventLoopFactory.h
        src/core/EventLoopFactory.cpp
        src/core/EventEmitter.h
        src/core/EventEmitter.cpp
        src/EventLoop.h
        src/core/Linkable.h
        src/core/Linkable.cpp
        src/modules/stream/Stream.h
        src/modules/stream/Stream.cpp
        src/modules/stream/Writable.h
        src/modules/stream/Writable.cpp
        src/modules/stream/Readable.h
        src/modules/stream/Readable.cpp
        src/modules/stream/Duplex.h
        src/modules/net/Socket.h
        src/modules/net/Socket.cpp
        src/modules/net/Server.h
        src/modules/net/Server.cpp
        src/modules/process/Process.h
        src/modules/process/Process.cpp
        src/modules/http/Server.h
        src/modules/http/Server.cpp
        src/modules/http/IncomingMessage.h
        src/modules/http/IncomingMessage.cpp
        src/modules/http/ServerResponse.h
        src/modules/http/ServerResponse.cpp
        src/modules/http/internal/http_parser.h
        src/modules/http/internal/http_parser.c
        src/modules/http/internal/HTTPRequestParser.h
        src/modules/http/internal/HTTPRequestParser.cpp)
