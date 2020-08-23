#ifndef LARGESTRUCT1_HH
#define LARGESTRUCT1_HH

#include "defs.hh"
#include "log.hh"
#include "memory.hh"

template<size_t N>
struct U8Buf {
    private:
        uint8_t data_[N];
    public:
        // default initialises whole struct 0, which is not what's wanted
        // default I want is create with uninitialised array..
        // maybe a uninitialised ctor then?
        //U8Buf(void) = default;
        U8Buf(void)
        {}


        U8Buf(U8Buf const &o) {
            nel::memcpy(data_, o.data_, N);
        }

        U8Buf(U8Buf &&o) {
            nel::memmove(data_, o.data_, N);
        }

        U8Buf &operator=(U8Buf &&o) {
            nel::memmove(data_, o.data_, N);
            return *this;
        }

        explicit U8Buf(uint8_t const f) {
            nel::memset(data_, f, N);
        }

    public:
        // U8Slice slice(size_t b, size_t e) {
        //     assert(b < e);
        //     assert(e < N);
        //     U8Slice(*this,
        // }
        size_t len(void) const {
            return N;
        }

        void fill(uint8_t f) {
            nel::memset(data_, f, N);
        }

        friend nel::Log &operator<<(nel::Log &outs, U8Buf const &val) {
            NEL_UNUSED(val);
            outs << "U8Buf<" << N << ">";
            return outs;
        }
};

#endif//LARGESTRUCT1_HH
