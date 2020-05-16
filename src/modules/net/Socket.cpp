//
// Created by palulukan on 5/8/20.
//

#include "Socket.h"

#include <cassert>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

std::shared_ptr<net::Socket> net::Socket::Create(int fd, bool allowHalfOpen, bool startReading)
{
    auto sp = std::make_shared<Socket>();
    sp->_init(fd, allowHalfOpen, startReading);
    return sp;
}

void net::Socket::setNoDelay(bool noDelay)
{
    auto spSeocketEvent = m_wpSocketEvent.lock();

    if(!spSeocketEvent || spSeocketEvent->GetFD() < 0)
    {
        EMIT_EVENT_ASYNC(error, TypeError("Invalid file descriptor"));
        return;
    }

    int nEnable = noDelay;
    if(setsockopt(spSeocketEvent->GetFD(), IPPROTO_TCP, TCP_NODELAY, &nEnable, sizeof(nEnable)) < 0)
        EMIT_EVENT(error, SystemError("setsockopt(TCP_NODELAY)", errno));
}

void net::Socket::_init(int fd, bool allowHalfOpen, bool startReading)
{
    if(fd < 0)
    {
        EMIT_EVENT_ASYNC(error, TypeError("Invalid file descriptor"));
        return;
    }

    m_bAllowHalfOpen = allowHalfOpen;

    assert(!m_wpSocketEvent.lock());

    auto spSocketEvent = std::make_shared<IOEvent>();
    LinkWith(spSocketEvent);
    m_wpSocketEvent = spSocketEvent;

    spSocketEvent->on_error([this](const Error &err) {
        destroy(err);
    });

    spSocketEvent->on_close([this](void) {
        EMIT_EVENT(close);
    });

    spSocketEvent->on_read([this](void) {
        _doRead();
    });

    spSocketEvent->on_write([this](void) {
        _doWrite();
    });

    spSocketEvent->on_HUP([this](void) {
        _doRead();
    });

    spSocketEvent->SetFD(fd, m_bFlowing ? (unsigned int)IOEvent::IOEvents::IOEV_READ : 0);
    spSocketEvent->Attach();

    if(startReading)
        resume();
}

void net::Socket::_destroy(const Error *pErr, const std::function<void(const Error &err)> &cb)
{
    UNUSED_ARG(pErr);
    UNUSED_ARG(cb);

    auto spSocketEvent = m_wpSocketEvent.lock();
    if(!spSocketEvent)
        return;

    spSocketEvent->Detach();
    spSocketEvent->Close();
}

void net::Socket::_write(void)
{
    _doWrite();
}

void net::Socket::_read(void)
{
    m_bReading = true;
    _doRead();
}

void net::Socket::_doWrite(void)
{
    auto spSocketEvent = m_wpSocketEvent.lock();
    assert(spSocketEvent);

    for(;;)
    {
        if(m_wrBuffer.empty())
        {
            if(!m_bWRShutdown && _onDrained())
            {
                if(shutdown(spSocketEvent->GetFD(), SHUT_WR) < 0)
                    EMIT_EVENT(error, SystemError("shutdown() failed", errno));

                m_bWRShutdown = true;
            }

            if(m_bHUP && !m_bAllowHalfOpen)
                spSocketEvent->Close();
            else
                spSocketEvent->ClearIOEventFlag(IOEvent::IOEvents::IOEV_WRITE);

            break;
        }

        auto nWritten = TEMP_FAILURE_RETRY(::write(spSocketEvent->GetFD(), m_wrBuffer.data(), m_wrBuffer.size()));
        if(nWritten < 0)
        {
            if(!IS_WOULDBLOCK(errno))
                EMIT_EVENT(error, SystemError("write() failed", errno));

            spSocketEvent->SetIOEventFlag(IOEvent::IOEvents::IOEV_WRITE);

            break;
        }

        m_wrBuffer = std::vector<uint8_t>(m_wrBuffer.begin() + nWritten, m_wrBuffer.end());
    }
}

void net::Socket::_doRead(void)
{
    auto spSocketEvent = m_wpSocketEvent.lock();
    assert(spSocketEvent);

    for(;!m_bHUP;)
    {
        std::vector<uint8_t> vecBuf(SOCKET_READ_BUF_SIZE);

        auto nRead = TEMP_FAILURE_RETRY(read(spSocketEvent->GetFD(), vecBuf.data(), vecBuf.size()));
        if(nRead < 0)
        {
            if(!IS_WOULDBLOCK(errno))
                EMIT_EVENT(error, SystemError("read() failed", errno));

            if(m_bReading)
                spSocketEvent->SetIOEventFlag(IOEvent::IOEvents::IOEV_READ);

            break;
        }

        if(!_push(vecBuf.data(), nRead))
        {
            m_bReading = false;
            spSocketEvent->ClearIOEventFlag(IOEvent::IOEvents::IOEV_READ);
        }

        if(nRead == 0)
        {
            m_bHUP = true;
            // Write up all pending data before closing connection from this side too
            _doWrite();

            break;
        }
    }
}
