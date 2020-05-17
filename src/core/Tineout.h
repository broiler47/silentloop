//
// Created by palulukan on 5/3/20.
//

#ifndef TINEOUT_H_CCE767453DE64C5C8E8E6E324C0DC2A7
#define TINEOUT_H_CCE767453DE64C5C8E8E6E324C0DC2A7

#include "Event.h"

class Timeout
{
    public:
        static std::shared_ptr<Timeout> CreateShared(void);

    public:
        EXPORT_EVENT_SIGNATURE(timeout) { m_spEventObj->on_timeout(cb); }

    private:
        Timeout(void);

    public:
        void Refresh(EventLoopBase::TimeInterval timeout);
        void Cancel(void);

    private:
        std::shared_ptr<Event> m_spEventObj;
};

std::shared_ptr<Timeout> SetTimeout(const std::function<void(void)>& cb, EventLoopBase::TimeInterval timeout);

#endif //TINEOUT_H_CCE767453DE64C5C8E8E6E324C0DC2A7
