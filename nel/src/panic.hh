#ifndef NEL_PANIC_HH
#define NEL_PANIC_HH

#define NEL_NO_RETURN __attribute__((noreturn))

namespace nel {

[[noreturn]] void __panic(char const *filen, int lineno, char const *const msg) noexcept;
void __panic_if(char const *filen, int lineno, char const *const msg, bool pred) noexcept;

[[noreturn]] void abort(void) noexcept;

void __assert(char const *filen, int lineno, char const *const msg, bool pred) noexcept;
}

#define nel_panic(msg) nel::__panic(__FILE__, __LINE__, (msg))
#define nel_panic_if(expr, msg) nel::__panic_if(__FILE__, __LINE__, (msg), (expr))
#define nel_panic_ifnot(expr, msg) nel::__panic_if(__FILE__, __LINE__, (msg), !(expr))


#define nel_assert(expr) nel::__assert(__FILE__, __LINE__, #expr, (expr))


#endif//NEL_PANIC_HH
