//
// Created by palulukan on 5/3/20.
//

#ifndef TINEOUT_H_CCE767453DE64C5C8E8E6E324C0DC2A7
#define TINEOUT_H_CCE767453DE64C5C8E8E6E324C0DC2A7

#include "EventBase.h"
#include "../EventLoopBase.h"

#include <functional>

class TimeoutEvent : public Event
{
    public:
        void Arm(EventLoopBase& eventLoop, const std::function<void(void)>& cb, EventLoopBase::TimeInterval timeout);
        void Cancel(void);

    private:
        void OnTimeout(void) override;

    private:
        std::function<void(void)> m_cb;
};

std::shared_ptr<TimeoutEvent> SetTimeout(EventLoopBase& eventLoop, const std::function<void(void)>& cb, EventLoopBase::TimeInterval timeout);

#endif //TINEOUT_H_CCE767453DE64C5C8E8E6E324C0DC2A7
