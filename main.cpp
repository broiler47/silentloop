#include "Log.h"
#include "EventLoop.h"

#include "Events/Tineout.h"
#include "Events/net/ListenerSocket.h"
#include "Events/net/Socket.h"

static void _ev_main(EventLoopBase& eventLoop)
{
    INFO("<=== START ===>");

//    SetTimeout(eventLoop, [](){
//        INFO("Hello, World!");
//    }, 3000);

    auto spListener = TCPListenerSocket::Create(eventLoop, "localhost", 7890);

    spListener->on_error([](const Error& err) {
        ERROR("Listener socket error: %s", err.Format());
        exit(1);
    });

    spListener->on_listening([](void) {
        INFO("Listening...");
    });

    spListener->on_close([](void) {
        INFO("Listener closed");
    });

    // Capturing std::shared_ptr of the event object into it's own
    // event handler callbacks is not a good idea since it will create
    // a circular dependency and as a result a potential event object
    // leaks. One possible workaround is to capture raw event object pointer.
    auto pListener = spListener.get();
    spListener->on_connection([pListener, &eventLoop](int fdClient) {
        DEBUG("Incoming connection: %d", fdClient);

        //pListener->Close();

        auto spSocket = Socket::Create(eventLoop, fdClient, false, true);
        spSocket->setNoDelay();

        spSocket->on_error([](const Error& err) {
            ERROR("TCP socket error: %s", err.Format());
        });

        auto pSocket = spSocket.get();
        spSocket->on_data([pSocket](const std::vector<uint8_t>& data) {
            pSocket->write("You've sent me: ");
            pSocket->write(data);
            pSocket->end("Goodbye!\n");
        });

        spSocket->on_drain([pSocket](void) {
            DEBUG("TCP socket write stream drained");
            //pSocket->end("Goodbye!\n");
        });

        spSocket->on_finish([](void) {
            DEBUG("TCP socket write stream finished");
        });

        spSocket->on_end([](void) {
            DEBUG("End of TCP socket read stream");
        });

        spSocket->on_close([](void) {
            DEBUG("TCP socket closed");
        });
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
