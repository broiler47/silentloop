//
// Created by palulukan on 5/16/20.
//

#include "Readable.h"

#include "modules/process/Process.h"

#include <cassert>

void stream::Readable::pause(void)
{
    m_bFlowing = false;
}

void stream::Readable::resume(void)
{
    if(!m_bFlowing && !m_bDestroyed)
    {
        m_bFlowing = true;

        NextTick([this]() {
            _read();
            _emitData();
        });
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
        _emitData();
    }
    else
    {
        m_bEnded = true;
        EMIT_EVENT(end);
    }

    return m_bFlowing;
}

void stream::Readable::_emitData(void)
{
    if(m_bFlowing && !m_bDestroyed && !m_rdBuffer.empty())
    {
        EMIT_EVENT(data, m_rdBuffer);
        m_rdBuffer.clear();
    }
}
