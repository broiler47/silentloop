//
// Created by palulukan on 5/11/20.
//

#include "Server.h"

std::shared_ptr<http::Server> http::Server::CreateShared(void)
{
    struct make_shared_enabler : public Server {};

    return std::make_shared<make_shared_enabler>();
}

http::Server::Server(void) :
    net::Server(false, false)
{
    on_connection([this](const std::shared_ptr<net::Socket>& spSocket) {
        auto pSocket = spSocket.get();
        auto cbClientError = [this, pSocket](const Error& err) {
            EMIT_EVENT(clientError, err, pSocket);
        };

        spSocket->on_error(cbClientError);

        auto spRequest = IncomingMessage::CreateShared(spSocket, [this](const std::shared_ptr<IncomingMessage> &spRequest, const std::shared_ptr<net::Socket> &spSocket) {
            auto spResponse = ServerResponse::CreateShared(spSocket);
            EMIT_EVENT(request, spRequest, spResponse);
        });

        spRequest->on_error(cbClientError);
    });
}

std::shared_ptr<http::Server> http::CreateServer(void)
{
    return Server::CreateShared();
}
