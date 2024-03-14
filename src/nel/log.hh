// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(NEL_LOG_HH)
#    define NEL_LOG_HH

namespace nel
{

class Log;

} // namespace nel

#    include <inttypes.h>

namespace nel
{

struct FileH: public CharWrite
{ // should be ByteWrite
    private:
        int fd_;

    public:
        constexpr FileH(int fd)
            : fd_(fd)
        {
        }

    private:
        // internal impls to stop self-use using vtable.
        int32_t NEL_WARN_UNUSED_RESULT write_(Char c);
        int32_t NEL_WARN_UNUSED_RESULT write_(Char c, uint32_t n);
        int32_t NEL_WARN_UNUSED_RESULT write_(Char const a[], uint32_t n);
        int32_t NEL_WARN_UNUSED_RESULT write_(Char const *const s);

    public:
        int32_t NEL_WARN_UNUSED_RESULT write(Char c) override
        {
            return write_(c);
        }

        int32_t NEL_WARN_UNUSED_RESULT write(Char c, uint32_t n) override
        {
            return write_(c, n);
        }

        int32_t NEL_WARN_UNUSED_RESULT write(Char const a[], uint32_t n) override
        {
            return write_(a, n);
        }

        int32_t NEL_WARN_UNUSED_RESULT write(Char const *const s) override
        {
            return write_(s);
        }
};

struct LogDest: public FileH
{
    public:
        LogDest();
};

extern Log log;

} // namespace nel

#endif // !defined(NEL_LOG_HH)
