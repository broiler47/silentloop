//
// Created by palulukan on 5/8/20.
//

#ifndef SOCKET_H_21546E3F7E4643558959A187FA231DFB
#define SOCKET_H_21546E3F7E4643558959A187FA231DFB

#include "modules/stream/Duplex.h"

namespace net
{

#define SOCKET_READ_BUF_SIZE 0x10000

class Socket : public stream::Duplex
{
    //EXPORT_EVENT(connect)
    //EXPORT_EVENT(lookup)
    //EXPORT_EVENT(ready)
    //EXPORT_EVENT(timeout)

    public:
        static std::shared_ptr<Socket> CreateShared(int fd, bool allowHalfOpen, bool startReading);

    public:
        void setNoDelay(bool noDelay = true);

    private:
        void _init(int fd, bool allowHalfOpen, bool startReading);

    private:
        void _destroy(const Error* pErr, const std::function<void(const Error& err)>& cb) override;
        void _write(void) override;
        void _read(void) override;

    private:
        void _doWrite(void);
        void _doRead(void);

    private:
        std::weak_ptr<IOEvent> m_wpSocketEvent;
        bool m_bAllowHalfOpen = false;
        bool m_bHUP = false;
        bool m_bWRShutdown = false;
};

}   // namespace net

#endif //SOCKET_H_21546E3F7E4643558959A187FA231DFB
