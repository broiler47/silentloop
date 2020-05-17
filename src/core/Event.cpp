//
// Created by palulukan on 5/2/20.
//

#include "Event.h"

#include <cassert>

#include "Log.h"
#include "EventLoopFactory.h"

std::shared_ptr<Event> Event::CreateShared(void)
{
    struct make_shared_enabler : public Event {};

    return std::make_shared<make_shared_enabler>();
}

Event::Event(void)
{
    //DEBUG("Event(0x%" PRIXPTR ")", this);
}

Event::~Event(void)
{
    //DEBUG("~Event(0x%" PRIXPTR ")", this);
}

void Event::Attach(void)
{
    assert(!m_pEL);
    assert(!m_handle);

    GetThreadEventLoop()->Add(GetSharedPtr<Event>());
}

void Event::Detach(void)
{
    if(!m_pEL)
        return;

    assert(m_handle);

    m_pEL->_removeEvent(m_handle);
}

void Event::SetTimer(EventLoopBase::TimeInterval timeout, bool bPeriodic)
{
    assert(m_pEL);
    assert(m_handle);

    m_pEL->_setTimer(m_handle, timeout, bPeriodic);
}

void Event::CancelTimer(void)
{
    assert(m_pEL);
    assert(m_handle);

    m_pEL->_cancelTimer(m_handle);
}

bool Event::_isAttached(void) const
{
    return m_pEL != nullptr;
}

void Event::_notifyIOStateChange(void)
{
    assert(m_pEL);
    assert(m_handle);

    m_pEL->_notifyIOStateChange(m_handle);
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
