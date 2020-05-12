//
// Created by palulukan on 5/2/20.
//

#include "Event.h"

#include <cassert>

extern thread_local std::shared_ptr<EventLoopBase> _tls_event_loop;

void Event::_attach(void)
{
    assert(!m_pEL);
    assert(!m_handle);

    auto spSelf = m_self.lock();

    if(!spSelf)
        throw std::runtime_error("Attempting ot attach unreferenced event. Please make sure that event objects are always created with Event::CreateEvent()!");

    if(!_tls_event_loop)
        throw std::runtime_error("Unable to attach event from thread with no local event loop");

    _tls_event_loop->Add(spSelf);
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

void Event::_notifyIOStateChange(void)
{
    assert(m_pEL);
    assert(m_handle);

    m_pEL->_notifyIOStateChange(m_handle);
}

void Event::_nextTick(const std::function<void(void)> &cb)
{
    assert(m_pEL);
    assert(m_handle);
    assert(cb);

    m_pEL->_nextTick(cb);
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
