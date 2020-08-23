#ifndef NEL_NODE_HH
#define NEL_NODE_HH

namespace nel {
template<typename T>
struct Node;
}

#include "optional.hh"
#include "panic.hh"

#include <new> // inplace new
#include <cstdlib> //std::free, std::malloc, std::realloc

namespace nel {

template<typename T, typename OutT>
struct Iterator {
    public:
    private:
        //shared ?
        // won't pick up changes if realloc'd
        typedef Node<T> ItNode;
        ItNode const &content_;
        size_t idx_;
        size_t last_;

    public:
        Iterator(ItNode const &p)
            : content_(p)
            , idx_(0)
            , last_(p.len())
        {}

        Iterator(ItNode const &p, size_t const)
            : content_(p)
            , idx_(0)
            , last_(0)
        {}

        Optional<OutT> next(void)
        {
            return (idx_ < last_)
                   ? Optional<OutT>::Some(content_.get_unchecked(idx_++))
                   : Optional<OutT>::None();
        }

};

template<typename T>
struct Enumerator {
    public:
    private:
        //shared ?
        // won't pick up changes if realloc'd
        typedef Node<T> ItNode;
        ItNode const &content_;
        size_t idx_;
        size_t last_;

    public:
        Enumerator(ItNode const &p)
            : content_(p)
            , idx_(0)
            , last_(p.len())
        {}

        Enumerator(ItNode const &p, size_t const)
            : content_(p)
            , idx_(0)
            , last_(0)
        {}

        constexpr bool is_done(void) const
        {
            return (idx_ >= last_);
        }
        constexpr operator bool(void) const
        {
            return (idx_ < last_);
        }

        constexpr T const &get(void) const
        {
            return content_.get_unchecked(idx_);
        }
        constexpr T const &operator*(void) const
        {
            return content_.get_unchecked(idx_);
        }

        void inc(void)
        {
            ++idx_;
        }
        void operator++(void)
        {
            ++idx_;
        }
};



template<typename T>
struct Node {
    public:
    private:
        // number allocated
        size_t alloc_;
        // number initialised
        size_t len_;
        // C++ does not like flexible arrays
        // so treat as a C struct and use malloc/free to manage..
        T values_[1];

    public:
        // manually do a delete,
        // don't mix delete/new & malloc/realloc/free
        // keep consistent with allocators

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
        // meh, cannot use new/delete
        // make own using malloc/free.
        static void free(Node *old)
        {
            if (old != nullptr) {
                old->~Node();
            }
            std::free(old);
        }

        // want to have realloc usage (better realloc characteristics)
        // which means C based impl (yuk).
        // TODO: handle alignment issues here..
        // how does C++ handle variable length structs?
        static Node *malloc(size_t capacity)
        {
            size_t sz = sizeof(Node) - sizeof(T) + capacity * sizeof(T);
            Node *item = static_cast<Node *>(std::realloc(nullptr, sz));
            item->alloc_ = capacity;
            item->len_ = 0;
            return item;
        }

        static Node *realloc(Node *old, size_t new_cap)
        {
            size_t sz = sizeof(Node) - sizeof(T) + new_cap * sizeof(T);
            Node *item = static_cast<Node *>(std::realloc(old, sz));
            item->alloc_ = new_cap;
            //len and data should be copied by realloc
            return item;
        }
#pragma GCC diagnostic pop

    public:
        ~Node(void)
        {
            for (size_t i = 0; i < len_; ++i) {
                values_[i].~T();
            }
        }

        // no copying..
        constexpr Node(Node const &) = delete;
        constexpr Node &operator=(Node const &) const = delete;

        // no moving
        constexpr Node(Node &&) = delete;
        constexpr Node &operator=(Node &&) const = delete;

        constexpr size_t capacity(void) const
        {
            return alloc_;
        }
        constexpr size_t len(void) const
        {
            return len_;
        }
        constexpr T &operator[](size_t idx)
        {
            nel_panic_ifnot(idx < len_, "index out of range");
            return get_unchecked(idx);
        }
        constexpr T const &operator[](size_t idx) const
        {
            nel_panic_ifnot(idx < len_, "index out of range");
            return get_unchecked(idx);
        }

        constexpr T const &get_unchecked(size_t idx)
        {
            return values_[idx];
        }
        constexpr T const &get_unchecked(size_t idx) const
        {
            return values_[idx];
        }

        constexpr bool is_empty(void) const
        {
            return len_ == 0;
        }

        void empty(void)
        {
            for (size_t i = 0; i < len_; ++i) {
                values_[i].~T();
            }
            len_ = 0;
        }
        // bool push_back(T &&v)
        // {
        //     bool ok = (len_ < alloc_);
        //     if (ok) {
        //         // remember, values at len and beyond are uninitialised.
        //         // so need to use new to contruct them.
        //         new (&values_[len_]) T(std::move(v));
        //         len_ += 1;
        //     }
        //     return ok;
        // }
        template<typename ...Args>
        void push_back_unchecked(Args &&...args)
        {
            // remember, values at len and beyond are uninitialised.
            // so need to use new to contruct them.
            new (&values_[len_]) T(std::forward<Args>(args)...);
            len_ += 1;
        }
        template<typename ...Args>
        void push_back(Args &&...args)
        {
            nel_panic_ifnot(len_ < alloc_, "no space in node");
            push_back_unchecked(std::forward<Args>(args)...);
        }

        Optional<T> pop_back_internal(void)
        {
            Optional<T> o(std::move(values_[len_]));
            values_[len_].~T();
            len_ -= 1;
        }
        Optional<T> pop_back(void)
        {
            return (len_ > 0)
                ? pop_back_internal()
                : Optional<T>::None();
        }
};



}

#endif//NEL_NODE_HH
