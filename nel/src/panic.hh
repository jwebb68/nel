#ifndef NEL_PANIC_HH
#define NEL_PANIC_HH

#define NEL_NO_RETURN __attribute__((noreturn))

namespace nel {

[[noreturn]] void panic_(char const *filen, int lineno,
                          char const *const msg) noexcept;


void panic_if_(char const *filen, int lineno, char const *const msg,
                bool pred) noexcept;


[[noreturn]] void abort(void) noexcept;


void assert_(char const *filen, int lineno, char const *const msg,
              bool pred) noexcept;

}

#define nel_panic(msg) nel::panic_(__FILE__, __LINE__, (msg))


#define nel_panic_if(expr, msg) nel::panic_if_(__FILE__, __LINE__, (msg), (expr))


#define nel_panic_ifnot(expr, msg) nel::panic_if_(__FILE__, __LINE__, (msg), !(expr))


#define nel_assert(expr) nel::assert_(__FILE__, __LINE__, #expr, (expr))


#endif//NEL_PANIC_HH
