//
// Created by palulukan on 5/2/20.
//

#include "EventLoop.h"

#include "Utils.h"
#include "Log.h"

#include <utility>
#include <cassert>
#include <chrono>
#include <climits>

EventLoop::EventLoop(std::function<void(void)> cbEvMain, std::shared_ptr<IOMuxBase> spFDMux) :
    m_cbEvMain(std::move(cbEvMain)),
    m_spIOMux(std::move(spFDMux))
{
    assert(m_spIOMux);
}

EventLoop::~EventLoop(void)
{
    // TODO: _detach all events
}

bool EventLoop::Add(const std::shared_ptr<Event> &spEvent)
{
    assert(spEvent);

    if(spEvent->_isAttached())
    {
        ERROR("Event is already attached to the event loop");
        return false;
    }

    auto evInfo = std::make_unique<_eventInfo>(spEvent, this);

    if(!_updateIOFD((EventHandle)evInfo.get()))
        return false;

    CATCH_ALL(spEvent->_onAttach(this, (EventHandle)evInfo.get()));

    m_lstEvents.push_back(std::unique_ptr<_eventInfo>(evInfo.release()));
    auto itEvent = --m_lstEvents.end();
    (*itEvent)->itEvent = itEvent;

    return true;
}

void EventLoop::Run(void)
{
    _updateTimePoint();

    if(m_cbEvMain)
        CATCH_ALL(m_cbEvMain());

    _processPendingRemovals();

    while(_tick());
}

void EventLoop::_removeEvent(EventLoopBase::EventHandle hEvent)
{
    assert(hEvent);

    if(!_isRegistered(hEvent))
    {
        ERROR("Attempting to remove unregistered event");
        return;
    }

    _removeFromTimerQueue(hEvent);

    m_vecRMPending.push_back(hEvent);

    auto *pEventInfo = (_eventInfo *)hEvent;
    pEventInfo->flags |= EVF_DETACHED;

    CATCH_ALL(pEventInfo->spEvent->_onDetached());

    if(pEventInfo->fdRegistered >= 0)
        m_spIOMux->Unregister(pEventInfo->fdRegistered);
}

void EventLoop::_notifyIOStateChange(EventLoopBase::EventHandle hEvent)
{
    assert(hEvent);

    if(!_isRegistered(hEvent))
    {
        ERROR("Attempting to update IO state of unregistered event");
        return;
    }

    _updateIOFD(hEvent);
}

void EventLoop::_setTimer(EventLoopBase::EventHandle hEvent, EventLoopBase::TimeInterval timeout, bool bPeriodic)
{
    assert(hEvent);

    if(!_isRegistered(hEvent))
    {
        ERROR("Attempting to set timer from unregistered event");
        return;
    }

    auto pEventInfo = (_eventInfo *)hEvent;

    if(pEventInfo->flags & EVF_TIMER_SET)
    {
        ERROR("Event timeor is already set");
        return;
    }

    pEventInfo->tpTimeout = std::chrono::milliseconds(timeout);
    pEventInfo->tpNextTimeout = m_tNow + pEventInfo->tpTimeout;
    pEventInfo->flags |= EVF_TIMER_SET | (bPeriodic ? EVF_TIMER_PERIODIC : 0);

    m_qTimers.push(hEvent);
}

void EventLoop::_cancelTimer(EventLoopBase::EventHandle hEvent)
{
    assert(hEvent);

    if(!_isRegistered(hEvent))
    {
        ERROR("Attempting to cancel timer from unregistered event");
        return;
    }

    auto pEventInfo = (_eventInfo *)hEvent;

    if(!(pEventInfo->flags & EVF_TIMER_SET))
    {
        ERROR("Event timer was not set");
        return;
    }

    _removeFromTimerQueue(hEvent);
}

bool EventLoop::_isRegistered(EventLoopBase::EventHandle hEvent)
{
    assert(hEvent);

    auto pEventInfo = (_eventInfo *)hEvent;
    return pEventInfo->pEventLoop == this && !(pEventInfo->flags & EVF_DETACHED);
}

bool EventLoop::_tick(void)
{
    // Phase 1: Run timers
    _updateTimePoint();

    while(!m_qTimers.empty())
    {
        auto pEventInfo = (_eventInfo *)m_qTimers.top();
        if(pEventInfo->tpNextTimeout <= m_tNow)
        {
            m_qTimers.pop();

            if(pEventInfo->flags & EVF_TIMER_PERIODIC)
            {
                pEventInfo->tpNextTimeout = m_tNow + pEventInfo->tpTimeout;
                m_qTimers.push(pEventInfo);
            }
            else
                _clearTimerFlags(pEventInfo);

            CATCH_ALL(pEventInfo->spEvent->OnTimer());
        }
        else
            break;
    }

    _processPendingRemovals();

    // Phase 2: Poll file descriptors for IO events
    _updateTimePoint();

    int nTimeout = m_lstEvents.empty() ? 0 : -1;
    if(!m_qTimers.empty())
    {
        auto tSleep = std::chrono::duration_cast<std::chrono::milliseconds>(((_eventInfo *)m_qTimers.top())->tpNextTimeout - m_tNow).count();
        if(tSleep >= 0)
            nTimeout = tSleep > INT_MAX ? INT_MAX : int(tSleep);
        else
            nTimeout = 0;
    }

    std::vector<std::pair<EventLoopBase::EventHandle, unsigned int>> vecEvents;
    if(!m_spIOMux->Poll(nTimeout, vecEvents))
    {
        ERROR("Critical error: Failed to poll for IO events. Breaking event loop!");
        return false;
    }
    for(const auto& ev : vecEvents)
    {
        auto pEventInfo = (_eventInfo *)ev.first;
        unsigned int eventMask = ev.second;

        if(eventMask & IOMuxBase::IOEV_ERROR && !(pEventInfo->flags & EVF_DETACHED))
            CATCH_ALL(pEventInfo->spEvent->OnError());

        if(eventMask & IOMuxBase::IOEV_HUP && !(pEventInfo->flags & EVF_DETACHED))
            CATCH_ALL(pEventInfo->spEvent->OnHUP());

        if(eventMask & IOMuxBase::IOEV_READ && !(pEventInfo->flags & EVF_DETACHED))
            CATCH_ALL(pEventInfo->spEvent->OnRead());

        if(eventMask & IOMuxBase::IOEV_WRITE && !(pEventInfo->flags & EVF_DETACHED))
            CATCH_ALL(pEventInfo->spEvent->OnWrite());
    }

    _processPendingRemovals();

    // Phase 3: Run setImmediate() callbacks
    // TODO: Implement

    return !m_lstEvents.empty();
}

void EventLoop::_processPendingRemovals(void)
{
    _drainNextTickQueue();

    for(auto hEvent : m_vecRMPending)
    {
        auto pEventInfo = (_eventInfo *)hEvent;
        m_lstEvents.erase(pEventInfo->itEvent);
    }

    m_vecRMPending.clear();
}

bool EventLoop::_updateIOFD(EventLoopBase::EventHandle hEvent)
{
    auto pEventInfo = (_eventInfo *)hEvent;

    int fdCurrent = pEventInfo->spEvent->GetFD();
    unsigned int nMaskCurrent = pEventInfo->spEvent->GetIOEventMask();

    if(pEventInfo->fdRegistered != fdCurrent)
    {
        if(pEventInfo->fdRegistered >= 0)
        {
            m_spIOMux->Unregister(pEventInfo->fdRegistered);

            pEventInfo->fdRegistered = -1;
            pEventInfo->nIOEventMask = 0;
        }

        if(fdCurrent >= 0 && !m_spIOMux->Register(fdCurrent, nMaskCurrent, hEvent))
        {
            ERROR("Failed to register IO event in the mux");
            return false;
        }

        pEventInfo->fdRegistered = fdCurrent;
        pEventInfo->nIOEventMask = nMaskCurrent;
    }
    else if(pEventInfo->nIOEventMask != nMaskCurrent && fdCurrent >= 0 && !m_spIOMux->UpdateIOEventMask(fdCurrent, nMaskCurrent, hEvent))
    {
        ERROR("Failed to update IO event mask");
        return false;
    }

    return true;
}

void EventLoop::_removeFromTimerQueue(EventLoopBase::EventHandle hEvent)
{
    auto pEventInfo = (_eventInfo *)hEvent;

    if(!(pEventInfo->flags & EVF_TIMER_SET))
        return;

    _clearTimerFlags(hEvent);

    m_qTimers.Remove(hEvent);
}

void EventLoop::_clearTimerFlags(EventLoopBase::EventHandle hEvent)
{
    auto pEventInfo = (_eventInfo *)hEvent;
    pEventInfo->flags &= ~(EVF_TIMER_SET | EVF_TIMER_PERIODIC);
}

void EventLoop::_updateTimePoint(void)
{
    m_tNow = std::chrono::steady_clock::now();
}
