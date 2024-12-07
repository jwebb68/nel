// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include <nel/log.hh>

#include <cstdio>

namespace nel
{
Log log;

Log &operator<<(Log &outs, char v)
{
    fprintf(stderr, "%c", v);
    return outs;
}

Log &operator<<(Log &outs, char const *v)
{
    fprintf(stderr, "%s", v);
    return outs;
}

Log &operator<<(Log &outs, bool const v)
{
    fprintf(stderr, "%s", (v)?"true":"false");
    return outs;
}

Log &operator<<(Log &outs, uint8_t const v)
{
    fprintf(stderr, "%" PRIu8, v);
    return outs;
}

Log &operator<<(Log &outs, uint16_t const v)
{
    fprintf(stderr, "%" PRIu16, v);
    return outs;
}

Log &operator<<(Log &outs, uint32_t const v)
{
    fprintf(stderr, "%" PRIu32, v);
    return outs;
}

Log &operator<<(Log &outs, uint64_t const v)
{
    fprintf(stderr, "%" PRIu64, v);
    return outs;
}


Log &operator<<(Log &outs, int8_t const v)
{
    fprintf(stderr, "%" PRId8, v);
    return outs;
}

Log &operator<<(Log &outs, int16_t const v)
{
    fprintf(stderr, "%" PRId16, v);
    return outs;
}

Log &operator<<(Log &outs, int32_t const v)
{
    fprintf(stderr, "%" PRId32, v);
    return outs;
}

Log &operator<<(Log &outs, int64_t const v)
{
    fprintf(stderr, "%" PRId64, v);
    return outs;
}

} // namespace nel
