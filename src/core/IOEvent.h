//
// Created by palulukan on 5/7/20.
//

#ifndef IOEVENT_H_C988CB20B45A48A0881D10633315187D
#define IOEVENT_H_C988CB20B45A48A0881D10633315187D

#include "Event.h"
#include "EventEmitter.h"
#include "io_mux/IOMuxBase.h"

class IOEvent : public Event
{
    EXPORT_EVENT(close)
    EXPORT_EVENT(read)
    EXPORT_EVENT(write)
    EXPORT_EVENT(HUP)

    public:
        ~IOEvent(void) override;

    public:
        using IOEvents = IOMuxBase::IOEvents;

    public:
        void Close(void);
        int GetFD(void) const override { return m_fd; }
        unsigned int GetIOEventMask(void) const override { return m_IOEventMask; }
        void SetFD(int fd, unsigned int mask = 0);
        void SetIOEventMask(unsigned int mask);
        void SetIOEventFlag(unsigned int mask) { SetIOEventMask(m_IOEventMask | mask); }
        void ClearIOEventFlag(unsigned int mask) { SetIOEventMask(m_IOEventMask & ~mask); }
        bool EnableNonBlockingIO(bool enable);

    protected:
        void OnRead(void) override { Event::OnRead(); EMIT_EVENT(read); }
        void OnWrite(void) override { Event::OnWrite(); EMIT_EVENT(write); }
        void OnHUP(void) override { Event::OnHUP(); EMIT_EVENT(HUP); }

    private:
        void _close(void);

    private:
        int m_fd = -1;
        unsigned int m_IOEventMask = 0;
};

#endif //IOEVENT_H_C988CB20B45A48A0881D10633315187D
