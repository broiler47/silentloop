#include "Log.h"
#include "EventLoop.h"

#include "Events/Tineout.h"
#include "Events/ListenerSocket.h"

static void _ev_main(EventLoopBase& eventLoop)
{
    INFO("<=== START ===>");

    SetTimeout(eventLoop, [](){
        INFO("Hello, World!");
    }, 3000);

    auto spListener = TCPListenerSocket::Create(eventLoop, "localhost", 7890);
    if(!spListener)
    {
        ERROR("Failed to open listener socket");
        exit(1);
    }

    spListener->on_error([](const std::string& what) {
        ERROR("Listener socket error: %s", what.c_str());
    });

    spListener->on_connection([](int fdClient) {
        DEBUG("Incoming connection: %d", fdClient);
    });
}

int main(void)
{
    Log::Instance().AddDestination(stdout);

    EventLoop eventLoop(&_ev_main);

    eventLoop.Run();

    Log::Destroy();

    return 0;
}
