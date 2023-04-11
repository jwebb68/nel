// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(NEL_NUM_HH)
#    define NEL_NUM_HH

namespace nel
{
namespace num
{

// undefine any define for abs, we're using a c++ style instead.
// #    ifdef abs
// #        undef abs
// #    endif
template<typename T>
constexpr T abs(T v)
{
    return (v < 0) ? -v : v;
}

}; // namespace num
}; // namespace nel

#endif // !defined(NEL_NUM_HH)
