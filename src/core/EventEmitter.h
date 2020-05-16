//
// Created by palulukan on 5/12/20.
//

#ifndef EVENTEMITTER_H_60597FC6BA5A4253B463B024646651C8
#define EVENTEMITTER_H_60597FC6BA5A4253B463B024646651C8

#include <functional>
#include <vector>

#define EXPORT_EVENT(name, ...)                                             \
    public:                                                                 \
        void on_##name(const std::function<void(__VA_ARGS__)>& cb)          \
        {                                                                   \
            if(cb)                                                          \
                _m_vec_cb_on_##name.push_back(cb);                          \
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

#define EVENT_LISTENER_COUNT(name) (_m_vec_cb_on_##name.length())

class EventEmitter
{
    public:
        template <typename TEventCb, typename ...Args>
        void EmitEventAsync(const std::vector<TEventCb>& vecCb, Args&&... args)
        {
            _nextTick([&vecCb, args...](void) {
                for(const auto & cb : vecCb)
                    CATCH_ALL(cb(args...));
            });
        }

    private:
        static void _nextTick(const std::function<void(void)>& cb);
};

#endif //EVENTEMITTER_H_60597FC6BA5A4253B463B024646651C8
