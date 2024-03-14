// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(NEL_DEFS_HH)
#    define NEL_DEFS_HH

#    include <stdint.h>

namespace nel
{

#    define NEL_WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#    define NEL_UNUSED(arg) ((void)(arg))

template<typename T>
constexpr void unused(T &&)
{
}

template<typename T, typename... Ts>
constexpr void unused(T &&, Ts &&...)
{
}

// arm32:
//__SIZEOF_INT = 4
//__SIZEOF_LONG_INT = 4
//__SIZEOF_PTR = 4
//__SIZE_TYPE__ == unsigned int
//__PTRDIFF_TYPE == int
// amd64:
//__SIZEOF_INT = 4
//__SIZEOF_LONG_INT = 8
//__SIZEOF_PTR = 8
//__SIZE_TYPE__ == long unsigned int
//__PTRDIFF_TYPE == long int
//__UINTPTR_TYPE == long unsigned int

// typedef long unsigned int USize; //rust: same size as void*
// typedef long signed int ISize;// rust: same size as void*
typedef __SIZE_TYPE__ USize;
typedef __PTRDIFF_TYPE__ ISize; // rust: same size as void*

typedef uint32_t Length;
typedef uint32_t Index;
typedef uint32_t Count;

#    define NEL_REQUIRES(Str, Tra) \
        static_assert(__is_base_of(Tra, Str), #Tra " interface not supported for " #Str)

} // namespace nel

#endif // NEL_DEFS_HH
