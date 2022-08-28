#ifndef NEL_HEAPED_VECTOR_HH
#define NEL_HEAPED_VECTOR_HH

namespace nel
{
namespace heaped
{

template<typename T>
struct Vector;

} // namespace heaped
} // namespace nel

#include <nel/heaped/node.hh>
#include <nel/enumerator.hh>
#include <nel/iterator.hh>
#include <nel/slice.hh>
#include <nel/optional.hh>
#include <nel/result.hh>
#include <nel/log.hh>

namespace nel
{
namespace heaped
{

/**
 * Vector
 *
 * A container of type T held in a contiguous block (a variable sized array.)
 * Capacity (max number of elements) is limited only by ram (unbounded).
 * Cannot be implicitly coped.
 * Can be implicitly moved.
 */
template<typename T>
struct Vector {
    public:
    private:
        // not using unique_ptr as didn't use new to alloc it.
        // TODO: create malloc/free unique_ptr or see if unique_ptr can be used
        // to call free directly.. (it seems to have capability to do so).
        // maybe use of allocator?
        typedef Node<T> VectorNode;
        VectorNode *item_;

    private:
        constexpr Vector(VectorNode *const n) noexcept: item_(n) {}

    public:
        /**
         * destroy the vector, deleting all elements owned by it.
         */
        ~Vector(void)
        {
            VectorNode::free(item_);
        }

        // default ctor is ok since it cannot throw.
        // TODO: but is it 'correct'? i.e. must arrays be created initialised?
        constexpr Vector(void) noexcept: item_(nullptr) {}

        // No copying..
        constexpr Vector(Vector const &o) = delete;
        constexpr Vector &operator=(Vector const &o) = delete;

        // moving ok.
        constexpr Vector(Vector &&o) noexcept: item_(std::move(o.item_))
        {
            o.item_ = nullptr;
        }
        constexpr Vector &operator=(Vector &&o) noexcept
        {
            if (this != &o) {
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
         * Create a vector with the initial allocation.
         *
         * @param initial The number of elements to initially allocate.
         *
         * @returns the vector.
         */
        static constexpr Vector with_capacity(Count cap) noexcept
        {
            if (cap == 0) { return Vector(); }
            Vector a(VectorNode::malloc(cap));
            return a;
        }

        /**
         * Attempt to create vector from initialiser list
         *
         * @param l initialiser list to use
         * @return on success, an Optional::Some holding the created vector.
         * @return on fail: Optional::None
         */
        static constexpr Optional<Vector> try_from(std::initializer_list<T> l) noexcept
        {
            Vector a = Vector::empty();
            auto r = a.push_back(l);
            if (r.is_err()) { return Optional<Vector>::None(); }
            return Optional<Vector>::Some(std::move(a));
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
            return (item_ == nullptr) ? 0 : item_->capacity();
        }

        /**
         * Returns the number of items currently in use.
         *
         * @returns the current in use count.
         */
        constexpr Length len(void) const noexcept
        {
            return (item_ == nullptr) ? 0 : item_->len();
        }

        /**
         * Determines if the vector is empty (i.e. in-use count of 0).
         *
         * @returns true if in-use is 0, else false.
         */
        constexpr bool is_empty(void) const noexcept
        {
            return item_ == nullptr || item_->is_empty();
        }

        /**
         * Clears the vector, i.e. removes and destroys all in-use elements.
         */
        void clear(void) noexcept
        {
            if (item_ != nullptr) { item_->clear(); }
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
            // nel_panic_if(item_ == nullptr, "invalid vector");
            // return *item_[idx];
            return slice()[idx];
        }
        constexpr T const &operator[](Index idx) const noexcept
        {
            // nel_panic_if(item_ == nullptr, "invalid vector");
            // return *item_[idx];
            return slice()[idx];
        }
#endif

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
            return (item_ == nullptr) ? Slice<T>::empty() : item_->slice();
        }
        constexpr Slice<T const> slice(void) const noexcept
        {
            return (item_ == nullptr) ? Slice<T const>::empty()
                                      : reinterpret_cast<VectorNode const *>(item_)->slice();
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

    public:
        /**
         * resize vector to size n.
         *
         * If n is greater than current length, the additional members are
         * initialised with copies of f.
         * If n is less than current length, then vec is simply truncated.
         * If n is same as current length, then no action.
         *
         * @param n new size of vector
         * @param f value to fill with if growing.
         * T must be bitcopyable
         * T must be clone-able.
         */
        void resize(Count n, T const &f)
        {
            item_ = VectorNode::resize(item_, n, f);
        }

        /**
         * Change the internal allocation to given number of elements.
         *
         * If new capacity is smaller than current, then current is reduced.
         * If new capacity is less than currently used, then removes all unused
         * allocations and does not reduce the in-use amount (does not delete
         * items in the vec).
         *
         * @param new_cap the new allocation to set.
         *
         * @returns true if (re)allocation succeeded.
         * @returns false otherwise.
         */
        // What to return on reserve fail?
        // Result<void, ?> ?
        // Optional<void> aka bool?
        bool try_reserve(Count new_cap) noexcept
        {
            // TODO: handle alignment issues here.
            // TODO: quanta as a template param?
            Count const quanta = 16;
            if (new_cap > 0) { new_cap = new_cap - new_cap % quanta + quanta; }

            // prevent new_cap going below len..
            // i.e. reserve does not free off items.
            if (new_cap < len()) { new_cap = len(); }

            if (item_ == nullptr) {
                // No current alloc..
                item_ = VectorNode::malloc(new_cap);
                if (item_ == nullptr) { return false; }
            } else if (new_cap == item_->capacity()) {
                // nothing to do.
            } else if (new_cap > item_->capacity()) {
                // Grow.. + realloc.
                // No copying of contained, so bitwise move is ok.
                VectorNode *p = VectorNode::realloc(item_, new_cap);
                if (p == nullptr) { return false; }
                item_ = p;
            } else if (new_cap < item_->capacity()) {
                // Shrink (release off excess..) + realloc.
                // Can shrink below current in use ? no.
                // No copying of contained, so bitwise move is ok.
                // Can realloc handle shrink?
                VectorNode *p = VectorNode::realloc(item_, new_cap);
                if (p == nullptr) { return false; }
                item_ = p;
            }
            return true;
        }

        /**
         * Push a value onto the end of the vec.
         *
         * @param val The value to move into the vec.
         * @returns if successful, Result<void, T>::Ok()
         * @returns if unsuccessful, Result<void, T>::Err() holding val
         */
        Result<void, T> push_back(T &&val) noexcept
        {
            bool ok;
            ok = try_reserve(len() + 1);
            if (!ok) { return Result<void, T>::Err(val); }
            if (item_ == nullptr) { return Result<void, T>::Err(val); }
            return item_->push_back(val);
        }
        // TODO: poss not consistent, as not returning Result<void, T>
        Result<void, std::initializer_list<T>> push_back(std::initializer_list<T> l) noexcept
        {
            typedef std::initializer_list<T> U;
            bool ok;
            ok = try_reserve(len() + l.size());
            if (!ok) { return Result<void, U>::Err(l); }
            if (item_ == nullptr) { return Result<void, U>::Err(l); }
            return item_->push_back(l);
        }
        template<typename... Args>
        Result<void, T> push_back(Args &&...args) noexcept
        {
            bool ok;
            ok = try_reserve(len() + 1);
            if (!ok) { return Result<void, T>::Err(std::forward<Args>(args)...); }
            if (item_ == nullptr) { return Result<void, T>::Err(std::forward<Args>(args)...); }
            return item_->push_back(std::forward<Args>(args)...);
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
            return (item_ == nullptr) ? Optional<T>::None() : item_->pop_back();
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
         * Create an iterator over the contents of the Vector.
         *
         * iterator is invalidated if vector goes out of scope/destroyed.
         *
         * @returns The iterator.
         */
        constexpr auto iter(void) const noexcept
        {
            return slice().iter();
        }
        constexpr auto iter(void) noexcept
        {
            return slice().iter();
        }

        constexpr Enumerator<T const> enumerate(void) const noexcept
        {
            return slice().enumerate();
        }
        constexpr Enumerator<T> enumerate(void) noexcept
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
        friend Log &operator<<(Log &outs, Vector const &v) noexcept
        {
            outs << "Vector(" << v.len() << "){"
                 << "\n";
            if (v.item_ != nullptr) { outs << *v.item_; }
            outs << "}";
            return outs;
        }
};

} // namespace heaped
} // namespace nel

#endif // NEL_HEAPED_VECTOR_HH
