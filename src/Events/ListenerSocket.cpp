//
// Created by palulukan on 5/6/20.
//

#include "ListenerSocket.h"

#include "Log.h"

#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

void ListenerSocket::Detach(void)
{
    if(_isAttached())
        IOEvent::_detach();
}

// TODO: Propagate errors asynchronously?

bool ListenerSocket::_listen(EventLoopBase& eventLoop, int backlog)
{
    Detach();

    if(GetFD() < 0)
    {
        ERROR("Attempting to listen on closed socket");
        return false;
    }

    if(!(GetIOEventMask() & IOEvents::IOEV_READ))
    {
        if(listen(GetFD(), backlog) < 0)
        {
            SYSCALL_ERROR("listen()");
            return false;
        }

        SetIOEventFlag(IOEvents::IOEV_READ);
    }

    _attach(eventLoop);

    return true;
}

void ListenerSocket::OnRead(void)
{
    Event::OnRead();

    for(;;)
    {
        int fdClient = int(TEMP_FAILURE_RETRY(accept4(GetFD(), nullptr, nullptr, SOCK_NONBLOCK | SOCK_CLOEXEC)));
        if(fdClient >= 0)
            EMIT_EVENT(connection, fdClient);
        else if(IS_WOULDBLOCK(errno))
            break;
        else
        {
            const char *szError = strerror(errno);
            SYSCALL_ERROR("accept4()");
            EMIT_EVENT(error, std::string("accept4() failed: ") + szError);
            break;
        }
    }
}

void ListenerSocket::OnError(void)
{
    Event::OnError();

    EMIT_EVENT(error, "Socket error");
}

bool TCPListenerSocket::Open(EventLoopBase& eventLoop, const std::string& strHost, uint16_t nPort, bool bInet6, int backlog)
{
    Close();

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
        ERROR("getaddrinfo() failed: %s", gai_strerror(nGaiRes));
        return false;
    }

    int fdSocket = -1;
    for(struct addrinfo *rp = result; rp != nullptr; rp = rp->ai_next)
    {
        if(rp->ai_socktype != SOCK_STREAM || rp->ai_family != aiFamily || !(rp->ai_flags & AI_PASSIVE))
            continue;

        fdSocket = socket(rp->ai_family, rp->ai_socktype | SOCK_NONBLOCK | SOCK_CLOEXEC, rp->ai_protocol);
        if(fdSocket < 0)
        {
            SYSCALL_ERROR("socket()");
            continue;
        }

        if(bind(fdSocket, rp->ai_addr, rp->ai_addrlen) < 0)
            SYSCALL_ERROR("bind()");
        else
            break;

        close(fdSocket);
    }

    freeaddrinfo(result);

    if(fdSocket < 0)
    {
        ERROR("Could not bind to requested address");
        return false;
    }

    int nEnable = 1;
    if(setsockopt(fdSocket, SOL_SOCKET, SO_REUSEADDR, &nEnable, sizeof(nEnable)) < 0)
        SYSCALL_ERROR("setsockopt(SO_REUSEADDR)");

    SetFD(fdSocket);

    return _listen(eventLoop, backlog);
}

std::shared_ptr<TCPListenerSocket> TCPListenerSocket::Create(EventLoopBase &eventLoop, const std::string &strHost, uint16_t nPort, bool bInet6, int backlog)
{
    auto sp = Event::CreateEvent<TCPListenerSocket>();

    if(!sp->Open(eventLoop, strHost, nPort, bInet6, backlog))
        return nullptr;

    return sp;
}
