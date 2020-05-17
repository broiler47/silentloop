//
// Created by palulukan on 5/2/20.
//

#ifndef EVENTLOOPBASE_H_A91553AEC84043ADB582AD4E4F5D2700
#define EVENTLOOPBASE_H_A91553AEC84043ADB582AD4E4F5D2700

#include "Linkable.h"

#include <memory>
#include <functional>
#include <queue>

class Event;

class EventLoopBase
{
    friend class Event;

    public:
        typedef unsigned int TimeInterval;

    public:
        virtual ~EventLoopBase(void) = default;

    public:
        virtual bool Add(const std::shared_ptr<Event>& spEvent) = 0;
        void NextTick(const std::function<void(void)>& cb, const std::shared_ptr<Linkable>& spOwner = nullptr);

    public:
        typedef void* EventHandle;

    protected:
        virtual void _removeEvent(EventHandle hEvent) = 0;
        virtual void _notifyIOStateChange(EventHandle hEvent) = 0;
        virtual void _setTimer(EventHandle hEvent, TimeInterval timeout, bool bPeriodic) = 0;
        virtual void _cancelTimer(EventHandle hEvent) = 0;

    protected:
        void _drainNextTickQueue(void);

    private:
        std::queue<std::pair<std::function<void(void)>, std::shared_ptr<Linkable>>> m_qNextTick;
};

#endif //EVENTLOOPBASE_H_A91553AEC84043ADB582AD4E4F5D2700
