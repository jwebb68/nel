// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
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

#include <nel/iterator.hh>
#include <nel/slice.hh>
#include <nel/optional.hh>
#include <nel/result.hh>
#include <nel/log.hh>

namespace nel
{
namespace heapless
{

/**
 * Vector
 *
 * A container of type T held in a contiguous block (a variable sized array.)
 * Capacity (max number of elements) is limited at create time.
 * Cannot be implicitly coped.
 * Can be implicitly moved.
 */
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
        /**
         * destroy the vector, deleting all elements owned by it.
         */
        ~Vector(void)
        {
            for (Index i = 0; i < len(); ++i) {
                values_[i].~T();
            }
        }

        // default ctor is safe, will always succeed.
        // just expensive in ram if T or N are large.
        constexpr Vector(void)
            : len_(0)
        {
        }

        // No copying..
        constexpr Vector(Vector const &o) = delete;
        constexpr Vector &operator=(Vector const &o) = delete;

        // Moving ok
        Vector(Vector &&o) noexcept
            : len_(std::move(o.len_))
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
        /**
         * Create a vector with no initial allocation.
         *
         * @returns the vector created.
         */
        static constexpr Vector empty(void) noexcept
        {
            return Vector();
        }

        /**
         * Attempt to create vector from initialiser list
         *
         * @param l initialiser list to use
         * @return on success, an Optional::Some holding the created vector.
         * @return on fail: Optional::None
         */
        // possibly experimental.
        // vec create using init lists
        // want moving not copying.
        // want copying but not via ctor (may not be poss), so it becomes a try_ returning an err.
        static constexpr Optional<Vector> try_from(std::initializer_list<T> l) noexcept
        {
            if (l.size() != N) { return Optional<Vector>::None(); }
            Vector v = Vector::empty();
            auto r = v.push_back(l);
            if (r.is_err()) { return Optional<Vector>::None(); }
            return Optional<Vector>::Some(std::move(v));
            // TODO: can all of this create the vec inplace in the optional?
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
         * Cast this vector into a full slice?
         *
         * Creates a slice from the vector.
         * Slice does not own the contents, vector does (vector is still valid).
         * Slice is invalidated if Vector goes out of scope/destroyed.
         *
         * @returns a slice over the the vector.
         */
        constexpr Slice<T> slice(void) noexcept
        {
            return Slice<T>::from(values_, len());
        }

        constexpr Slice<T const> const slice(void) const noexcept
        {
            return Slice<T const>::from(values_, len());
        }

        /**
         * Get a partial slice over the range of elements in the Vector.
         *
         * @param b the start index of the range to slice.
         * @param e the end index of the range to slice.
         *
         * @returns if vec is empty, return empty slice
         * @returns if b&e >= vec len, return empty slice
         * @returns if b >= vec len, return empty slice
         * @returns if e > vec len, clamp to last elem.
         * @returns else return slice over region b..e of vec.
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
         * Change the internal allocation to given number of elements.
         *
         * If new capacity is smaller than current, then current is reduced.
         * If new capacity is less than currently used, then removes all unused
         * allocations and does not reduce the in-use amount (does not delete
         * items in the vec).
         *
         * In a fixed vec, the internal allocation can never change.
         *
         * @param new_cap the new allocation to set.
         *
         * @returns true if new capacity is same as current..
         * @returns false otherwise.
         */
        bool try_reserve(Count new_capacity) noexcept
        {
            return new_capacity == N;
        }

        /**
         * Push a value onto the end of the vec.
         *
         * @param val The value to move into the vec.
         * @returns if successful, Result<void, T>::Ok()
         * @returns if unsuccessful, Result<void, T>::Err() holding val
         */
        // move value into vec
        // on fail still move, but return.
        // allow inplace create instead of move.
        // if fails to store, can create be avoided..?
        Result<void, T> push_back(T &&val) noexcept
        {
            if (len() >= capacity()) {
                // Really? must one be created for err?
                return Result<void, T>::Err(val);
            }
            // Remember, values at len and beyond are uninitialised.
            // So need to use new to construct them.
            new (&values_[len()]) T(val);
            len_ += 1;
            return Result<void, T>::Ok();
        }

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

        Result<void, std::initializer_list<T>> push_back(std::initializer_list<T> l) noexcept
        {
            if (len() + l.size() > capacity()) {
                // Really? must one be created for err?
                return Result<void, std::initializer_list<T>>::Err(l);
            }
            // Remember, values at len and beyond are uninitialised.
            // So need to use new to construct them.
            Index i = len();
            for (auto it = l.begin(); it != l.end(); ++i, ++it) {
                new (&values_[i]) T(std::move(*it));
            }
            len_ = i;
            return Result<void, std::initializer_list<T>>::Ok();
        }

        // move contents of vec into this?
        // Result<void, Vector<T>> push_back(Vector<T> &l) noexcept?
        // move contents of slice into this?
        // Result<void, Slice<T>> push_back(Slice<T> &l) noexcept?
        // move contents of iter into this? into_vec?
        // Result<void, Slice<T>> push_back(Iter<T> &l) noexcept?

        /**
         * Remove and return the last item in the vec.
         *
         * @returns on success: Optional::Some holding the value
         * @returns on fail: Optional::None
         */
        Optional<T> pop_back(void) noexcept
        {
            if (len() == 0) { return None; }
            len_ -= 1;
            T &e = values_[len()];
            auto v = Some(std::move(e));
            e.~T();
            return v;
        }

        // insert_at(idx, T &&) ?
        // insert_at(idx, Array<T> &&) ?
        // insert_at(idx, iter<T> it) ?
        // remove_at(idx) ? drops or returns item
        // remove_at(beg,end) ? drops or returns items..(how?)
        // sort ?
        // find ?

    public:
        /**
         * Create an iterator over the contents of the Array.
         *
         * iterator is invalidated if array goes out of scope/destroyed.
         *
         * @returns The iterator.
         */
        constexpr auto iter(void) noexcept
        {
            return slice().iter();
        }

        constexpr auto iter(void) const noexcept
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
        friend Log &operator<<(Log &outs, Vector const &v) noexcept
        {
            outs << "Vector<" << N << ">(" << v.len() << "){";
#if 1
            outs << '\n';
            Index i = 0;
            v.iter().for_each([&outs, &i](T const &e) {
                outs << '[' << i << "]:" << e << '\n';
                ++i;
            });
#elif 0
            outs << '\n';
            Index i = 0;
            v.iter().for_each2([&outs, &i](T const &e) {
                outs << '[' << i << "]:" << e << '\n';
                ++i;
            });
#else
            auto it = v.iter();
            Index i = 0;
            it.for_each2([&](T const &e) {
                outs << ',' << '[' << i << "]:" << e;
                ++i;
            });
#endif
            outs << '}';
            return outs;
        }
};

} // namespace heapless
} // namespace nel

#endif // NEL_HEAPLESS_VECTOR_HH
