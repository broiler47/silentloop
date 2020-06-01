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
    spServer->on_connection([pServer = spServer.get()](const std::shared_ptr<net::Socket>& spSocket) {
        DEBUG("TCP: Incoming connection");

        pServer->Close();

        spSocket->setNoDelay();

        spSocket->on_error([](const Error& err) {
            ERROR("TCP socket error: %s", err.Format());
        });

        spSocket->on_data([pSocket = spSocket.get()](const std::vector<uint8_t>& data) {
            DEBUG("TCP: Client: %s", std::string(data.begin(), data.end()).c_str());

            pSocket->write("You've sent me: ");
            pSocket->write(data);
            pSocket->end("Goodbye!\n");
        });

        spSocket->on_drain([pSocket = spSocket.get()](void) {
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
        DEBUG("HTTP: Incoming request: HTTP %s", request->getHttpVersion().c_str());

        struct _serverContext
        {
            // It is also safe to save std::shared_ptr of response object here.
            //std::shared_ptr<http::ServerResponse> response;
            std::string body;
        };
        auto spContext = std::make_shared<_serverContext>();
        //spContext->response = response;

        request->on_error([](const Error& err) {
            ERROR("HTTP IncomingMessage error: %s", err.Format());
        });

        request->on_close([](void) {
            DEBUG("HTTP IncomingMessage closed");
        });

        request->on_data([spContext](const std::vector<uint8_t>& vecData) {
            DEBUG("HTTP IncomingMessage data");
            spContext->body.append(vecData.data(), vecData.data() + vecData.size());
        });

        request->on_end([spContext, response](void) {
            DEBUG("HTTP IncomingMessage end");

            response->setHeader("Hello", "World");

            response->write("Request body: ");
            response->end(spContext->body);
        });

        response->on_error([](const Error& err) {
            DEBUG("HTTP ServerResponse error: %s", err.Format());
        });

        response->on_close([](void) {
            DEBUG("HTTP ServerResponse closed");
        });

        response->on_drain([](void) {
            DEBUG("HTTP ServerResponse drain");
        });

        response->on_finish([](void) {
            DEBUG("HTTP ServerResponse finish");
        });
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
//                             "Test: Trailer\r\n"
//                             "\r\n";
//
//    http::internal::HTTPRequestParser parser;
//    parser.Feed({strTestReq, strTestReq + strlen(strTestReq)});
}

static void _ev_main(void)
{
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
