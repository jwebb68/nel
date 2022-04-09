#ifndef NEL_HEAPLESS_ARRAY_HH
#define NEL_HEAPLESS_ARRAY_HH

#include <cstddef> // size_t

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
            if (this != &o) {
                // Expensive to move o to temp, then to swap o and this (using moves)
                // quicker to just move o into this.
                // Only because move cannot fail (and leave this destructed).
                this->~Array();
                new (this) Array(std::move(o));
            }
            return *this;
        }

    public:
        static Array fill(T const &v) noexcept
        {
            Array a;
            for (size_t i = 0; i < N; ++i) {
                new (&a.values_[i]) T(v);
            }
            return a;
        }

    public:
        /**
         * Determine if the array is empty.
         *
         * @returns true if array is empty, false otherwise.
         */
        constexpr bool is_empty(void) const noexcept
        {
            return N == 0;
        }

        /**
         * Return the number of items in the array.
         *
         * @returns number of items in the array.
         */
        constexpr size_t len(void) const
        {
            return N;
        }

        /**
         * Item access in array.
         *
         * @param idx The index of the item to get.
         *
         * @returns reference to the item.
         * @warning Will panic if idx is out-of-range for array.
         */
        // as array access can fail, redo to try_get() and return v or error
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

        /**
         * Cast this array into a full slice?
         *
         * Creates a slice from the array.
         * Slice does not own the contents, array does (array still valid).
         * Slice is invalidated if Array goes out of scope/destroyed.
         *
         * @returns a slice over all of the array.
         */
        // This could be a into_slice()?, or a as_slice()? or a deref_as_slice()?
        // or a conversion func: operator Slice<T>(void)? (but I don't want it implicit/automatic
        constexpr Slice<T> slice(void)
        {
            return Slice<T>::from(values_, len());
        }
        constexpr Slice<T const> slice(void) const
        {
            return Slice<T const>::from(values_, len());
        }

    public:
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
