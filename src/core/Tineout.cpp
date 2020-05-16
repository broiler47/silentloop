//
// Created by palulukan on 5/3/20.
//

#include "Tineout.h"

#include "Event.h"

void Timeout::Refresh(EventLoopBase::TimeInterval timeout)
{
    Cancel();

    auto spEvent = std::make_shared<Event>();
    m_wpEvent = spEvent;

    spEvent->Attach();

    LinkWith(spEvent);

    spEvent->on_timeout([this](void) {
        Cancel();
        EMIT_EVENT(timeout);
    });

    spEvent->SetTimeout(timeout);
}

void Timeout::Cancel(void)
{
    auto spEvent = m_wpEvent.lock();
    if(!spEvent)
        return;

    spEvent->Detach();
}

std::shared_ptr<Timeout> SetTimeout(const std::function<void(void)> &cb, EventLoopBase::TimeInterval timeout)
{
    auto sp = std::make_shared<Timeout>();

    sp->on_timeout(cb);
    sp->Refresh(timeout);

    return sp;
}
