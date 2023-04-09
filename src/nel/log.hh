// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(NEL_LOG_HH)
#    define NEL_LOG_HH

namespace nel
{

class Log;

} // namespace nel

#    include <inttypes.h>

namespace nel
{

class Log
{
    public:
        friend Log &operator<<(Log &outs, char v);
        friend Log &operator<<(Log &outs, char const *v);
        friend Log &operator<<(Log &outs, uint8_t const v);
        friend Log &operator<<(Log &outs, uint16_t const v);
        friend Log &operator<<(Log &outs, uint32_t const v);
        friend Log &operator<<(Log &outs, int const v);
        friend Log &operator<<(Log &outs, long unsigned int const v);
};

extern Log log;

} // namespace nel

#endif // !defined(NEL_LOG_HH)
