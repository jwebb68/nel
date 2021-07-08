#ifndef NEL_HEAPED_VECTOR_HH
#define NEL_HEAPED_VECTOR_HH

namespace nel {
namespace heaped {

template<typename T>
struct Vector;

} // namespace heaped
} // namespace nel

#include "log.hh"
#include "heaped/node.hh"
#include "iterator.hh"
#include "enumerator.hh"
#include "slice.hh"
#include "result.hh"
#include "optional.hh"

namespace nel {
namespace heaped {

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
        typedef Node<T> VecNode;
        VecNode *item_;

    public:
        // Own an existing array.
        // hmm, fraught with issues that
        // Array(T *const v, size_t len)
        //     : item_(v)
        // {}
        ~Vector(void)
        {
            VecNode::free(item_);
        }

        constexpr Vector(void) noexcept
            : item_(nullptr)
        {}

        // No copying..
        constexpr Vector(Vector const &o) = delete;
        constexpr Vector &operator=(Vector const &o) = delete;

        Vector(Vector &&o) noexcept
            : item_(std::move(o.item_))
        {
            o.item_ = nullptr;
        }
        Vector &operator=(Vector &&o) noexcept
        {
            Vector t(std::move(o));
            swap(t);
            return *this;
        }
        void swap(Vector &o) noexcept
        {
            std::swap(item_, o.item_);
        }

        constexpr Vector(std::initializer_list<T> l)
            : item_(nullptr)
        {
            // Ah, can fail...
            reserve(capacity() + l.size());
            // Maybe should be 'item_ = l' ?
            // i.e. assign.
            new (item_) VecNode(l);
        }

    public:
        // For conversion to array..?
        VecNode *detach(void) noexcept
        {
            VecNode *t = item_;
            item_ = nullptr;
            return t;
        }

    public:
        Vector(size_t initial) noexcept
            : item_(VecNode::malloc(initial))
        {}

        constexpr size_t capacity(void) const noexcept
        {
            return (item_ == nullptr) ? 0 : item_->capacity();
        }
        constexpr size_t len(void) const noexcept
        {
            return (item_ == nullptr) ? 0 : item_->len();
        }
        constexpr bool is_empty(void) const noexcept
        {
            return item_ == nullptr || item_->is_empty();
        }
        void empty(void) noexcept
        {
            if (item_ != nullptr) {
                item_->empty();
            }
        }

        // What to return on reserve fail?
        // Result<void, ?> ?
        void reserve(size_t capacity) noexcept
        {
            // TODO: overalloc to quanta..
            size_t cap = capacity;

            // TODO: could over alloc to some quanta, e.g. 16, 8, etc..
            // TODO: handle alignment issues here.
            if (item_ == nullptr) {
                // No current alloc..
                item_ = VecNode::malloc(cap);
            } else if (cap > item_->capacity()) {
                // Grow.. + realloc.
                // No copying of contained, so bitwise move is ok.
                item_ = VecNode::realloc(item_, cap);
            } else if (cap < item_->capacity()) {
                // Shrink (release off excess..) + realloc.
                // Can shrink below current in use ?
                if (cap > item_->len()) {
                    // No copying of contained, so bitwise move is ok.
                    // Can realloc handle shrink?
                    item_ = VecNode::realloc(item_, cap);
                } else {
                    // Not shrinking if same or below current in use.
                }
            } else {
                // cap == item_->capacity()
                // no action
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
        template<typename ...Args>
        Result<void, T> push_back(Args &&...args) noexcept
        {
            reserve(len()+1);
            return (item_ == nullptr)
                ? Result<void, T>::Err(std::forward<Args>(args)...)
                : item_->push_back(std::forward<Args>(args)...);
        }
        Optional<T> pop_back(void) noexcept
        {
            return (item_ == nullptr)
                   ? Optional<T>::None()
                   : item_->pop_back();
        }

        constexpr T &operator[](size_t idx) noexcept
        {
            nel_panic_if(item_ == nullptr, "invalid vector");
            return *item_[idx];
        }
        constexpr T const &operator[](size_t idx) const noexcept
        {
            nel_panic_if(item_ == nullptr, "invalid vector");
            return *item_[idx];
        }

        constexpr Iterator<T const> iter(void) const noexcept
        {
            return slice().iter();
        }
        constexpr Iterator<T> iter(void) noexcept
        {
            return slice().iter();
        }

        constexpr Enumerator<T const> enumer(void) const noexcept
        {
            return slice().enumer();
        }
        constexpr Enumerator<T> enumer(void) noexcept
        {
            return slice().enumer();
        }

        constexpr Slice<T> slice(void) noexcept
        {
            return (item_ == nullptr)
                   ? Slice<T>(nullptr, 0)
                   : item_->slice();
        }

        constexpr Slice<T> slice(size_t b, size_t e) noexcept
        {
            return slice().subslice(b, e);
        }


        constexpr Slice<T const> slice(void) const noexcept
        {
            return (item_ == nullptr)
                   ? Slice<T const>(nullptr, 0)
                   : reinterpret_cast<VecNode const *>(item_)->slice();
        }

        constexpr Slice<T const> slice(size_t b, size_t e) const noexcept
        {
            return slice().subslice(b, e);
        }

    public:
        friend Log &operator<<(Log &outs, Vector const &v) noexcept
        {
            outs << "Vector("<< v.len() <<"){" << "\n";
            if (v.item_ != nullptr) {
                outs << *v.item_;
            }
            outs << "}";
            return outs;
        }
};

} // namespace heaped
} // namespace nel

#endif // NEL_HEAPED_VECTOR_HH
