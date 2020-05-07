//
// Created by palulukan on 5/2/20.
//

#include "Event.h"

#include <cassert>

//EventLoopBase& Event::_eventLoop(void)
//{
//    assert(m_pEL);
//
//    return *m_pEL;
//}

void Event::_attach(EventLoopBase &eventLoop)
{
    assert(!m_pEL);
    assert(!m_handle);

    auto spSelf = m_self.lock();

    if(!spSelf)
        throw std::runtime_error("Attempting ot attach unreferenced event. Please make sure that event objects are always created with Event::CreateEvent()!");

    eventLoop.Add(spSelf);
}

bool Event::_isAttached(void) const
{
    return m_pEL != nullptr;
}

void Event::_detach(void)
{
    assert(m_pEL);
    assert(m_handle);

    m_pEL->_removeEvent(m_handle);
}

void Event::_setTimeout(EventLoopBase::TimeInterval timeout)
{
    assert(m_pEL);
    assert(m_handle);

    m_pEL->_setTimeout(m_handle, timeout);
}

void Event::_cancelTimeout(void)
{
    assert(m_pEL);
    assert(m_handle);

    m_pEL->_cancelTimeout(m_handle);
}

void Event::_notifyIOEventMaskUpdate(int fd, unsigned int mask)
{
    assert(m_pEL);
    assert(m_handle);

    m_pEL->_notifyIOEventMaskUpdate(m_handle, fd, mask);
}

void Event::_onCreated(const std::weak_ptr<Event> &self)
{
    assert(self.lock());
    assert(!m_self.lock());

    m_self = self;
}

void Event::_onAttach(EventLoopBase* pEL, EventLoopBase::EventHandle handle)
{
    assert(pEL);
    assert(handle);

    assert(!m_pEL);
    assert(!m_handle);

    m_pEL = pEL;
    m_handle = handle;
}

void Event::_onDetached(void)
{
    assert(m_pEL);
    assert(m_handle);

    m_pEL = nullptr;
    m_handle = nullptr;
}
