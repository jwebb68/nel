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

        static Node *realloc(Node *const old_n, Count const new_cap) noexcept
        {
            // Size of region to allocate excluding align padding.
            // Assume alignof(T) is included in align of Node.
            Count new_sz = sizeof(Node) - sizeof(T) + new_cap * sizeof(T);
            Count old_sz
                = (old_n == nullptr) ? 0 : sizeof(Node) - sizeof(T) + old_n->alloc_ * sizeof(T);

            // Check that max_align is multiple of align (i.e. is realign necessary..)
            // If it isn't, then becomes:
            Node *const new_n = reinterpret_cast<Node *>(std::realloc(old_n, new_sz));
            if (new_n == nullptr) { return nullptr; }

            // Remember, if realloc fails, then old is still valid..
            // nel_assert(is_aligned(new_n));
            if (new_sz > old_sz) {
                // clear new memory,
                // assumes realloc adds to end
                memset((uint8_t *)new_n + old_sz, 0xa5, new_sz - old_sz);
            }
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
        static constexpr Optional<Node *> try_from(std::initializer_list<T> l) noexcept
        {
            // cannot use Optional<Node> as size is not known at compile time.
            // TODO: maybe should be try_malloc?
            Node *p = Node::malloc(l.size());
            if (p == nullptr) { return None; }
            auto r = p->push_back(l);
            if (r.is_err()) { return None; }
            return Some(std::move(p));
        }

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

    public:
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
            len_ = 0;
        }

        static Node *resize(Node *old, size_t newsize, T const &fill)
        {
            if (old != nullptr && newsize == old->len()) {
                return old;
            } else if (old == nullptr || newsize > old->len()) {
                // growing.. no excess
                Node *p;
                size_t newalloc = newsize;
                if (old == nullptr || newalloc > old->alloc()) {
                    p = Node::realloc(old, newalloc);
                    if (p == nullptr) {
                        Node::free(old);
                        return nullptr;
                    }
                }
                for (Index i = (old == nullptr) ? 0 : old->len(); i < newsize; ++i) {
                    new (&p->values_[i]) T(fill);
                }
                p->len_ = newsize;
                return p;
            } else if (newsize < old->len()) {
                // shrinking, drop excess.
                for (Index i = newsize; i < old->len(); ++i) {
                    old->values_[i].~T();
                }
                old->len_ = newsize;
                return old;
            }
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

        Result<void, std::initializer_list<T>> push_back(std::initializer_list<T> l) noexcept
        {
            typedef std::initializer_list<T> U;
            if (len() + l.size() > capacity()) {
                // TODO: don't want to create a T on error though..
                // but if one is already created then return it.
                // must the arg be moved on error?
                return Result<void, U>::Err(l);
            }

            Index i = len_;
            for (auto it = l.begin(); it != l.end(); ++i, ++it) {
                new (&values_[i]) T(std::move(*it));
            }
            len_ = i;
            return Result<void, U>::Ok();
        }

        // rename to try_pop_back?
        Optional<T> pop_back(void) noexcept
        {
            if (len() == 0) { return Optional<T>::None(); }

            len_ -= 1;
            T &e = values_[len()];
            auto o = Some(std::move(e));
            e.~T();
            return o;
        }

    public:
        constexpr auto iter(void) const noexcept
        {
            return slice().iter();
        }

        constexpr auto iter(void) noexcept
        {
            return slice().iter();
        }

    public:
        friend Log &operator<<(Log &outs, Node const &v) noexcept
        {
#if 0
            for (Index i = 0; i < v.len(); ++i) {
                outs << "[" << i << "]:" << v.values_[i] << '\n';
            }
#elif 1
            Index i = 0;
            v.iter().for_each([&outs, &i](T const &e) {
                outs << '[' << i << "]:" << e << '\n';
                ++i;
            });
#elif 0
            Index i = 0;
            v.iter().for_each2([&outs, &i](T const &e) {
                outs << '[' << i << "]:" << e << '\n';
                ++i;
            });
#else
            auto it = v.iter();
            Index i = 0;
            outs << '[' << i << "]:" << it.deref();
            ++i;
            it.inc();
            it.for_each2([&](T const &e) {
                outs << ',' << '[' << i << "]:" << e;
                ++i;
            });

#endif
            return outs;
        }
};

} // namespace heaped
} // namespace nel

#endif // NEL_HEAPED_NODE_HH
