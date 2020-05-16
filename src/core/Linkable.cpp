//
// Created by palulukan on 5/16/20.
//

#include "Linkable.h"

#include "EventLoopFactory.h"

#include <cassert>

void Linkable::LinkWith(const std::shared_ptr<Linkable> &sp)
{
    sp->_link(GetSharedPtr());
}

void Linkable::NextTick(const std::function<void(void)> &cb)
{
    GetThreadEventLoop()->NextTick(cb, GetSharedPtr());
}

void Linkable::_link(const std::shared_ptr<Linkable> &sp)
{
    if(_lookup(sp))
    {
        WARNING("Circular linking dependency detected");
        assert(false);
    }

    m_vecLinks.push_back(sp);
}

bool Linkable::_lookup(const std::shared_ptr<Linkable> &sp)
{
    if(sp == GetSharedPtr())
        return true;

    for(const auto& spLink : m_vecLinks)
    {
        if(spLink == sp || spLink->_lookup(sp))
            return true;
    }

    return false;
}
