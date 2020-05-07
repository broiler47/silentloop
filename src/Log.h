//
// Created by palulukan on 5/5/20.
//

#ifndef LOG_H_75FE5A1CE8164DEF8906CF3760DFBEBB
#define LOG_H_75FE5A1CE8164DEF8906CF3760DFBEBB

#include <cstdio>
#include <vector>
#include <cerrno>
#include <cstring>

class Log
{
    public:
        static Log& Instance(void);
        static void Destroy(void);

    private:
        Log(void) = default;
        ~Log(void) = default;
        Log(const Log&) = delete;
        Log(const Log&&) = delete;
        Log& operator=(const Log&) = delete;
        Log& operator=(const Log&&) = delete;

    public:
        enum class Level : unsigned int
        {
            DEBUG = 0,
            INFO,
            WARNING,
            ERROR,

            LOG_LEVEL_MAX
        };

    public:
        void AddDestination(FILE* pf);
        void Write(Level level, const char* fmt, ...);

    private:
        std::vector<FILE *> m_vecDestinations;

    private:
        static Log *m_pInstance;
};

#define DEBUG(...)   (Log::Instance().Write(Log::Level::DEBUG,    __VA_ARGS__))
#define INFO(...)    (Log::Instance().Write(Log::Level::INFO,     __VA_ARGS__))
#define WARNING(...) (Log::Instance().Write(Log::Level::WARNING,  __VA_ARGS__))
#define ERROR(...)   (Log::Instance().Write(Log::Level::ERROR,    __VA_ARGS__))

#define SYSCALL_ERROR(name) ERROR("%s failed (%d): %s", (name), errno, strerror(errno))

#endif //LOG_H_75FE5A1CE8164DEF8906CF3760DFBEBB
