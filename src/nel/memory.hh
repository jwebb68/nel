// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(NEL_MEMORY_HH)
#    define NEL_MEMORY_HH

#    include <nel/defs.hh> // Length

#    include <inttypes.h> // uint8_t

namespace nel
{

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

namespace elem
{
/**
 * copy elements [s,s+n) to [d,d+n)
 * Caller is required to make sure there is enough space allocated in d
 */
template<typename T>
void copy(T *d, T const *s, Length const n)
{
    T *const e = d + n;
    for (; d != e; ++d) {
        *d = *s;
        ++s;
    }
}

void copy(uint8_t *const d, uint8_t const *const s, Length const n);

#    if 0
template<typename T>
Result<void, Error> WARN_UNUSED_RESULT try_copy(T *d, T const *s, Length const n)
{
    T *const e = d + n;
    for (; d != e; ++d) {
        auto r = (*d).try_copy_from(*s);
        if (r.is_err()) { return r; }
        ++s;
    }
    return Result<void, Error>::Ok();
}
#    endif

/**
 * set elements [d,d+n) to value s
 * Caller is required to make sure there is enough space allocated in d
 */
template<typename T>
void set(T *d, T const &s, Length const n)
{
    T *const e = d + n;
    for (; d != e; ++d) {
        *d = s;
    }
}

void set(uint8_t *const d, uint8_t const s, Length const n);

#    if 0
template<typename T>
Result<void, Error> WARN_UNUSED_RESULT try_set(T *d, T const &s, Length const n)
{
    T *const e = d + n;
    for (; d != e; ++d) {
        auto r = (*d).try_copy_from(f);
        if (r.is_err()) { return r; }
    }
    return Result<void, Error>::Ok();
}
#    endif

/**
 * wipe memory back to debug pattern
 * Caller is required to make sure there is enough space allocated in d
 */
void wipe(uint8_t *const d, Length const n);

inline void wipe(void *const d, Length n)
{
    wipe(static_cast<uint8_t *>(d), n);
}

/**
 * move elements [s,s+n) to [d,d+n)
 * Caller is required to make sure there is enough space allocated in d
 * [s,s+n) is set to default debug pattern (0xa5)
 */
template<typename T>
void move(T *d, T *s, Length const n)
{
    // don't move if dest is same as src.
    if (d == s) { return; }
    T *const e = d + n;
    for (; d != e; ++d) {
        *d = nel::move(*s);
        ++s;
    }
}

void move(uint8_t *const d, uint8_t *const s, Length const n);

/**
 * Compare for equality [a,a+n) to [b,b+n) element-wise.
 * Caller is required to make sure a and b are valid for n
 * Stops at first fail.
 */
template<typename T>
bool eq(T const *a, T const *b, Length const n)
{
    T const *const e = a + n;
    for (; a != e; ++a) {
        if (*a != *b) { return false; }
        ++b;
    }
    return true;
}

bool eq(uint8_t const *a, uint8_t const *b, Length const n);

/**
 * Compare for inequality [a,a+n) to [b,b+n) element-wise.
 * Caller is required to make sure a and b are valid for n
 * Stops at first fail.
 */
template<typename T>
bool ne(T const *a, T const *b, Length const n)
{
    T *const e = a + n;
    for (; a != e; ++a) {
        if (*a == *b) { return false; }
        ++b;
    }
    return true;
}

bool ne(uint8_t const *a, uint8_t const *b, Length const n);

#    if 0
/**
 * swap items [s,s+n) with [d,d+n)
 */
template<typename T>
void swap(T &d, T &s)
{
    d.swap(s);
}

template<typename T>
void swap(T *const d, T *const s, Length n)
{
    for (Index i = 0; i < n; ++i) {
        swap(*d, *s);
    }
}

/**
 * byte-wise swap memory [s,s+n) with [d,d+n)
 */
void swap(uint8_t *const d, uint8_t *const s, Length const n);
#    endif

}; // namespace elem

}; // namespace nel

#endif // defined(NEL_MEMORY_HH)
