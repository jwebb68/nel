// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(NEL_CHARSTR_HH)
#    define NEL_CHARSTR_HH

#    include <stdint.h>

namespace nel
{

template<uint32_t const N>
struct CharStr;

}

#    include <nel/charwrite.hh>
#    include <nel/formatter.hh>
#    include <nel/slice.hh>

namespace nel
{

template<uint32_t const N>
struct CharStr: public CharWrite
{
    public:
        typedef char Char;

    private:
        Char buf_[N];
        uint32_t len_;

        constexpr CharStr(uint32_t)
            : len_(0)
        {
        }

    public:
        constexpr CharStr() = default;
        // constexpr CharStr() : len_(0) {}

    public:
        constexpr bool is_empty() const
        {
            return len_ = 0;
        }

        constexpr static CharStr empty()
        {
            return CharStr(0);
        }

    public:
        // constexpr int32_t NEL_WARN_UNUSED_RESULT push(CharStr &);

        void reverse()
        {
            Char *b = &buf_[0];
            Char *e = &buf_[len_];
            for (; b < e; b += 1, e -= 1) {
                Char *e1 = e - 1;
                // swap(b, e1);
                Char t = *b;
                *b = *e1;
                *e1 = t;
            }
        }

    public:
        constexpr Char *ptr()
        {
            return &buf_[0];
        }

        constexpr Char const *ptr() const
        {
            return &buf_[0];
        }

        constexpr uint32_t len() const
        {
            return len_;
        }

        constexpr uint32_t cap() const
        {
            return N;
        }

    public:
        Slice<Char> slice()
        {
            return Slice(ptr(), len());
        }

        Slice<Char const> slice() const
        {
            return Slice(ptr(), len());
        }

    public:
        int32_t NEL_WARN_UNUSED_RESULT push(Char c)
        {
            if (cap() <= len()) { return -1; }
            buf_[len_] = c;
            len_ += 1;
            return 1;
        }

        int32_t NEL_WARN_UNUSED_RESULT push(Char c, uint32_t n)
        {
            if (cap() <= len()) { return -1; }
            auto free = cap() - len();
            if (n > free) { n = free; }
            nel::elem::set(&buf_[len_], c, n);
            len_ += n;
            return n;
        }

        int32_t NEL_WARN_UNUSED_RESULT push(Char const a[], uint32_t n)
        {
            if (cap() <= len()) { return -1; }
            auto free = cap() - len();
            if (n > free) { n = free; }
            nel::elem::copy(&buf_[len_], a, n);
            len_ += n;
            return n;
        }

        int32_t NEL_WARN_UNUSED_RESULT push(Char const *const s)
        {
            Char const *i = s;
            for (; *i != '\0'; i += 1)
                ;
            return push(s, i - s);
        }

    public:
        int32_t NEL_WARN_UNUSED_RESULT write(Char c) override
        {
            return push(c);
        }

        int32_t NEL_WARN_UNUSED_RESULT write(Char c, uint32_t n) override
        {
            return push(c, n);
        }

        int32_t NEL_WARN_UNUSED_RESULT write(Char const a[], uint32_t n) override
        {
            return push(a, n);
        }

        int32_t NEL_WARN_UNUSED_RESULT write(Char const *const s) override
        {
            return push(s);
        }

    public:
        friend Formatter &fmt(Formatter &fmt, CharStr const &v)
        {
            fmt.write(ptr(), len());
            return fmt;
        }

        friend Formatter &dbgfmt(Formatter &fmt, CharStr const &v)
        {
            fmt.write('"');
            // embedded codes, \n \r \t etc need to be converted to their
            // debug representations
            Char const *i = &v.buf_[0];
            Char const *const e = &v.buf_[v.len_];

            Char const *p = i;
            for (; i != e; i += 1) {
                Char c = *i;
                // TODO: handle other codes esp unicode ones.
                switch (c) {
                    case '\0':
                        fmt.write(p, i - p);
                        p = i;
                        fmt.write("\\0");
                        break;
                    case '\n':
                        fmt.write(p, i - p);
                        p = i;
                        fmt.write("\\n");
                        break;
                    case '\r':
                        fmt.write(p, i - p);
                        p = i;
                        fmt.write("\\r");
                        break;
                    case '\t':
                        fmt.write(p, i - p);
                        p = i;
                        fmt.write("\\t");
                        break;
                    default:
                        break;
                }
            }
            fmt.write(p, i - p);
            fmt.write('"');
            return fmt;
        }
};

} // namespace nel

#endif //! defined(NEL_CHARSTR_HH)
