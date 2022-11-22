#ifndef LARGESTRUCT1_HH
#define LARGESTRUCT1_HH

#include <nel/log.hh>
#include <nel/memory.hh>
#include <nel/defs.hh>

typedef long unsigned int Length;
typedef long unsigned int Index;


template<Length const N>
struct U8Buf {
    private:
        uint8_t data_[N];
    public:
        // default initialises whole struct 0, which is not what's wanted
        // default I want is create with uninitialised array..
        // maybe a uninitialised ctor then?
        //U8Buf(void) = default;
        constexpr U8Buf(void)
        {}

        constexpr ~U8Buf(void) {
            nel::memset(data_, 0xa5, N);
        }

        constexpr U8Buf(U8Buf const &o) {
            nel::memcpy(data_, o.data_, N);
        }

        constexpr U8Buf(U8Buf &&o) {
            nel::memmove(data_, o.data_, N);
        }

        constexpr U8Buf &operator=(U8Buf &&o) {
            nel::memmove(data_, o.data_, N);
            return *this;
        }

        //constexpr explicit U8Buf(uint8_t const f) {
        constexpr U8Buf(uint8_t const f) {
            nel::memset(data_, f, N);
        }

    public:
        constexpr Length len(void) const {
            return N;
        }

        void fill(uint8_t f) {
            nel::memset(data_, f, N);
        }

        friend nel::Log &operator<<(nel::Log &outs, U8Buf const &val) {
            outs << "U8Buf<" << val.len() << ">{";
            if (val.len() > 0) {
                outs  << val.data_[0];
                for (Index i = 1; i < val.len(); ++i) {
                    outs << " " << val.data_[i];
                }
            }
            outs << "}";
            return outs;
        }
};

#endif//LARGESTRUCT1_HH
