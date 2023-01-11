#ifndef NEL_HEAPLESS_ARRAY_HH
#define NEL_HEAPLESS_ARRAY_HH

#include <nel/defs.hh> //NEL_UNUSED, Length

namespace nel
{
namespace heapless
{

template<typename T, Length const N>
struct Array;

} // namespace heapless
} // namespace nel

#include <nel/iterator.hh>
#include <nel/slice.hh>
#include <nel/log.hh>
#include <nel/panic.hh>

namespace nel
{
namespace heapless
{

/**
 * Fixed size array, owning the values
 * All items allocated when created.
 * All items deleted when destroyed.
 * cannot have size changed once created
 * All elements are initialised (this may be an issue)
 * have safe access to elems
 * can subslice array, safely
 * can iter array
 */
template<typename T, Length const N>
struct Array {
    public:
    private:
        // default ctor does not init this array.
        // does this class follow suite? nope, expects these to be initialised.
        T values_[N];

    private:
        // no default construction
        // but needed internally..
        constexpr Array(void) = default;

        constexpr Array(std::initializer_list<T> l)
        {
            // Interesting
            // gcc: if init-list ctor is empty, then values are created inplace .. not.
            // gcc: creates init list in temp space, then copies into required space..
            //      todo: avoid this copying.. can I get them created inplace..
            Index i = 0;
            for (auto it = l.begin(); i < N && it != l.end(); ++it, ++i) {
                // TODO: init list elements want to be copied, not moved..
                new (&values_[i]) T(std::move(*it));
            }
        }


        // If type has copy, then could fill? use slice for that..

        // fill constructor
        // TODO:  what if copy of T fails..? try_fill? how?
        Array(T const &v)
        {
            iter().for_each([&](T &e) { new (&e) T(v); });
        }

    public:
        /**
         * Destroy array and it's elements.
         */
        ~Array(void) = default;

        // No copying.
        constexpr Array(Array const &o) = delete;
        constexpr Array &operator=(Array const &o) = delete;

        // Moving allowed.
        constexpr Array(Array &&o) noexcept
        {
            for (Index i = 0; i < N; ++i) {
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
        // heapless arrays cannot be created 'empty'

        /**
         * Initialise an array with given value.
         * @return array filled with copies of value.
         * @warning UB if copying of v fails.
         */
        static Array filled(T const &v)
        {
            Array a = Array(v);
            return a;
        }

#if 0
        /**
         * Atempt to create and Initialise an array with given value.
         * @return on success: array filled with copies of value.
         * @return on fail: None
         */
        static constexpr Optional<Array> try_filled(T const &v)
        {
            return Array(v);
        }
#endif

        /**
         * Attempt to create an array from initialiser list
         *
         * @param l initialiser list to use
         * @return on success, an Optional::Some holding the created array.
         * @return on fail: Optional::None
         */
        // possibly experimental.
        // array create using init lists
        // want moving not copying.
        // want copying but not via ctor (may not be poss), so it becomes a try_ returning an err.
        // If type has copy, then could fill? use slice for that..
        static constexpr Optional<Array> try_from(std::initializer_list<T> l) noexcept
        {
            // TODO: can the array create be made inplace into the optional?
            // prob only if initlist ctor is public, which I don't want.
            return (l.size() != N) ? None : Some(Array(l));
        }

    public:
        /**
         * Determine if the array is empty.
         *
         * @returns true if array is empty, false otherwise.
         */
        constexpr bool is_empty(void) const noexcept
        {
            return len() == 0;
        }

        /**
         * Return the number of items in the array.
         *
         * @returns number of items in the array.
         */
        constexpr Length len(void) const
        {
            return N;
        }

        // Move item in (idx).
        // Move item out (idx).
        // [const] ref item (idx)

#if 0
        /**
         * Item access in array.
         *
         * @param idx The index of the item to get.
         *
         * @returns reference to the item.
         * @warning Will panic if idx is out-of-range for array.
         */
        // as array access can fail, redo to try_get() and return v or error
        constexpr T &operator[](Index idx) noexcept
        {
            return slice()[idx];
        }
        constexpr T const &operator[](Index idx) const noexcept
        {
            return slice()[idx];
        }
#endif

        /**
         * Return a reference to the value at idx or None.
         *
         * @param idx index of element to get
         *
         * @returns If idx is out-of range, return None.
         * @returns else return ref to item at index..
         */
        constexpr Optional<T &> try_get(Index idx) noexcept
        {
            return slice().try_get(idx);
        }

        constexpr Optional<T const &> try_get(Index idx) const noexcept
        {
            return slice().try_get(idx);
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

        /**
         * Get a partial slice over the range of elements in the array.
         *
         * @param b the start index of the range to slice.
         * @param e the end index of the range to slice.
         *
         * @returns if array is empty, return empty slice
         * @returns if b&e >= array len, return empty slice
         * @returns if b >= array len, return empty slice
         * @returns if e > array len, clamp to last elem.
         * @returns else return slice over region b..e of array.
         */
        constexpr Slice<T> subslice(Index b, Index e) noexcept
        {
            return slice().subslice(b, e);
        }

        constexpr Slice<T const> subslice(Index b, Index e) const noexcept
        {
            return slice().subslice(b, e);
        }

        /**
         * Create an iterator over the contents of the Array.
         *
         * iterator is invalidated if array goes out of scope/destroyed.
         *
         * @returns The iterator.
         */
        constexpr auto iter(void) const
        {
            return slice().iter();
        }

        constexpr auto iter(void)
        {
            return slice().iter();
        }

    public:
        /**
         * Format/emit a representation of this object as a charstring
         * for debugging purposes.
         *
         * @param val the value to format
         * @param outs the stream to dump the representation into.
         */
        // TODO: replace <<(Log ) with dbgfmt, so separate out from
        // any other form of conversion to charstring.
        // TODO: insert into formatter and not final dest type.
        friend Log &operator<<(Log &outs, Array const &v) noexcept
        {
            outs << "Array<" << v.len() << ">{";
            outs << '\n';
            Index i = 0;
            v.iter().for_each([&outs, &i](T const &e) {
                outs << '[' << i << "]:" << e << '\n';
                ++i;
            });
            outs << '}';
            return outs;
        }
};

} // namespace heapless
} // namespace nel

#endif // NEL_HEAPLESS_ARRAY_HH
