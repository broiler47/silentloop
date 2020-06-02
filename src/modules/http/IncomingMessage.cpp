//
// Created by palulukan on 5/11/20.
//

#include "IncomingMessage.h"

#include <cassert>

std::shared_ptr<http::IncomingMessage> http::IncomingMessage::CreateShared(const std::shared_ptr<net::Socket> &spSocket,
    const std::function<void(const std::shared_ptr<IncomingMessage>&, const std::shared_ptr<net::Socket>&)>& cbOnParsed)
{
    struct make_shared_enabler : public IncomingMessage {};

    auto sp = std::make_shared<make_shared_enabler>();
    sp->_init(spSocket, cbOnParsed);
    return sp;
}

void http::IncomingMessage::_init(const std::shared_ptr<net::Socket> &spSocket,
                                  const std::function<void(const std::shared_ptr<IncomingMessage>&, const std::shared_ptr<net::Socket>&)>& cbOnParsed)
{
    assert(spSocket);
    assert(cbOnParsed);

    m_cbOnParsed = cbOnParsed;
    m_wpSocket = spSocket;

    LinkWith(spSocket);

    resume();

    spSocket->on_data([this](const std::vector<uint8_t>& vecData) {
        if(!m_bParserError)
            m_parser.Feed(vecData);
    });

    spSocket->on_end([this](void) {
        if(!m_bParserError)
            m_parser.Feed({});
    });

    m_parser.on_error([this](const char* name, const char* description) {
        UNUSED_ARG(description);

        m_bParserError = true;

        auto spSocket = m_wpSocket.lock();
        assert(spSocket);

        pause();
        spSocket->pause();

        _onReadError(Error(name, HTTP_PARSER_ERROR));
    });

    m_parser.on_headers_complete([this](void) {
        if(m_cbOnParsed)
        {
            auto spThis = std::dynamic_pointer_cast<IncomingMessage>(GetSharedPtr());

            auto spSocket = m_wpSocket.lock();
            assert(spSocket);

            m_cbOnParsed(spThis, spSocket);
        }
    });

    m_parser.on_data([this](const char* at, size_t length) {
        if(!_push(at, length))
        {
            auto spSocket = m_wpSocket.lock();
            assert(spSocket);

            spSocket->pause();
        }
    });

    m_parser.on_end([this](void) {
        m_bCompleted = true;
        _push(nullptr, 0);
    });
}

void http::IncomingMessage::_destroy(const Error *pErr, const std::function<void(const Error &err)> &cb)
{
    UNUSED_ARG(cb);

    auto spSocket = m_wpSocket.lock();
    assert(spSocket);

    spSocket->destroy(pErr);
}

void http::IncomingMessage::_read(void)
{
    auto spSocket = m_wpSocket.lock();
    assert(spSocket);

    spSocket->resume();
}
