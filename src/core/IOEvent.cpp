//
// Created by palulukan on 5/7/20.
//

#include "IOEvent.h"

#include "Log.h"
#include "EventLoopFactory.h"

#include <cassert>

#include <unistd.h>
#include <fcntl.h>

std::shared_ptr<IOEvent> IOEvent::CreateShared(void)
{
    struct make_shared_enabler : public IOEvent {};

    return std::make_shared<make_shared_enabler>();
}

IOEvent::~IOEvent(void)
{
    _close();
}

bool IOEvent::EnableNonBlockingIO(bool enable)
{
    if(m_fd < 0)
    {
        ERROR("Changing blocking mode on closed file descriptor");
        return false;
    }

    int flags = fcntl(m_fd, F_GETFL, 0);
    if(flags < 0)
    {
        SYSCALL_ERROR("fcntl(F_GETFL)");
        return false;
    }

    if(((flags & O_NONBLOCK) != 0) == enable)
    {
        DEBUG("O_NONBLOCK is already configured for FD %d", m_fd);
        return true;
    }

    flags = enable ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);
    if(fcntl(m_fd, F_SETFL, flags) < 0)
    {
        SYSCALL_ERROR("fcntl(F_SETFL)");
        return false;
    }

    return true;
}

void IOEvent::Close(void)
{
    if(_isAttached())
    {
        GetThreadEventLoop()->NextTick([this](void) {
            if(m_fd >= 0)
                EMIT_EVENT(close);

            _close();
        }, GetSharedPtr());

        Detach();
    }
    else
        _close();
}

void IOEvent::SetFD(int fd, unsigned int mask)
{
    assert(fd >= 0);
    assert(m_fd < 0);

    m_fd = fd;
    m_IOEventMask = mask;

    if(_isAttached())
        _notifyIOStateChange();
}

void IOEvent::SetIOEventMask(unsigned int mask)
{
    if(mask == m_IOEventMask)
        return;

    m_IOEventMask = mask;

    if(_isAttached())
        _notifyIOStateChange();
}

void IOEvent::_close(void)
{
    if(m_fd >= 0)
    {
        assert(!_isAttached());

        close(m_fd);
        m_fd = -1;
    }

    m_IOEventMask = 0;
}
