#ifndef NEL_HEAPED_ARRAY_HH
#define NEL_HEAPED_ARRAY_HH

namespace nel {
namespace heaped {

template<typename T>
struct Array;

}
}

#include "heaped/node.hh"
#include "iterator.hh"
#include "enumerator.hh"
#include "slice.hh"

namespace nel {
namespace heaped {

// fixed size array
// all items allocd when created
// all items deletd when destroyed
//
template<typename T>
struct Array {
    public:
    private:
        typedef Node<T> ArrNode;
        // cannot use new/delete as created using malloc/realloc.
        ArrNode *item_;

    public:
        ~Array(void)
        {
            ArrNode::free(item_);
        }

        // if no init list?
        // only if T has default cons
        // is this default?
        constexpr Array(void) noexcept
            : item_(nullptr)
        {}

        // initialiser list initialisation required here..

        // if type has copy, then could fill?

        // no copying
        constexpr Array(Array const &o) = delete;
        constexpr Array &operator=(Array const &o) = delete;

        // moving allowed
        constexpr Array(Array &&o) noexcept
            : item_(std::move(o.item_))
        {
            o.item_ = nullptr;
        }

        constexpr Array &operator=(Array &&o) noexcept
        {
            Array t(std::move(o));
            swap(t);
            return *this;
        }
        void swap(Array &o) noexcept
        {
            std::swap(item_, o.item_);
        }

    public:
        // how to convert vector to array
        // into on vec, then need to pass Node into array using private ctor.. needs arr::friend vec
        // from on array, then need to detach node from vec using provate func.. needs vec:friend arr
        // rust: as_ptr(&self) -> *const T
        // rust: as_mut_ptr(&mut self) -> *mut T

        constexpr Array(ArrayNode *const n) noexcept
            : item_(n)
        {}

    public:
        constexpr size_t len(void) const noexcept
        {
            return (item != nullptr) ? item_->len() : 0;
        }

        // move item in (idx) - use mut []
        // move item out (idx) - use mut []

        constexpr T &operator[](int idx) noexcept
        {
            nel_panic_ifnot(item_ != nullptr, "invalid array");
            return (*item_)[idx];
        }
        constexpr T const &operator[](int idx) const noexcept
        {
            nel_panic_ifnot(item_ != nullptr, "invalid array");
            return (*item_)[idx];
        }

        constexpr Slice<T> slice(void)
        {
            return Slice<T>(values_, len());
        }
        constexpr Slice<T> slice(size_t b, size_t e)
        {
            // err N yet given differing range?
            return slice().slice(b,e);
        }

        constexpr Slice<T const> slice(void) const
        {
            return Slice<T>(values_, len());
        }
        constexpr Slice<T const> slice(size_t b, size_t e) const
        {
            // err N yet given differing range?
            return slice().slice(b,e);
        }

        constexpr Iterator<T const> iter(void) const
        {
            return slice().iter();
        }
        constexpr Iterator<T> iter(void)
        {
            return slice().iter();
        }

        constexpr Enumerator<T const> enumer(void) const
        {
            return slice().enumer();
        }
        constexpr Enumerator<T> enumer(void)
        {
            return slice().enumer();
        }

    public:
        friend Log &operator<<(Log &outs, Array const &v) noexcept
        {
            outs << "Array("<< v.len() <<"){" << "\n";
            for (size_t i=0; i < v.len(); ++i) {
                outs << "["<<i<<"]:" << v.values_[i] << "\n";
            }
            outs << "}";
            return outs;
        }

};

}
}


#endif//NEL_HEAPED_ARRAY_HH
