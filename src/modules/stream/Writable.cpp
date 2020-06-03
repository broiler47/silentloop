//
// Created by palulukan on 5/16/20.
//

#include "Writable.h"

#include <cassert>

stream::Writable::Writable(size_t nWritableHighWaterMark) :
    m_nWritableHighWaterMark(nWritableHighWaterMark)
{
    assert(m_nWritableHighWaterMark > 0);
}

bool stream::Writable::write(const void *buf, size_t size)
{
    if(m_bDestroyed)
    {
        EMIT_EVENT_ASYNC(error, TypeError("Stream destroyed", ERR_STREAM_DESTROYED));
        return false;
    }

    if(m_bFinish)
    {
        EMIT_EVENT_ASYNC(error, Error("Stream is finished"));
        return false;
    }

    if(m_bWriteError)
    {
        EMIT_EVENT_ASYNC(error, Error("Stream is not writable due to previous error"));
        return false;
    }

    if(buf)
        m_wrBuffer.insert(m_wrBuffer.end(), (uint8_t *)buf, (uint8_t *)buf + size);

    _notifyWrite();

    return m_wrBuffer.size() < m_nWritableHighWaterMark;
}

void stream::Writable::end(const void *buf, size_t size)
{
    if(m_bDestroyed)
    {
        EMIT_EVENT_ASYNC(error, TypeError("Stream destroyed", ERR_STREAM_DESTROYED));
        return;
    }

    if(m_bFinish)
    {
        EMIT_EVENT_ASYNC(error, Error("Stream is finished"));
        return;
    }

    write(buf, size);

    m_bFinish = true;
}

bool stream::Writable::_onDrained(void)
{
    if(!m_wrBuffer.empty())
    {
        _notifyWrite();
        return false;
    }

    if(m_bFinish)
        EMIT_EVENT_ASYNC0(finish);
    else
        EMIT_EVENT_ASYNC0(drain);

    return m_bFinish;
}

void stream::Writable::_onWriteError(const Error& err)
{
    m_bWriteError = true;
    EMIT_EVENT(error, err);
}

void stream::Writable::_notifyWrite(void)
{
    if(!m_bWrNotified)
    {
        m_bWrNotified = true;

        NextTick([this](void) {
            m_bWrNotified = false;
            _write();
        });
    }
}
