//
// Created by palulukan on 5/2/20.
//

#ifndef EVENT_H_AAE9E5AD98E84D38BA94F2A4E34F9A4C
#define EVENT_H_AAE9E5AD98E84D38BA94F2A4E34F9A4C

#include "EventLoopBase.h"
#include "IOMux/IOMuxBase.h"

#include "Utils.h"

#include <functional>
#include <string>

#define EXPORT_EVENT(name, ...)                                                             \
    public:                                                                                 \
        void on_##name(const std::function<void(__VA_ARGS__)>& cb) { cb_on_##name = cb; }   \
    protected:                                                                              \
        std::function<void(__VA_ARGS__)> cb_on_##name;                                      \

#define EMIT_EVENT(name, ...)                       \
    do                                              \
    {                                               \
        if(cb_on_##name)                            \
            CATCH_ALL(cb_on_##name(__VA_ARGS__));   \
    } while(0)                                      \

class Event
{
    friend class EventLoop;

    public:
        template <typename TEvent>
        static std::shared_ptr<TEvent> CreateEvent(void)
        {
            auto sp = std::make_shared<TEvent>();

            auto spBase = std::static_pointer_cast<Event>(sp);
            spBase->_onCreated(spBase);

            return sp;
        }

    protected:
        Event(void) = default;

    public:
        virtual ~Event(void) = default;

    protected:
        virtual void OnTimeout(void) {}
        //virtual void OnInterval(void) {}
        //virtual void OnImmediate(void) {}
        //virtual void OnAdvance(void) {}
        virtual void OnRead(void) {}
        virtual void OnWrite(void) {}
        virtual void OnClose(void) {}
        virtual void OnError(void) {}

    // For internal use of derivatives of Event object or EventLoop
    protected:
        virtual int GetFD(void) const { return -1; }
        virtual unsigned int GetIOEventMask(void) const { return 0; }

    // For internal use of derivatives of Event object
    protected:
        //EventLoopBase& _eventLoop(void);
        void _attach(EventLoopBase& eventLoop);
        bool _isAttached(void) const;
        void _detach(void);
        void _setTimeout(EventLoopBase::TimeInterval timeout);
        void _cancelTimeout(void);
        void _notifyIOEventMaskUpdate(int fd, unsigned int mask);

    private:
        void _onCreated(const std::weak_ptr<Event>& self);

    // Intended to use only by EventLoop
    protected:
        void _onAttach(EventLoopBase* pEL, EventLoopBase::EventHandle handle);
        void _onDetached(void);

    private:
        std::weak_ptr<Event> m_self;
        EventLoopBase *m_pEL = nullptr;
        EventLoopBase::EventHandle m_handle = nullptr;
};

#endif //EVENT_H_AAE9E5AD98E84D38BA94F2A4E34F9A4C