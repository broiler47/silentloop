//
// Created by palulukan on 5/16/20.
//

#include "Linkable.h"

#include "Log.h"

#include <cassert>

void Linkable::LinkWith(const std::shared_ptr<Linkable> &sp)
{
    // NOTE: In case this object is not managed by std::shared_ptr,
    // shared_from_this() will only throw std::bad_weak_ptr in C++17 or later.
    std::shared_ptr<Linkable> spThis;
    try
    {
        spThis = shared_from_this();
    }
    catch(std::bad_weak_ptr&)
    {
        auto errMsg = "Linkable objects should be managed by std::shared_ptr and should not be linked from the constructor";
        ERROR(errMsg);
        throw std::runtime_error(errMsg);
    }

    sp->_link(spThis);
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

bool Linkable::_lookup(const std::shared_ptr<Linkable> &sp) const
{
    if(sp == shared_from_this())
        return true;

    for(const auto& spLink : m_vecLinks)
    {
        if(spLink == sp || spLink->_lookup(sp))
            return true;
    }

    return false;
}
