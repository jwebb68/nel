// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(NEL_TYPES_HH)
#    define NEL_TYPES_HH

namespace nel
{

// because specialising struct templates for void is a sodding pain.
// so use a 'Void' type.
struct Void {
    public:
        friend constexpr bool operator==(Void const &/*lhs*/, Void const &/*rhs*/)
        {
            return true;
        }
        friend constexpr bool operator!=(Void const &/*lhs*/, Void const &/*rhs*/)
        {
            return false;
        }
};


// literals for int types.
// will suffice until actual structs are brought into play.
// until then, use builtin types (not great as too many implicit conversions).
consteval uint8_t operator ""_u8(unsigned long long int v)
{
    return v;
}

consteval uint16_t operator ""_u16(unsigned long long int v)
{
    return v;
}

consteval uint32_t operator ""_u32(unsigned long long int v)
{
    return v;
}

consteval uint64_t operator ""_u64(unsigned long long int v)
{
    return v;
}

consteval int8_t operator ""_i8(unsigned long long int v)
{
    return v;
}

consteval int16_t operator ""_i16(unsigned long long int v)
{
    return v;
}

consteval int32_t operator ""_i32(unsigned long long int v)
{
    return v;
}

consteval int64_t operator ""_i64(unsigned long long int v)
{
    return v;
}

consteval float operator ""_f32(long double v)
{
    return v;
}

consteval double operator ""_f64(long double v)
{
    return v;
}

}

#endif//!defined(NEL_TYPES_HH)
