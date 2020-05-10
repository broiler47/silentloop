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

    protected:
        bool _listen(int fd, int backlog);

    private:
        void OnRead(void) override;
        void OnError(void) override;

    EXPORT_EVENT(listening)
    EXPORT_EVENT(connection, int fd)
};

class TCPListenerSocket : public ListenerSocket
{
    public:
        static std::shared_ptr<TCPListenerSocket> Create(EventLoopBase& eventLoop, const std::string& strHost, uint16_t nPort, bool bInet6 = false, int backlog = nDefaultBacklogValue);

    private:
        void _open(EventLoopBase& eventLoop, const std::string& strHost, uint16_t nPort, bool bInet6, int backlog);
};

#endif //LISTENERSOCKET_H_D7182DCC314849458AE80ED98E07159B
