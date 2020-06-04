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
        Writable(size_t nWritableHighWaterMark = 16 * 1024);

    public:
        //void cork(void);
        //void uncork(void);
        //int writableCorked(void);

        //void setDefaultEncoding(std::string)

        void write(const char* str) { write(str, strlen(str)); }
        bool write(const void* buf, size_t size);
        bool write(const std::string& strData) { return write(strData.data(), strData.size()); }
        bool write(const std::vector<uint8_t>& vecData) { return write(vecData.data(), vecData.size()); }

        void end(const char* str) { end(str, strlen(str)); }
        void end(const std::string& strData) { end(strData.data(), strData.size()); }
        void end(const std::vector<uint8_t>& vecData) { end(vecData.data(), vecData.size()); }
        void end(const void* buf = nullptr, size_t size = 0);

        bool isWritable(void) { return !m_bDestroyed && !m_bFinish && !m_bWriteError; };
        //bool isWritableEnded(void);
        //bool isWritableFinished(void);

    protected:
        // Will be called to notify underlying implementation about changes in stream state:
        //  - new data pushed to m_wrBuffer
        //  - stream ended (changed it's state to "finished")
        // Implementation should pull data, if any, from m_wrBuffer until it is empty.
        // After all data from m_wrBuffer is fully processed, stream should be notified by calling _onDrained().
        virtual void _write(void) = 0;

    protected:
        // Notifies the stream that all data from m_wrBuffer is fully processed.
        // Returns true if stream is in "finished" state, otherwise - false.
        // Finished state means that there will be no new data in m_wrBuffer and no more
        // calls to _write(). Implementation may choose to gracefully free it's allocated
        // resources after detecting this condition.
        // This method may emit events synchronously.
        bool _onDrained(void);

        // Notifies the stream about irrecoverable error condition while processing the data.
        // No more writes to the stream will be possible after calling this function.
        // This method may emit events synchronously.
        void _onWriteError(const Error& err);

    private:
        void _notifyWrite(void);

    protected:
        std::vector<uint8_t> m_wrBuffer;

    private:
        size_t m_nWritableHighWaterMark;
        bool m_bFinish = false;
        bool m_bWrNotified = false;
        bool m_bWriteError = false;
};

}   // namespace stream

#endif //WRITABLE_H_9E7208510D9E4915AF7238C26CAD5D80
