//
// Created by palulukan on 5/3/20.
//

#include "Timer.h"

std::shared_ptr<Timeout> Timeout::CreateShared(bool bPeriodic)
{
    struct make_shared_enabler : public Timeout
    {
        make_shared_enabler(bool bPeriodic) :
            Timeout(bPeriodic)
        {}
    };

    return std::make_shared<make_shared_enabler>(bPeriodic);
}

Timeout::Timeout(bool bPeriodic) :
    m_bPeriodic(bPeriodic),
    m_spEventObj(Event::CreateShared())
{
    if(!m_bPeriodic)
    {
        auto pEvent = m_spEventObj.get();
        on_timeout([pEvent](void) {
            pEvent->Detach();
        });
    }
}

void Timeout::Refresh(EventLoopBase::TimeInterval timeout)
{
    Cancel();

    m_spEventObj->Attach();
    m_spEventObj->SetTimer(timeout, m_bPeriodic);
}

void Timeout::Cancel(void)
{
    m_spEventObj->Detach();
}

static std::shared_ptr<Timeout> _createTimer(const std::function<void(void)> &cb, EventLoopBase::TimeInterval timeout, bool bPeriodic)
{
    auto sp = Timeout::CreateShared(bPeriodic);

    sp->on_timeout(cb);
    sp->Refresh(timeout);

    return sp;
}

std::shared_ptr<Timeout> SetTimeout(const std::function<void(void)> &cb, EventLoopBase::TimeInterval timeout)
{
    return _createTimer(cb, timeout, false);
}

std::shared_ptr<Timeout> SetInterval(const std::function<void(void)> &cb, EventLoopBase::TimeInterval interval)
{
    return _createTimer(cb, interval, true);
}
