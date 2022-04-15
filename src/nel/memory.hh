#ifndef NEL_MEMORY_HH
#define NEL_MEMORY_HH

#include <nel/defs.hh>

#include <utility> // std::move, std::swap
#include <cstdint> // uint8_t

namespace nel
{

/**
 * copy memory [s,s+n) to [d,d+n)
 * Caller is required to make sure there is enough space allocated in d
 */
void memcpy(uint8_t *const d, uint8_t const *const s, Length const n) noexcept;

/**
 * set memory [d,d+n) to value s
 * Caller is required to make sure there is enough space allocated in d
 */
void memset(uint8_t *const d, uint8_t const s, Length const n) noexcept;

/**
 * move memory [s,s+n) to [d,d+n)
 * Caller is required to make sure there is enough space allocated in d
 * [s,s+n) is set to default debug pattern (0xa5)
 */
void memmove(uint8_t *const d, uint8_t *const s, Length const n) noexcept;

/**
 * bytewise swap memory [s,s+n) with [d,d+n)
 */
void memswap(uint8_t *const d, uint8_t *const s, Length const n) noexcept;

/**
 * move items [s,s+n) to [d,d+n)
 * caller must ensure there is enough space in d.
 */
template<typename T>
void memmove(T *d, T *s, Length n) noexcept
{
    for (Index i = 0; i < n; ++i) {
        d[i] = std::move(s[i]);
    }
}

/**
 * copy items [s,s+n) to [d,d+n)
 * caller must ensure there is enough space in d.
 */
template<typename T>
void memcpy(T *const d, T const *const s, Length n) noexcept
{
    for (Index i = 0; i < n; ++i) {
        d[i] = s[i];
    }
}


/**
 * set items [s,s+n) to (copy of) d
 * caller must ensure there is enough space in d.
 */
template<typename T>
void memset(T *const d, T const &s, Length n) noexcept
{
    for (Index i = 0; i < n; ++i) {
        d[i] = s;
    }
}

/**
 * swap items [s,s+n) with [d,d+n)
 */
template<typename T>
void memswap(T *const d, T *const s, Length n) noexcept
{
    for (Index i = 0; i < n; ++i) {
        std::swap(*d, *s);
    }
}

} // namespace nel

#endif // NEL_MEMORY_HH
