//
// Created by palulukan on 5/11/20.
//

#include "ServerResponse.h"

std::shared_ptr<http::ServerResponse> http::ServerResponse::CreateShared(const std::shared_ptr<net::Socket> &spSocket)
{
    struct make_shared_enabler : public ServerResponse {};

    auto sp = std::make_shared<make_shared_enabler>();
    sp->_init(spSocket);
    return sp;
}

void http::ServerResponse::_init(const std::shared_ptr<net::Socket> &spSocket)
{

}

void http::ServerResponse::_destroy(const Error *pErr, const std::function<void(const Error &err)> &cb)
{

}

void http::ServerResponse::_write(void)
{

}
