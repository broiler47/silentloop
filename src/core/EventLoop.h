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
        void _setTimer(EventHandle hEvent, TimeInterval timeout, bool bPeriodic) override;
        void _cancelTimer(EventHandle hEvent) override;

    private:
        bool _tick(void);
        bool _isRegistered(EventHandle hEvent);
        void _processPendingRemovals(void);
        bool _updateIOFD(EventHandle hEvent);
        void _removeFromTimerQueue(EventHandle hEvent);
        void _clearTimerFlags(EventHandle hEvent);
        void _updateTimePoint(void);

    private:
        enum _eventFlags : unsigned int
        {
            EVF_TIMER_SET       = 0x01,
            EVF_TIMER_PERIODIC  = 0x02,
            EVF_DETACHED        = 0x04
        };

        struct _eventInfo
        {
            _eventInfo(const std::shared_ptr<Event>& sp, EventLoop *pEL) :
                spEvent(sp),
                itEvent(),
                pEventLoop(pEL),
                tpTimeout(),
                tpNextTimeout(),
                flags(0),
                fdRegistered(-1),
                nIOEventMask(0)
            {}

            std::shared_ptr<Event> spEvent;
            std::list<std::unique_ptr<_eventInfo>>::iterator itEvent;
            EventLoop *pEventLoop;
            std::chrono::milliseconds tpTimeout;
            std::chrono::time_point<std::chrono::steady_clock> tpNextTimeout;
            unsigned int flags;
            int fdRegistered;
            unsigned int nIOEventMask;
        };

        template<typename T, typename TComp>
        struct ELTimerQueue : public std::priority_queue<T, std::vector<T>, TComp>
        {
            bool Remove(const T& value)
            {
                auto it = std::find(this->c.begin(), this->c.end(), value);
                if(it != this->c.end())
                {
                    this->c.erase(it);
                    std::make_heap(this->c.begin(), this->c.end(), this->comp);

                    return true;
                }

                return false;
            }
        };

        struct ELTimerQueueComparator
        {
            typedef ELTimerQueue<EventHandle, ELTimerQueueComparator> TimerQueue;

            bool operator()(EventHandle a, EventHandle b) const
            {
                return ((_eventInfo *)a)->tpNextTimeout > ((_eventInfo *)b)->tpNextTimeout;
            }
        };

    private:
        std::function<void(void)> m_cbEvMain;
        std::list<std::unique_ptr<_eventInfo>> m_lstEvents;
        std::vector<EventHandle> m_vecRMPending;
        ELTimerQueueComparator::TimerQueue m_qTimers;
        std::shared_ptr<IOMuxBase> m_spIOMux;
        std::chrono::steady_clock::time_point m_tNow;
};

#endif //EVENTLOOP_H_021BAF9BC98F45CD80DD6D0B2B2366D8
