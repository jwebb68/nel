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

template<typename T>
struct Vector {
    public:
    private:
        // May be shared between Vector<T> and Array<T> and Slice<T>?
        // Vector: dynamic
        // Array: static
        // Array from vector: move node from vector to array.
        // Vector from array: move node from vector into array,  arguably nope.

        // Don't use unique_ptr as didn't use new to alloc it.
        // TODO: create malloc/free unique_ptr or see if unique_ptr can be used
        // to call free directly.. (it seems to have capability to do so).
        typedef Node<T> VectorNode;
        VectorNode *item_;

    public:
        // default ctor is ok since it cannot throw.
        // TODO: but is it 'correct'? i.e. must arrays be created initialised?
        constexpr Vector(void) noexcept: item_(nullptr) {}

    protected:
        constexpr Vector(VectorNode *const n) noexcept: item_(n) {}

    public:
        ~Vector(void)
        {
            VectorNode::free(item_);
        }

        static constexpr Vector empty(void) noexcept
        {
            return Vector();
        }

        // No copying..
        constexpr Vector(Vector const &o) = delete;
        constexpr Vector &operator=(Vector const &o) = delete;

        Vector(Vector &&o) noexcept: item_(std::move(o.item_))
        {
            o.item_ = nullptr;
        }
        Vector &operator=(Vector &&o) noexcept
        {
            if (this != &o) {
                this->~Vector();
                new (this) Vector(std::move(o));
            }
            return *this;
        }

        // vector::from(std::initializer_list<T> l)
        constexpr Vector(std::initializer_list<T> l): item_(nullptr)
        {
            // Ah, can fail..., should be a try_reserve then
            // and as try_reserve can fail should be a try_from_initlist.
            reserve(capacity() + l.size());
            // Maybe should be 'item_ = l' ?
            // i.e. assign.
            new (item_) VectorNode(l);
        }

        static constexpr Vector fill(T const &f, Count n)
        {
            if (n == 0) { return Vector(); }
            Vector a(VectorNode::malloc(n));
            new (a.item_) VectorNode(f);
            return a;
        }

    public:
        // For conversion to array..?
        // VectorNode *detach(void) noexcept
        // {
        //     VectorNode *t = item_;
        //     item_ = nullptr;
        //     return t;
        // }

    public:
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
        // What to return on reserve fail?
        // Result<void, ?> ?
        // should be a try_reserve.
        void reserve(Count capacity) noexcept
        {
            // TODO: overalloc to quanta..
            // TODO: could over alloc to some quanta, e.g. 16, 8, etc..

            // TODO: handle alignment issues here.
            if (item_ == nullptr) {
                // No current alloc..
                Count cap = capacity;
                item_ = VectorNode::malloc(cap);
            } else {
                Count cap = len() + capacity;
                if (cap > item_->capacity()) {
                    // Grow.. + realloc.
                    // No copying of contained, so bitwise move is ok.
                    item_ = VectorNode::realloc(item_, cap);
                } else if (cap < item_->capacity()) {
                    // Shrink (release off excess..) + realloc.
                    // Can shrink below current in use ?
                    // No copying of contained, so bitwise move is ok.
                    // Can realloc handle shrink?
                    item_ = VectorNode::realloc(item_, cap);
                } else {
                    // Not shrinking if same
                }
            }
        }

        // The problem with returning a Result<void, T> here is that
        // if there's no space, then the push-back fails and a T gets created
        // only to be dropped by the caller (so waste of time creating the T).
        // But, this func doesn't know what the caller will do with it.
        // Also, if a bool is returned, then it wont be created, but then
        // if a T is passed in, it'd be destroyed (instead of maybe being moved back.)
        // 1. return bool:
        //    - success: pass in T, T moved to vector.
        //    - success: pass in T args, T created, T created inplace in vector.
        //    - success: move ctor called once for each arg.
        //    - fails: pass in T, gets destroyed  (no mech to pass back out).
        //    - fails: pass in T args, destroy all args..
        //    - move ctor not called for each arg
        //    - dtor called for each arg.
        // 2. return result<void,T>
        //    - success: pass in T, T moved to vector.
        //    - success: pass in T args, T created inplace in vector.
        //    - fails: pass in T, passed back out, dropped by caller.
        //    - fails: pass in T args, creates T, passed out, dropped by caller..
        //    - success: move ctor called once for each arg.
        //    - fail: move ctor called once for each arg.
        //    - fail: dtor called once for T.
        template<typename... Args>
        Result<void, T> push_back(Args &&...args) noexcept
        {
            reserve(len() + 1);
            return (item_ == nullptr) ? Result<void, T>::Err(std::forward<Args>(args)...)
                                      : item_->push_back(std::forward<Args>(args)...);
        }

        Optional<T> pop_back(void) noexcept
        {
            return (item_ == nullptr) ? Optional<T>::None() : item_->pop_back();
        }

    public:
        constexpr Iterator<T const> iter(void) const noexcept
        {
            return slice().iter();
        }
        constexpr Iterator<T> iter(void) noexcept
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
