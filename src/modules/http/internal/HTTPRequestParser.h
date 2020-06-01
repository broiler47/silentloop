//
// Created by palulukan on 5/16/20.
//

#ifndef HTTPREQUESTPARSER_H_7220583AEE374E71BDA72205F1990729
#define HTTPREQUESTPARSER_H_7220583AEE374E71BDA72205F1990729

#include "EventLoop.h"

namespace http::internal
{

#include "http_parser.h"

class HTTPRequestParser : public SyncEventEmitter
{
    EXPORT_EVENT(error, const char* name, const char* description)
    EXPORT_EVENT(headers_complete)
    EXPORT_EVENT(data, const char* at, size_t length)
    EXPORT_EVENT(end)

    public:
        HTTPRequestParser(void);

    public:
        void Feed(const std::vector<uint8_t>& vecData);

    private:
        static int _on_message_begin(http_parser* parser);
        static int _on_url(http_parser* parser, const char *at, size_t length);
        static int _on_header_field(http_parser* parser, const char *at, size_t length);
        static int _on_header_value(http_parser* parser, const char *at, size_t length);
        static int _on_headers_complete(http_parser* parser);
        static int _on_body(http_parser* parser, const char *at, size_t length);
        static int _on_message_complete(http_parser* parser);

    private:
        void _reset(void);
        void _processHdr(bool bVal, const char *at, size_t length);
        void _headersComplete(void);
        void _bodyChunk(const char *at, size_t length);
        void _bodyEnd(void);

    public:
        int m_nHttpMajor;
        int m_nHttpMinor;
        std::string m_strMethod;
        std::string m_strURL;
        std::vector<std::string> m_vecRawHeaders;
        std::vector<std::string> m_vecRawTrailers;

    private:
        http_parser m_httpParser;
        http_parser_settings m_httpParserSettings;
        std::string m_strCurHdr;
        bool m_bHdrVal = true;
        bool m_bTrailers = false;
};

}   // namespace http::internal

#endif //HTTPREQUESTPARSER_H_7220583AEE374E71BDA72205F1990729
