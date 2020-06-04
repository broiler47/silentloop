//
// Created by palulukan on 5/12/20.
//

#ifndef EVENTEMITTER_H_60597FC6BA5A4253B463B024646651C8
#define EVENTEMITTER_H_60597FC6BA5A4253B463B024646651C8

#include "Linkable.h"
#include "Utils.h"

#include <functional>
#include <vector>

#define EXPORT_EVENT_SIGNATURE(name, ...)   \
    void on_##name(const std::function<void(__VA_ARGS__)>& cb)

#define EXPORT_EVENT(name, ...)                                             \
    public:                                                                 \
        EXPORT_EVENT_SIGNATURE(name, __VA_ARGS__)                           \
        {                                                                   \
            if(cb)                                                          \
                _m_vec_cb_on_##name.push_back(cb);                          \
        }                                                                   \
    protected:                                                              \
        std::vector<std::function<void(__VA_ARGS__)>> _m_vec_cb_on_##name;  \

#define EXPORT_EVENT_WATCHABLE(name, ...)                                   \
    public:                                                                 \
        EXPORT_EVENT_SIGNATURE(name, __VA_ARGS__)                           \
        {                                                                   \
            if(!cb)                                                         \
                return;                                                     \
            _m_vec_cb_on_##name.push_back(cb);                              \
            OnNewListener(#name);                                           \
        }                                                                   \
    protected:                                                              \
        std::vector<std::function<void(__VA_ARGS__)>> _m_vec_cb_on_##name;  \

#define EMIT_EVENT(name, ...)                       \
    do                                              \
    {                                               \
        for(const auto & cb : _m_vec_cb_on_##name)  \
            CATCH_ALL(cb(__VA_ARGS__));             \
    } while(0)                                      \

#define EMIT_EVENT_ASYNC(name, ...) EmitEventAsync(_m_vec_cb_on_##name, __VA_ARGS__)

#define EMIT_EVENT_ASYNC0(name) EmitEventAsync(_m_vec_cb_on_##name)

#define EVENT_LISTENERS_COUNT(name) (_m_vec_cb_on_##name.size())

#include "Log.h"

class EventEmitterBase
{
    public:
        virtual ~EventEmitterBase(void) = default;

    public:
        virtual void OnNewListener(const char* szName) { UNUSED_ARG(szName); };
};

class SyncEventEmitter : public EventEmitterBase
{};

class EventEmitter : public EventEmitterBase, public Linkable
{
    public:
        template <typename TEventCb, typename ...Args>
        void EmitEventAsync(const std::vector<TEventCb>& vecCb, Args&&... args)
        {
            NextTick([&vecCb, args...](void) {
                for(const auto & cb : vecCb)
                    CATCH_ALL(cb(args...));
            });
        }
};

#endif //EVENTEMITTER_H_60597FC6BA5A4253B463B024646651C8
