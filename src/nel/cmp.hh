// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(NEL_CMP_HH)
#    define NEL_CMP_HH

namespace nel
{
namespace cmp
{

template<typename T>
constexpr T min(T a, T b)
{
    return (a > b) ? b : a;
}

template<typename T>
constexpr T max(T a, T b)
{
    return (a > b) ? a : b;
}

template<typename T>
constexpr T clamp(T val, T lower, T upper)
{
    return (val < lower) ? lower : (val > upper) ? upper : val;
}

}; // namespace cmp
}; // namespace nel

#endif // defined(NEL_CMP_HH)
