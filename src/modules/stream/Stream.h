//
// Created by palulukan on 5/8/20.
//

#ifndef STREAM_H_16C558E69E3C4CB0BF9AAE0E97DE86EB
#define STREAM_H_16C558E69E3C4CB0BF9AAE0E97DE86EB

#include "EventLoop.h"

namespace stream
{

class Stream : public IOEvent
{
    public:
        //void destroy(void);
        //bool isDestroyrd(void);
};

class WritableStream : public virtual Stream
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

        bool write(const std::string& strData);
        bool write(const std::vector<uint8_t>& vecData);
        bool write(const void* buf, size_t size);

        void end(void);
        void end(const std::string& strData);
        void end(const std::vector<uint8_t>& vecData);
        void end(const void* buf, size_t size);

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

class ReadableStream : public virtual Stream
{
    EXPORT_EVENT(data, const std::vector<uint8_t>& vecData)
    EXPORT_EVENT(end)
    //EXPORT_EVENT(pause)
    //EXPORT_EVENT(resume)
    //EXPORT_EVENT(readable)

    public:
        //bool isPaused(void);
        void pause(void);
        //void pipe(WritableStream& ws);
        //void unpipe(WritableStream& ws);
        //std::vector<uint8_t> read(size_t size = 0);
        //bool isReadable(void);
        //std::string getReadableEncoding(void);
        //void setEncoding(std::string)
        //bool isReadableEnded(void);
        //bool isReadableFlowing(void);
        void resume(void);
        //void unshift(const std::string& strData);
        //void unshift(const std::vector<uint8_t>& vecData);

    protected:
        virtual void _read(void) = 0;

    protected:
        bool _push(const void* buf, size_t size);

    private:
        void _emitData(void);

    protected:
        std::vector<uint8_t> m_rdBuffer;
        bool m_bFlowing = false;
        bool m_bEnded = false;
};

class DuplexStream : public ReadableStream, public WritableStream
{
};

}   // namespace stream

#endif //STREAM_H_16C558E69E3C4CB0BF9AAE0E97DE86EB
