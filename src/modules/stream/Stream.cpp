//
// Created by palulukan on 5/8/20.
//

#include "Stream.h"

#include "Log.h"

#include <cassert>

bool stream::Writable::write(const std::string &strData)
{
    return write(strData.data(), strData.size());
}

bool stream::Writable::write(const std::vector<uint8_t> &vecData)
{
    return write(vecData.data(), vecData.size());
}

bool stream::Writable::write(const void *buf, size_t size)
{
    assert(buf);

    if(m_bFinish)
    {
        EMIT_EVENT_ASYNC(error, Error("Stream is finished"));
        return false;
    }

    bool res = m_wrBuffer.empty();

    m_wrBuffer.insert(m_wrBuffer.end(), (uint8_t *)buf, (uint8_t *)buf + size);

    if(!m_bWrNotified)
    {
        m_bWrNotified = true;

        _nextTick([this](void) {
            m_bWrNotified = false;

            _write();
        });
    }

    return res;
}

void stream::Writable::end(void)
{
    m_bFinish = true;
}

void stream::Writable::end(const std::string &strData)
{
    end(strData.data(), strData.size());
}

void stream::Writable::end(const std::vector<uint8_t> &vecData)
{
    end(vecData.data(), vecData.size());
}

void stream::Writable::end(const void *buf, size_t size)
{
    write(buf, size);
    end();
}

bool stream::Writable::_onDrained(void)
{
    if(m_bFinish)
        EMIT_EVENT_ASYNC0(finish);
    else
        EMIT_EVENT_ASYNC0(drain);

    return m_bFinish;
}

void stream::Readable::pause(void)
{
    m_bFlowing = false;
}

void stream::Readable::resume(void)
{
    if(!m_bFlowing)
    {
        m_bFlowing = true;

        _nextTick([this]() {
            _read();
            _emitData();
        });
    }
}

bool stream::Readable::_push(const void *buf, size_t size)
{
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
    if(m_bFlowing && !m_rdBuffer.empty())
    {
        EMIT_EVENT(data, m_rdBuffer);
        m_rdBuffer.clear();
    }
}
