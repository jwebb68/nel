#ifndef NEL_PANIC_HH
#define NEL_PANIC_HH

#define NEL_NO_RETURN __attribute__((noreturn))

namespace nel {

[[noreturn]] void __panic(char const *const msg, char const *filen, int lineno) noexcept;
void __panic_if(bool pred, char const *const msg, char const *filen, int lineno) noexcept;

[[noreturn]] void abort(void) noexcept;

}

#define nel_panic(msg) nel::__panic((msg), __FILE__, __LINE__)
#define nel_panic_if(expr, msg) nel::__panic_if((expr), (msg), __FILE__, __LINE__)
#define nel_panic_ifnot(expr, msg) nel::__panic_if(!(expr), (msg), __FILE__, __LINE__)

#endif//NEL_PANIC_HH
