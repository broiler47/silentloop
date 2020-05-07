//
// Created by palulukan on 5/6/20.
//

#ifndef LISTENERSOCKET_H_D7182DCC314849458AE80ED98E07159B
#define LISTENERSOCKET_H_D7182DCC314849458AE80ED98E07159B

#include "Events/core/IOEvent.h"

class ListenerSocket : public IOEvent
{
    public:
        static const int nDefaultBacklogValue = 32;

    public:
        void Detach(void);

    protected:
        bool _listen(EventLoopBase& eventLoop, int backlog);

    private:
        void OnRead(void) override;
        void OnError(void) override;

    EXPORT_EVENT(connection, int fd)
    EXPORT_EVENT(error, const std::string& what)
};

class TCPListenerSocket : public ListenerSocket
{
    public:
        static std::shared_ptr<TCPListenerSocket> Create(EventLoopBase& eventLoop, const std::string& strHost, uint16_t nPort, bool bInet6 = false, int backlog = nDefaultBacklogValue);

    public:
        bool Open(EventLoopBase& eventLoop, const std::string& strHost, uint16_t nPort, bool bInet6, int backlog = nDefaultBacklogValue);
};

#endif //LISTENERSOCKET_H_D7182DCC314849458AE80ED98E07159B
