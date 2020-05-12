//
// Created by palulukan on 5/8/20.
//

#include "Error.h"

#include <sstream>
#include <cstring>
#include <map>

Error::Error(const std::string &msg, ErrorCode err, int _errno) :
    m_strMsg(msg),
    m_errCode(err),
    m_nErrno(_errno)
{
}

const char *Error::_format(const char* errTypeName) const
{
    static const std::map<ErrorCode, const char*> mapErrorCodes = {
        { ERR_UNKNOWN, "ERR_UNKNOWN" },
        { ERR_SYSTEM_ERROR, "ERR_SYSTEM_ERROR" },
        { ERR_SERVER_ALREADY_LISTEN, "ERR_SERVER_ALREADY_LISTEN" }
    };

    if(m_strFormatted.empty())
    {
        auto itErrorCodeName = mapErrorCodes.find(m_errCode);
        if(itErrorCodeName == mapErrorCodes.end())
            itErrorCodeName = mapErrorCodes.find(ERR_UNKNOWN);  // ERR_UNKNOWN should always be present in the error code names map

        std::stringstream strm;
        strm << errTypeName << " [" << itErrorCodeName->second << "]: " << m_strMsg;

        if(m_nErrno)
            strm << "; errno: " << m_nErrno << " (" << strerror(m_nErrno) << ")";

        m_strFormatted = strm.str();
    }

    return m_strFormatted.c_str();
}
