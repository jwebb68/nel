#ifndef NEL_LOG_HH
#define NEL_LOG_HH

namespace nel {

class Log;

}

#include <cstddef> // size_t

namespace nel {

class Log {
    public:
        friend Log &operator<<(Log &outs, char const *v);
        friend Log &operator<<(Log &outs, size_t const v);
};

extern Log log;

}

#endif//NEL_LOG_HH
