//
// Created by palulukan on 5/2/20.
//

#ifndef EVENTBASE_H_FF5E96CED3BD4FD2AB60BD5D52D45D35
#define EVENTBASE_H_FF5E96CED3BD4FD2AB60BD5D52D45D35

#include "../EventLoopBase.h"

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

    protected:
        virtual int GetFD(void) const { return -1; }

    // For internal use of derivatives of Event object
    protected:
        EventLoopBase& EventLoop(void);
        void Attach(EventLoopBase& eventLoop);
        bool isAttached(void) const;
        void Detach(void);
        void SetTimeout(EventLoopBase::TimeInterval timeout);
        void CancelTimeout(void);

    private:
        void _onCreated(const std::weak_ptr<Event>& self);

    // Intended to use only by EventLoop
    protected:
        bool _onAttach(EventLoopBase* pEL, EventLoopBase::EventHandle handle);
        void _onDetached(void);

    private:
        std::weak_ptr<Event> m_self;
        EventLoopBase *m_pEL = nullptr;
        EventLoopBase::EventHandle m_handle = nullptr;
};

class IOEvent : public Event
{
    public:
        ~IOEvent(void) override;

    protected:
        int GetFD(void) const override { return  m_fd; };

    private:
        int m_fd = -1;
};

#endif //EVENTBASE_H_FF5E96CED3BD4FD2AB60BD5D52D45D35
