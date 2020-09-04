#include "panic.hh"

#include <cstdlib> // abort
#include <cstdio> // printf

namespace nel {

[[noreturn]] void abort(void) noexcept
{
    std::abort();
}


[[noreturn]] void panic_(char const *filen, int lineno,
                         char const *const msg) noexcept
{
    fprintf(stderr, "%s:%d: %s: %s", filen, lineno, "PANIC", msg);
    abort();
}


void panic_if_(char const *filen,  int lineno,
               char const *const msg, bool pred) noexcept
{
    if (pred) {
     panic_(filen, lineno, msg);
    }
}


void assert_(char const *filen,  int lineno,
             char const *const msg, bool pred) noexcept
{
    if (!pred) {
        fprintf(stderr, "%s:%d: %s: %s", filen, lineno, "ASSERT FAIL", msg);
        abort();
    }
}


}
