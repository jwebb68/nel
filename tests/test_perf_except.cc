#include "test.hh"

#include <stdexcept>

// ======================================================
noinline int _foo_e(int c, int d, int err)
{
    if (c == err) throw std::runtime_error("foo");
    if (c == d) return c + 1;
    return _foo_e(c + 1, d, err);
}
noinline int foo_e(int c, int d, int err)
{
    try
    {
        if (c == err) throw std::runtime_error("foo");
        if (c == d) return c + 1;
        return _foo_e(c + 1, d, err);
    }
    catch (const std::runtime_error &exc)
    {
        return -1;
    }
}
