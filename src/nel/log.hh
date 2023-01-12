// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#ifndef NEL_LOG_HH
#define NEL_LOG_HH

namespace nel
{

class Log;

} // namespace nel

#include <inttypes.h>

namespace nel
{

class Log
{
    public:
        friend Log &operator<<(Log &outs, char v) noexcept;
        friend Log &operator<<(Log &outs, char const *v) noexcept;
        friend Log &operator<<(Log &outs, uint8_t const v) noexcept;
        friend Log &operator<<(Log &outs, int const v) noexcept;
        friend Log &operator<<(Log &outs, long unsigned int const v) noexcept;
};

extern Log log;

} // namespace nel

#endif // NEL_LOG_HH
