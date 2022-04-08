#include "panic.hh"

#include <cstdio> // printf
#include <cstdlib> // abort

namespace nel
{

[[noreturn]] void abort(void) noexcept
{
    std::abort();
}

[[noreturn]] void panic_(char const *file_name, int line_num, char const *const msg) noexcept
{
    fprintf(stderr, "%s:%d: %s: %s", file_name, line_num, "PANIC", msg);
    abort();
}

void panic_if_(char const *file_name, int line_num, char const *const msg, bool pred) noexcept
{
    if (pred) { panic_(file_name, line_num, msg); }
}

void assert_(char const *file_name, int line_num, char const *const msg, bool pred) noexcept
{
    if (!pred) {
        fprintf(stderr, "%s:%d: %s: %s", file_name, line_num, "ASSERT FAIL", msg);
        abort();
    }
}

} // namespace nel
