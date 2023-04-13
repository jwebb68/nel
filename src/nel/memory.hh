// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(NEL_MEMORY_HH)
#    define NEL_MEMORY_HH

#    include <nel/defs.hh> // Length

#    include <inttypes.h> // uint8_t

#    include <cstddef> // nullptr_t

namespace nel
{

/**
 * Convert arg into a r-reference
 *
 * @param v arg to convert
 * @return r-reference to v
 *
 * mirrors c++ std definition.
 * probably not as complete as though.
 *
 * having own implementation as nel is to not need std.
 */
template<typename T>
constexpr T &&move(T &v)
{
    return static_cast<T &&>(v);
}

/**
 * Convert arg into a r-reference
 *
 * @param v arg to convert
 * @return r-reference to v
 *
 * mirrors c++ std definition.
 * probably not as complete as though.
 *
 * having own implementation as nel is to not need std.
 */
template<typename T>
constexpr T &&move(T &&v)
{
    return static_cast<T &&>(v);
}

/**
 * Convert arg into a r-reference
 *
 * @param v arg to convert
 * @return r-reference to v
 *
 * mirrors c++ std definition.
 * probably not as complete as though.
 *
 * having own implementation as nel is to not need std.
 */
template<typename T>
constexpr T &&forward(T &v)
{
    return static_cast<T &&>(v);
}

/**
 * Convert arg into a r-reference
 *
 * @param v arg to convert
 * @return r-reference to v
 *
 * mirrors c++ std definition.
 * probably not as complete as though.
 *
 * Having own implementation as nel is to not need std.
 */
template<typename T>
constexpr T &&forward(T &&v)
{
    return static_cast<T &&>(v);
}

namespace elem
{

/**
 * Wipe memory region [d, d+n) or [b, e) back to debug pattern.
 *
 * @param d destination region to wipe.
 * @param n size in elements of region to wipe.
 * @param b begin of region to wipe.
 * @param e end of region to wipe.
 *
 * @warning UB if [d, d+n) is not writable.
 * @warning Does NOT call element destructor per element.
 */
void wipe(uint8_t *const d, Length const n);

inline void wipe(uint8_t *const b, uint8_t *const e)
{
    wipe(b, e - b);
}

template<typename T>
void wipe(T *const b, T *const e)
{
    wipe(reinterpret_cast<uint8_t *>(b), reinterpret_cast<uint8_t *>(e));
}

template<typename T>
void wipe(T d[], Length n)
{
    wipe(d, &(d[n]));
}

template<typename T>
void wipe(T &d)
{
    wipe(&d, 1);
}

/**
 * Set elements [d,d+n) copy of s.
 *
 * @param d destination array to receive values.
 * @param s source value to use as template.
 * @param n number of elements to copy.
 *
 * @note Uses element copy-assn operator to copy.
 *
 * @warning UB if [d, d+n) is not writable.
 * @warning UB if [d, d+n) is not initialised.
 */
template<typename T>
void set(T d[], T const &s, Length const n)
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
 * Copy elements [s,s+n) to [d,d+n).
 *
 * @param d destination array to receive values.
 * @param s source array to provide templates.
 * @param n number of elements to copy.
 *
 * @note safe to call if d == s.
 * @note Uses element copy-assn operator to copy.
 *
 * @warning UB if [s, s+n) is not readable.
 * @warning UB if [d, d+n) is not writable.
 * @warning UB if [d, d+n) is not initialised.
 */
template<typename T>
void copy(T d[], T const s[], Length const n)
{
    // don't copy if dest is same as src.
    if (d == s) { return; }
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
 * Move elements [s,s+n) to [d,d+n).
 *
 * @param d destination array to receive values.
 * @param s source array to move from.
 * @param n number of elements to copy.
 *
 * @warning UB if [d, d+n) is not writable.
 * @warning UB if [d, d+n) is not initialised.
 * @warning UB if [s, s+n) is not readable.
 * @warning UB if [s, s+n) is not writable.
 *
 * @note safe to call if d == s.
 * @note Uses element move-assn operator to copy.
 */
template<typename T>
void move(T d[], T s[], Length const n)
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
 *
 * @param a start of first range to test.
 * @param b start of second range to test.
 * @param n length of regions to test.
 *
 * @returns true if regions are same and have same content.
 * @returns false if regions are different or have diff content.
 *
 * @note comparison will stop at first fail.
 * @note uses the element == operator.
 *
 * @warning UB if [a,a+n) region cannot be read from.
 * @warning UB if [b,b+n) region cannot be read from.
 * @warning UB if [a, a+n) is not initialised.
 * @warning UB if [b, b+n) is not initialised.
 */
template<typename T>
bool eq(T const a[], T const b[], Length const n)
{
    // if region starts are same then same..
    // saves traversing the regions.
    if (a == b) { return true; }
    // empty regions are always eq.
    if (n == 0) { return true; }
    T const *const e = a + n;
    for (; a != e; ++a) {
        if (!(*a == *b)) { return false; }
        ++b;
    }
    return true;
}

template<typename T>
bool eq(nullptr_t, T const *, Length const n)
{
    // empty regions are always eq.
    return (n == 0);
}

template<typename T>
bool eq(T const a[], nullptr_t, Length const n)
{
    return eq(nullptr, a, n);
}

template<typename T>
bool eq(nullptr_t, nullptr_t, Length const n)
{
    // empty regions are always eq.
    return (n == 0);
}

bool eq(uint8_t const *a, uint8_t const *b, Length const n);

/**
 * Compare for inequality [a,a+n) to [b,b+n) element-wise.
 *
 * @param a start of first range to test.
 * @param b start of second range to test.
 * @param n length of regions to test.
 *
 * @returns false if regions are same or have same content.
 * @returns true if regions are different and have diff content.
 *
 * @note comparison will stop at first fail.
 * @note uses the element != operator.
 *
 * @warning UB if [a, a+n) region cannot be read from.
 * @warning UB if [b, b+n) region cannot be read from.
 * @warning UB if [a, a+n) is not initialised.
 * @warning UB if [b, b+n) is not initialised.
 */
template<typename T>
bool ne(T const a[], T const b[], Length const n)
{
    if (a == b) { return false; }
    T const *const e = a + n;
    for (; a != e; ++a) {
        if (*a != *b) { return true; }
        ++b;
    }
    return false;
}

template<typename T>
bool ne(nullptr_t, T const *, Length const n)
{
    // empty regions are always eq.
    return (n != 0);
}

template<typename T>
bool ne(T const a[], nullptr_t, Length const n)
{
    return ne(nullptr, a, n);
}

template<typename T>
bool ne(nullptr_t, nullptr_t, Length const n)
{
    // empty regions are always eq.
    return (n != 0);
}

bool ne(uint8_t const *a, uint8_t const *b, Length const n);

}; // namespace elem

}; // namespace nel

#endif // defined(NEL_MEMORY_HH)
