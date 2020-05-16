//
// Created by palulukan on 5/3/20.
//

#include "Tineout.h"

#include "Event.h"

std::shared_ptr<Timeout> Timeout::CreateShared(void)
{
    struct make_shared_enabler : public Timeout {};

    return std::make_shared<make_shared_enabler>();
}

void Timeout::Refresh(EventLoopBase::TimeInterval timeout)
{
    Cancel();

    auto spEvent = Event::CreateShared();
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
    auto sp = Timeout::CreateShared();

    sp->on_timeout(cb);
    sp->Refresh(timeout);

    return sp;
}
