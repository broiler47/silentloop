//
// Created by palulukan on 5/2/20.
//

#ifndef EVENT_H_AAE9E5AD98E84D38BA94F2A4E34F9A4C
#define EVENT_H_AAE9E5AD98E84D38BA94F2A4E34F9A4C

#include "EventLoopBase.h"
#include "Error.h"
#include "Utils.h"
#include "EventEmitter.h"

class Event : public EventEmitter
{
    friend class EventLoop;

    EXPORT_EVENT(error, const Error& err)

    public:
        template <typename TEvent, typename ...Args>
        static std::shared_ptr<TEvent> CreateEvent(Args&&... args)
        {
            auto sp = std::make_shared<TEvent>(std::forward<Args>(args)...);

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
        virtual void OnRead(void) {}
        virtual void OnWrite(void) {}
        virtual void OnHUP(void) {}
        virtual void OnError(void) {}

    // For internal use of derivatives of Event object or EventLoop
    protected:
        virtual int GetFD(void) const { return -1; }
        virtual unsigned int GetIOEventMask(void) const { return 0; }

    // For internal use of derivatives of Event object
    protected:
        void _attach(void);
        bool _isAttached(void) const;
        void _detach(void);
        void _setTimeout(EventLoopBase::TimeInterval timeout);
        void _cancelTimeout(void);
        void _notifyIOStateChange(void);
        void _nextTick(const std::function<void(void)>& cb) override;

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
