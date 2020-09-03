#include "panic.hh"

#include <cstdlib> // abort
#include <cstdio> // printf

namespace nel {

[[noreturn]] void abort(void) noexcept
{
    std::abort();
}


[[noreturn]] void __panic(char const *filen, int lineno,
                          char const *const msg) noexcept
{
    fprintf(stderr, "%s:%d: %s: %s", filen, lineno, "PANIC", msg);
    abort();
}


void __panic_if(char const *filen,  int lineno,
                char const *const msg, bool pred) noexcept
{
    if (pred) {
        __panic(filen, lineno, msg);
    }
}

void __assert(char const *filen,  int lineno,
                char const *const msg, bool pred) noexcept
{
    if (!pred) {
        fprintf(stderr, "%s:%d: %s: %s", filen, lineno, "ASSERT FAIL", msg);
        abort();
    }
}


}
