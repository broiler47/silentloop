//
// Created by palulukan on 5/16/20.
//

#include "Writable.h"

#include "modules/process/Process.h"

#include <cassert>

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

    bool res = m_wrBuffer.empty();

    if(!size)
        return res;

    assert(buf);

    m_wrBuffer.insert(m_wrBuffer.end(), (uint8_t *)buf, (uint8_t *)buf + size);

    if(!m_bWrNotified)
    {
        m_bWrNotified = true;

        NextTick([this](void) {
            m_bWrNotified = false;
            _write();
        });
    }

    return res;
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

    if(size)
        write(buf, size);

    m_bFinish = true;
}

bool stream::Writable::_onDrained(void)
{
    if(m_bFinish)
        EMIT_EVENT_ASYNC0(finish);
    else
        EMIT_EVENT_ASYNC0(drain);

    return m_bFinish;
}
