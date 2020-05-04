//
// Created by palulukan on 5/2/20.
//

#include "EventBase.h"

#include <cassert>

#include <unistd.h>

EventLoopBase& Event::EventLoop(void)
{
    assert(m_pEL);

    return *m_pEL;
}

void Event::Attach(EventLoopBase &eventLoop)
{
    assert(!m_pEL);
    assert(!m_handle);

    auto spSelf = m_self.lock();

    assert(spSelf);

    eventLoop.Add(spSelf);
}

bool Event::isAttached(void) const
{
    return m_pEL != nullptr;
}

void Event::Detach(void)
{
    assert(m_pEL);
    assert(m_handle);

    m_pEL->_removeEvent(m_handle);
}

void Event::SetTimeout(EventLoopBase::TimeInterval timeout)
{
    assert(m_pEL);
    assert(m_handle);

    m_pEL->_setTimeout(m_handle, timeout);
}

void Event::CancelTimeout(void)
{
    assert(m_pEL);
    assert(m_handle);

    m_pEL->_cancelTimeout(m_handle);
}

void Event::_onCreated(const std::weak_ptr<Event> &self)
{
    assert(self.lock());
    assert(!m_self.lock());

    m_self = self;
}

void Event::_onAttached(EventLoopBase* pEL, EventLoopBase::EventHandle handle)
{
    assert(pEL);
    assert(handle);

    assert(!m_pEL);
    assert(!m_handle);

    m_pEL = pEL;
}

void Event::_onDetached(void)
{
    assert(m_pEL);
    assert(m_handle);

    m_pEL = nullptr;
    m_handle = nullptr;
}

IOEvent::~IOEvent(void)
{
    if(m_fd >= 0)
    {
        assert(false);

        close(m_fd);
        m_fd = -1;
    }
}
