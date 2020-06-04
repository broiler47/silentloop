//
// Created by palulukan on 5/8/20.
//

#ifndef ERROR_H_23683DA7E6BC4F0F837BE39653E1123C
#define ERROR_H_23683DA7E6BC4F0F837BE39653E1123C

#include <string>
#include <stdexcept>

enum ErrorCode
{
    ERR_UNKNOWN = 0,
    ERR_SYSTEM_ERROR,
    ERR_SERVER_ALREADY_LISTEN,
    ERR_STREAM_DESTROYED,
    HTTP_PARSER_ERROR,
    ERR_HTTP_HEADERS_SENT,
    ERR_HTTP_INVALID_STATUS_CODE,

    _ERR_ERROR_CODE_MAX
};

class Error : public std::runtime_error
{
    public:
        // TODO: May throw exceptions while copied
        Error(const std::string& msg = "Unknown", ErrorCode err = ERR_SYSTEM_ERROR, int _errno = 0, const std::string& strErrTypeName = "Error");

    public:
        virtual ~Error(void) = default;

    public:
        ErrorCode GetCode(void) const { return m_errCode; }
        const char* GetMessage(void) const { return m_strMsg.c_str(); }
        int GetErrno(void) const { return m_nErrno; }
        const char* what(void) const noexcept override { return _format(); }

    protected:
        const char* _format(void) const noexcept;

    private:
        std::string m_strMsg;
        ErrorCode m_errCode;
        int m_nErrno;
        std::string m_strErrTypeName;
        mutable std::string m_strFormatted;
};

class TypeError : public Error
{
    public:
        TypeError(const std::string& msg = "Unknown", ErrorCode err = ERR_SYSTEM_ERROR) :
            Error(msg, err, 0, "TypeError")
        {}
};

class SystemError : public Error
{
    public:
        SystemError(const std::string& msg = "Unknown", int _errno = 0, ErrorCode err = ERR_SYSTEM_ERROR) :
            Error(msg, err, _errno, "SystemError")
        {}
};

#endif //ERROR_H_23683DA7E6BC4F0F837BE39653E1123C
