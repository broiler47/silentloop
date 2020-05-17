//
// Created by palulukan on 5/3/20.
//

#ifndef TIMER_H_5EF70BE0F9A44D8F81B6A5E32667B6F7
#define TIMER_H_5EF70BE0F9A44D8F81B6A5E32667B6F7

#include "Event.h"

class Timeout
{
    public:
        static std::shared_ptr<Timeout> CreateShared(bool bPeriodic);

    public:
        EXPORT_EVENT_SIGNATURE(timeout) { m_spEventObj->on_timer(cb); }

    private:
        Timeout(bool bPeriodic);

    public:
        void Refresh(EventLoopBase::TimeInterval timeout);
        void Cancel(void);

    private:
        bool m_bPeriodic;
        std::shared_ptr<Event> m_spEventObj;
};

std::shared_ptr<Timeout> SetTimeout(const std::function<void(void)>& cb, EventLoopBase::TimeInterval timeout);

std::shared_ptr<Timeout> SetInterval(const std::function<void(void)>& cb, EventLoopBase::TimeInterval interval);

#endif //TIMER_H_5EF70BE0F9A44D8F81B6A5E32667B6F7
