#include "memory.hh"

#include "panic.hh"

#include <cstring> // std::memcpy, std::memset
#include <cstdlib> //std::free, std::malloc, std::realloc
#include <memory> // std::align
#include <cstring> // std::memmove

namespace nel {

void memcpy(uint8_t *const d, uint8_t const *const s, size_t const n) noexcept
{
    // for (size_t i=0; i < n; ++i) {
    //     d[i] = s[i];
    // }
    // uint8_t const *is = s;
    // for (uint8_t *id = d, *const e = (d + n); id != e; ++is, ++id) {
    //     *id = *is;
    // }
    std::memcpy(d, s, n);
}


void memset(uint8_t *const d, uint8_t const s, size_t const n) noexcept
{
    // for (size_t i=0; i < n; ++i) {
    //     d[i] = s;
    // }
    // for (uint8_t *id = d, *const e = (d + n); id != e; ++id) {
    //     *id = s;
    // }
    std::memset(d, s, n);
}


void memmove(uint8_t *const d, uint8_t *const s, size_t const n) noexcept
{
    // for (size_t i=0; i < n; ++i) {
    //     d[i] = s[i];
    //     s[i] = 0xa5;
    // }
    // for (uint8_t *id=d, *is=s, *const e=(d+n); id != e; ++is, ++id) {
    //     *id = *is;
    //     *is = 0xa5;
    // }
    // for (uint8_t *id = d, *is = s, *const e = (d + n); id != e; ++is, ++id) {
    //     *id = *is;
    // }
    // for (uint8_t *id = d, *is = s, *const e = (d + n); id != e; ++is, ++id) {
    //     *is = 0xa5;
    // }
    memcpy(d, s, n);
    memset(s, 0xa5, n);
}


void memswap(uint8_t *const d, uint8_t *const s, size_t const n) noexcept
{
    // for (size_t i=0; i < n; ++i) {
    //     uint8_t t = d[i];
    //     d[i] = s[i];
    //     s[i] = t;
    // }
    for (uint8_t *id = d, *is = s, *const e = (d + n); id != e; ++is, ++id) {
        uint8_t t = *id;
        *id = *is;
        *is = t;
    }
}


struct Alloc {
        size_t align_;
        uint8_t payload[1];

    public:
        static Alloc *from_payload_ptr(void *p)
        {
            if (p == nullptr) {
                return nullptr;
            }

            uint8_t *u8p = reinterpret_cast<uint8_t *>(p);
            // has previous ptr been corrupted..
            // nasty that this needs to be stored..
            // nasty, using offsetof.
            uint8_t *u8a = u8p - offsetof(Alloc, payload);
            Alloc *a = reinterpret_cast<Alloc *>(u8a);
            nel_assert((size_t)(u8p - u8a) == a->align_);
            return a;
        }


        void *to_payload_ptr(void)
        {
            return &payload;
        }
};


void free_aligned(void *p)
{
    std::free(Alloc::from_payload_ptr(p));
}


static uint8_t *as_u8ptr_mut(void *p)
{
    return reinterpret_cast<uint8_t *>(p);
}


void *realloc_aligned(void *oldp, size_t align, size_t size) noexcept
{
    Alloc *olda = Alloc::from_payload_ptr(oldp);

    // unaligned start of allocated memory.
    Alloc *newa = reinterpret_cast<Alloc *>(std::realloc(olda, size + align));
    if (newa == nullptr) {
        return newa;
    }

    void *newp;
    if (oldp != nullptr && newa == olda) {
        // reallocing and not moved in realloc.
        newp = oldp;
    } else {
        // new alloc.. or moved in realloc.
        // align to next highest al boundary.
        void *p1 = newa;
        size_t sz2 = size + align;
        uint8_t *aligneda_u8 = as_u8ptr_mut(std::align(align, size, p1, sz2));
        // TODO: handle realign fails..
        // if realign fails, object is invalid!!
        nel_assert(aligneda_u8 != nullptr);
        nel_assert((aligneda_u8 + size) <= (as_u8ptr_mut(newa) + size + align));
        if (newa != olda && aligneda_u8 != as_u8ptr_mut(newa)) {
            // if realloced and needed aligning then move data.
            // more slowness, shame malloc cannot do this when allocing.
            std::memmove(aligneda_u8, newa, size);
        }
        Alloc *aligneda = reinterpret_cast<Alloc *>(aligneda_u8);
        aligneda->align_ = align;
        newp = aligneda->to_payload_ptr();
    }
    return newp;
}


void *malloc_aligned(size_t align, size_t size) noexcept
{
    return realloc_aligned(nullptr, align, size);
}



}
