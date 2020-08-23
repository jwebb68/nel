#ifndef NEL_RC_HH
#define NEL_RC_HH

namespace nel {
template<typename T>
struct RC;
}

#include "element.hh"

#include <utility> //std::move, std::forward

namespace nel {
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

                // constexpr Node(T &&v)
                //     : n_refs_(1)
                //     , value_(std::move(v))
                // {}

                template<typename ...Args>
                constexpr Node(Args &&...args)
                    : n_refs_(1)
                    , value_(std::forward<Args>(args)...)
                {}

            public:
                // could be members, but then i'd end up with delete this in the release impl..
                // which I don't want.
                static void grab(Node *const v)
                {
                    if (v != nullptr) {
                        ++v->n_refs_;
                    }
                }
                static void release(Node *const v)
                {
                    if (v != nullptr) {
                        if (--v->n_refs_ == 0) {
                            delete v;
                        }
                    }
                }
                constexpr static T unwrap(Node *const v)
                {
                    T t(v->value_.unwrap());
                    release(v);
                    return t;
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

        // it's shared, can copy this.
        constexpr RC(RC const &o)
            : node_(nullptr)
        {
            Node::grab(o.node_);
            node_ = o.node_;
        }
        constexpr RC &operator=(RC const &o) const
        {
            if (this != &o) {
                //~RC();
                //new (this) RC(o);
                RC t(o);
                swap(t);
            }
            return *this;
        }
        void swap(RC &o)
        {
            std::swap(node_, o.node_);
        }

        constexpr RC(void)
            : node_(nullptr)
        {}

        constexpr RC(RC &&o)
            : node_(std::move(o.node_))
        {
            o.node = nullptr;
        }
        constexpr RC &operator=(RC &&o)
        {
            // ~RC();
            // new (this) RC(std::move(o));
            RC t(std::move(o));
            swap(t);
            return *this;
        }
    public:
        // constexpr RC(T &&v)
        //     : node_(nullptr)
        // {
        //     // Node created pre-grabbed
        //     node_ = new Node(std::move(v));
        // }
        template<typename ...Args>
        constexpr RC(Args &&...args)
            : node_(nullptr)
        {
            // Node created pre-grabbed
            node_ = new Node(std::forward<Args>(args)...);
        }
    public:
        constexpr T &operator*(void)
        {
            assert(node_ != nullptr);
            return node_->value;
        }
        constexpr T const &operator*(void) const
        {
            assert(node_ != nullptr);
            return node_->value;
        }
        constexpr bool has_value(void) const
        {
            return node_ != nullptr;
        }
        constexpr T unwrap(void)
        {
            assert(node_ != nullptr);
            T v(Node::unwrap(node_));
            node_ = nullptr;
            return v;
        }
        constexpr bool operator==(RC const &o) const
        {
            // same struct, must be same..
            return node_ == o.node_;
        }
        constexpr bool operator!=(RC const &o) const
        {
            // same struct, must be same..
            return node_ != o.node_;
        }
        constexpr bool operator<(RC const &o) const
        {
            return *(this) < *o;
        }
        constexpr bool operator>(RC const &o) const
        {
            return *(this) > *o;
        }
        constexpr bool operator<=(RC const &o) const
        {
            return *(this) <= *o;
        }
        constexpr bool operator>=(RC const &o) const
        {
            return *(this) >= *o;
        }
};

}

#endif//NEL_RC_HH
