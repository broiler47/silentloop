//
// Created by palulukan on 5/3/20.
//

#include "Tineout.h"

void TimeoutEvent::Arm(EventLoopBase &eventLoop, const std::function<void(void)> &cb, EventLoopBase::TimeInterval timeout)
{
    Cancel();

    m_cb = cb;

    Attach(eventLoop);
    SetTimeout(timeout);
}

void TimeoutEvent::Cancel(void)
{
    if(!isAttached())
        return;

    CancelTimeout();
    Detach();
}

void TimeoutEvent::OnTimeout(void)
{
    Event::OnTimeout();

    Detach();

    if(m_cb)
        m_cb();
}

std::shared_ptr<TimeoutEvent> SetTimeout(EventLoopBase &eventLoop, const std::function<void(void)> &cb, EventLoopBase::TimeInterval timeout)
{
    auto sp = Event::CreateEvent<TimeoutEvent>();
    sp->Arm(eventLoop, cb, timeout);
    return sp;
}
