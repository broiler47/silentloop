//
// Created by palulukan on 5/12/20.
//

#include "EventLoopFactory.h"

thread_local std::shared_ptr<EventLoopBase> _tls_event_loop;

void RunThreadEventLoop(std::function<void(void)> cbEvMain, std::shared_ptr<IOMuxBase> spFDMux)
{
    if(_tls_event_loop)
        throw std::runtime_error("Only one event loop can be attached to each thread");

    auto spEL = std::make_shared<EventLoop>(std::move(cbEvMain), std::move(spFDMux));

    _tls_event_loop = spEL;

    spEL->Run();
}

std::shared_ptr<EventLoopBase> GetThreadEventLoop(void)
{
    if(!_tls_event_loop)
        throw std::runtime_error("No event loop associated with current thread");

    return _tls_event_loop;
}
