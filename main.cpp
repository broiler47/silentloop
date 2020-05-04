#include <iostream>

#include "EventLoop.h"

#include "Events/Tineout.h"

static void _ev_main(EventLoopBase& eventLoop)
{
    std::cout << "<=== START ===>\n";

    SetTimeout(eventLoop, [](){
        std::cout << "Hello, World!\n";
    }, 3000);
}

int main(void)
{
    EventLoop eventLoop(&_ev_main);

    eventLoop.Run();

    return 0;
}
