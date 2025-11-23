// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-

#if defined(TEST)
#    include <catch2/catch2_panic.hh>

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

#endif//defined(TEST)
