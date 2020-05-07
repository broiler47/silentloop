//
// Created by palulukan on 5/5/20.
//

#include "Log.h"

#include <cassert>
#include <cstdarg>

Log *Log::m_pInstance = nullptr;

Log &Log::Instance(void)
{
    if(!m_pInstance)
        m_pInstance = new Log();

    return *m_pInstance;
}

void Log::Destroy(void)
{
    if(!m_pInstance)
        return;

    delete m_pInstance;
    m_pInstance = nullptr;
}

void Log::AddDestination(FILE *pf)
{
    assert(pf);

    m_vecDestinations.push_back(pf);
}

void Log::Write(Log::Level level, const char *fmt, ...)
{
    assert(level < Level::LOG_LEVEL_MAX);
    if(level >= Level::LOG_LEVEL_MAX)
        level = Level::ERROR;

    const char *arrLevels[] = {
        "DEBUG",
        "INFO",
        "WARNING",
        "ERROR"
    };

    va_list vl;
    va_start(vl, fmt);

    unsigned int nLevel = (unsigned int)level;

    for(auto pf : m_vecDestinations)
    {
        fprintf(pf, "%s: ", arrLevels[nLevel]);

        va_list vlCopy;
        va_copy(vlCopy, vl);

        vfprintf(pf, fmt, vlCopy);

        va_end(vlCopy);

        fprintf(pf, "\n");
    }

    va_end(vl);
}
