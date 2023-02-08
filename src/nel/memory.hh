// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(NEL_MEMORY_HH)
#    define NEL_MEMORY_HH

#    include <nel/defs.hh> // Length
#    include <inttypes.h> // uint8_t

namespace nel
{

/**
 * copy memory [s,s+n) to [d,d+n)
 * Caller is required to make sure there is enough space allocated in d
 */
void memcpy(uint8_t *const d, uint8_t const *const s, Length const n);

/**
 * set memory [d,d+n) to value s
 * Caller is required to make sure there is enough space allocated in d
 */
void memset(uint8_t *const d, uint8_t const s, Length const n);

/**
 * move memory [s,s+n) to [d,d+n)
 * Caller is required to make sure there is enough space allocated in d
 * [s,s+n) is set to default debug pattern (0xa5)
 */
void memmove(uint8_t *const d, uint8_t *const s, Length const n);

/**
 * byte-wise swap memory [s,s+n) with [d,d+n)
 */
void memswap(uint8_t *const d, uint8_t *const s, Length const n);

/**
 * move items [s,s+n) to [d,d+n)
 * caller must ensure there is enough space in d.
 */
template<typename T>
void memmove(T *d, T *s, Length n)
{
    for (Index i = 0; i < n; ++i) {
        d[i] = move(s[i]);
    }
}

/**
 * copy items [s,s+n) to [d,d+n)
 * caller must ensure there is enough space in d.
 */
template<typename T>
void memcpy(T *const d, T const *const s, Length n)
{
    for (Index i = 0; i < n; ++i) {
        d[i] = s[i];
    }
}

template<typename T>
bool try_memcpy(T *const d, T const *const s, Length n)
{
    for (Index i = 0; i < n; ++i) {
        auto r = T::try_copy(s[i]);
        bool ok;
        ok = !r.is_err();
        if (ok) {
            d[i] = r.unwrap();
        } else {
            for (Index j = i; i != 0; --i) {
                d[i - 1].~T();
            }
            return false;
        }
    }
    return true;
}

/**
 * set items [s,s+n) to (copy of) d
 * caller must ensure there is enough space in d.
 */
template<typename T>
void memset(T *const d, T const &s, Length n)
{
    for (Index i = 0; i < n; ++i) {
        d[i] = s;
    }
}

template<typename T>
bool try_memset(T *const d, T const &s, Length n)
{
    for (Index i = 0; i < n; ++i) {
        auto r = T::try_copy(s);
        bool ok;
        ok = !r.is_err();
        if (ok) {
            d[i] = r.unwrap();
        } else {
            for (Index j = i; i != 0; --i) {
                d[i - 1].~T();
            }
            return false;
        }
    }
    return true;
}

/**
 * swap items [s,s+n) with [d,d+n)
 */
template<typename T>
void swap(T &d, T &s)
{
    d.swap(s);
}

template<typename T>
void memswap(T *const d, T *const s, Length n)
{
    for (Index i = 0; i < n; ++i) {
        swap(*d, *s);
    }
}

template<typename T>
constexpr T &&move(T &v)
{
    return static_cast<T &&>(v);
}

template<typename T>
constexpr T &&move(T &&v)
{
    return static_cast<T &&>(v);
}

template<typename T>
constexpr T &&forward(T &v)
{
    return static_cast<T &&>(v);
}

template<typename T>
constexpr T &&forward(T &&v)
{
    return static_cast<T &&>(v);
}

}; // namespace nel

#endif // defined(NEL_MEMORY_HH)
