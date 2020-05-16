//
// Created by palulukan on 5/16/20.
//

#ifndef EVENTLOOPFACTORY_H_B0268C68D9AF44BAB4D95D5D72624371
#define EVENTLOOPFACTORY_H_B0268C68D9AF44BAB4D95D5D72624371

#include "EventLoop.h"

void RunThreadEventLoop(std::function<void(void)> cbEvMain = nullptr, std::shared_ptr<IOMuxBase> spFDMux = std::make_shared<IOMux_epoll>(32));
std::shared_ptr<EventLoopBase> GetThreadEventLoop(void);

#endif //EVENTLOOPFACTORY_H_B0268C68D9AF44BAB4D95D5D72624371
