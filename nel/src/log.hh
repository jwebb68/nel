#ifndef NEL_LOG_HH
#define NEL_LOG_HH

namespace nel {

class Log;

}

#include <cstddef> // size_t

namespace nel {

class Log {
    public:
        friend Log &operator<<(Log &outs, char const *v) noexcept;
        friend Log &operator<<(Log &outs, size_t const v) noexcept;
};

extern Log log;

}

#endif//NEL_LOG_HH
