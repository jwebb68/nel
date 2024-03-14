// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(LARGESTRUCT1_HH)
#    define LARGESTRUCT1_HH

#    include <nel/log.hh>
#    include <nel/memory.hh>
#    include <nel/defs.hh>

template<nel::Length const N>
struct U8Buf
{
    private:
        uint8_t data_[N];

    public:
        // default initialises whole struct 0, which is not what's wanted
        // default I want is create with uninitialised array..
        // maybe a uninitialised ctor then?
        // U8Buf(void) = default;
        constexpr U8Buf(void) {}

        constexpr ~U8Buf(void)
        {
            nel::elem::set(data_, 0xa5, N);
        }

        constexpr U8Buf(U8Buf const &o)
        {
            nel::elem::copy(data_, o.data_, N);
        }

        constexpr U8Buf(U8Buf &&o)
        {
            nel::elem::move(data_, o.data_, N);
        }

        constexpr U8Buf &operator=(U8Buf &&o)
        {
            nel::elem::move(data_, o.data_, N);
            return *this;
        }

        // constexpr explicit U8Buf(uint8_t const f) {
        constexpr U8Buf(uint8_t const f)
        {
            nel::elem::set(data_, f, N);
        }

    public:
        constexpr nel::Length len(void) const
        {
            return N;
        }

        void fill(uint8_t f)
        {
            nel::elem::set(data_, f, N);
        }

        nel::Formatter &dbgfmt(nel::Formatter &fmt) const
        {
            fmt << "U8Buf<" << len() << ">{";
            if (len() > 0) {
                fmt << data_[0];
                for (nel::Index i = 1; i < len(); ++i) {
                    fmt << ' ' << data_[i];
                }
            }
            fmt << '}';
            return fmt;
        }
};

#endif // !defined(LARGESTRUCT1_HH)
