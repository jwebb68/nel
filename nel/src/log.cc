#include "log.hh"

#include <cstdio>

namespace nel {
Log log;


Log &operator<<(Log &outs, char const *v) noexcept
{
    fprintf(stderr, "%s", v);
    return outs;
}

Log &operator<<(Log &outs, size_t const v) noexcept
{
    fprintf(stderr, "%zu", v);
    return outs;
}


}
