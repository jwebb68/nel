#include "log.hh"

#include <cstdio>

namespace nel {
Log log;


Log &operator<<(Log &outs, char const *v)
{
    fprintf(stderr, "%s", v);
    return outs;
}

Log &operator<<(Log &outs, size_t const v)
{
    fprintf(stderr, "%zu", v);
    return outs;
}


}
