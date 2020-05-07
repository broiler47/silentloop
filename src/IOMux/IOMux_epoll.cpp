//
// Created by palulukan on 5/5/20.
//

#include "IOMux_epoll.h"

#include "Log.h"
#include "Events/core/IOEvent.h"

#include <cassert>

#include <unistd.h>

std::shared_ptr<IOMuxBase> IOMux_epoll::Create(unsigned int maxEvents)
{
    assert(maxEvents > 0);

    return std::make_shared<IOMux_epoll>(maxEvents);
}

IOMux_epoll::IOMux_epoll(unsigned int maxEvents) :
    m_vecReadyEvents(maxEvents),
    m_fdEpoll(-1)
{
    assert(maxEvents > 0);

    m_fdEpoll = epoll_create1(EPOLL_CLOEXEC);
    if(m_fdEpoll < 0)
    {
        SYSCALL_ERROR("epoll_create1(EPOLL_CLOEXEC)");
        throw std::runtime_error("epoll_create1() failed");
    }
}

IOMux_epoll::~IOMux_epoll(void)
{
    if(m_fdEpoll >= 0)
    {
        close(m_fdEpoll);
        m_fdEpoll = -1;
    }
}

bool IOMux_epoll::Register(int fd, unsigned int mask, EventLoopBase::EventHandle hEvent)
{
    return _epollCtl(EPOLL_CTL_ADD, fd, mask, hEvent);
}

void IOMux_epoll::Unregister(int fd)
{
    _epollCtl(EPOLL_CTL_DEL, fd, 0, nullptr);
}

bool IOMux_epoll::UpdateIOEventMask(int fd, unsigned int mask, EventLoopBase::EventHandle hEvent)
{
    return _epollCtl(EPOLL_CTL_MOD, fd, mask, hEvent);
}

bool IOMux_epoll::Poll(int timeout, std::vector<std::pair<EventLoopBase::EventHandle, unsigned int>>& res)
{
    int nReady = TEMP_FAILURE_RETRY(epoll_wait(m_fdEpoll, m_vecReadyEvents.data(), int(m_vecReadyEvents.size()), timeout));
    if(nReady < 0)
    {
        SYSCALL_ERROR("epoll_wait()");
        return false;
    }

    res.resize(nReady);
    for(int i = 0; i < nReady; ++i)
    {
        unsigned int eventMask = (m_vecReadyEvents[i].events & EPOLLERR) ? (unsigned int)IOEV_ERROR : 0;
        if(m_vecReadyEvents[i].events & (EPOLLRDHUP | EPOLLHUP))
            eventMask |= IOEV_CLOSE;
        if(m_vecReadyEvents[i].events & (EPOLLIN | EPOLLPRI))
            eventMask |= IOEV_READ;
        if(m_vecReadyEvents[i].events & EPOLLOUT)
            eventMask |= IOEV_WRITE;

        EventLoopBase::EventHandle hEvent = m_vecReadyEvents[i].data.ptr;

        res[i] = std::make_pair(hEvent, eventMask);
    }

    return true;
}

bool IOMux_epoll::_epollCtl(int op, int fd, unsigned int mask, EventLoopBase::EventHandle hEvent)
{
    assert(fd >= 0);

    uint32_t events = EPOLLRDHUP | EPOLLHUP | EPOLLERR;

    if(mask & IOEV_READ)
        events |= EPOLLIN | EPOLLPRI;

    if(mask & IOEV_WRITE)
        events |= EPOLLOUT;

    struct epoll_event ev = {
        .events = events,
        .data = {
            .ptr = hEvent
        }
    };

    if(epoll_ctl(m_fdEpoll, op, fd, &ev) < 0)
    {
        switch(op)
        {
            case EPOLL_CTL_ADD: SYSCALL_ERROR("epoll_ctl(EPOLL_CTL_ADD)"); break;
            case EPOLL_CTL_MOD: SYSCALL_ERROR("epoll_ctl(EPOLL_CTL_MOD)"); break;
            case EPOLL_CTL_DEL: SYSCALL_ERROR("epoll_ctl(EPOLL_CTL_DEL)"); break;
        }

        return false;
    }

    return true;
}
