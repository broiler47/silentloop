//
// Created by palulukan on 5/8/20.
//

#include "Socket.h"

#include <cassert>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

std::shared_ptr<net::Socket> net::Socket::CreateShared(int fd, bool allowHalfOpen, bool startReading)
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

    if(startReading)
        resume();

    assert(!m_wpSocketEvent.lock());

    auto spSocketEvent = IOEvent::CreateShared();
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
                _onWriteError(SystemError("write() failed", errno));

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
#if SOCKET_READ_BUF_SIZE <= 0x10000
        uint8_t rdBuf[SOCKET_READ_BUF_SIZE];
#else
        std::vector<uint8_t> vecBuf(SOCKET_READ_BUF_SIZE);
        auto rdBuf = vecBuf.data();
#endif

        auto nRead = TEMP_FAILURE_RETRY(read(spSocketEvent->GetFD(), rdBuf, SOCKET_READ_BUF_SIZE));
        if(nRead < 0)
        {
            if(!IS_WOULDBLOCK(errno))
                EMIT_EVENT(error, SystemError("read() failed", errno));

            if(m_bFlowing)
                spSocketEvent->SetIOEventFlag(IOEvent::IOEvents::IOEV_READ);

            break;
        }

        if(!_push(rdBuf, nRead))
            spSocketEvent->ClearIOEventFlag(IOEvent::IOEvents::IOEV_READ);

        if(nRead == 0)
        {
            m_bHUP = true;
            // Write up all pending data before closing connection from this side too
            _doWrite();

            break;
        }
    }
}
