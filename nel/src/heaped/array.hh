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

#include "node.hh"
#include "iterator.hh"
#include "enumerator.hh"
#include "slice.hh"

#include <cstddef> // size_t

namespace nel
{
namespace heaped
{

/**
 * An array of type T of fixed size
 *
 * The array is created and managed on the heap.
 * Array can be moved in O(1) (no byte-moving happens)
 * Can create an empty array: ::empty().
 * All Items are destroyed when array is destroyed.
 * All Items are created when array is created.
 * size cannot be changed once created.
 * size is not in type?
 */
template<typename T>
struct Array {
    public:
    private:
        typedef Node<T> ArrayNode;
        // Cannot use new/delete as created using malloc/realloc.
        // and cannot use unique_ptr as using malloc/realloc.
        // or: how can i use realloc in c++ with new/delete?
        ArrayNode *item_;

    protected:
        // Need default constr so can create unassigned vars
        // e.g Array<T> foo;
        // but then why would you want to?
        constexpr Array(void) noexcept: item_(nullptr) {}

        constexpr Array(ArrayNode *const n) noexcept: item_(n) {}

        // How to convert vector to array ?
        // Vector::into_arr, then need to pass Node into array using private ctor.. needs
        // arr::friend vec Array::from(vec), then need to detach node from vec using private func..
        // needs vec:friend arr rust: as_ptr(&self) -> *const T rust: as_mut_ptr(&mut self) -> *mut
        // T should vec convert to array? constexpr Array(Vector<T> &vec) noexcept
        //     : item_(vec.item_)
        // {
        //     vec.item_ = nullptr;
        // }

    public:
        ~Array(void) noexcept
        {
            ArrayNode::free(item_);
        }

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

        // Initialiser list initialisation required here..

        // create array from vec.
        // static constexpr Array from(Vector<T> &vec) noexcept
        // {
        //     return Array(vec);
        // }

        /**
         * Create an array, of size n, initial value f in all entries.
         *
         * @param f The value to fill with.
         * @param n The size of the array.
         *
         * @returns the created array
         */
        static constexpr Array fill(T const &f, size_t n)
        {
            if (n == 0) { return Array::empty(); }
            Array a(ArrayNode::malloc(n));
            new (a.item_) ArrayNode(f);
            return a;
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

#if 0
        constexpr Array &operator=(Array &&o) noexcept
        {
            if (this != &o) {
                // slow
                Array t(std::move(o));
                swap(t);
            }
            return *this;
        }
        void swap(Array &o) noexcept
        {
            std::swap(item_, o.item_);
        }
#else
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
#endif

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
        constexpr size_t len(void) const noexcept
        {
            return (item_ == nullptr) ? 0 : item_->len();
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
            // nel_panic_if_not(item_ != nullptr, "invalid array");
            // return (*item_)[idx];
            return slice()[idx];
        }
        constexpr T const &operator[](size_t idx) const noexcept
        {
            // nel_panic_if_not(item_ != nullptr, "invalid array");
            // return (*item_)[idx];
            return slice()[idx];
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
            return (item_ == nullptr) ? Slice<T>::empty()
                                      : Slice<T>::from(item_->ptr(), item_->len());
        }
        constexpr Slice<T const> slice(void) const
        {
            return (item_ == nullptr) ? Slice<T const>::empty()
                                      : Slice<T const>::from(item_->ptr(), item_->len());
        }

    public:
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
        friend Log &operator<<(Log &outs, Array const &v) noexcept
        {
            outs << "Array(" << v.len() << "){"
                 << "\n";
            for (size_t i = 0; i < v.len(); ++i) {
                outs << "[" << i << "]:" << v.values_[i] << "\n";
            }
            outs << "}";
            return outs;
        }
};

} // namespace heaped
} // namespace nel

#endif // NEL_HEAPED_ARRAY_HH
