#ifndef NEL_HEAPED_RC_HH
#define NEL_HEAPED_RC_HH

namespace nel {
namespace heaped {

template<typename T>
struct RC;

}
}

#include "element.hh"

#include <utility> //std::move, std::forward

namespace nel {
namespace heaped {

template<typename T>
struct RC {
        // contained value on the heap
        // single threaded refernce counted sharing.
    private:
        struct Node {
            public:
                size_t n_refs_;
                Element<T> value_;

            public:
                // no copying.. use refcounting
                Node(Node const &o) =  delete;
                Node &operator=(Node const &o) =  delete;

                // no moving.. use refcounting
                Node(Node &&o) =  delete;
                Node &operator=(Node &&o) =  delete;

                // value might not have a default..
                //constexpr Node(void) = delete;

                template<typename ...Args>
                constexpr Node(Args &&...args) noexcept
                    : n_refs_(1)
                    , value_(std::forward<Args>(args)...)
                {}

            public:
                // could be members, but then i'd end up with delete this in the release impl..
                // which I don't want.
                static void grab(Node *const v) noexcept
                {
                    if (v != nullptr) {
                        ++v->n_refs_;
                    }
                }
                static void release(Node *const v) noexcept
                {
                    if (v != nullptr) {
                        if (--v->n_refs_ == 0) {
                            delete v;
                        }
                    }
                }
                static T unwrap(Node *const v) noexcept
                {
                    // value moved out from value_ on unwrap
                    // value_ is eff. destroyed by unwrap
                    // so release the internal bit as no longer valid.
                    auto o = v->value_.unwrap();
                    release(v);
                    return o;
                }
        };

    private:
        // cannot use box, as box would call delete on contained on delete of this
        // which is not what's wanted.
        // weak_box? //weak_ptr?
        Node *node_;

    public:
        ~RC(void)
        {
            Node::release(node_);
        }

        // it's meant to be shared, so can copy this.
        constexpr RC(RC const &o) noexcept
            : node_(nullptr)
        {
            Node::grab(o.node_);
            node_ = o.node_;
        }
        constexpr RC &operator=(RC const &o) const noexcept
        {
            if (this != &o) {
                //~RC();
                //new (this) RC(o);
                RC t(o);
                swap(t);
            }
            return *this;
        }
        void swap(RC &o) noexcept
        {
            std::swap(node_, o.node_);
        }

        constexpr RC(void) noexcept
            : node_(nullptr)
        {}

        constexpr RC(RC &&o) noexcept
            : node_(std::move(o.node_))
        {
            o.node = nullptr;
        }
        constexpr RC &operator=(RC &&o) noexcept
        {
            // ~RC();
            // new (this) RC(std::move(o));
            RC t(std::move(o));
            swap(t);
            return *this;
        }

    public:
        template<typename ...Args>
        constexpr RC(Args &&...args) noexcept
            : node_(new Node(std::forward<Args>(args)...))
        {
            // Node created pre-grabbed
        }

    public:
        constexpr T &operator*(void) noexcept
        {
            nel_panic_ifnot(has_value(), "not a value");
            return node_->value;
        }
        constexpr T const &operator*(void) const noexcept
        {
            nel_panic_ifnot(has_value(), "not a value");
            return node_->value;
        }
        constexpr bool has_value(void) const noexcept
        {
            return node_ != nullptr;
        }
        constexpr T unwrap(void) noexcept
        {
            nel_panic_ifnot(has_value(), "not a value");
            auto o = Node::unwrap(node_);
            node_ = nullptr;
            return o;
        }
        constexpr bool operator==(RC const &o) const noexcept
        {
            // same struct, must be same..
            return node_ == o.node_;
        }
        constexpr bool operator!=(RC const &o) const noexcept
        {
            // same struct, must be same..
            return node_ != o.node_;
        }
        constexpr bool operator<(RC const &o) const noexcept
        {
            return *(this) < *o;
        }
        constexpr bool operator>(RC const &o) const noexcept
        {
            return *(this) > *o;
        }
        constexpr bool operator<=(RC const &o) const noexcept
        {
            return *(this) <= *o;
        }
        constexpr bool operator>=(RC const &o) const noexcept
        {
            return *(this) >= *o;
        }
};

}
}

#endif//NEL_HEAPED_RC_HH
