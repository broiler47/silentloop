//
// Created by palulukan on 5/11/20.
//

#ifndef SERVER_H_E6970359168147B0BA31D962F7676524
#define SERVER_H_E6970359168147B0BA31D962F7676524

#include "modules/net/Server.h"

#include "IncomingMessage.h"
#include "ServerResponse.h"

namespace http
{

class Server : public net::Server
{
    //EXPORT_EVENT(checkContinue)
    //EXPORT_EVENT(checkExpectation)
    EXPORT_EVENT(clientError, const Error& err, net::Socket* pSocket)   // TODO: Default handler: 400 - Bad request
    //EXPORT_EVENT(connect)
    EXPORT_EVENT(request, const std::shared_ptr<IncomingMessage>& request, const std::shared_ptr<ServerResponse>& response)
    //EXPORT_EVENT(upgrade)

    public:
        static std::shared_ptr<Server> CreateShared(void);

    protected:
        Server(void);

    public:
        //void SetHeadersTimeout(unsigned int ms);
        //void SetMaxHeadersCount(unsigned int n);
        //void SetkeepAliveTimeout(unsigned int ms);
};

std::shared_ptr<Server> CreateServer(void);

}   // namespace http

#endif //SERVER_H_E6970359168147B0BA31D962F7676524
