#ifndef NEL_MEMORY_HH
#define NEL_MEMORY_HH

#include <nel/defs.hh>

#include <utility> // std::move, std::swap
#include <cstddef> // size_t
#include <cstdint> // uint8_t

namespace nel
{

void memcpy(uint8_t *const d, uint8_t const *const s, size_t const n) noexcept;
void memset(uint8_t *const d, uint8_t const s, size_t const n) noexcept;
void memmove(uint8_t *const d, uint8_t *const s, size_t const n) noexcept;
void memswap(uint8_t *const d, uint8_t *const s, size_t const n) noexcept;

template<typename T>
void memmove(T *d, T *s, size_t n) noexcept
{
    for (size_t i = 0; i < n; ++i) {
        d[i] = std::move(s[i]);
    }
}

template<typename T>
void memcpy(T *const d, T const *const s, size_t n) noexcept
{
    for (size_t i = 0; i < n; ++i) {
        d[i] = s[i];
    }
}

template<typename T>
void memset(T *const d, T const &s, size_t n) noexcept
{
    for (size_t i = 0; i < n; ++i) {
        d[i] = s;
    }
}

template<typename T>
void memswap(T *const d, T *const s, size_t n) noexcept
{
    for (size_t i = 0; i < n; ++i) {
        std::swap(*d, *s);
    }
}

} // namespace nel

#endif // NEL_MEMORY_HH
