#ifndef NEL_DEFS_HH
#define NEL_DEFS_HH

#define NEL_WARN_UNUSED __attribute__((warn_unused_result))
#define NEL_UNUSED(arg) ((void)(arg))

namespace nel
{

typedef long unsigned int Length;
typedef long unsigned int Index;
typedef long unsigned int Count;

} // namespace nel

#endif // NEL_DEFS_HH