// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(NEL_HEAPLESS_ARRAY_HH)
#    define NEL_HEAPLESS_ARRAY_HH

#    include <nel/defs.hh> //NEL_UNUSED, Length

namespace nel
{
namespace heapless
{

template<typename T, Length const N>
struct Array;

} // namespace heapless
} // namespace nel

#    include <nel/iterator.hh>
#    include <nel/slice.hh>
#    include <nel/memory.hh> // move
// #    include <printio.hh>
#    include <nel/log.hh>
#    include <nel/new.hh> //new (p) T()
#    include <nel/defs.hh> // Length

namespace nel
{
namespace heapless
{

/**
 * Array
 * Manages a block of ram within itself (i.e. heapless)
 * all elements are initialised.
 * all elements are destroyed when array is destroyed.
 * Array cannot be resized.
 * Elements can be accessed by [].
 * Elements can be iterated over.
 * Array can be moved, calling the move operator on each elem.
 * Array cannot be copied implicitly.
 */
template<typename T, Length const N>
struct Array
{
    public:
    private:
        // default ctor does not init this array.
        // does this class follow suite? nope, expects these to be initialised.
        T values_[N];

    public:
        /**
         * Destroy array and it's elements.
         */
        // do not need to implement.
        // default one will dtuct elements.
        ~Array() = default;

    private:
        // prevent implicit copying/assignment as it's expensive.
        Array(Array const &) = delete;
        Array operator=(Array const &) = delete;

    public:
        /**
         * construct array by moving from other
         */
        // do not need to implement
        // default will move element-wise.
        Array(Array &&) = default;

        /**
         * move contents of array from other to this
         */
        // do not need to implement
        // default will move element-wise.
        Array &operator=(Array &&) = default;

    public:
        // allow copy/assign, as explicit as it's expensive.
        // TODO: make these static constructor funcs, that can fail, returning error if so.
        // maybe want a Result<T,E> i.e. Result<Array<..>, Err>.
        // TODO: error handling using Results
        bool NEL_WARN_UNUSED_RESULT try_copy_from(Array const &src)
        {
            return try_copy_from(src.values_);
        }

        bool NEL_WARN_UNUSED_RESULT try_copy_from(T const src[N])
        {
            T const *s = src;
            // don't want to use a for_each here as when the copy fails will need to clean up.
            // and cannot do that with a for_each.
            for (T *d = values_; d != values_ + N; ++d) {
                // maybe assign oper is not the best..
                // how to indicate fail if so?
                // bool ok = d.try_copy_from(*s); ?
                *d = *s;
                ++s;
            }
            return true;
        }

    public:
        // no default construction
        // but needed internally..
        constexpr Array(void) = default;

        // init list initialiser.
        template<typename... Args>
        constexpr Array(Args &&...args)
            : values_(forward<Args>(args)...)
        {
        }

    private:
#    if 0
        constexpr Array(std::initializer_list<T> l)
        {
            // Interesting
            // gcc: if init-list ctor is empty, then values are created inplace .. not.
            // gcc: creates init list in temp space, then copies into required space..
            //      todo: avoid this copying.. can I get them created inplace..
            Index i = 0;
            for (auto it = l.begin(); i < N && it != l.end(); ++it, ++i) {
                // TODO: init list elements want to be copied, not moved..
                new (&values_[i]) T(move(*it));
            }
        }
#    endif

        // If type has copy, then could fill? use slice for that..

        // fill constructor
        // TODO:  what if copy of T fails..? try_fill? how?
        constexpr Array(T const &v)
        {
            iter().for_each([&](T &e) { new (&e) T(v); });
        }

    public:
        // heapless arrays cannot be created 'empty'

        /**
         * Initialise an array with given value.
         * @return array filled with copies of value.
         * @warning UB if copying of v fails.
         */
        // not very good as failure to copy v will panic.
        // and don't want panics from lib code.
        constexpr static Array filled_with(T const &v)
        {
            return Array(v);
        }

#    if 0
        /**
         * Attempt to create and Initialise an array with given value.
         * @return on success: array filled with copies of value.
         * @return on fail: None
         */
        // TODO: should fail return an error i.e. Result<Array, Err>?
        static constexpr Optional<Array> try_filled_with(T const &v)
        {
            // cannot use ctor here as ctor cannot return on fail
            // so will need to create array uninitialsed
            // then populate with copies of v
            // if copying of v fails, destroy those already created (returning to uninitialsed)
            // then exit
            // but this will call array::dtor which will attempt to destroy the values again..
            return Array(v);
        }
#    endif

#    if 0
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
        // want copying but not via ctor (may not be possible), so it becomes
        // a try_ returning an err.
        // If type has copy, then could fill? use slice for that..
        static constexpr Optional<Array> try_from(std::initializer_list<T> l)
        {
            // TODO: can the array create be made inplace into the optional?
            // prob only if initialiser list ctor is public, which I don't want.
            return (l.size() != N) ? None : Some(Array(l));
        }
#    endif

    public:
        /**
         * Determine if the array is empty.
         *
         * @returns true if array is empty, false otherwise.
         */
        constexpr bool is_empty(void) const
        {
            return len() == 0;
        }

        /**
         * get pointer to start of data
         *
         * @returns ptr to start of array elements
         */
        constexpr T *ptr(void)
        {
            return &values_[0];
        }

        constexpr T const *ptr(void) const
        {
            return &values_[0];
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

        /**
         * Item access.
         *
         * @param idx The index of the item to get.
         *
         * @returns reference to the item.
         * @warning Will panic if idx is out-of-range.
         */
        constexpr T &checked_get(Index idx)
        {
            return slice().checked_get(idx);
        }

        constexpr T const &checked_get(Index idx) const
        {
            return slice().checked_get(idx);
        }

        /**
         * Item access.
         *
         * @param idx The index of the item to get.
         *
         * @returns reference to the item.
         * @warning UB if index is out of range.
         */
        constexpr T &unchecked_get(Index idx)
        {
            return slice().unchecked_get(idx);
        }

        constexpr T const &unchecked_get(Index idx) const
        {
            return slice().unchecked_get(idx);
        }

        /**
         * Item access.
         *
         * @param idx The index of the item to get.
         *
         * @returns reference to the item.
         * @warning Will panic if idx is out-of-range.
         */
        // as access can fail, redo to try_get() and return v or error
        constexpr T &operator[](Index idx)
        {
            return slice()[idx];
        }

        constexpr T const &operator[](Index idx) const
        {
            return slice()[idx];
        }

        /**
         * Return a reference to the value at idx or None.
         *
         * @param idx index of element to get
         *
         * @returns If idx is out-of range, return None.
         * @returns else return ref to item at index..
         */
        constexpr Optional<T &> try_get(Index idx)
        {
            return slice().try_get(idx);
        }

        constexpr Optional<T const &> try_get(Index idx) const
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
        constexpr Slice<T> slice(Index b, Index e)
        {
            return slice().slice(b, e);
        }

        constexpr Slice<T const> slice(Index b, Index e) const
        {
            return slice().slice(b, e);
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
        friend Log &operator<<(Log &outs, Array const &v)
        {
            outs << "Array<" << v.len() << ">{";
            outs << v.iter();
            outs << '}';
            return outs;
        }
};

}; // namespace heapless
}; // namespace nel

#endif // defined(NEL_HEAPLESS_ARRAY_HH)
