// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(NEL_HEAPED_ARRAY_HH)
#    define NEL_HEAPED_ARRAY_HH

namespace nel
{
namespace heaped
{

template<typename T>
struct Array;

} // namespace heaped
} // namespace nel

#    include <nel/heaped/node.hh>
#    include <nel/iterator.hh>
#    include <nel/slice.hh>
#    include <nel/optional.hh>
#    include <nel/log.hh>

#    include <nel/panic.hh>

#    include <nel/new.hh> // new
#    include <nel/defs.hh>

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
struct Array
{
    public:
        typedef T Type;

    private:
        typedef Node<Type> ArrayNode;
        // Cannot use new/delete as created using malloc/realloc.
        // and cannot use unique_ptr as using malloc/realloc.
        // or: how can i use realloc in c++ with new/delete?
        ArrayNode *item_;

    private:
        // Need default constr so can create unassigned vars
        // e.g Array<T> foo;
        // but then why would you want to?
        constexpr Array(void)
            : item_(nullptr)
        {
        }

        constexpr Array(ArrayNode *const n)
            : item_(n)
        {
        }

    public:
        /**
         * destroy the array and all items contained.
         */
        constexpr ~Array(void)
        {
            ArrayNode::free(item_);
        }

    private:
        // No copying.
        // copying an array is very expensive, so disabled.
        // if a copy is required, then must be done explicitly.
        constexpr Array(Array const &o) = delete;
        constexpr Array &operator=(Array const &o) = delete;

    public:
        // Moving allowed.
        // Moving is allowed since it's a fast O(1) op.
        // and does not create any extra resources..
        constexpr Array(Array &&o)
            : item_(nel::move(o.item_))
        {
            o.item_ = nullptr;
        }

        constexpr Array &operator=(Array &&o)
        {
            if (this != &o) {
                item_ = nel::move(o.item_);
                o.item_ = nullptr;
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
        static constexpr Array empty(void)
        {
            return Array();
        }

#    if 1
        /**
         * Create an array, of size n, initial value f in all entries.
         *
         * @param f The value to fill with.
         * @param n The size of the array.
         *
         * @returns the created array
         */
        static constexpr Array filled(Type const &f, Count n)
        {
            if (n == 0) { return Array::empty(); }
            ArrayNode *node = ArrayNode::malloc(n);
            if (node == nullptr) { nel_panic(""); }
            new (node) ArrayNode(f);
            auto a = Array(node);
            return a;
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
        // want moving not copying, but c++ default is to copy
        // how to init the initlist in const space? (it'd be copying then)
        // want copying but not via ctor (may not be possible), so it becomes a try_ returning an
        // err.
        // TODO: determine how efficient this implementation is..
        static constexpr Optional<Array> try_from(std::initializer_list<Type> &&l)
        {
            return ArrayNode::try_from(move(l)).template map<Array>([](ArrayNode * p) -> Array {
                return Array(p);
            });
        }
#    endif

    public:
        /**
         * Determine if the array is empty.
         *
         * @returns true if array is empty, false otherwise.
         */
        bool is_empty(void) const
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
            return (item_ == nullptr) ? 0 : item_->len();
        }

#    if 0
        /**
         * Item access in array.
         *
         * @param idx The index of the item to get.
         *
         * @returns reference to the item.
         * @warning Will panic if idx is out-of-range for array.
         */
        // as array access can fail, redo to try_get() and return v or error
        constexpr Type &operator[](Index idx)
        {
            // nel_panic_if_not(item_ != nullptr, "invalid array");
            // return (*item_)[idx];
            return slice()[idx];
        }
        constexpr Type const &operator[](Index idx) const
        {
            // nel_panic_if_not(item_ != nullptr, "invalid array");
            // return (*item_)[idx];
            return slice()[idx];
        }
#    endif //

        /**
         * Return a reference to the value at idx or None.
         *
         * @param idx index of element to get
         *
         * @returns If array is empty, return None.
         * @returns If idx is out-of range, return None.
         * @returns else return ref to item at index..
         */
        constexpr Optional<Type &> try_get(Index idx)
        {
            return slice().try_get(idx);
        }

        constexpr Optional<Type const &> try_get(Index idx) const
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
        constexpr Slice<Type> slice(void)
        {
            return (item_ == nullptr) ? Slice<T>::empty() : item_->slice();
        }

        constexpr Slice<Type const> slice(void) const
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
        constexpr Slice<Type> slice(Index b, Index e)
        {
            return slice().slice(b, e);
        }

        constexpr Slice<Type const> slice(Index b, Index e) const
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
        constexpr auto iter(void)
        {
            return slice().iter();
        }

        constexpr auto iter(void) const
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
            outs << "Array(" << v.len() << "){";
            outs << v.iter();
            outs << '}';
            return outs;
        }
};

} // namespace heaped
} // namespace nel

#endif // !defined(NEL_HEAPED_ARRAY_HH)
