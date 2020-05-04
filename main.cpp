#include <iostream>

#include "EventLoop.h"

#include "Events/Tineout.h"

static void _ev_main(EventLoopBase& eventLoop)
{
    SetTimeout(eventLoop, [](){
        std::cout << "Hello, World!\n";
    }, 1000);
}

int main(void)
{
    EventLoop eventLoop(&_ev_main);

    eventLoop.Run();

    return 0;
}
