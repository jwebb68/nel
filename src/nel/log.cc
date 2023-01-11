#include <nel/log.hh>

#include <cstdio>

namespace nel
{
Log log;

Log &operator<<(Log &outs, char v) noexcept
{
    fprintf(stderr, "%c", v);
    return outs;
}

Log &operator<<(Log &outs, char const *v) noexcept
{
    fprintf(stderr, "%s", v);
    return outs;
}

Log &operator<<(Log &outs, uint8_t const v) noexcept
{
    fprintf(stderr, "%" PRIu8, v);
    return outs;
}

Log &operator<<(Log &outs, int const v) noexcept
{
    fprintf(stderr, "%u", v);
    return outs;
}

Log &operator<<(Log &outs, long unsigned int const v) noexcept
{
    fprintf(stderr, "%lu", v);
    return outs;
}

} // namespace nel
