// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(NEL_CHARWRITE_HH)
#    define NEL_CHARWRITE_HH

namespace nel
{
struct CharWrite;
}

#    include <nel/defs.hh> // NEL_WARN_UNUSED_RESULT
#    include <stdint.h>

namespace nel
{

struct CharWrite
{
    public:
        typedef char Char;

    public:
        virtual int32_t NEL_WARN_UNUSED_RESULT write(Char c) = 0;
        virtual int32_t NEL_WARN_UNUSED_RESULT write(Char c, uint32_t n) = 0;
        virtual int32_t NEL_WARN_UNUSED_RESULT write(Char const a[], uint32_t n) = 0;
        virtual int32_t NEL_WARN_UNUSED_RESULT write(Char const *const s) = 0;
};

} // namespace nel

#endif //! defined(NEL_CHARWRITE_HH)
