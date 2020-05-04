//
// Created by palulukan on 5/2/20.
//

#include "EventLoop.h"

EventLoop::EventLoop(const std::function<void(EventLoopBase&)> &cbInit)
{

}

bool EventLoop::Add(const std::shared_ptr<Event> &spEvent)
{
    return false;
}

void EventLoop::Run(void)
{

}

void EventLoop::_removeEvent(EventLoopBase::EventHandle hEvent)
{

}

void EventLoop::_setTimeout(EventLoopBase::EventHandle hEvent, EventLoopBase::TimeInterval timeout)
{

}

void EventLoop::_cancelTimeout(EventLoopBase::EventHandle hEvent)
{

}
