// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(NEL_HEAPED_NODE_HH)
#    define NEL_HEAPED_NODE_HH

namespace nel
{
namespace heaped
{

template<typename T>
struct Node;

} // namespace heaped
} // namespace nel

#    include <nel/iterator.hh>
#    include <nel/optional.hh>
#    include <nel/result.hh>
#    include <nel/slice.hh>
#    include <nel/log.hh>
#    include <nel/panic.hh>
#    include <nel/defs.hh>
#    include <nel/memory.hh> // move,forward
#    include <nel/new.hh> // new (p) T()

#    include <cstdlib> // std::free, std::malloc, std::realloc

namespace nel
{
namespace heaped
{

template<typename T>
struct Node
{
    public:
        typedef T Type;

    private:
        // Number allocated.
        Length alloc_;
        // Number initialised.
        Length len_;
        // Treat as a C struct and use malloc/free to manage..
        // Meh, C++ does not like flexible arrays.
        Type values_[1];

    public:
        // Manually do a delete.
        // Don't mix delete/new & malloc/realloc/free.
        // Keep consistent with allocators.
#    if defined(__clang__)
#    else
#        pragma GCC diagnostic push
#        pragma GCC diagnostic ignored "-Wclass-memaccess"
#    endif
        // Meh, cannot use new/delete.
        // Make own using malloc/free.
        // But, can use realloc for better growing..
        // TODO: handle alignment..
        // An aligning malloc?
        static void free(Node *old)
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
        static Node *malloc(Count capacity)
        {
            Node *new_n = realloc(nullptr, capacity);
            new_n->len_ = 0;
            return new_n;
        }

        static Node *realloc(Node *const old_n, Count const new_cap)
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
                elem::wipe(reinterpret_cast<uint8_t *>(new_n) + old_sz, new_sz - old_sz);
            }
            new_n->alloc_ = new_cap;
            return new_n;
        }
#    if defined(__clang__)
#    else
#        pragma GCC diagnostic pop
#    endif

    public:
        constexpr ~Node(void)
        {
            for (Index i = 0; i < len(); ++i) {
                values_[i].~Type();
            }
        }

        // No copying..
        constexpr Node(Node const &) = delete;
        constexpr Node &operator=(Node const &) const = delete;

        // No moving
        constexpr Node(Node &&) = delete;
        constexpr Node &operator=(Node &&) = delete;

#    if 0
        // use placement new to init.
        // want this as a static func so ca return errors..
        constexpr Node(std::initializer_list<Type> &&l)
        {
            // How to fail if not big enough.
            nel_panic_if_not(l.size() <= capacity(), "not big enough");
            Index i = 0;
            for (auto it = l.begin(); i < capacity() && it != l.end(); ++it, ++i) {
                new (&values_[i]) Type(forward<T>(*it));
            }
            len_ = i;
        }
#    endif

    public:
#    if 0
        static constexpr Optional<Node *> try_from(std::initializer_list<Type> &&l)
        {
            // cannot use Optional<Node> as size is not known at compile time.
            // TODO: maybe should be try_malloc?
            Node *p = Node::malloc(l.size());
            if (p == nullptr) { return None; }
            return p->push_back(l).ok().template map<Node *>([p]() -> Node * { return p; });
        }
#    endif

        // use placement new to init.
        constexpr Node(Type const &f)
        {
            Slice<Type>::from(values_, capacity())
                .iter()
                .for_each(
                    // cannot use assign as values uninitialised.
                    [&f](auto &e) { new (&e) Type(f); });
            len_ = capacity();
        }

    public:
        constexpr Type *ptr(void)
        {
            return values_;
        }

        constexpr Type const *ptr(void) const
        {
            return values_;
        }

    public:
        constexpr Count capacity(void) const
        {
            return alloc_;
        }

        constexpr Length len(void) const
        {
            return len_;
        }

        constexpr bool is_empty(void) const
        {
            return len() == 0;
        }

    public:
        constexpr Slice<Type> slice(void)
        {
            return Slice<Type>::from(values_, len());
        }

        constexpr Slice<Type const> slice(void) const
        {
            return Slice<Type const>::from(values_, len());
        }

    public:
        // why would a node allow it's items to be destroyed?
        // vector empty/clear..
        void clear(void)
        {
            iter().for_each([](Type &e) { e.~Type(); });
            len_ = 0;
        }

        static Node *resize(Node *old, size_t newsize, Type const &fill)
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
                    new (&p->values_[i]) Type(fill);
                }
                p->len_ = newsize;
                return p;
            } else if (newsize < old->len()) {
                // shrinking, drop excess.
                for (Index i = newsize; i < old->len(); ++i) {
                    old->values_[i].~Type();
                }
                old->len_ = newsize;
                return old;
            }
        }

        // rename to try-push_back?
        template<typename... Args>
        Result<void, T> push_back(Args &&...args)
        {
            if (len() >= capacity()) {
                // TODO: don't want to create a T on error though..
                // but if one is already created then return it.
                // must the arg be moved on error?
                return Result<void, Type>::Err(forward<Args>(args)...);
            }

            new (&values_[len()]) Type(forward<Args>(args)...);
            len_ += 1;
            return Result<void, Type>::Ok();
        }

#    if 0
        Result<void, std::initializer_list<Type>> push_back(std::initializer_list<Type> l)
        {
            typedef std::initializer_list<Type> U;
            if (len() + l.size() > capacity()) {
                // TODO: don't want to create a T on error though..
                // but if one is already created then return it.
                // must the arg be moved on error?
                return Result<void, U>::Err(l);
            }

            Index i = len_;
            for (auto it = l.begin(); it != l.end(); ++i, ++it) {
                new (&values_[i]) T(move(*it));
            }
            len_ = i;
            return Result<void, U>::Ok();
        }
#    endif

        // rename to try_pop_back?
        Optional<Type> pop_back(void)
        {
            if (len() == 0) { return Optional<Type>::None(); }

            len_ -= 1;
            auto &e = values_[len()];
            auto o = Some(move(e));
            // should be safe to  destroy, value has been moved out.
            e.~Type();
            return o;
        }

    public:
        constexpr auto iter(void) const
        {
            return slice().iter();
        }

        constexpr auto iter(void)
        {
            return slice().iter();
        }

    public:
        friend Log &operator<<(Log &outs, Node const &v)
        {
            outs << v.iter();
            return outs;
        }
};

} // namespace heaped
} // namespace nel

#endif // !defined(NEL_HEAPED_NODE_HH)
