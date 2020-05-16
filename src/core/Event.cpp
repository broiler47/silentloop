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

    // NOTE: In case this object is not managed by std::shared_ptr,
    // shared_from_this() will only throw std::bad_weak_ptr in C++17 or later.
    std::shared_ptr<Event> spThis;
    try
    {
        spThis = static_pointer_cast<Event>(shared_from_this());
    }
    catch(std::bad_weak_ptr&)
    {
        auto errMsg = "Event objects should be managed by std::shared_ptr and should not be linked from the constructor";
        ERROR(errMsg);
        throw std::runtime_error(errMsg);
    }

    GetThreadEventLoop()->Add(spThis);
}

void Event::Detach(void)
{
    if(!m_pEL)
        return;

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
