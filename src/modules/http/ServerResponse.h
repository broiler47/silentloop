//
// Created by palulukan on 5/11/20.
//

#ifndef SERVERRESPONSE_H_A4D95CC3464B4912ACD687D2C5D60558
#define SERVERRESPONSE_H_A4D95CC3464B4912ACD687D2C5D60558

#include "modules/net/Socket.h"

#include <map>

namespace http
{

class ServerResponse : public stream::Writable
{
    public:
        static std::shared_ptr<ServerResponse> CreateShared(const std::shared_ptr<net::Socket>& spSocket);

    private:
        ServerResponse(void) = default;

    private:
        // TODO: Take request type into consideration while generating response
        void _init(const std::shared_ptr<net::Socket>& spSocket);

    public:
        //void writeContinue(void);
        //void writeProcessing(void);
        //void addTrailers(const std::map<std::string, std::string>& headers);
        void flushHeaders(void);
        //std::string getHeader(const std::string& name);
        //std::vector<std::string> getHeaderNames(void);
        //std::map<std::string, std::string>& getHeaders(void);
        //bool hasHeader(const std::string& name);
        bool isHeadersSent(void) const { return m_state == _st_BODY; }
        //void removeHeader(const std::string& name);
        void writeHead(int nStatusCode) { writeHead(nStatusCode, {}); }
        void writeHead(int nStatusCode, const std::map<std::string, std::string>& headers) { writeHead(nStatusCode, "", headers); }
        void writeHead(int nStatusCode, const std::string& strStatusMessage, const std::map<std::string, std::string>& headers);
        void setHeader(const std::string& name, const std::string& value) { setHeader(name, std::vector<std::string>{ value }); }
        void setHeader(const std::string& name, const std::vector<std::string>& value);

    public:
        int statusCode = 200;
        std::string statusMessage;
        //bool sendDate = true;

    private:
        void _destroy(const Error* pErr, const std::function<void(const Error& err)>& cb) override;
        void _write(void) override;

    private:
        const std::string& _getStatusMessage(void) const;

    private:
        enum _state
        {
            _st_STATUS,
            _st_HEADERS,
            _st_BODY
        };

    private:
        std::weak_ptr<net::Socket> m_wpSocket;
        _state m_state = _st_STATUS;
        std::map<std::string, std::vector<std::string>> m_mapHeaders;
        std::string m_strHeadersCache;
};

}   // namespace http

#endif //SERVERRESPONSE_H_A4D95CC3464B4912ACD687D2C5D60558
