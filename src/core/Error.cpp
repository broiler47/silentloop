//
// Created by palulukan on 5/8/20.
//

#include "Error.h"

#include <cstring>
#include <map>

Error::Error(const std::string& msg, ErrorCode err, int _errno, const std::string& strErrTypeName) :
    std::runtime_error(""),
    m_strMsg(msg),
    m_errCode(err),
    m_nErrno(_errno),
    m_strErrTypeName(strErrTypeName)
{
}

const char *Error::_format(void) const noexcept
{
    static const std::map<ErrorCode, const char*> mapErrorCodes = {
        { ERR_UNKNOWN, "ERR_UNKNOWN" },
        { ERR_SYSTEM_ERROR, "ERR_SYSTEM_ERROR" },
        { ERR_SERVER_ALREADY_LISTEN, "ERR_SERVER_ALREADY_LISTEN" },
        { ERR_STREAM_DESTROYED, "ERR_STREAM_DESTROYED" },
        { HTTP_PARSER_ERROR, "HTTP_PARSER_ERROR" },
        { ERR_HTTP_HEADERS_SENT, "ERR_HTTP_HEADERS_SENT" },
        { ERR_HTTP_INVALID_STATUS_CODE, "ERR_HTTP_INVALID_STATUS_CODE" }
    };

    try
    {
        if(m_strFormatted.empty())
        {
            auto itErrorCodeName = mapErrorCodes.find(m_errCode);
            if(itErrorCodeName == mapErrorCodes.end())
                itErrorCodeName = mapErrorCodes.find(ERR_UNKNOWN);  // ERR_UNKNOWN should always be present in the error code names map

            m_strFormatted = m_strErrTypeName + " [" + itErrorCodeName->second + "]: " + m_strMsg;
            if(m_nErrno)
                m_strFormatted += "; errno: " + std::to_string(m_nErrno) + " (" + strerror(m_nErrno) + ")";
        }

        return m_strFormatted.c_str();
    }
    catch(...)
    {
        return "<Exception occurred while formatting error description>";
    }
}
