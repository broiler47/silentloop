//
// Created by palulukan on 5/3/20.
//

#ifndef UTILS_H_7A9F4C6AB6644A5597D6355ECAAE9F7E
#define UTILS_H_7A9F4C6AB6644A5597D6355ECAAE9F7E

#include "Log.h"

#include <exception>

#define UNUSED_ARG(x) ((void)(x))

#define CATCH_ALL(fn)                                           \
    try                                                         \
    {                                                           \
        (fn);                                                   \
    }                                                           \
    catch(std::exception& ex)                                   \
    {                                                           \
        ERROR("Ignoring unhandled exception: %s", ex.what());   \
    }                                                           \
    catch(...)                                                  \
    {                                                           \
        ERROR("Ignoring unknown unhandled exception");          \
    }                                                           \

#if EAGAIN != EWOULDBLOCK
#define IS_WOULDBLOCK(x) ((x) == EAGAIN || (x) == EWOULDBLOCK)
#else
#define IS_WOULDBLOCK(x) ((x) == EAGAIN)
#endif

#endif //UTILS_H_7A9F4C6AB6644A5597D6355ECAAE9F7E
