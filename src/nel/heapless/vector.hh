#ifndef NEL_HEAPLESS_VECTOR_HH
#define NEL_HEAPLESS_VECTOR_HH

#include <nel/defs.hh> //NEL_UNUSED, Length

namespace nel
{
namespace heapless
{

template<typename T, Length const N>
struct Vector;

} // namespace heapless
} // namespace nel

#include <nel/enumerator.hh>
#include <nel/iterator.hh>
#include <nel/slice.hh>
#include <nel/optional.hh>
#include <nel/result.hh>
#include <nel/log.hh>

namespace nel
{
namespace heapless
{

template<typename T, Length const N>
struct Vector {
    public:
    private:
        // Number initialised.
        Length len_;
        // Must create with N uninitialised.
        // So really want only the memory allocated.
        // Will manually initialise on push_back, deinitialise on pop_back/destruct
        // C++: default array constr does not call default on each, but allocs uninit?
        T values_[N];

    public:
        ~Vector(void)
        {
            for (Index i = 0; i < len(); ++i) {
                values_[i].~T();
            }
        }

        // No copying..
        constexpr Vector(Vector const &o) = delete;
        constexpr Vector &operator=(Vector const &o) = delete;

        // Moving ok
        Vector(Vector &&o) noexcept: len_(std::move(o.len_))
        {
            o.len_ = 0;
            for (Index i = 0; i < len(); ++i) {
                new (&values_[i]) T(std::move(o.values_[i]));
            }
        }
        Vector &operator=(Vector &&o) noexcept
        {
            if (this != &o) {
                // Expensive to call swap on large inplace object.
                // So move to final dest.
                // Only as move cannot fail and leave this destructed.
                this->~Vector();
                new (this) Vector(std::move(o));
            }
            return *this;
        }

    public:
        // default ctor is safe, will always succeed.
        constexpr Vector(void): len_(0) {}

        static constexpr Vector empty(void) noexcept
        {
            return Vector();
        }

        constexpr Vector(std::initializer_list<T> l)
        {
            Index i = 0;
            for (auto it = l.begin(); i < capacity() && it != l.end(); ++it, ++i) {
                new (&values_[i]) T(std::move(*it));
            }
            len_ = i;
            // TODO: what if l.size() > capacity()? want to fail compile..
        }

    public:
        /**
         * Returns the number of items currently in the allocation
         *
         * The allocation is not the number in use, only the number before a reallocation occurs.
         *
         * @returns the current allocation amount.
         */
        constexpr Count capacity(void) const noexcept
        {
            return N;
        }

        /**
         * Returns the number of items currently in use.
         *
         * @returns the current in use count.
         */
        constexpr Length len(void) const noexcept
        {
            return len_;
        }

        /**
         * Determines if the vector is empty (i.e. in-use count of 0).
         *
         * @returns true if in-use is 0, else false.
         */
        constexpr bool is_empty(void) const noexcept
        {
            return len_ == 0;
        }

        /**
         * Clears the vector, i.e. removes and destroys all in-use elements.
         */
        void clear(void) noexcept
        {
            for (Index i = 0; i < len(); ++i) {
                values_[i].~T();
            }
            len_ = 0;
        }

#if 0
        /**
         * Item access in vector.
         *
         * @param idx The index of the item to get.
         *
         * @returns reference to the item
         * @warning Will panic if idx is out-of-range for vector
         */
        constexpr T &operator[](Index idx) noexcept
        {
            return slice()[idx];
        }
        constexpr T const &operator[](Index idx) const noexcept
        {
            return slice()[idx];
        }
#endif // 0

        /**
         * Return a reference to the value at idx or None.
         *
         * @param idx index of element to get
         *
         * @returns If the vector is empty, return None.
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
        constexpr Slice<T> slice(void) noexcept
        {
            return Slice<T>::from(values_, len());
        }
        constexpr Slice<T const> const slice(void) const noexcept
        {
            return Slice<T const>::from(values_, len());
        }

        // TODO: fail on fixed?
        // Or return Result<void, ?>
        // And fail if not N
        // i.e. reserve(0) on cap of 4 does what?
        // Shouldn't be errT=bool but don't know what to use..
        // for heaped err would be from mem-allocator
        // Result<void, bool> reserve(Count new_capacity) noexcept
        void reserve(Count new_capacity) noexcept
        {
            NEL_UNUSED(new_capacity);
        }

        // move value into vec
        // on fail still move, but return.
        // allow inplace create instead of move.
        // if fails to store, can create be avoided..?
        template<typename... Args>
        Result<void, T> push_back(Args &&...args) noexcept
        {
            if (len() >= capacity()) {
                // Really? must one be created for err?
                return Result<void, T>::Err(std::forward<Args>(args)...);
            }
            // Remember, values at len and beyond are uninitialised.
            // So need to use new to construct them.
            new (&values_[len()]) T(std::forward<Args>(args)...);
            len_ += 1;
            return Result<void, T>::Ok();
        }

        Optional<T> pop_back(void) noexcept
        {
            if (len() == 0) { return Optional<T>::None(); }

            auto v = Optional<T>::Some(std::move(values_[len()]));
            values_[len()].~T();
            len_ -= 1;
            return v;
        }

        // insert_at ?/push_at?
        // remove_at?/pop_at?
        // sort ?

        constexpr Iterator<T> iter(void) noexcept
        {
            return slice().iter();
        }
        constexpr Iterator<T const> iter(void) const noexcept
        {
            return slice().iter();
        }

        constexpr Enumerator<T> enumerate(void) noexcept
        {
            return slice().enumerate();
        }
        constexpr Enumerator<T const> enumerate(void) const noexcept
        {
            return slice().enumerate();
        }

    public:
        friend Log &operator<<(Log &outs, Vector const &v) noexcept
        {
            outs << "Vector<" << N << ">(" << v.len() << "){"
                 << "\n";
            for (Index i = 0; i < v.len(); ++i) {
                outs << "[" << i << "]:" << v.values_[i] << "\n";
            }
            outs << "}";
            return outs;
        }
};

} // namespace heapless
} // namespace nel

#endif // NEL_HEAPLESS_VECTOR_HH
