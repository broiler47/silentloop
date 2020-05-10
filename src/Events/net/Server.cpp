//
// Created by palulukan on 5/10/20.
//

#include "Server.h"

#include "Log.h"

#include <cassert>

#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

std::shared_ptr<Server> Server::Create(EventLoopBase &eventLoop, const std::string &strHost, uint16_t nPort, bool bInet6, int backlog)
{
    auto sp = Event::CreateEvent<Server>();
    sp->_open(eventLoop, strHost, nPort, bInet6, backlog);
    return sp;
}

void Server::_open(EventLoopBase &eventLoop, const std::string &strHost, uint16_t nPort, bool bInet6, int backlog)
{
    _attach(eventLoop);

    const auto aiFamily = bInet6 ? AF_INET6 : AF_INET;

    struct addrinfo hints = {
        .ai_flags = AI_PASSIVE,
        .ai_family = aiFamily,
        .ai_socktype = SOCK_STREAM,
        .ai_protocol = 0,
        .ai_addrlen = 0,
        .ai_addr = nullptr,
        .ai_canonname = nullptr,
        .ai_next = nullptr
    };

    struct addrinfo *result = nullptr;
    int nGaiRes = getaddrinfo(strHost.c_str(), std::to_string(nPort).c_str(), &hints, &result);
    if(nGaiRes != 0)
    {
        EMIT_EVENT_ASYNC(error, Error(std::string("getaddrinfo() failed: ") + gai_strerror(nGaiRes)));
        return Close();
    }

    int nLsatError = 0;
    int fdSocket = -1;
    for(struct addrinfo *rp = result; rp != nullptr; rp = rp->ai_next)
    {
        if(rp->ai_socktype != SOCK_STREAM || rp->ai_family != aiFamily || !(rp->ai_flags & AI_PASSIVE))
            continue;

        fdSocket = socket(rp->ai_family, rp->ai_socktype | SOCK_NONBLOCK | SOCK_CLOEXEC, rp->ai_protocol);
        if(fdSocket < 0)
        {
            nLsatError = errno;
            continue;
        }

        if(bind(fdSocket, rp->ai_addr, rp->ai_addrlen) < 0)
            nLsatError = errno;
        else
            break;

        close(fdSocket);
        fdSocket = -1;
    }

    freeaddrinfo(result);

    if(fdSocket < 0)
    {
        EMIT_EVENT_ASYNC(error, SystemError("Could not bind to requested address", nLsatError));
        return Close();
    }

    int nEnable = 1;
    if(setsockopt(fdSocket, SOL_SOCKET, SO_REUSEADDR, &nEnable, sizeof(nEnable)) < 0)
    {
        // This is not a critical error so no need to notify anybody
        SYSCALL_ERROR("setsockopt(SO_REUSEADDR)");
    }

    if(!_listen(fdSocket, backlog))
    {
        close(fdSocket);
        return Close();
    }
}

bool Server::_listen(int fd, int backlog)
{
    assert(fd >= 0);

    if(listen(fd, backlog) < 0)
    {
        EMIT_EVENT_ASYNC(error, SystemError("Could not start listening socket", errno));
        return false;
    }

    SetFD(fd, IOEvents::IOEV_READ);

    EMIT_EVENT_ASYNC0(listening);

    return true;
}

void Server::_newClient(int fd)
{
    auto spSocket = Socket::Create(_eventLoop(), fd, false, true);

    EMIT_EVENT(connection, spSocket);
}

void Server::OnRead(void)
{
    Event::OnRead();

    for(;;)
    {
        int fdClient = int(TEMP_FAILURE_RETRY(accept4(GetFD(), nullptr, nullptr, SOCK_NONBLOCK | SOCK_CLOEXEC)));
        if(fdClient >= 0)
            _newClient(fdClient);
        else if(IS_WOULDBLOCK(errno))
            break;
        else
        {
            EMIT_EVENT(error, SystemError("accept4() failed", errno));
            break;
        }
    }
}

void Server::OnError(void)
{
    Event::OnError();

    EMIT_EVENT(error, Error("Socket error"));
    Close();
}
