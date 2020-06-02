//
// Created by palulukan on 5/8/20.
//

#include "Stream.h"

void stream::Stream::destroy(const Error *pErr)
{
    if(m_bDestroyed)
        return;

    if(pErr)
        EMIT_EVENT_ASYNC(error, *pErr);

    _destroy(pErr, [this](const Error& err) {
        EMIT_EVENT_ASYNC(error, err);
    });

    m_bDestroyed = true;

    EMIT_EVENT_ASYNC0(close);
}
