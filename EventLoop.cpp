//
// Created by palulukan on 5/2/20.
//

#include "EventLoop.h"

#include <utility>
#include <cassert>
#include <chrono>

EventLoop::EventLoop(std::function<void(EventLoopBase&)>  cbInit) :
    m_cbInit(std::move(cbInit))
{
}

EventLoop::~EventLoop(void)
{
    // TODO: Detach all events
}

bool EventLoop::Add(const std::shared_ptr<Event> &spEvent)
{
    assert(spEvent);

    // TODO: Check if already registered

    auto evInfo = std::make_unique<_eventInfo>(spEvent, this);

    // TODO: spEvent->GetFD()

    // TODO: Exceptions
    if(!spEvent->_onAttach(this, (EventHandle)evInfo.get()))
        return false;

    m_lstEvents.push_back(std::unique_ptr<_eventInfo>(evInfo.release()));
    auto itEvent = --m_lstEvents.end();
    (*itEvent)->itEvent = itEvent;

    return true;
}

void EventLoop::Run(void)
{
    if(m_cbInit)
        m_cbInit(*this);

    while(_tick());
}

void EventLoop::_removeEvent(EventLoopBase::EventHandle hEvent)
{
    assert(hEvent);
    assert(_isRegistered(hEvent));

    m_vecRMPending.push_back(hEvent);

    auto *pEventInfo = (_eventInfo *)hEvent;
    pEventInfo->flags &= EVF_DETACHED;
    // TODO: Exceptions
    pEventInfo->spEvent->_onDetached();
}

void EventLoop::_setTimeout(EventLoopBase::EventHandle hEvent, EventLoopBase::TimeInterval timeout)
{
    assert(hEvent);
    assert(_isRegistered(hEvent));

    auto pEventInfo = (_eventInfo *)hEvent;

    // Is this event already in the timeout queue?
    assert((pEventInfo->flags & EVF_TIMEOUT_SET) == 0);

    pEventInfo->tpNextTimeout = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout);
    pEventInfo->flags |= EVF_TIMEOUT_SET;

    m_qTimeouts.push(hEvent);
}

void EventLoop::_cancelTimeout(EventLoopBase::EventHandle hEvent)
{
    // TODO: Implement
}

bool EventLoop::_isRegistered(EventLoopBase::EventHandle hEvent)
{
    assert(hEvent);

    auto pEventInfo = (_eventInfo *)hEvent;
    return pEventInfo->pEventLoop == this && !(pEventInfo->flags & EVF_DETACHED);
}

bool EventLoop::_tick(void)
{
    // TODO: Exceptions

    // Phase 1 - Run timers
    auto now = std::chrono::steady_clock::now();
    while(!m_qTimeouts.empty())
    {
        auto pEventInfo = (_eventInfo *)m_qTimeouts.top();
        if(pEventInfo->tpNextTimeout <= now)
        {
            m_qTimeouts.pop();

            pEventInfo->flags &= ~EVF_TIMEOUT_SET;
            pEventInfo->spEvent->OnTimeout();
        }
        else
            break;
    }
    _processPendingRemovals();

    // TODO: Implement

    return !m_lstEvents.empty();
}

void EventLoop::_processPendingRemovals(void)
{
    for(auto hEvent : m_vecRMPending)
    {
        auto pEventInfo = (_eventInfo *)hEvent;

        // TODO: remove evnt's FD from event loop

        m_lstEvents.erase(pEventInfo->itEvent);
    }

    m_vecRMPending.clear();
}
