//
// Created by palulukan on 5/16/20.
//

#include "Readable.h"

#include <cassert>

stream::Readable::Readable(size_t nHighWaterMark) :
    m_nReadableHighWaterMark(nHighWaterMark)
{
    assert(m_nReadableHighWaterMark > 0);
}

void stream::Readable::pause(void)
{
    m_bFlowing = false;
}

void stream::Readable::resume(void)
{
    if(!m_bFlowing && !m_bDestroyed)
    {
        m_bFlowing = true;
        _notifyRead();
    }
}

bool stream::Readable::_push(const void *buf, size_t size)
{
    if(m_bDestroyed)
    {
        EMIT_EVENT_ASYNC(error, TypeError("Stream destroyed", ERR_STREAM_DESTROYED));
        return false;
    }

    if(m_bEnded)
    {
        EMIT_EVENT(error, Error("Received data after stream end"));
        return false;
    }

    if(size)
    {
        assert(buf);

        m_rdBuffer.insert(m_rdBuffer.end(), (uint8_t *)buf, (uint8_t *)buf + size);
    }
    else
        m_bEnded = true;

    _emitData();

    m_bBufferOverrun = m_rdBuffer.size() > m_nReadableHighWaterMark;

    return m_bFlowing && !m_bBufferOverrun;
}

void stream::Readable::_onReadError(const Error &err)
{
    EMIT_EVENT(error, err);
}

void stream::Readable::_emitData(void)
{
    if(!m_bFlowing || m_bDestroyed || m_bEndEmitted)
        return;

    if(!m_rdBuffer.empty())
    {
        EMIT_EVENT(data, m_rdBuffer);
        m_rdBuffer.clear();

        if(m_bBufferOverrun)
        {
            m_bBufferOverrun = false;
            _notifyRead();
        }
    }

    if(m_bEnded)
    {
        EMIT_EVENT(end);
        m_bEndEmitted = true;
    }
}

void stream::Readable::_notifyRead(void)
{
    NextTick([this]() {
        _read();
        _emitData();
    });
}
