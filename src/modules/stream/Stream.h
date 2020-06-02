//
// Created by palulukan on 5/8/20.
//

#ifndef STREAM_H_16C558E69E3C4CB0BF9AAE0E97DE86EB
#define STREAM_H_16C558E69E3C4CB0BF9AAE0E97DE86EB

#include "EventLoop.h"

namespace stream
{

class Stream : public EventEmitter
{
    EXPORT_EVENT(error, const Error& err)
    EXPORT_EVENT(close)

    public:
        void destroy(void) { destroy(nullptr); }
        void destroy(const Error& err) { destroy(&err); }
        void destroy(const Error* pErr);
        bool isDestroyed(void) const { return m_bDestroyed; }

    protected:
        // Will be called to notify that stream was destroyed by calling destroy().
        // All resources allocated by underlying implementation should be freed immediately.
        // Arguments:
        //  pErr - error that was the reason of stream destruction. May be NULL. Should
        //         be ignored if implementation does not care about the exact reason of this event.
        //  cb - callback that must be called in case when any additional error should occur
        //       while handling this event.
        virtual void _destroy(const Error* pErr, const std::function<void(const Error& err)>& cb) = 0;

    protected:
        bool m_bDestroyed = false;
};

}   // namespace stream

#endif //STREAM_H_16C558E69E3C4CB0BF9AAE0E97DE86EB
