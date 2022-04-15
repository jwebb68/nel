#ifndef NEL_HEAPED_NODE_HH
#define NEL_HEAPED_NODE_HH

namespace nel
{
namespace heaped
{

template<typename T>
struct Node;

} // namespace heaped
} // namespace nel

#include <nel/enumerator.hh>
#include <nel/iterator.hh>
#include <nel/optional.hh>
#include <nel/result.hh>
#include <nel/slice.hh>
#include <nel/log.hh>
#include <nel/panic.hh>
#include <nel/defs.hh>

#include <new> // Inplace new
#include <utility> // std::move, std::forward
#include <cstdlib> // std::free, std::malloc, std::realloc

namespace nel
{
namespace heaped
{

template<typename T>
struct Node {
    public:
    private:
        // Number allocated.
        Length alloc_;
        // Number initialised.
        Length len_;
        // Treat as a C struct and use malloc/free to manage..
        // Meh, C++ does not like flexible arrays.
        T values_[1];

    public:
        // Manually do a delete.
        // Don't mix delete/new & malloc/realloc/free.
        // Keep consistent with allocators.
#if defined(__clang__)
#else
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif
        // Meh, cannot use new/delete.
        // Make own using malloc/free.
        // But, can use realloc for better growing..
        // TODO: handle alignment..
        // An aligning malloc?
        static void free(Node *old) noexcept
        {
            if (old != nullptr) { old->~Node(); }
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
        //     Length const align = alignof(Node);
        //     // p % alignment == 0?
        //     return ((Length)p & (align-1)) == 0;
        // }
        static Node *malloc(Count capacity) noexcept
        {
            Node *new_n = realloc(nullptr, capacity);
            new_n->len_ = 0;
            return new_n;
        }

        static Node *realloc(Node *old_n, Count new_cap) noexcept
        {
            // Size of region to allocate excluding align padding.
            // Assume alignof(T) is included in align of Node.
            Count sz = sizeof(Node) - sizeof(T) + new_cap * sizeof(T);

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
#    pragma GCC diagnostic pop
#endif

    public:
        ~Node(void) noexcept
        {
            for (Index i = 0; i < len(); ++i) {
                values_[i].~T();
            }
        }

        // No copying..
        constexpr Node(Node const &) noexcept = delete;
        constexpr Node &operator=(Node const &) const noexcept = delete;

        // No moving
        constexpr Node(Node &&) noexcept = delete;
        constexpr Node &operator=(Node &&) noexcept = delete;

        // use placement new to init.
        // want this as a static func so ca return errors..
        constexpr Node(std::initializer_list<T> l) noexcept
        {
            // How to fail if not big enough.
            nel_panic_if_not(l.size() <= capacity(), "not big enough");
            Index i = 0;
            for (auto it = l.begin(); i < capacity() && it != l.end(); ++it, ++i) {
                new (&values_[i]) T(std::move(*it));
            }
            len_ = i;
        }

    public:
        // use placement new to init.
        constexpr Node(T const &f) noexcept
        {
            // auto it = Slice<T>::from(values_, capacity()).iter();
            // while (true) {
            //     Optional<T &> v = it.next();
            //     if (v.is_none()) { break; }
            //     T &x= v.unwrap();
            //     new (&x) T(f);
            // }
            // // cannot use slice.fill as that expects init'ed values.
            // // and this function is
            // for (Index i=0;i < capacity(); ++i) {
            //     // cannot use assign as values uninitialised.
            //     new (&values_[i]) T(f);
            // }
            Slice<T>::from(values_, capacity())
                .iter()
                .for_each(
                    // cannot use assign as values uninitialised.
                    [&f](T &e) { new (&e) T(f); });
            // for (T *it = values_, *e = (values_ + capacity()); it != e; ++it) {
            //     // cannot use assign as values uninitialised.
            //     new (it) T(f);
            // }
            len_ = capacity();
        }

    public:
        constexpr T *ptr(void) noexcept
        {
            return values_;
        }
        constexpr T const *ptr(void) const noexcept
        {
            return values_;
        }

    public:
        constexpr Count capacity(void) const noexcept
        {
            return alloc_;
        }
        constexpr Length len(void) const noexcept
        {
            return len_;
        }

        constexpr bool is_empty(void) const noexcept
        {
            return len() == 0;
        }

        // TODO: to use try_get as index access can fail.
        constexpr T &operator[](size_t idx) noexcept
        {
            // nel_panic_if_not(idx < len(), "index out of range");
            // return values_[idx];
            return slice()[idx];
        }
        constexpr T const &operator[](size_t idx) const noexcept
        {
            // nel_panic_if_not(idx < len(), "index out of range");
            // return values_[idx];
            return slice()[idx];
        }

        constexpr Slice<T> slice(void) noexcept
        {
            return Slice<T>::from(values_, len());
        }
        constexpr Slice<T const> slice(void) const noexcept
        {
            return Slice<T const>::from(values_, len());
        }

    public:
        // why would a node allow it's items to be destroyed?
        // vector empty/clear..
        void clear(void) noexcept
        {
            iter().for_each([](T &e) { e.~T(); });
            // for (Index i = 0; i < len(); ++i) {
            //     values_[i].~T();
            // }
            len_ = 0;
        }

        // rename to try-push_back?
        template<typename... Args>
        Result<void, T> push_back(Args &&...args) noexcept
        {
            if (len() >= capacity()) {
                // TODO: don't want to create a T on error though..
                // but if one is already created then return it.
                // must the arg be moved on error?
                return Result<void, T>::Err(std::forward<Args>(args)...);
            }

            new (&values_[len()]) T(std::forward<Args>(args)...);
            len_ += 1;
            return Result<void, T>::Ok();
        }

        // rename to try_pop_back?
        Optional<T> pop_back(void) noexcept
        {
            if (len() == 0) { return Optional<T>::None(); }

            auto o = Optional<T>::Some(std::move(values_[len()]));
            values_[len_].~T();
            len_ -= 1;
            return o;
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
        friend Log &operator<<(Log &outs, Node const &v) noexcept
        {
            for (Index i = 0; i < v.len(); ++i) {
                outs << "[" << i << "]:" << v.values_[i] << "\n";
            }
            return outs;
        }
};

} // namespace heaped
} // namespace nel

#endif // NEL_HEAPED_NODE_HH
