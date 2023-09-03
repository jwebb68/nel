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

Log &operator<<(Log &outs, int const v)
{
    fprintf(stderr, "%u", v);
    return outs;
}

Log &operator<<(Log &outs, long unsigned int const v)
{
    fprintf(stderr, "%lu", v);
    return outs;
}

Log &operator<<(Log &outs, float const v)
{
    fprintf(stderr, "%#f", (double)v);
    return outs;
}

Log &operator<<(Log &outs, double const v)
{
    fprintf(stderr, "%#f", v);
    return outs;
}

} // namespace nel
