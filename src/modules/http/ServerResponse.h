//
// Created by palulukan on 5/11/20.
//

#ifndef SERVERRESPONSE_H_A4D95CC3464B4912ACD687D2C5D60558
#define SERVERRESPONSE_H_A4D95CC3464B4912ACD687D2C5D60558

#include "modules/net/Socket.h"

namespace http
{

class ServerResponse : public stream::Writable
{
    public:
        static std::shared_ptr<ServerResponse> CreateShared(const std::shared_ptr<net::Socket>& spSocket);

    private:
        ServerResponse(void) = default;

    private:
        void _init(const std::shared_ptr<net::Socket>& spSocket);

    private:
    private:
        void _destroy(const Error* pErr, const std::function<void(const Error& err)>& cb) override;
        void _write(void) override;
};

}   // namespace http

#endif //SERVERRESPONSE_H_A4D95CC3464B4912ACD687D2C5D60558
