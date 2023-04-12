// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(NEL_HEAPLESS_VECTOR_HH)
#    define NEL_HEAPLESS_VECTOR_HH

#    include <nel/defs.hh> //NEL_UNUSED, Length

namespace nel
{
namespace heapless
{

template<typename T, Length const N>
struct Vector;

} // namespace heapless
} // namespace nel

#    include <nel/manual.hh>
#    include <nel/iterator.hh>
#    include <nel/slice.hh>
#    include <nel/optional.hh>
#    include <nel/result.hh>
// #    include <printio.hh>
// #include <nel/log.hh>
#    include <nel/memory.hh> // move
#    include <nel/new.hh> //  placement new
#    include <nel/defs.hh> // Length

namespace nel
{
namespace heapless
{

/**
 * Vector
 *
 * A container of type T held in a contiguous block (a variable sized array.)
 * Capacity (max number of elements) is limited at create time.
 * Manages a block of ram within itself (i.e. heapless)
 * Elements moved in when pushed, moved out when popped.
 * Once full, push will fail.
 * Once empty, pop will fail.
 * All remaining elements are destroyed when vector is destroyed.
 * Vector cannot be resized.
// * Present Elements can be accessed by [].
 * Present Elements can be iterated over.
 * Vector can be moved, calling the move operator on each elem.
 * Vector cannot be copied implicitly.
*/
template<typename T, Length const N>
struct Vector
{
    public:
        typedef T Type;

    private:
        // Number initialised.
        Length len_;

        // Must create with N uninitialised.
        // So really want only the memory allocated.
        // TODO: adjust for alignment
        // for now assume all types are in same alignment
        Manual<Type[N]> elems_;

        constexpr Type *ptr()
        {
            return elems_.ptr()[0];
        }

        constexpr Type const *ptr() const
        {
            return elems_.ptr()[0];
        }

        constexpr Type *ptr(ptrdiff_t d)
        {
            return &ptr()[d];
        }

        constexpr Type const *ptr(ptrdiff_t d) const
        {
            return &ptr()[d];
        }

        constexpr Type *end()
        {
            return ptr(len_);
        }

        constexpr Type const *end() const
        {
            return ptr(len_);
        }

    public:
        /**
         * destroy the vector, deleting all elements owned by it.
         */
        constexpr ~Vector(void)
        {
            iter().for_each([&](auto &v) -> void { v.~Type(); });
        }

        // default ctor is safe, will always succeed.
        // and is fast.
        // if sizeof(T) or N is large will just eat ram.
        constexpr Vector(void)
            : len_(0)
        {
        }

        constexpr Vector(Length len, Type const &fill)
        {
            nel_panic_if(len > capacity(), "heapless::vector: invalid len");
            len_ = len;
            iter().for_each([&fill](auto &e) { new (&e) Type(fill); });
        }

        constexpr static Vector filled_with(Length len, Type const &fill)
        {
            return Vector(len, fill);
        }

    private:
        // No (implicit) copying..
        // Copying means potential alloc of resource that may fail to alloc.
        Vector(Vector const &) = delete;
        Vector &operator=(Vector const &) = delete;

    public:
        // Moving ok
        constexpr Vector(Vector &&o)
            : len_(move(o.len_))
        {
            Type *d = ptr();
            o.iter().for_each([&d](auto &e) {
                new (d) Type(move(e));
                ++d;
            });
            o.len_ = 0;
            // moved contents to this, o is now empty/invalid.
            // but cannot destroy o.values_ as this now owns them.
        }

        // can't be constexpr until dtors are.
        Vector &operator=(Vector &&o)
        {
            if (this != &o) {
                Type *s = o.ptr();
                Type *d = ptr();
                if (len_ == o.len_) {
                    for (; d != end(); ++d) {
                        *d = move(*s);
                        ++s;
                    }
                } else if (len_ < o.len_) {
                    for (; d != end(); ++d) {
                        *d = move(*s);
                        ++s;
                    }
                    for (; s != o.end(); ++s) {
                        new (d) Type(move(*s));
                        ++d;
                    }
                } else if (len_ > o.len_) {
                    for (; s != o.end(); ++d) {
                        *d = move(*s);
                        ++s;
                    }
                    for (; d != end(); ++d) {
                        d->~Type();
                    }
                }
                len_ = move(o.len_);
                o.len_ = 0;
            }
            return *this;
        }

    public:
        /**
         * Create a vector with no initial allocation.
         *
         * @returns the vector created.
         */
        static constexpr Vector empty(void)
        {
            return Vector();
        }

#    if 0
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
        // want copying but not via ctor (may not be possible), so it becomes
        // a try_ returning an err.
        static constexpr Optional<Vector> try_from(std::initializer_list<T> l)
        {
            if (l.size() != N) { return None; }
            Vector v = Vector::empty();
            auto r = v.push_back(l);
            if (r.is_err()) { return None; }
            return Optional<Vector>::Some(move(v));
            // TODO: can all of this create the vec inplace in the optional?
        }
#    endif

    public:
        /**
         * Returns the number of items currently in the allocation
         *
         * The allocation is not the number in use, only the number before a reallocation occurs.
         *
         * @returns the current allocation amount.
         */
        constexpr Count capacity(void) const
        {
            return N;
        }

        /**
         * Returns the number of items currently in use.
         *
         * @returns the current in use count.
         */
        constexpr Length len(void) const
        {
            return len_;
        }

        /**
         * Determines if the vector is empty (i.e. in-use count of 0).
         *
         * @returns true if in-use is 0, else false.
         */
        constexpr bool is_empty(void) const
        {
            return len_ == 0;
        }

        constexpr bool is_full(void) const
        {
            return len_ == N;
        }

    public:
        constexpr bool operator==(Vector const &o) const
        {
            if (len_ != o.len_) { return false; }
            for (T const *it = ptr(), *ito = o.ptr(); it != end(); ++ito, ++it) {
                if (*it != *ito) { return false; }
            }
            return true;
        }

        constexpr bool operator!=(Vector const &o) const
        {
            if (len_ != o.len_) { return true; }
            for (T const *it = ptr(), *ito = o.ptr(); it != end(); ++ito, ++it) {
                if (*it == *ito) { return false; }
            }
            return true;
        }

    public:
        /**
         * Clears the vector, i.e. removes and destroys all in-use elements.
         */
        void clear(void)
        {
            iter().for_each([&](auto &v) -> void { v.~Type(); });
            len_ = 0;
        }

        /**
         * Resize the vector to be of length newlen.
         *
         * If vector is longer, then drop the excessive elements.
         * If vector is shorter, then create new elements from filler.
         * If newlen is greater than capacity then go up to capacity.
         *
         * Filler must be copyable.
         * If filler fails to copy, then panics.
         *
         * note: may be replaced with a try_resize.
         */
        void resize(Length newlen, Type const &filler)
        {
            if (newlen > capacity()) { newlen = capacity(); }

            if (newlen < len()) {
                // shrinking, just drop excess.
                for (Type *it = ptr(newlen); it != end(); ++it) {
                    it->~Type();
                }
                len_ = newlen;
            } else if (newlen > len_) {
                // growing, add new
                for (Type *it = ptr(len()); it != ptr(newlen); ++it) {
                    new (it) Type(filler);
                }
                len_ = newlen;
            } else {
                // same, do nothing
            }
        }

        /**
         * Item access.
         *
         * @param idx The index of the item to get.
         *
         * @returns reference to the item.
         * @warning Will panic if idx is out-of-range.
         */
        constexpr Type &checked_get(Index idx)
        {
            return slice().checked_get(idx);
        }

        constexpr Type const &checked_get(Index idx) const
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
        constexpr Type &unchecked_get(Index idx)
        {
            return slice().unchecked_get(idx);
        }

        constexpr Type const &unchecked_get(Index idx) const
        {
            return slice().unchecked_get(idx);
        }

        /**
         * Item access.
         *
         * @param idx The index of the item to get.
         *
         * @returns reference to the item
         * @warning Will panic if idx is out-of-range.
         */
        // as access can fail, redo to try_get() and return v or error
        constexpr Type &operator[](Index idx)
        {
            return slice()[idx];
        }

        constexpr Type const &operator[](Index idx) const
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
        constexpr Optional<Type &> try_get(Index idx)
        {
            return slice().try_get(idx);
        }

        constexpr Optional<Type const &> try_get(Index idx) const
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
        constexpr Slice<Type> slice(void)
        {
            return Slice(ptr(), len());
        }

        constexpr Slice<Type const> const slice(void) const
        {
            return Slice(ptr(), len());
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
        constexpr Slice<Type> slice(Index b, Index e)
        {
            return slice().slice(b, e);
        }

        constexpr Slice<Type const> slice(Index b, Index e) const
        {
            return slice().slice(b, e);
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
        bool try_reserve(Count new_capacity)
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
        Result<void, Type> NEL_WARN_UNUSED_RESULT push(Type &&val)
        {
            if (len() >= capacity()) {
                // Really? must one be created for err?
                return Result<void, Type>::Err(move(val));
            }
            // Remember, values at len and beyond are uninitialised.
            // So need to use new to construct them.
            new (end()) Type(move(val));
            len_ += 1;
            return Result<void, Type>::Ok();
        }

#    if 0
        template<typename... Args>
        Result<void, Type> push(Args &&...args)
        {
            if (len() >= capacity()) {
                // Really? must one be created for err?
                return Result<void, Type>::Err(forward<Args>(args)...);
            }
            // Remember, values at len and beyond are uninitialised.
            // So need to use new to construct them.
            new (end()) Type(forward<Args>(args)...);
            len_ += 1;
            return Result<void, Type>::Ok();
        }
#    endif

#    if 0
        Result<void, std::initializer_list<Type>> push(std::initializer_list<Type> l)
        {
            if (len() + l.size() > capacity()) {
                // Really? must one be created for err?
                return Result<void, std::initializer_list<Type>>::Err(l);
            }
            // Remember, values at len and beyond are uninitialised.
            // So need to use new to construct them.
            Index i = len();
            for (auto it = l.begin(); it != l.end(); ++i, ++it) {
                new (&values_[i]) Type(move(*it));
            }
            len_ = i;
            return Result<void, std::initializer_list<Type>>::Ok();
        }
#    endif

        // move contents of vec into this?
        // Result<void, Vector<T>> push_back(Vector<T> &l) ?
        // move contents of slice into this?
        // Result<void, Slice<T>> push_back(Slice<T> &l) ?
        // move contents of iter into this? into_vec?
        // Result<void, Slice<T>> push_back(Iter<T> &l) ?

        /**
         * Remove and return the last item in the vec.
         *
         * @returns on success: Optional::Some holding the value
         * @returns on fail: Optional::None
         */
        Optional<Type> pop(void)
        {
            if (len() == 0) { return None; }
            len_ -= 1;
            return Some(move(*end()));
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
        friend Log &operator<<(Log &outs, Vector const &v)
        {
            outs << "Vector<" << N << ">(" << v.len() << "){";
            outs << v.iter();
            outs << '}';
            return outs;
        }
};

}; // namespace heapless
}; // namespace nel

#endif // !defined(NEL_HEAPLESS_VECTOR_HH)
