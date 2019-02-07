#include "test.hh"

noinline int foo_int(int c, int d, int err)
{
    if (c == err) return -1;
    if (c == d) return c + 1;
    return foo_int(c + 1, d, err);
}
