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

    spListener->on_error([](const Error& err) {
        ERROR("Listener socket error: %s", err.What());
    });

    spListener->on_listening([](void) {
        INFO("Listening...");
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
