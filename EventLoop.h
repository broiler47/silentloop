//
// Created by palulukan on 5/2/20.
//

#ifndef EVENTLOOP_H_021BAF9BC98F45CD80DD6D0B2B2366D8
#define EVENTLOOP_H_021BAF9BC98F45CD80DD6D0B2B2366D8

#include "EventLoopBase.h"
#include "Events/EventBase.h"

#include <functional>

class EventLoop : public EventLoopBase
{
    public:
        explicit EventLoop(const std::function<void(EventLoopBase&)>& cbInit = nullptr);

    public:
        bool Add(const std::shared_ptr<Event>& spEvent) override;

    public:
        void Run(void);

    protected:
        void _removeEvent(EventHandle hEvent) override;
        void _setTimeout(EventHandle hEvent, TimeInterval timeout) override;
        void _cancelTimeout(EventHandle hEvent) override;
};

#endif //EVENTLOOP_H_021BAF9BC98F45CD80DD6D0B2B2366D8
