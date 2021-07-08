#ifndef NEL_HEAPED_NODE_HH
#define NEL_HEAPED_NODE_HH

namespace nel {
namespace heaped {

template<typename T>
struct Node;

} // namespace heaped
} // namespace nel

#include "iterator.hh"
#include "enumerator.hh"
#include "slice.hh"
#include "result.hh"
#include "optional.hh"
#include "panic.hh"

#include <new> // Inplace new
#include <cstdlib> // std::free, std::malloc, std::realloc
#include <utility> // std::move, std::forward

namespace nel {
namespace heaped {

template<typename T>
struct Node {
    public:
    private:
        // Number allocated.
        size_t alloc_;
        // Number initialised.
        size_t len_;
        // Treat as a C struct and use malloc/free to manage..
        // Meh, C++ does not like flexible arrays.
        T values_[1];

    public:
        // Manually do a delete.
        // Don't mix delete/new & malloc/realloc/free.
        // Keep consistent with allocators.
#if defined(__clang__)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif
        // Meh, cannot use new/delete.
        // Make own using malloc/free.
        // But, can use realloc for better growing..
        // TODO: handle alignment..
        // An aligning malloc?
        static void free(Node *old) noexcept
        {
            if (old != nullptr) {
                old->~Node();
            }
            std::free(old);
        }

        // Want to have realloc usage (better realloc characteristics)
        // which means C based impl (yuk).
        // Could rewrite allocator interaface to have a realloc func
        // but why isn't this present already? (what reasoning why not?)
        // TODO: handle alignment issues here..
        // How does C++ handle variable length structs?
        // static bool is_aligned(void *p)
        // {
        //     size_t const align = alignof(Node);
        //     // p % alignment == 0?
        //     return ((size_t)p & (align-1)) == 0;
        // }
        static Node *malloc(size_t capacity) noexcept
        {
            Node *new_n = realloc(nullptr, capacity);
            new_n->len_ = 0;
            return new_n;
        }

        static Node *realloc(Node *old_n, size_t new_cap) noexcept
        {
            // Size of region to allocate excluding align padding.
            // Assume alignof(T) is included in align of Node.
            size_t sz = sizeof(Node) - sizeof(T) + new_cap * sizeof(T);

            // Check that max_align is multiple of align (i.e. is realign necessary..)
            // If it isn't, then becomes:
            Node *new_n = reinterpret_cast<Node *>(std::realloc(old_n, sz));
            // TODO: handle (re)alloc fails.
            // Remember, if realloc fails, then old is still valid..
            nel_assert(new_n != nullptr);
            // nel_assert(is_aligned(new_n));
            new_n->alloc_ = new_cap;
            return new_n;
        }
#if defined(__clang__)
#else
#pragma GCC diagnostic pop
#endif

    public:
        ~Node(void)
        {
            for (size_t i = 0; i < len(); ++i) {
                values_[i].~T();
            }
        }

        // No copying..
        constexpr Node(Node const &) = delete;
        constexpr Node &operator=(Node const &) const = delete;

        // No moving
        constexpr Node(Node &&) = delete;
        constexpr Node &operator=(Node &&) = delete;

        Node(std::initializer_list<T> l)
        {
            // How to fail if not big enough.
            nel_panic_if_not(l.size() <= capacity(), "not big enough");
            size_t i = 0;
            for (auto it = l.begin(); i < capacity() && it != l.end(); ++it, ++i) {
                new (&values_[i]) T(std::move(*it));
            }
            len_ = i;
        }

    public:
        constexpr size_t capacity(void) const noexcept
        {
            return alloc_;
        }
        constexpr size_t len(void) const noexcept
        {
            return len_;
        }
        constexpr T &operator[](size_t idx) noexcept
        {
            nel_panic_if_not(idx < len(), "index out of range");
            return values_[idx];
        }
        constexpr T const &operator[](size_t idx) const noexcept
        {
            nel_panic_if_not(idx < len(), "index out of range");
            return values_[idx];
        }

        constexpr bool is_empty(void) const noexcept
        {
            return len() == 0;
        }

        void empty(void) noexcept
        {
            for (size_t i = 0; i < len(); ++i) {
                values_[i].~T();
            }
            len_ = 0;
        }

        constexpr Slice<T> slice(void) noexcept
        {
            return Slice<T>(values_, len());
        }
        constexpr Slice<T const> slice(void) const noexcept
        {
            return Slice<T const>(values_, len());
        }


        constexpr Slice<T> slice(size_t b, size_t e) noexcept
        {
            return slice().subslice(b, e);
        }
        constexpr Slice<T const> slice(size_t b, size_t e) const noexcept
        {
            return slice().subslice(b, e);
        }



        template<typename ...Args>
        Result<void, T> push_back(Args &&...args) noexcept
        {
            if (len() >= capacity()) {
                return Result<void, T>::Err(std::forward<Args>(args)...);
            }

            new (&values_[len()]) T(std::forward<Args>(args)...);
            len_ += 1;
            return Result<void, T>::Ok();
        }

        Optional<T> pop_back(void) noexcept
        {
            if (len() == 0) {
                return Optional<T>::None();
            }

            auto o = Optional<T>::Some(std::move(values_[len()]));
            values_[len_].~T();
            len_ -= 1;
            return o;
        }
    public:
        friend Log &operator<<(Log &outs, Node const &v) noexcept
        {
            for (size_t i=0; i < v.len(); ++i) {
                outs << "["<<i<<"]:" << v.values_[i] << "\n";
            }
            return outs;
        }

};

} // namespace heaped
} // namespace nel

#endif // NEL_HEAPED_NODE_HH
