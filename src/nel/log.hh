#ifndef NEL_LOG_HH
#define NEL_LOG_HH

namespace nel
{

class Log;

} // namespace nel

namespace nel
{

class Log
{
    public:
        friend Log &operator<<(Log &outs, char const *v) noexcept;
        friend Log &operator<<(Log &outs, long unsigned int const v) noexcept;
};

extern Log log;

} // namespace nel

#endif // NEL_LOG_HH
