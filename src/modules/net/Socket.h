//
// Created by palulukan on 5/8/20.
//

#ifndef SOCKET_H_21546E3F7E4643558959A187FA231DFB
#define SOCKET_H_21546E3F7E4643558959A187FA231DFB

#include "modules/stream/Stream.h"

namespace net
{

#define SOCKET_READ_BUF_SIZE 65536

class Socket : public stream::DuplexStream
{
    //EXPORT_EVENT(connect)
    //EXPORT_EVENT(lookup)
    //EXPORT_EVENT(ready)
    //EXPORT_EVENT(timeout)

    public:
        static std::shared_ptr<Socket> Create(EventLoopBase& eventLoop, int fd, bool allowHalfOpen, bool startReading);

    public:
        void setNoDelay(bool noDelay = true);

    private:
        void _init(EventLoopBase& eventLoop, int fd, bool allowHalfOpen, bool startReading);

    private:
        void OnRead(void) override;
        void OnWrite(void) override;
        void OnHUP(void) override;
        void OnError(void) override;

    private:
        void _write(void) override;
        void _read(void) override;

    private:
        void _doWrite(void);
        void _doRead(void);

    private:
        bool m_bAllowHalfOpen = false;
        bool m_bHUP = false;
        bool m_bWRShutdown = false;
};

}   // namespace net

#endif //SOCKET_H_21546E3F7E4643558959A187FA231DFB
