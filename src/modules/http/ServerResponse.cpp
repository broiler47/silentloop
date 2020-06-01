//
// Created by palulukan on 5/11/20.
//

#include "ServerResponse.h"

#include "StatusCodes.h"

#include <cassert>

std::shared_ptr<http::ServerResponse> http::ServerResponse::CreateShared(const std::shared_ptr<net::Socket> &spSocket)
{
    struct make_shared_enabler : public ServerResponse {};

    auto sp = std::make_shared<make_shared_enabler>();
    sp->_init(spSocket);
    return sp;
}

void http::ServerResponse::_init(const std::shared_ptr<net::Socket> &spSocket)
{
    assert(spSocket);

    m_wpSocket = spSocket;

    LinkWith(spSocket);

    spSocket->on_drain([this](void) {
        auto spSocket = m_wpSocket.lock();
        assert(spSocket);

        if(_onDrained())
            spSocket->end();
    });
}

void http::ServerResponse::flushHeaders(void)
{
    if(isHeadersSent())
        return;

    if(m_state == _st_STATUS)
    {
        m_strHeadersCache = std::string("HTTP/1.1 ") + std::to_string(statusCode) + " " + _getStatusMessage() + "\r\n";
        m_state = _st_HEADERS;
    }

    // TODO: "Date" header
    // TODO: "Connection" header depending on http protocol version
    // TODO: "Transfer-encoding" header depending on http protocol version

    for(const auto& p : m_mapHeaders)
    {
        for(const auto &v : p.second)
            m_strHeadersCache += p.first + ": " + v + "\r\n";
    }
    m_mapHeaders.clear();
}

void http::ServerResponse::writeHead(int nStatusCode, const std::string &strStatusMessage, const std::map<std::string, std::string> &headers)
{
    statusCode = nStatusCode;
    statusMessage = strStatusMessage;

    for(const auto& p : headers)
        m_mapHeaders[p.first] = { p.second };
}

void http::ServerResponse::setHeader(const std::string& name, const std::vector<std::string>& value)
{
    if(isHeadersSent())
        throw Error("Cannot set headers after they are sent to the client", ERR_HTTP_HEADERS_SENT);

    // TODO: Validate header name and value

    m_mapHeaders[name] = value;
}

void http::ServerResponse::_destroy(const Error *pErr, const std::function<void(const Error &err)> &cb)
{
    UNUSED_ARG(cb);

    auto spSocket = m_wpSocket.lock();
    assert(spSocket);

    spSocket->destroy(*pErr);
}

void http::ServerResponse::_write(void)
{
    auto spSocket = m_wpSocket.lock();
    assert(spSocket);

    if(!isHeadersSent())
    {
        flushHeaders();

        m_strHeadersCache += "\r\n";
        m_state = _st_BODY;

        spSocket->write(m_strHeadersCache);
        m_strHeadersCache.clear();
    }

    spSocket->write(m_wrBuffer);
}

const std::string& http::ServerResponse::_getStatusMessage(void) const
{
    if(statusMessage.length())
        return statusMessage;

    auto it = STATUS_CODES.find(statusCode);
    if(it == STATUS_CODES.end())
        throw Error(std::string("Invalid status code: ") + std::to_string(statusCode), ERR_HTTP_INVALID_STATUS_CODE);

    return it->second;
}
