//
// Created by palulukan on 5/16/20.
//

#include "HTTPRequestParser.h"

#include "Log.h"

http::internal::HTTPRequestParser::HTTPRequestParser(void)
{
    http_parser_init(&m_httpParser, HTTP_REQUEST);
    m_httpParser.data = this;

    http_parser_settings_init(&m_httpParserSettings);
    m_httpParserSettings.on_message_begin = &_on_message_begin;
    m_httpParserSettings.on_url = &_on_url;
    m_httpParserSettings.on_header_field = &_on_header_field;
    m_httpParserSettings.on_header_value = &_on_header_value;
    m_httpParserSettings.on_headers_complete = &_on_headers_complete;
    m_httpParserSettings.on_body = &_on_body;
    m_httpParserSettings.on_message_complete = &_on_message_complete;
}

void http::internal::HTTPRequestParser::Feed(const std::vector<uint8_t> &vecData)
{
    if(http_parser_execute(&m_httpParser, &m_httpParserSettings, (const char *)vecData.data(), vecData.size()) != vecData.size())
    {
        auto err = HTTP_PARSER_ERRNO(&m_httpParser);
        if(err != HPE_OK)
            EMIT_EVENT(error, http_errno_name(err), http_errno_description(err));
    }

    if(m_httpParser.upgrade)
    {
        // TODO: Implement

        WARNING("HTTP Upgrade is not supported");
        EMIT_EVENT(error, "Not implemented", "Upgrade is not supported");
    }
}

int http::internal::HTTPRequestParser::_on_message_begin(http_parser *parser)
{
    auto pThis = (HTTPRequestParser *)parser->data;
    pThis->_reset();
    return 0;
}

int http::internal::HTTPRequestParser::_on_url(http_parser *parser, const char *at, size_t length)
{
    auto pThis = (HTTPRequestParser *)parser->data;
    pThis->m_strURL.append(at, length);
    return 0;
}

int http::internal::HTTPRequestParser::_on_header_field(http_parser *parser, const char *at, size_t length)
{
    auto pThis = (HTTPRequestParser *)parser->data;
    pThis->_processHdr(false, at, length);
    return 0;
}

int http::internal::HTTPRequestParser::_on_header_value(http_parser *parser, const char *at, size_t length)
{
    auto pThis = (HTTPRequestParser *)parser->data;
    pThis->_processHdr(true, at, length);
    return 0;
}

int http::internal::HTTPRequestParser::_on_headers_complete(http_parser *parser)
{
    auto pThis = (HTTPRequestParser *)parser->data;
    pThis->_processHdr(false, nullptr, 0);
    pThis->m_nHttpMajor = parser->http_major;
    pThis->m_nHttpMinor = parser->http_minor;
    pThis->m_strMethod = http_method_str((http_method)parser->method);
    pThis->_headersComplete();
    return 0;
}

int http::internal::HTTPRequestParser::_on_body(http_parser *parser, const char *at, size_t length)
{
    auto pThis = (HTTPRequestParser *)parser->data;
    pThis->_bodyChunk(at, length);
    return 0;
}

int http::internal::HTTPRequestParser::_on_message_complete(http_parser *parser)
{
    auto pThis = (HTTPRequestParser *)parser->data;
    pThis->_processHdr(false, nullptr, 0);
    pThis->_bodyEnd();
    return 0;
}

void http::internal::HTTPRequestParser::_reset(void)
{
    m_nHttpMajor = 0;
    m_nHttpMinor = 0;
    m_strMethod.clear();
    m_strURL.clear();
    m_vecRawHeaders.clear();
    m_vecRawTrailers.clear();
    m_strCurHdr.clear();
    m_bHdrVal = true;
    m_bTrailers = false;
}

void http::internal::HTTPRequestParser::_processHdr(bool bVal, const char *at, size_t length)
{
    if(m_bHdrVal != bVal)
    {
        auto &rContainer = m_bTrailers ? m_vecRawTrailers : m_vecRawHeaders;

        // Even though header value could be an empty string, header should always have a name
        if(!(rContainer.empty() && m_strCurHdr.empty()))
        {
            rContainer.push_back(m_strCurHdr);
            m_strCurHdr.clear();
        }

        m_bHdrVal = !m_bHdrVal;
    }

    // End condition - there will be no more headers
    if(at)
        m_strCurHdr.append(at, length);
    else
    {
        m_bHdrVal = true;
        m_bTrailers = true;
    }
}

void http::internal::HTTPRequestParser::_headersComplete(void)
{
    EMIT_EVENT(headers_complete);
}

void http::internal::HTTPRequestParser::_bodyChunk(const char *at, size_t length)
{
    EMIT_EVENT(data, at, length);
}

void http::internal::HTTPRequestParser::_bodyEnd(void)
{
    EMIT_EVENT(end);
}
