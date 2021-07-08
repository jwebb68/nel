#ifndef NEL_LOG_HH
#define NEL_LOG_HH

namespace nel {

class Log;

} // namespace nel

#include <cstddef> // size_t

namespace nel {

class Log {
    public:
        friend Log &operator<<(Log &outs, char const *v) noexcept;
        friend Log &operator<<(Log &outs, size_t const v) noexcept;
};

extern Log log;

} // namespace nel

#endif//NEL_LOG_HH
