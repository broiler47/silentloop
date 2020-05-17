//
// Created by palulukan on 5/3/20.
//

#include "Tineout.h"

std::shared_ptr<Timeout> Timeout::CreateShared(void)
{
    struct make_shared_enabler : public Timeout {};

    return std::make_shared<make_shared_enabler>();
}

Timeout::Timeout(void) :
    m_spEventObj(Event::CreateShared())
{
    auto pEvent = m_spEventObj.get();
    m_spEventObj->on_timeout([pEvent](void) {
        pEvent->Detach();
    });
}

void Timeout::Refresh(EventLoopBase::TimeInterval timeout)
{
    Cancel();

    m_spEventObj->Attach();
    m_spEventObj->SetTimeout(timeout);
}

void Timeout::Cancel(void)
{
    m_spEventObj->Detach();
}

std::shared_ptr<Timeout> SetTimeout(const std::function<void(void)> &cb, EventLoopBase::TimeInterval timeout)
{
    auto sp = Timeout::CreateShared();

    sp->on_timeout(cb);
    sp->Refresh(timeout);

    return sp;
}
