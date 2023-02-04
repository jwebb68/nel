// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(NEL_DEFS_HH)
#    define NEL_DEFS_HH

namespace nel
{

#    define NEL_WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#    define NEL_UNUSED(arg) ((void)(arg))

typedef long unsigned int Length;
typedef long unsigned int Index;
typedef long unsigned int Count;

} // namespace nel

#endif // NEL_DEFS_HH
