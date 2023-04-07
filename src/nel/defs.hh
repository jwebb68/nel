// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(NEL_DEFS_HH)
#    define NEL_DEFS_HH

namespace nel
{

#    define NEL_WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#    define NEL_UNUSED(arg) ((void)(arg))

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

typedef long unsigned int Length;
typedef long unsigned int Index;
typedef long unsigned int Count;

} // namespace nel

#endif // NEL_DEFS_HH
