//
// Created by palulukan on 5/3/20.
//

#ifndef TINEOUT_H_CCE767453DE64C5C8E8E6E324C0DC2A7
#define TINEOUT_H_CCE767453DE64C5C8E8E6E324C0DC2A7

#include "EventEmitter.h"
#include "Linkable.h"
#include "EventLoopBase.h"

class Timeout : public EventEmitter
{
    EXPORT_EVENT(timeout)

    public:
        static std::shared_ptr<Timeout> CreateShared(void);

    private:
        Timeout(void) = default;

    public:
        void Refresh(EventLoopBase::TimeInterval timeout);
        void Cancel(void);

    private:
        std::weak_ptr<Event> m_wpEvent;
};

std::shared_ptr<Timeout> SetTimeout(const std::function<void(void)>& cb, EventLoopBase::TimeInterval timeout);

#endif //TINEOUT_H_CCE767453DE64C5C8E8E6E324C0DC2A7
