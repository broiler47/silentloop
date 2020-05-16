//
// Created by palulukan on 5/10/20.
//

#include "Server.h"

#include "Log.h"

#include <cassert>

#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

net::Server::Server(bool allowHalfOpen, bool pauseOnConnect) :
    m_bAllowHalfOpen(allowHalfOpen),
    m_bPauseOnConnect(pauseOnConnect)
{
}

void net::Server::Listen(uint16_t nPort, const std::string &strHost, int backlog, bool bInet6)
{
    auto spListenerEvent = m_wpListenerEvent.lock();
    if(!spListenerEvent)
    {
        spListenerEvent = std::make_shared<IOEvent>();
        LinkWith(spListenerEvent);
        m_wpListenerEvent = spListenerEvent;

        spListenerEvent->on_error([this](const Error& err) {
            EMIT_EVENT(error, err);
            Close();
        });

        spListenerEvent->on_close([this](void) {
            EMIT_EVENT(close);
        });

        spListenerEvent->on_read([this](void) {
            auto spListenerEvent = m_wpListenerEvent.lock();

            assert(spListenerEvent);
            assert(spListenerEvent->GetFD() >= 0);

            for(;;)
            {
                int fdClient = int(TEMP_FAILURE_RETRY(accept4(spListenerEvent->GetFD(), nullptr, nullptr, SOCK_NONBLOCK | SOCK_CLOEXEC)));
                if(fdClient >= 0)
                {
                    auto spSocket = Socket::Create(fdClient, m_bAllowHalfOpen, !m_bPauseOnConnect);
                    EMIT_EVENT(connection, spSocket);
                }
                else if(IS_WOULDBLOCK(errno))
                    break;
                else
                {
                    EMIT_EVENT(error, SystemError("accept4() failed", errno));
                    break;
                }
            }
        });
    }

    if(spListenerEvent->GetFD() >= 0)
    {
        EMIT_EVENT_ASYNC(error, Error("Already listening", ERR_SERVER_ALREADY_LISTEN));
        return;
    }

    _openTCPSocket(spListenerEvent, nPort, strHost, backlog, bInet6);
}

void net::Server::Close(void)
{
    auto spListenerEvent = m_wpListenerEvent.lock();
    if(spListenerEvent)
        spListenerEvent->Close();
}

void net::Server::_openTCPSocket(const std::shared_ptr<IOEvent>& spListenerEvent, uint16_t nPort, const std::string& strHost, int backlog, bool bInet6)
{
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
        return;
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
        return;
    }

    int nEnable = 1;
    if(setsockopt(fdSocket, SOL_SOCKET, SO_REUSEADDR, &nEnable, sizeof(nEnable)) < 0)
    {
        // This is not a critical error so no need to notify anybody
        SYSCALL_ERROR("setsockopt(SO_REUSEADDR)");
    }

    if(!_listen(spListenerEvent, fdSocket, backlog))
    {
        close(fdSocket);
        return;
    }
}

bool net::Server::_listen(const std::shared_ptr<IOEvent>& spListenerEvent, int fd, int backlog)
{
    assert(fd >= 0);

    if(listen(fd, backlog) < 0)
    {
        EMIT_EVENT_ASYNC(error, SystemError("Could not start listening socket", errno));
        return false;
    }

    spListenerEvent->SetFD(fd, IOEvent::IOEvents::IOEV_READ);
    spListenerEvent->Attach();

    EMIT_EVENT_ASYNC0(listening);

    return true;
}

std::shared_ptr<net::Server> net::CreateServer(bool allowHalfOpen, bool pauseOnConnect)
{
    return std::make_shared<Server>(allowHalfOpen, pauseOnConnect);
}
