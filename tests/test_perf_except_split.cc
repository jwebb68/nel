#include "test.hh"

#include <stdexcept>


noinline int _foo_es(int c, int d, int err)
{
    try
    {
        if (c == err) throw std::runtime_error("foo");
        if (c == d) return c + 1;
        return _foo_es(c + 1, d, err);
    }
    catch (const std::runtime_error &exc)
    {
        throw;
    }
}
noinline int foo_es(int c, int d, int err)
{
    try
    {
        if (c == err) throw std::runtime_error("foo");
        if (c == d) return c + 1;
        return _foo_es(c + 1, d, err);
    }
    catch (const std::runtime_error &exc)
    {
        return -1;
    }
}
