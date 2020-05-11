//
// Created by palulukan on 5/10/20.
//

#ifndef SERVER_H_94B91C01AE0845958D9EDD1DF27FEC0C
#define SERVER_H_94B91C01AE0845958D9EDD1DF27FEC0C

#include "Socket.h"

namespace net
{

class Server : public IOEvent
{
    public:
        static std::shared_ptr<Server> Create(EventLoopBase& eventLoop, const std::string& strHost, uint16_t nPort, bool bInet6 = false, int backlog = nDefaultBacklogValue);

    public:
        static const int nDefaultBacklogValue = 32;

    private:
        void _open(EventLoopBase& eventLoop, const std::string& strHost, uint16_t nPort, bool bInet6, int backlog);
        bool _listen(int fd, int backlog);
        void _newClient(int fd);

    private:
        void OnRead(void) override;
        void OnError(void) override;

    EXPORT_EVENT(listening)
    EXPORT_EVENT(connection, const std::shared_ptr<Socket>& spSocket)
};

}   // namespace net

#endif //SERVER_H_94B91C01AE0845958D9EDD1DF27FEC0C
