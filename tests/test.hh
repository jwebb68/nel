#ifndef NEL_TEST_HH
#define NEL_TEST_HH

#define UNUSED(x) ((void)(x))

// to prevent clang optimising through function pointers
// to prevent clang doing tail-call-optimisations prevent recursive
// functions being TCOd.
//#define noinline __attribute__ ((noinline))
#define noinline


#endif//NEL_TEST_HH
