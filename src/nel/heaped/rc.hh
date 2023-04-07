// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(NEL_HEAPED_RC_HH)
#    define NEL_HEAPED_RC_HH

namespace nel
{
namespace heaped
{

template<typename T>
struct RC;

} // namespace heaped
} // namespace nel

#    include <nel/element.hh>
#    include <nel/panic.hh>
#    include <nel/defs.hh>

namespace nel
{
namespace heaped
{

template<typename T>
struct RC
{
        // Contained value on the heap.
        // Single threaded reference counted sharing.
    private:
        struct Node
        {
            private:
                Count n_refs_;
                bool has_value_;
                Element<T> value_;

            public:
                // No copying.. use refcounting.
                Node(Node const &o) = delete;
                Node &operator=(Node const &o) = delete;

                // No moving.. use refcounting.
                Node(Node &&o) = delete;
                Node &operator=(Node &&o) = delete;

                // Value might not have a default..
                // constexpr Node(void) = delete;

                template<typename... Args>
                constexpr Node(Args &&...args)
                    : n_refs_(1)
                    , has_value_(true)
                    , value_(forward<Args>(args)...)
                {
                }

            public:
                // Could be members, but then it'll end up with 'delete this' in the release impl..
                // which I don't want.
                static void grab(Node *const v)
                {
                    if (v != nullptr) { ++v->n_refs_; }
                }

                static void release(Node *const v)
                {
                    if (v != nullptr) {
                        if (--v->n_refs_ == 0) { delete v; }
                    }
                }

                static T unwrap(Node *const v)
                {
                    // Value moved out from value_ on unwrap().
                    // Value_ is eff. destroyed by unwrap().
                    // So release the internal bit as no longer valid.
                    nel_panic_if_not(v->has_value_, "invalid rc node");
                    auto o = move(*(v->value_));
                    v->has_value_ = false;
                    release(v);
                    return o;
                }

            public:
                T &ref(void)
                {
                    nel_panic_if_not(has_value(), "invalid rc node");
                    return *value_;
                }

                T const &ref(void) const
                {
                    nel_panic_if_not(has_value(), "invalid rc node");
                    return *value_;
                }

                bool has_value(void) const
                {
                    return has_value_;
                }
        };

    private:
        // Cannot use Box, as Box would call delete on contained on delete of this
        // which is not what's wanted.
        // weak_box? weak_ptr?
        Node *node_;

    public:
        ~RC(void)
        {
            Node::release(node_);
        }

        // It's meant to be shared, so can copy this.
        // much badness, non-const ref for a copy?
        constexpr RC(RC &o)
            : node_(nullptr)
        {
            Node::grab(o.node_);
            node_ = o.node_;
        }

        constexpr RC(RC const &o)
            : node_(nullptr)
        {
            Node::grab(o.node_);
            node_ = o.node_;
        }

        // constexpr RC &operator=(RC const &o) const
        constexpr RC &operator=(RC &o)
        {
            if (this != &o) {
                //~RC();
                // new (this) RC(o);
                RC t(o);
                swap(t);
            }
            return *this;
        }

        constexpr RC &operator=(RC const &o) const
        {
            if (this != &o) {
                //~RC();
                // new (this) RC(o);
                RC t(o);
                swap(t);
            }
            return *this;
        }

        void swap(RC &o)
        {
            swap(node_, o.node_);
        }

        constexpr RC(void)
            : node_(nullptr)
        {
        }

        constexpr RC(RC &&o)
            : node_(move(o.node_))
        {
            o.node_ = nullptr;
        }

        constexpr RC &operator=(RC &&o)
        {
            if (this != &o) {
                this->~RC();
                new (this) RC(move(o));
            }
            return *this;
        }

    public:
        constexpr RC(T &&v)
            : node_(new Node(move(v)))
        {
            // Node created pre-grabbed.
        }

        template<typename... Args>
        constexpr RC(Args &&...args)
            : node_(new Node(forward<Args>(args)...))
        {
            // Node created pre-grabbed.
        }

    public:
        /**
         * Return a mutable reference to the boxed value.
         *
         * @returns reference to the boxed value
         * @warning Panics if no value to return (e.g. use after move).
         */
        constexpr T &operator*(void)
        {
            nel_panic_if_not(has_value(), "not a value");
            return node_->ref();
        }

        /**
         * Return a reference to the boxed value.
         *
         * @returns reference to the boxed value
         * @warning Panics if no value to return (e.g. use after move).
         */
        constexpr T const &operator*(void) const
        {
            nel_panic_if_not(has_value(), "not a value");
            return node_->ref();
        }

        /**
         * Determines if this box has a value.
         *
         * @returns true if there is a value, false otherwise.
         */
        constexpr bool has_value(void) const
        {
            return node_ != nullptr && node_->has_value();
        }

        /**
         * Removes and returns the value contained in the box.
         *
         * The box is invalidated by this action.
         * And all references to the box are invalidated.
         *
         * @returns the value in the box.
         * @warning Panics if no value to return (e.g. use after move).
         */
        constexpr T unwrap(void)
        {
            nel_panic_if_not(has_value(), "not a value");
            auto o = Node::unwrap(node_);
            node_ = nullptr;
            return o;
        }

        // constexpr bool operator==(RC const &o) const
        // {
        //     return node_ == o.node_;
        // }
        // constexpr bool operator!=(RC const &o) const
        // {
        //     return node_ != o.node_;
        // }
        // constexpr bool operator<(RC const &o) const
        // {
        //     return *(this) < *o;
        // }
        // constexpr bool operator>(RC const &o) const
        // {
        //     return *(this) > *o;
        // }
        // constexpr bool operator<=(RC const &o) const
        // {
        //     return *(this) <= *o;
        // }
        // constexpr bool operator>=(RC const &o) const
        // {
        //     return *(this) >= *o;
        // }
};

} // namespace heaped
} // namespace nel

#endif // !defined(NEL_HEAPED_RC_HH)
