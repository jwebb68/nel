#include "memory.hh"

namespace nel {

void memcpy(uint8_t *const d, uint8_t const *const s, size_t const n)
{
    // for (size_t i=0; i < n; ++i) {
    //     d[i] = s[i];
    // }
    uint8_t const *is = s;
    for (uint8_t *id = d, *const e = (d + n); id != e; ++is, ++id) {
        *id = *is;
    }
}
void memset(uint8_t *const d, uint8_t const s, size_t const n)
{
    // for (size_t i=0; i < n; ++i) {
    //     d[i] = s;
    // }
    for (uint8_t *id = d, *const e = (d + n); id != e; ++id) {
        *id = s;
    }
}
void memmove(uint8_t *const d, uint8_t *const s, size_t const n)
{
    // for (size_t i=0; i < n; ++i) {
    //     d[i] = s[i];
    //     s[i] = 0xa5;
    // }
    // for (uint8_t *id=d, *is=s, *const e=(d+n); id != e; ++is, ++id) {
    //     *id = *is;
    //     *is = 0xa5;
    // }
    for (uint8_t *id = d, *is = s, *const e = (d + n); id != e; ++is, ++id) {
        *id = *is;
    }
    for (uint8_t *id = d, *is = s, *const e = (d + n); id != e; ++is, ++id) {
        *is = 0xa5;
    }
}
void memswap(uint8_t *const d, uint8_t *const s, size_t const n)
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


}
