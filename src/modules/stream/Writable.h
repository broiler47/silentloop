//
// Created by palulukan on 5/16/20.
//

#ifndef WRITABLE_H_9E7208510D9E4915AF7238C26CAD5D80
#define WRITABLE_H_9E7208510D9E4915AF7238C26CAD5D80

#include "Stream.h"

namespace stream
{

class Writable : public virtual Stream
{
    EXPORT_EVENT(drain)
    EXPORT_EVENT(finish)
    //EXPORT_EVENT(pipe)
    //EXPORT_EVENT(unpipe)

    public:
        //void cork(void);
        //void uncork(void);
        //int writableCorked(void);

        //void setDefaultEncoding(std::string)

        bool write(const void* buf, size_t size);
        bool write(const std::string& strData) { return write(strData.data(), strData.size()); }
        bool write(const std::vector<uint8_t>& vecData) { return write(vecData.data(), vecData.size()); }

        void end(const std::string& strData) { end(strData.data(), strData.size()); }
        void end(const std::vector<uint8_t>& vecData) { end(vecData.data(), vecData.size()); }
        void end(const void* buf = nullptr, size_t size = 0);

        //bool isWritable(void);
        //bool isWritableEnded(void);
        //bool isWritableFinished(void);

    protected:
        virtual void _write(void) = 0;

    protected:
        bool _onDrained(void);

    protected:
        std::vector<uint8_t> m_wrBuffer;
        bool m_bFinish = false;
        bool m_bWrNotified = false;
};

}   // namespace stream

#endif //WRITABLE_H_9E7208510D9E4915AF7238C26CAD5D80
