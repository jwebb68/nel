// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(CATCH2_PANIC_HH)
#    define CATCH2_PANIC_HH

#    if defined(TEST)
#        include <stdexcept>

namespace test
{

struct PanicError: public std::runtime_error
{
        PanicError(std::string const &s)
            : std::runtime_error(s)
        {
        }
};

} // namespace test

#        if !defined(CATCH_CONFIG_DISABLE)

#            ifdef CATCH_CONFIG_PREFIX_ALL
#                define CATCH_REQUIRE_PANIC(expr) \
                    CATCH_REQUIRE_THROWS_AS(expr, ::test::PanicError)
#                define CATCH_CHECK_PANIC(expr) CATCH_CHECK_THROWS_AS(expr, ::test::PanicError)
#            else //! defined(CATCH_CONFIG_PREFIX_ALL)
#                define REQUIRE_PANIC(expr) REQUIRE_THROWS_AS(expr, ::test::PanicError)
#                define CHECK_PANIC(expr) CHECK_THROWS_AS(expr, ::test::PanicError)
#            endif // defined(CATCH_CONFIG_PREFIX_ALL)
#        else // defined(CATCH_CONFIG_DISABLE)
#            define REQUIRE_PANIC(expr) (void)(0)
#            define CHECK_PANIC(expr) (void)(0)
#        endif // defined(CATCH_CONFIG_DISABLE)

#    endif//defined(TEST)

#endif//!defined(CATCH2_PANIC_HH)
