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
        void destroy(void);
        void destroy(const Error& err);
        bool isDestroyed(void) const { return m_bDestroyed; }

    protected:
        virtual void _destroy(const Error* pErr, const std::function<void(const Error& err)>& cb) = 0;

    private:
        void _doDestroy(const Error* pErr);

    protected:
        bool m_bDestroyed = false;
};

}   // namespace stream

#endif //STREAM_H_16C558E69E3C4CB0BF9AAE0E97DE86EB
