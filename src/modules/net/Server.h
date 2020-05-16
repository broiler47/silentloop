//
// Created by palulukan on 5/10/20.
//

#ifndef SERVER_H_94B91C01AE0845958D9EDD1DF27FEC0C
#define SERVER_H_94B91C01AE0845958D9EDD1DF27FEC0C

#include "Socket.h"

namespace net
{

class Server : public EventEmitter, public Linkable
{
    EXPORT_EVENT(error, const Error& err)
    EXPORT_EVENT(close)
    EXPORT_EVENT(listening)
    EXPORT_EVENT(connection, const std::shared_ptr<Socket>& spSocket)

    public:
        static std::shared_ptr<Server> CreateShared(bool allowHalfOpen, bool pauseOnConnect);

    protected:
        Server(bool allowHalfOpen, bool pauseOnConnect);

    public:
        static const int nDefaultBacklogValue = 32;

    public:
        void Listen(uint16_t nPort, const std::string &strHost = "0.0.0.0", int backlog = nDefaultBacklogValue, bool bInet6 = false);
        void Close(void);
        //void setTimeout(unsigned int ms);

    private:
        void _openTCPSocket(const std::shared_ptr<IOEvent>& spListenerEvent, uint16_t nPort, const std::string& strHost, int backlog, bool bInet6);
        bool _listen(const std::shared_ptr<IOEvent>& spListenerEvent, int fd, int backlog);

    private:
        std::weak_ptr<IOEvent> m_wpListenerEvent;
        bool m_bAllowHalfOpen;
        bool m_bPauseOnConnect;
};

std::shared_ptr<Server> CreateServer(bool allowHalfOpen = false, bool pauseOnConnect = false);

}   // namespace net

#endif //SERVER_H_94B91C01AE0845958D9EDD1DF27FEC0C
