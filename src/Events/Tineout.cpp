//
// Created by palulukan on 5/3/20.
//

#include "Tineout.h"

#include "Utils.h"

void TimeoutEvent::Arm(EventLoopBase &eventLoop, EventLoopBase::TimeInterval timeout)
{
    Cancel();

    _attach(eventLoop);
    _setTimeout(timeout);
}

void TimeoutEvent::Cancel(void)
{
    if(!_isAttached())
        return;

    _cancelTimeout();
    _detach();
}

void TimeoutEvent::OnTimeout(void)
{
    Event::OnTimeout();

    _detach();

    EMIT_EVENT(timeout);
}

std::shared_ptr<TimeoutEvent> SetTimeout(EventLoopBase &eventLoop, const std::function<void(void)> &cb, EventLoopBase::TimeInterval timeout)
{
    auto sp = Event::CreateEvent<TimeoutEvent>();
    sp->on_timeout(cb);
    sp->Arm(eventLoop, timeout);
    return sp;
}
