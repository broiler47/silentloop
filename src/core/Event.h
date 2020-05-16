//
// Created by palulukan on 5/2/20.
//

#ifndef EVENT_H_AAE9E5AD98E84D38BA94F2A4E34F9A4C
#define EVENT_H_AAE9E5AD98E84D38BA94F2A4E34F9A4C

#include "EventLoopBase.h"
#include "Error.h"
#include "Utils.h"
#include "EventEmitter.h"
#include "Linkable.h"

class Event : public Linkable, public EventEmitter
{
    friend class EventLoop;

    EXPORT_EVENT(timeout)
    EXPORT_EVENT(error, const Error& err)

    public:
        static std::shared_ptr<Event> CreateShared(void);

    protected:
        Event(void);

    public:
        ~Event(void) override;

    public:
        void Attach(void);
        void Detach(void);
        void SetTimeout(EventLoopBase::TimeInterval timeout);
        void CancelTimeout(void);
        virtual int GetFD(void) const { return -1; }
        virtual unsigned int GetIOEventMask(void) const { return 0; }

    protected:
        virtual void OnTimeout(void) { EMIT_EVENT(timeout); }
        //virtual void OnInterval(void) {}
        //virtual void OnImmediate(void) {}
        virtual void OnRead(void) {}
        virtual void OnWrite(void) {}
        virtual void OnHUP(void) {}
        virtual void OnError(void) { EMIT_EVENT(error, Error("Error on file descriptor")); }

    // For internal use of derivatives of Event object
    protected:
        bool _isAttached(void) const;
        void _notifyIOStateChange(void);

    // Intended to use only by EventLoop
    protected:
        void _onAttach(EventLoopBase* pEL, EventLoopBase::EventHandle handle);
        void _onDetached(void);

    private:
        EventLoopBase *m_pEL = nullptr;
        EventLoopBase::EventHandle m_handle = nullptr;
};

#endif //EVENT_H_AAE9E5AD98E84D38BA94F2A4E34F9A4C
