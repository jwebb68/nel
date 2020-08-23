#include "panic.hh"

#include <cstdlib> // abort
#include <cstdio> // printf

namespace nel {

[[noreturn]] void abort(void) noexcept
{
    std::abort();
}


[[noreturn]] void __panic(char const *const msg, char const *filen, int lineno) noexcept
{
    fprintf(stderr, "%s:%d: %s: %s", filen, lineno, "PANIC", msg);
    abort();
}

void __panic_if(bool pred, char const *const msg, char const *filen, int lineno) noexcept
{
    if (pred) {
        __panic(msg, filen, lineno);
    }
}

}
