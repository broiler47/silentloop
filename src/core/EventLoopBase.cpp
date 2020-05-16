//
// Created by palulukan on 5/8/20.
//

#include "EventLoopBase.h"

#include "Utils.h"

#include <cassert>

void EventLoopBase::NextTick(const std::function<void(void)> &cb)
{
    assert(cb);

    m_qNextTick.push(cb);

    //DEBUG("Adding NextTick callback: %lu", m_qNextTick.size());
}

void EventLoopBase::_drainNextTickQueue(void)
{
    while(!m_qNextTick.empty())
    {
        //DEBUG("Processing NextTick queue callback: %lu", m_qNextTick.size());

        CATCH_ALL(m_qNextTick.front()());
        m_qNextTick.pop();
    }
}
