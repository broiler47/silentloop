//
// Created by palulukan on 5/5/20.
//

#ifndef IOMUX_EPOLL_H_05204F13D3D941EEA73455463108BBFA
#define IOMUX_EPOLL_H_05204F13D3D941EEA73455463108BBFA

#include "IOMuxBase.h"

#include <memory>

#include <sys/epoll.h>

class IOMux_epoll : public IOMuxBase
{
    public:
        static std::shared_ptr<IOMuxBase> Create(unsigned int maxEvents = 32);

    public:
        explicit IOMux_epoll(unsigned int maxEvents);
        ~IOMux_epoll(void) override;

    public:
        bool Register(int fd, unsigned int mask, EventLoopBase::EventHandle hEvent) override;
        void Unregister(int fd) override;
        bool UpdateIOEventMask(int fd, unsigned int mask, EventLoopBase::EventHandle hEvent) override;
        bool Poll(int timeout, std::vector<std::pair<EventLoopBase::EventHandle, unsigned int>>& res) override;

    private:
        bool _epollCtl(int op, int fd, unsigned int mask, EventLoopBase::EventHandle hEvent);

    private:
        std::vector<epoll_event> m_vecReadyEvents;
        int m_fdEpoll;
};

#endif //IOMUX_EPOLL_H_05204F13D3D941EEA73455463108BBFA
