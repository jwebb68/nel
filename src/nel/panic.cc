// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include <nel/panic.hh>

#if !defined(TEST)
#include <cstdio> // printf
#include <cstdlib> // abort
#endif

namespace nel
{
#if !defined(TEST)
[[noreturn]] void abort(void)
{
    std::abort();
}

[[noreturn]] void panic(Context const &ctx)
{
    fprintf(stderr, "\n%s:%d:[%s]: %s\n", ctx.file_name_, ctx.line_num_, ctx.fn_name_, "PANIC");
    abort();
}

[[noreturn]] void panic(char const *const msg, Context const &ctx)
{
    fprintf(stderr,
            "\n%s:%d:[%s]: %s: %s\n",
            ctx.file_name_,
            ctx.line_num_,
            ctx.fn_name_,
            "PANIC",
            msg);
    abort();
}
#endif

#if !defined(TEST)
[[noreturn]] void assert_fail(Context const &ctx)
{
    fprintf(stderr,
            "\n%s:%d:[%s]: %s\n",
            ctx.file_name_,
            ctx.line_num_,
            ctx.fn_name_,
            "ASSERT FAIL");
    abort();
}

[[noreturn]] void assert_fail(char const *const msg, Context const &ctx)
{
    fprintf(stderr,
            "\n%s:%d:[%s]: %s: %s\n",
            ctx.file_name_,
            ctx.line_num_,
            ctx.fn_name_,
            "ASSERT FAIL",
            msg);
    abort();
}
#endif

} // namespace nel
