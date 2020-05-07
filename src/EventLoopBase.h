//
// Created by palulukan on 5/2/20.
//

#ifndef EVENTLOOPBASE_H_A91553AEC84043ADB582AD4E4F5D2700
#define EVENTLOOPBASE_H_A91553AEC84043ADB582AD4E4F5D2700

#include <memory>

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
        virtual void _notifyIOEventMaskUpdate(EventHandle hEvent, int fd, unsigned int mask) = 0;
        virtual void _setTimeout(EventHandle hEvent, TimeInterval timeout) = 0;
        virtual void _cancelTimeout(EventHandle hEvent) = 0;
};

#endif //EVENTLOOPBASE_H_A91553AEC84043ADB582AD4E4F5D2700
