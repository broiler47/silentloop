//
// Created by palulukan on 5/8/20.
//

#include "Socket.h"

#include <cassert>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

std::shared_ptr<Socket> Socket::Create(EventLoopBase& eventLoop, int fd, bool allowHalfOpen, bool startReading)
{
    auto sp = Event::CreateEvent<Socket>();
    sp->_init(eventLoop, fd, allowHalfOpen, startReading);
    return sp;
}

void Socket::setNoDelay(bool noDelay)
{
    if(GetFD() < 0)
    {
        EMIT_EVENT_ASYNC(error, TypeError("Invalid file descriptor"));
        return;
    }

    int nEnable = noDelay;
    if(setsockopt(GetFD(), IPPROTO_TCP, TCP_NODELAY, &nEnable, sizeof(nEnable)) < 0)
        EMIT_EVENT(error, SystemError("setsockopt(TCP_NODELAY)", errno));
}

void Socket::_init(EventLoopBase& eventLoop, int fd, bool allowHalfOpen, bool startReading)
{
    m_bAllowHalfOpen = allowHalfOpen;

    _attach(eventLoop);

    if(fd < 0)
    {
        EMIT_EVENT_ASYNC(error, TypeError("Invalid file descriptor"));
        return Close();
    }

    if(startReading)
        resume();

    SetFD(fd, m_bFlowing ? (unsigned int)IOEvents::IOEV_READ : 0);
}

void Socket::OnRead(void)
{
    Event::OnRead();

    _doRead();
}

void Socket::OnWrite(void)
{
    Event::OnWrite();

    _doWrite();
}

void Socket::OnHUP(void)
{
    Event::OnHUP();

    _doRead();
}

void Socket::OnError(void)
{
    Event::OnError();

    EMIT_EVENT(error, Error("Socket error"));
    Close();
}

void Socket::_write(void)
{
    _doWrite();
}

void Socket::_read(void)
{
    SetIOEventFlag(IOEvents::IOEV_READ);
    _doRead();
}

void Socket::_doWrite(void)
{
    for(;;)
    {
        if(m_wrBuffer.empty())
        {
            if(!m_bWRShutdown && _onDrained())
            {
                if(shutdown(GetFD(), SHUT_WR) < 0)
                    EMIT_EVENT(error, SystemError("shutdown() failed", errno));

                m_bWRShutdown = true;
            }

            if(m_bHUP && !m_bAllowHalfOpen)
                Close();
            else
                ClearIOEventFlag(IOEvents::IOEV_WRITE);

            break;
        }

        auto nWritten = TEMP_FAILURE_RETRY(::write(GetFD(), m_wrBuffer.data(), m_wrBuffer.size()));
        if(nWritten < 0)
        {
            if(!IS_WOULDBLOCK(errno))
                EMIT_EVENT(error, SystemError("write() failed", errno));

            SetIOEventFlag(IOEvents::IOEV_WRITE);

            break;
        }

        m_wrBuffer = std::vector<uint8_t>(m_wrBuffer.begin() + nWritten, m_wrBuffer.end());
    }
}

void Socket::_doRead(void)
{
    for(;!m_bHUP;)
    {
        std::vector<uint8_t> vecBuf(SOCKET_READ_BUF_SIZE);

        auto nRead = TEMP_FAILURE_RETRY(read(GetFD(), vecBuf.data(), vecBuf.size()));
        if(nRead < 0)
        {
            if(!IS_WOULDBLOCK(errno))
                EMIT_EVENT(error, SystemError("read() failed", errno));

            break;
        }

        if(!_push(vecBuf.data(), nRead))
            ClearIOEventFlag(IOEvents::IOEV_READ);

        if(nRead == 0)
        {
            m_bHUP = true;
            // Write up all pending data before closing connection from this side too
            _doWrite();

            break;
        }
    }
}
