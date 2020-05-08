//
// Created by palulukan on 5/8/20.
//

#ifndef ERROR_H_23683DA7E6BC4F0F837BE39653E1123C
#define ERROR_H_23683DA7E6BC4F0F837BE39653E1123C

#include <string>

class Error
{
    public:
        Error(const std::string& err = "Unknown", int _errno = 0);

    public:
        const char* What(void) const;
        int GetErrno(void) const;
        const char* Format(void) const;

    private:
        std::string m_strWhat;
        int m_nErrno;
        mutable std::string m_strFormatted;
};

#endif //ERROR_H_23683DA7E6BC4F0F837BE39653E1123C
