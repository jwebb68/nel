// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include <nel/panic.hh>

#if defined(TEST)
#    include <sstream>
#endif

#include <cstdio> // printf
#include <cstdlib> // abort

namespace nel
{

#if defined(TEST)
[[noreturn]] inline void _panic(Context const &ctx)
{
    std::stringstream ss;
    ss << "panic in " << ctx.fn_name_ << ' ' << ctx.file_name_ << ':' << ctx.line_num_ << '\n';
    throw nel::test::PanicError(ss.str());
}

[[noreturn]] inline void _panic(Context const &ctx, const char *prefix, std::string expr)
{
    std::stringstream ss;
    ss << "panic in " << ctx.fn_name_ << ' ' << ctx.file_name_ << ':' << ctx.line_num_ << '\n';
    ss << prefix << ": " << expr << '\n';
    throw nel::test::PanicError(ss.str());
}
#endif

#if !defined(TEST)
[[noreturn]] void abort(void);

[[noreturn]] void abort(void)
{
    std::abort();
}
#endif

[[noreturn]] void panic0(Context const &ctx)
{
#if defined(TEST)
    _panic(ctx);
#else
    fprintf(stderr, "\n%s:%d:[%s]: %s\n", ctx.file_name_, ctx.line_num_, ctx.fn_name_, "PANIC");
    abort();
#endif
}

[[noreturn]] void panic(char const *const msg, Context const &ctx)
{
#if defined(TEST)
    _panic(ctx, "", msg);
#else
    fprintf(stderr,
            "\n%s:%d:[%s]: %s: %s\n",
            ctx.file_name_,
            ctx.line_num_,
            ctx.fn_name_,
            "PANIC",
            msg);
    abort();
#endif
}

[[noreturn]] void assert_fail(char const *const msg, Context const &ctx)
{
#if defined(TEST)
    _panic(ctx, "ASSERT FAIL", msg);
#else
    fprintf(stderr,
            "\n%s:%d:[%s]: %s: %s\n",
            ctx.file_name_,
            ctx.line_num_,
            ctx.fn_name_,
            "ASSERT FAIL",
            msg);
    abort();
#endif
}

} // namespace nel
