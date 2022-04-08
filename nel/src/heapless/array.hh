#ifndef NEL_HEAPLESS_ARRAY_HH
#define NEL_HEAPLESS_ARRAY_HH

namespace nel
{
namespace heapless
{

template<typename T, size_t const N>
struct Array;

} // namespace heapless
} // namespace nel

#include "iterator.hh"
#include "enumerator.hh"
#include "slice.hh"
#include "panic.hh"

namespace nel
{
namespace heapless
{

// Fixed size array.
// All items allocd when created.
// All items deletd when destroyed.
//
template<typename T, size_t const N>
struct Array {
    public:
    private:
        T values_[N];

    public:
        // Does this call dtor on each values_ entry?
        ~Array(void) = default;

        // If no init list?
        // Only if T has default cons
        // Is this default?
        constexpr Array(void) = default;

        constexpr Array(std::initializer_list<T> l)
        {
            // Interesting
            // gcc: if init-list ctor is empty, then values are created inplace .. not.
            // gcc: creates init list in temp space, then copies into required space..
            //      todo: avoid this copying.. can I get them created inplace..
            size_t i = 0;
            for (auto it = l.begin(); i < N && it != l.end(); ++it, ++i) {
                new (&values_[i]) T(std::move(*it));
            }
        }
        // If type has copy, then could fill? use slice for that..

        // No copying.
        constexpr Array(Array const &o) = delete;
        constexpr Array &operator=(Array const &o) = delete;

        // Moving allowed.
        constexpr Array(Array &&o) noexcept
        {
            for (size_t i = 0; i < N; ++i) {
                new (&values_[i]) T(std::move(o.values_[i]));
            }
        }
        constexpr Array &operator=(Array &&o) noexcept
        {
            // Expensive to move o to temp, then to swap o and this (using moves)
            // quicker to just move o into this.
            // Only because move cannot fail (and leave this destructed).
            ~Array();
            new (this) Array(std::move(o));
        }

    public:
        constexpr size_t len(void) const
        {
            return N;
        }

        // Move item in (idx).
        // Move item out (idx).
        // [const] ref item (idx)
        constexpr T &operator[](size_t idx) noexcept
        {
            nel_panic_if_not(idx < len(), "index out of range");
            return values_[idx];
        }
        constexpr T const &operator[](size_t idx) const noexcept
        {
            nel_panic_if_not(idx < len(), "index out of range");
            return values_[idx];
        }

        constexpr Slice<T> slice(void)
        {
            return Slice<T>::from(values_, len());
        }
        // constexpr Slice<T> slice(size_t b, size_t e)
        // {
        //     // Err N yet given differing range?
        //     return slice().slice(b,e);
        // }

        constexpr Slice<T const> slice(void) const
        {
            return Slice<T const>::from(values_, len());
        }
        // constexpr Slice<T const> subslice(size_t b, size_t e) const
        // {
        //     // Err N yet given differing range?
        //     return slice().slice(b,e);
        // }

        constexpr Iterator<T const> iter(void) const
        {
            return slice().iter();
        }
        constexpr Iterator<T> iter(void)
        {
            return slice().iter();
        }

        constexpr Enumerator<T const> enumerate(void) const
        {
            return slice().enumerate();
        }
        constexpr Enumerator<T> enumerate(void)
        {
            return slice().enumerate();
        }

    public:
        friend Log &operator<<(Log &outs, Array const &v) noexcept
        {
            outs << "Array<" << v.len() << ">{"
                 << "\n";
            for (size_t i = 0; i < v.len(); ++i) {
                outs << "[" << i << "]:" << v.values_[i] << "\n";
            }
            outs << "}";
            return outs;
        }
};

} // namespace heapless
} // namespace nel

#endif // NEL_HEAPLESS_ARRAY_HH
