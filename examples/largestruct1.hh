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
            nel::elem::wipe(data_, N);
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
            if (this != &o) { nel::elem::move(data_, o.data_, N); }
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

        constexpr void fill(uint8_t f)
        {
            nel::elem::set(data_, f, N);
        }

        friend nel::Log &operator<<(nel::Log &outs, U8Buf const &val)
        {
            outs << "U8Buf<" << val.len() << ">{";
            if (val.len() > 0) {
                outs << val.data_[0];
                for (nel::Index i = 1; i < val.len(); ++i) {
                    outs << ' ' << val.data_[i];
                }
            }
            outs << '}';
            return outs;
        }
};

#endif // !defined(LARGESTRUCT1_HH)
