//
// Created by palulukan on 5/16/20.
//

#include "Process.h"

#include "EventLoop.h"

void process::NextTick(const std::function<void(void)> &cb)
{
    GetThreadEventLoop()->NextTick(cb);
}
