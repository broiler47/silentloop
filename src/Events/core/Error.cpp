//
// Created by palulukan on 5/8/20.
//

#include "Error.h"

#include <sstream>
#include <cstring>

Error::Error(const std::string &err, int _errno) :
    m_strWhat(err),
    m_nErrno(_errno)
{
}

const char *Error::What(void) const
{
    return m_strWhat.c_str();
}

int Error::GetErrno(void) const
{
    return m_nErrno;
}

const char *Error::Format(void) const
{
    if(m_strFormatted.empty())
    {
        std::stringstream strm;
        strm << "Error: " << m_strWhat;

        if(m_nErrno)
            strm << "; errno: " << m_nErrno << "(" << strerror(m_nErrno) << ")";

        strm << "\n";

        m_strFormatted = strm.str();
    }

    return m_strFormatted.c_str();
}
