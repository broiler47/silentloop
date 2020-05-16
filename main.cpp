#include "Log.h"

#include "EventLoop.h"

#include "modules/net/Server.h"

static void _timersTest(void)
{
    SetTimeout([](){
        INFO("Hello, World!");
    }, 3000);

}

static void _tcpServerTest(void)
{
    auto spServer = net::CreateServer();
    spServer->Listen(7890);

    spServer->on_error([](const Error& err) {
        ERROR("TCP: Server error: %s", err.Format());
        exit(1);
    });

    spServer->on_listening([](void) {
        INFO("TCP: Listening...");
    });

    spServer->on_close([](void) {
        INFO("TCP: Server socket closed");
    });

    // Capturing std::shared_ptr of the event object into it's own
    // event handler callbacks is not a good idea since it will create
    // a circular dependency and as a result a potential event object
    // leaks. One possible workaround is to capture raw event object pointer.
    auto pServer = spServer.get();
    spServer->on_connection([pServer](const std::shared_ptr<net::Socket>& spSocket) {
        DEBUG("TCP: Incoming connection");

        pServer->Close();

        spSocket->setNoDelay();

        spSocket->on_error([](const Error& err) {
            ERROR("TCP socket error: %s", err.Format());
        });

        auto pSocket = spSocket.get();
        spSocket->on_data([pSocket](const std::vector<uint8_t>& data) {
            DEBUG("TCP: Client: %s", std::string(data.begin(), data.end()).c_str());

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

static void _ev_main(void)
{
    INFO("<=== START ===>");

    _timersTest();
    _tcpServerTest();
}

int main(void)
{
    Log::Instance().AddDestination(stdout);

    RunThreadEventLoop(&_ev_main);

    Log::Destroy();

    return 0;
}
