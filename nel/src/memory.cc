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
    // for (size_t i=0; i < n; ++i) {
    //     uint8_t t = s[i];
    //     s[i] = 0xa5;
    //     d[i] = t;
    // }
    // for (uint8_t *id=d, *is=s, *const e=(d+n); id != e; ++is, ++id) {
    //     *id = *is;
    //     *is = 0xa5;
    // }
    // could use memmove followed by memset
    // but this would be bad for the cache
    // in that move is read(addr1) + write(addr2);
    // and set is write(addr1).
    // note the toggling of addr1 read(a1), write(a2), write(a1)
    // whereas this may be better: read(a1), write(a1), write(a2)
    // but it won't get optimised into simple instructions.
    for (uint8_t *id=d, *is=s, *const e=(d+n); id != e; ++is, ++id) {
        uint8_t t = *is;
        *is = 0xa5;
        *id = t;
    }
    // for (uint8_t *id = d, *is = s, *const e = (d + n); id != e; ++is, ++id) {
    //     *id = *is;
    // }
    // for (uint8_t *id = d, *is = s, *const e = (d + n); id != e; ++is, ++id) {
    //     *is = 0xa5;
    // }
    // memcpy(d, s, n);
    // memset(s, 0xa5, n);
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


void *realloc_aligned(void *old_p, size_t align, size_t size) noexcept
{
    Alloc *old_a = Alloc::from_payload_ptr(old_p);

    // unaligned start of allocated memory.
    Alloc *new_a = reinterpret_cast<Alloc *>(std::realloc(old_a, size + align));
    if (new_a == nullptr) {
        return new_a;
    }

    void *new_p;
    if (old_p != nullptr && new_a == old_a) {
        // reallocing and not moved in realloc.
        new_p = old_p;
    } else {
        // new alloc.. or moved in realloc.
        // align to next highest al boundary.
        void *p1 = new_a;
        size_t sz2 = size + align;
        uint8_t *aligneda_u8 = as_u8ptr_mut(std::align(align, size, p1, sz2));
        // TODO: handle realign fails..
        // if realign fails, object is invalid!!
        nel_assert(aligneda_u8 != nullptr);
        nel_assert((aligneda_u8 + size) <= (as_u8ptr_mut(new_a) + size + align));
        if (new_a != old_a && aligneda_u8 != as_u8ptr_mut(new_a)) {
            // if realloc'd and needed aligning then move data.
            // more slowness, shame malloc cannot do this when allocing.
            std::memmove(aligneda_u8, new_a, size);
        }
        Alloc *aligned_a = reinterpret_cast<Alloc *>(aligneda_u8);
        aligned_a->align_ = align;
        new_p = aligned_a->to_payload_ptr();
    }
    return new_p;
}


void *malloc_aligned(size_t align, size_t size) noexcept
{
    return realloc_aligned(nullptr, align, size);
}



}
