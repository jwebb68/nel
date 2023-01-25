// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#ifndef NEL_PANIC_HH
#define NEL_PANIC_HH

#define NEL_NO_RETURN __attribute__((noreturn))

namespace nel
{

[[noreturn]] void panic_(char const *file_name, int line_num, char const *const msg);

void panic_if_(char const *file_name, int line_num, char const *const msg, bool pred);

[[noreturn]] void abort(void);

void assert_(char const *file_name, int line_num, char const *const msg, bool pred);

} // namespace nel

#define nel_panic(msg) nel::panic_(__FILE__, __LINE__, (msg))

#define nel_panic_if(expr, msg) nel::panic_if_(__FILE__, __LINE__, (msg), (expr))

#define nel_panic_if_not(expr, msg) nel::panic_if_(__FILE__, __LINE__, (msg), !(expr))

#define nel_assert(expr) nel::assert_(__FILE__, __LINE__, #expr, (expr))

#endif // NEL_PANIC_HH
