//
// Created by palulukan on 5/2/20.
//

#ifndef EVENTLOOP_H_021BAF9BC98F45CD80DD6D0B2B2366D8
#define EVENTLOOP_H_021BAF9BC98F45CD80DD6D0B2B2366D8

#include "EventLoopBase.h"
#include "Event.h"
#include "io_mux/IOMux.h"

#include <functional>
#include <list>
#include <queue>
#include <vector>
#include <chrono>

class EventLoop : public EventLoopBase
{
    public:
        EventLoop(std::function<void(void)> cbEvMain, std::shared_ptr<IOMuxBase> spFDMux);
        ~EventLoop(void) override;

    public:
        bool Add(const std::shared_ptr<Event>& spEvent) override;

    public:
        void Run(void);

    protected:
        void _removeEvent(EventHandle hEvent) override;
        void _notifyIOStateChange(EventHandle hEvent) override;
        void _setTimeout(EventHandle hEvent, TimeInterval timeout) override;
        void _cancelTimeout(EventHandle hEvent) override;

    private:
        bool _tick(void);
        bool _isRegistered(EventHandle hEvent);
        void _processPendingRemovals(void);
        bool _updateIOFD(EventHandle hEvent);

    private:
        enum _eventFlags : unsigned int
        {
            EVF_TIMEOUT_SET = 0x01,
            EVF_DETACHED    = 0x02
        };

        struct _eventInfo
        {
            _eventInfo(const std::shared_ptr<Event>& sp, EventLoop *pEL) :
                spEvent(sp),
                itEvent(),
                pEventLoop(pEL),
                tpNextTimeout(),
                flags(0),
                fdRegistered(-1),
                nIOEventMask(0)
            {}

            std::shared_ptr<Event> spEvent;
            std::list<std::unique_ptr<_eventInfo>>::iterator itEvent;
            EventLoop *pEventLoop;
            std::chrono::time_point<std::chrono::steady_clock> tpNextTimeout;
            unsigned int flags;
            int fdRegistered;
            unsigned int nIOEventMask;
        };

        struct ELTimerQueueComparator
        {
            typedef std::priority_queue<EventHandle, std::vector<EventHandle>, ELTimerQueueComparator> TimerQueue;

            bool operator()(EventHandle a, EventHandle b) const
            {
                return ((_eventInfo *)a)->tpNextTimeout > ((_eventInfo *)b)->tpNextTimeout;
            }
        };

    private:
        std::function<void(void)> m_cbEvMain;
        std::list<std::unique_ptr<_eventInfo>> m_lstEvents;
        std::vector<EventHandle> m_vecRMPending;
        ELTimerQueueComparator::TimerQueue m_qTimeouts;
        std::shared_ptr<IOMuxBase> m_spIOMux;
};

#endif //EVENTLOOP_H_021BAF9BC98F45CD80DD6D0B2B2366D8
