// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-

#if defined(TEST)

#    include <nel/panic.hh>

#    include <catch2/catch2_panic.hh> // test::PanicError
#    include <sstream>


namespace nel
{

[[noreturn]] void panic(Context const &ctx)
{
    std::stringstream ss;
    ss << "panic in " << ctx.fn_name_ << ' ' << ctx.file_name_ << ':' << ctx.line_num_ << '\n';
    //ss << prefix << '\n';
    throw ::test::PanicError(ss.str());
}

[[noreturn]] void panic(char const *msg, Context const &ctx)
{
    std::stringstream ss;
    ss << "panic in " << ctx.fn_name_ << ' ' << ctx.file_name_ << ':' << ctx.line_num_ << '\n';
    ss << msg << '\n';
    throw ::test::PanicError(ss.str());
}

}

#endif//defined(TEST)
