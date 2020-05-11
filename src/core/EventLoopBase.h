//
// Created by palulukan on 5/2/20.
//

#ifndef EVENTLOOPBASE_H_A91553AEC84043ADB582AD4E4F5D2700
#define EVENTLOOPBASE_H_A91553AEC84043ADB582AD4E4F5D2700

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

    public:
        typedef void* EventHandle;

    protected:
        virtual void _removeEvent(EventHandle hEvent) = 0;
        virtual void _notifyIOStateChange(EventHandle hEvent) = 0;
        virtual void _setTimeout(EventHandle hEvent, TimeInterval timeout) = 0;
        virtual void _cancelTimeout(EventHandle hEvent) = 0;

    protected:
        void _nextTick(const std::function<void(void)>& cb);
        void _drainNextTickQueue(void);

    private:
        std::queue<std::function<void(void)>> m_qNextTick;
};

#endif //EVENTLOOPBASE_H_A91553AEC84043ADB582AD4E4F5D2700
