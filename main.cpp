#include "Log.h"
#include "EventLoop.h"

#include "Events/Tineout.h"

static void _ev_main(EventLoopBase& eventLoop)
{
    INFO("<=== START ===>");

    SetTimeout(eventLoop, [](){
        INFO("Hello, World!");
    }, 3000);
}

int main(void)
{
    Log::Instance().AddDestination(stdout);

    EventLoop eventLoop(&_ev_main);

    eventLoop.Run();

    Log::Destroy();

    return 0;
}
