//
// Created by palulukan on 5/11/20.
//

#ifndef EVENTLOOP_H_174CD232F1B04A8B90D315AC558FC54D
#define EVENTLOOP_H_174CD232F1B04A8B90D315AC558FC54D

#include "core/EventLoop.h"

void RunThreadEventLoop(std::function<void(void)> cbEvMain = nullptr, std::shared_ptr<IOMuxBase> spFDMux = IOMux_epoll::Create());

#include "core/Event.h"
#include "core/IOEvent.h"

#include "core/Tineout.h"

#endif //EVENTLOOP_H_174CD232F1B04A8B90D315AC558FC54D
