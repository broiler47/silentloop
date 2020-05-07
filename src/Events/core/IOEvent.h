//
// Created by palulukan on 5/7/20.
//

#ifndef IOEVENT_H_C988CB20B45A48A0881D10633315187D
#define IOEVENT_H_C988CB20B45A48A0881D10633315187D

#include "Event.h"

class IOEvent : public Event
{
    public:
        ~IOEvent(void) override;

    public:
        using IOEvents = IOMuxBase::IOEvents;

    public:
        virtual void Close(void);

    protected:
        int GetFD(void) const override { return m_fd; }
        unsigned int GetIOEventMask(void) const override { return m_IOEventMask; }
        void SetFD(int fd);
        void SetIOEventMask(unsigned int mask);
        void SetIOEventFlag(unsigned int mask) { SetIOEventMask(m_IOEventMask | mask); }
        void ClearIOEventFlag(unsigned int mask) { SetIOEventMask(m_IOEventMask & ~mask); }
        bool EnableNonBlockingFD(bool enable);

    private:
        void _close(void);

    private:
        int m_fd = -1;
        unsigned int m_IOEventMask = 0;
};

#endif //IOEVENT_H_C988CB20B45A48A0881D10633315187D
