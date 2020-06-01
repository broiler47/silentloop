//
// Created by palulukan on 5/11/20.
//

#ifndef INCOMINGMESSAGE_H_144BF24D2A6F4EB49D4BC70D3DEAC159
#define INCOMINGMESSAGE_H_144BF24D2A6F4EB49D4BC70D3DEAC159

#include "modules/net/Socket.h"

#include "modules/http/internal/HTTPRequestParser.h"

namespace http
{

class IncomingMessage : public stream::Readable
{
    //EXPORT_EVENT(aborted)

    public:
        static std::shared_ptr<IncomingMessage> CreateShared(const std::shared_ptr<net::Socket>& spSocket,
            const std::function<void(const std::shared_ptr<IncomingMessage>&, const std::shared_ptr<net::Socket>&)>& cbOnParsed);

    private:
        void _init(const std::shared_ptr<net::Socket>& spSocket,
                   const std::function<void(const std::shared_ptr<IncomingMessage>&, const std::shared_ptr<net::Socket>&)>& cbOnParsed);

    public:
        //bool isAborted(void);
        bool isComplete(void) const { return m_bCompleted; }
        //??? getHeaders(void);
        std::string getHttpVersion(void) const { return std::to_string(m_parser.m_nHttpMajor) + "." + std::to_string(m_parser.m_nHttpMinor); }
        int getHttpVersionMajor(void) const { return m_parser.m_nHttpMajor; }
        int getHttpVersionMinor(void) const { return m_parser.m_nHttpMinor; }
        std::string getMethod(void) const { return m_parser.m_strMethod; }
        const std::vector<std::string>& getRawHeaders(void) const { return m_parser.m_vecRawHeaders; }
        const std::vector<std::string>& getRawTrailers(void) const { return m_parser.m_vecRawTrailers; }
        //??? getTrailers(void);
        //setTimeout
        std::string getURL(void) const { return m_parser.m_strURL; }

    private:
        void _destroy(const Error* pErr, const std::function<void(const Error& err)>& cb) override;
        void _read(void) override;

    private:
        std::weak_ptr<net::Socket> m_wpSocket;
        std::function<void(const std::shared_ptr<IncomingMessage>&, const std::shared_ptr<net::Socket>&)> m_cbOnParsed;
        internal::HTTPRequestParser m_parser;
        bool m_bParserError = false;
        bool m_bCompleted = false;
};

}   // namespace http

#endif //INCOMINGMESSAGE_H_144BF24D2A6F4EB49D4BC70D3DEAC159
