// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include <nel/formatter.hh>

#include <nel/charstr.hh>

namespace nel
{

Formatter &operator<<(Formatter &fmt, bool b)
{
    int32_t rc = fmt.write((b) ? "true" : "false");
    nel::unused(rc);
    return fmt;
}

Formatter &operator<<(Formatter &fmt, Formatter::Char c)
{
    int32_t rc = fmt.write(c);
    nel::unused(rc);
    return fmt;
}

Formatter &operator<<(Formatter &fmt, Formatter::Char const *s)
{
    int32_t rc = fmt.write(s);
    nel::unused(rc);
    return fmt;
}

Formatter &operator<<(Formatter &fmt, uint8_t v)
{
    return fmt << static_cast<uint32_t>(v);
}

Formatter &operator<<(Formatter &fmt, uint16_t v)
{
    return fmt << static_cast<uint32_t>(v);
}

Formatter &operator<<(Formatter &fmt, uint32_t v)
{
    nel::CharStr<10 + 1> buf;
    int32_t rc;
    do {
        rc = buf.write((v % 10) + '0');
        if (rc < 0) { break; }
        v /= 10;
    } while (v > 0);

    buf.reverse();

    rc = fmt.write(buf.ptr(), buf.len());

    return fmt;
}

Formatter &operator<<(Formatter &fmt, int8_t v)
{
    return fmt << static_cast<int32_t>(v);
}

Formatter &operator<<(Formatter &fmt, int16_t v)
{
    return fmt << static_cast<int32_t>(v);
}

Formatter &operator<<(Formatter &fmt, int32_t v)
{
    CharStr<10 + 1 + 1> buf;
    int32_t rc;
    bool neg = (v < 0);
    do {
        rc = buf.write((v % 10) + '0');
        v /= 10;
        if (rc < 0) { break; }
    } while (v > 0);
    if (neg) { rc = buf.write('-'); }

    buf.reverse();
    rc = fmt.write(buf.ptr(), buf.len());

    return fmt;
}

} // namespace nel
