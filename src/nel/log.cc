// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include <nel/log.hh>

#include <unistd.h>

namespace nel
{

int32_t NEL_WARN_UNUSED_RESULT FileH::write_(Char c)
{
    ssize_t rc = ::write(fd_, &c, 1);
    return rc;
}

int32_t NEL_WARN_UNUSED_RESULT FileH::write_(Char c, uint32_t n)
{
    uint32_t l = 0;
    for (; l < n;) {
        int32_t rc = write_(c);
        l += 1;
        if (rc <= 0) {
            l = rc;
            break;
        }
    }
    return l;
}

int32_t NEL_WARN_UNUSED_RESULT FileH::write_(Char const a[], uint32_t n)
{
    return ::write(fd_, a, n);
}

int32_t NEL_WARN_UNUSED_RESULT FileH::write_(Char const *const s)
{
    Char const *i = s;
    for (; *i != '\0'; i += 1)
        ;
    return write_(s, i - s);
}

LogDest::LogDest()
    : FileH(STDERR_FILENO)
{
}

LogDest log;

} // namespace nel
