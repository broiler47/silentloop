//
// Created by palulukan on 5/16/20.
//

#include "EventEmitter.h"

#include "EventLoopFactory.h"

void EventEmitter::_nextTick(const std::function<void(void)> &cb)
{
    GetThreadEventLoop()->NextTick(cb);
}
