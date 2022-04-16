#ifndef NEL_HEAPED_ARRAY_HH
#define NEL_HEAPED_ARRAY_HH

namespace nel
{
namespace heaped
{

template<typename T>
struct Array;

} // namespace heaped
} // namespace nel

#include <nel/heaped/node.hh>
#include <nel/enumerator.hh>
#include <nel/iterator.hh>
#include <nel/slice.hh>
#include <nel/log.hh>
#include <nel/defs.hh>

namespace nel
{
namespace heaped
{

/**
 * An array of type T of fixed size
 *
 * The array is created and managed on the heap.
 * Array can be moved in O(1) (no byte-moving happens)
 * Array is not default copyable, if a copy is required it's an explicit try_copy
 * Can create an empty array: ::empty().
 * All Items are destroyed when array is destroyed.
 * All Items are created when array is created.
 * size cannot be changed once created.
 */
// TODO: put size of array in type, i.e. template<typename T, Length N>
// would mean types checked at compile time
// cannot move to array of diff size.. checked at compile time
template<typename T>
struct Array {
    public:
    private:
        typedef Node<T> ArrayNode;
        // Cannot use new/delete as created using malloc/realloc.
        // and cannot use unique_ptr as using malloc/realloc.
        // or: how can i use realloc in c++ with new/delete?
        ArrayNode *item_;

    private:
        // Need default constr so can create unassigned vars
        // e.g Array<T> foo;
        // but then why would you want to?
        constexpr Array(void) noexcept: item_(nullptr) {}

        constexpr Array(ArrayNode *const n) noexcept: item_(n) {}

    public:
        /**
         * destroy the array and all items contained.
         */
        ~Array(void) noexcept
        {
            ArrayNode::free(item_);
        }

        // No copying.
        // copying an array is very expensive, so disabled.
        // if a copy is required, then must be done explicitly.
        constexpr Array(Array const &o) = delete;
        constexpr Array &operator=(Array const &o) = delete;

        // Moving allowed.
        // Moving is allowed since it's a fast O(1) op.
        constexpr Array(Array &&o) noexcept: item_(std::move(o.item_))
        {
            o.item_ = nullptr;
        }
        constexpr Array &operator=(Array &&o) noexcept
        {
            if (this != &o) {
                // Fast.
                // Move once, not twice.
                // But only works if Array<T>(&&) does not throw/error.
                this->~Array();
                new (this) Array(std::move(o));
            }
            return *this;
        }

    public:
        /**
         * Create an empty array
         *
         * Empty array has length 0 and no content.
         *
         * @returns the created array
         */
        static constexpr Array empty(void) noexcept
        {
            return Array();
        }

        /**
         * Create an array, of size n, initial value f in all entries.
         *
         * @param f The value to fill with.
         * @param n The size of the array.
         *
         * @returns the created array
         */
        static constexpr Array filled(T const &f, Count n)
        {
            if (n == 0) { return Array::empty(); }
            Array a(ArrayNode::malloc(n));
            new (a.item_) ArrayNode(f);
            return a;
        }

        /**
         * Attempt to create an array from initialiser list
         *
         * @param l initialiser list to use
         * @return on success, an Optional::Some holding the created array.
         * @return on fail: Optional::None
         */
        // possibly experimental.
        // want moving not copying, but c++ default is to copy
        // how to init is initlist in const space? (it'd be copying then)
        // want copying but not via ctor (may not be possible), so it becomes a try_ returning an
        // err.
        // TODO: determine how efficient this implementation is..
        static constexpr Optional<Array> try_from(std::initializer_list<T> l) noexcept
        {
            return ArrayNode::try_from(l).map(
                std::function([](ArrayNode *&&p) -> Array { return Array(p); }));
        }

    public:
        /**
         * Determine if the array is empty.
         *
         * @returns true if array is empty, false otherwise.
         */
        bool is_empty(void) const noexcept
        {
            return len() == 0;
        }

        /**
         * Return the number of items in the array.
         *
         * @returns number of items in the array.
         */
        constexpr Length len(void) const noexcept
        {
            return (item_ == nullptr) ? 0 : item_->len();
        }

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
            // nel_panic_if_not(item_ != nullptr, "invalid array");
            // return (*item_)[idx];
            return slice()[idx];
        }
        constexpr T const &operator[](Index idx) const noexcept
        {
            // nel_panic_if_not(item_ != nullptr, "invalid array");
            // return (*item_)[idx];
            return slice()[idx];
        }
#endif //

        /**
         * Return a reference to the value at idx or None.
         *
         * @param idx index of element to get
         *
         * @returns If array is empty, return None.
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
            return (item_ == nullptr) ? Slice<T>::empty() : item_->slice();
        }
        constexpr Slice<T const> slice(void) const
        {
            return (item_ == nullptr) ? Slice<T const>::empty()
                                      : reinterpret_cast<ArrayNode const *>(item_)->slice();
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
        constexpr Iterator<T> iter(void) noexcept
        {
            return slice().iter();
        }
        constexpr Iterator<T const> iter(void) const noexcept
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
            outs << "Array(" << v.len() << "){"
                 << "\n";
            for (Index i = 0; i < v.len(); ++i) {
                outs << "[" << i << "]:" << v.values_[i] << "\n";
            }
            outs << "}";
            return outs;
        }
};

} // namespace heaped
} // namespace nel

#endif // NEL_HEAPED_ARRAY_HH