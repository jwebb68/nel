// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(NEL_PANIC_HH)
#    define NEL_PANIC_HH

#if defined(TEST)
#include <stdexcept>
#endif

namespace nel
{

#if defined(TEST)
namespace test {

struct PanicError
    : public std::runtime_error
{
    PanicError(std::string const &s)
        : std::runtime_error(s)
    {}
};

}
#endif

struct Context
{
    public:
        char const *fn_name_;
        char const *file_name_;
        int line_num_;

    public:
        constexpr Context(char const *fn_name = __builtin_FUNCTION(),
                          char const *file_name = __builtin_FILE(),
                          int line_num = __builtin_LINE())
            : fn_name_(fn_name)
            , file_name_(file_name)
            , line_num_(line_num)
        {
        }

        // public:
        //     friend nel::Log &operator<<(nel::Log &outs, Context const &v) {
        //         return  outs << v.file_name_ << ':' << v.line_num_;
        //     }
};

[[noreturn]] void panic0(Context ctx = Context());

[[noreturn]] void panic(char const *const msg, Context ctx = Context());

inline void panic_if(bool pred, char const *const msg, Context ctx = Context())
{
    if (pred) { panic(msg, ctx); }
}

inline void panic_if_not(bool pred, char const *const msg, Context ctx = Context())
{
    if (!pred) { panic(msg, ctx); }
}

[[noreturn]] void assert_fail(char const *const msg, Context ctx);

#undef assert
inline void assert(bool pred, char const *const msg, Context ctx = Context())
{
    if (!pred) { assert_fail(msg, ctx); }
}


#if defined(TEST)
#if !defined(CATCH_CONFIG_DISABLE)

#ifdef CATCH_CONFIG_PREFIX_ALL
#define CATCH_REQUIRE_PANIC(expr) CATCH_REQUIRE_THROWS_AS(expr, nel::test::PanicError)
#define CATCH_CHECK_PANIC(expr) CATCH_CHECK_THROWS_AS( expr, nel::test::PanicError)
#else//!defined(CATCH_CONFIG_PREFIX_ALL)
#define REQUIRE_PANIC(expr) REQUIRE_THROWS_AS(expr, nel::test::PanicError)
#define CHECK_PANIC(expr) CHECK_THROWS_AS( expr, nel::test::PanicError)
#endif//defined(CATCH_CONFIG_PREFIX_ALL)

#else//defined(CATCH_CONFIG_DISABLE)
#define REQUIRE_PANIC( expr) (void)(0)
#define CHECK_PANIC( expr) (void)(0)
#endif//defined(CATCH_CONFIG_DISABLE)
#endif

} // namespace nel

#endif // !defined(NEL_PANIC_HH)
