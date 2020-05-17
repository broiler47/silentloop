#include "Log.h"

#include "EventLoop.h"

#include "modules/net/Server.h"
#include "modules/http/Server.h"

static unsigned int nReqests = 0;

static void _timersTest(void)
{
    INFO("~~~ Timers ~~~");

    SetTimeout([](){
        INFO("Hello, World!");
    }, 3000);

    SetInterval([](void) {
        INFO("Requests/sec: %u", nReqests);
        nReqests = 0;
    }, 1000);
}

static void _tcpServerTest(void)
{
    INFO("~~~ TCP ~~~");

    const int nPort = 7890;

    auto spServer = net::CreateServer();
    spServer->Listen(nPort);

    spServer->on_error([](const Error& err) {
        ERROR("TCP: Server error: %s", err.Format());
        exit(1);
    });

    spServer->on_listening([](void) {
        INFO("TCP: Listening on port %d", nPort);
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

static void _httpServerTest(void)
{
    INFO("~~~ HTTP ~~~");

    const int nPort = 8890;

    auto spServer = http::CreateServer();
    spServer->Listen(nPort);

    spServer->on_error([](const Error& err) {
        ERROR("HTTP: Server error: %s", err.Format());
        exit(1);
    });

    spServer->on_listening([](void) {
        INFO("HTTP: Listening on port %d", nPort);
    });

    spServer->on_close([](void) {
        INFO("HTTP: Server socket closed");
    });

    spServer->on_connection([](const std::shared_ptr<net::Socket>& spSocket) {
        UNUSED_ARG(spSocket);
        //DEBUG("HTTP: Incoming connection");
        ++nReqests;
    });

    spServer->on_clientError([](const Error& err, net::Socket* pSocket) {
        //DEBUG("HTTP: Client error: %s", err.Format());

        if(pSocket->isWritable())
            pSocket->end("HTTP/1.1 400 Bad Request\r\n\r\n");
        else
            pSocket->destroy();
    });

    spServer->on_request([](const std::shared_ptr<http::IncomingMessage>& request, const std::shared_ptr<http::ServerResponse>& response) {
        DEBUG("HTTP: Incoming request");

        UNUSED_ARG(request);
        UNUSED_ARG(response);
    });

//    const char *strTestReq = "GET / HTTP/1.1\r\n"
//                             "x: \r\n"
//                             "Content-Type: text/plain\r\n"
//                             "Content-Length: 6\r\n"
//                             "\r\n"
//                             "foobar";

//    const char *strTestReq = "GET / HTTP/1.1\r\n"
//                             "Content-Type: text/plain\r\n"
//                             "Transfer-Encoding: chunked\r\n"
//                             "\r\n"
//                             "7\r\n"
//                             "Mozilla\r\n"
//                             "9\r\n"
//                             "Developer\r\n"
//                             "7\r\n"
//                             "Network\r\n"
//                             "0\r\n"
//                             "\r\n";
//
//    http::internal::HTTPRequestParser parser;
//    parser.Feed({strTestReq, strTestReq + strlen(strTestReq)});
}

static void _ev_main(void)
{
    INFO("<=== START ===>");

    _timersTest();
    _tcpServerTest();
    _httpServerTest();
}

int main(void)
{
    Log::Instance().AddDestination(stdout);

    RunThreadEventLoop(&_ev_main);

    Log::Destroy();

    return 0;
}
