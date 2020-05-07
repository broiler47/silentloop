//
// Created by palulukan on 5/5/20.
//

#ifndef IOMUXBASE_H_EA574BFDCB4C4379B1406359BD7B86B1
#define IOMUXBASE_H_EA574BFDCB4C4379B1406359BD7B86B1

#include "../EventLoopBase.h"

#include <vector>

class IOMuxBase
{
    public:
        virtual ~IOMuxBase(void) = default;

    public:
        enum IOEvents : unsigned int
        {
            IOEV_READ   = 1,
            IOEV_WRITE  = 2,
            IOEV_CLOSE  = 4,    // Always polled
            IOEV_ERROR  = 8     // Always polled
        };

    public:
        virtual bool Register(int fd, unsigned int mask, EventLoopBase::EventHandle hEvent) = 0;
        virtual void Unregister(int fd) = 0;
        virtual bool UpdateIOEventMask(int fd, unsigned int mask, EventLoopBase::EventHandle hEvent) = 0;
        virtual bool Poll(int timeout, std::vector<std::pair<EventLoopBase::EventHandle, unsigned int>>& res) = 0;
};

#endif //IOMUXBASE_H_EA574BFDCB4C4379B1406359BD7B86B1
