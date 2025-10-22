// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(NEL_OPTIONAL_HH)
#    define NEL_OPTIONAL_HH

namespace nel
{

template<typename T>
struct Optional;

struct NoneT;

} // namespace nel

#    include <nel/element.hh>
#    include <nel/log.hh>
#    include <nel/types.hh> // Void
#    include <nel/panic.hh>
#    include <nel/memory.hh> // move,forward
#    include <nel/new.hh> // new (p) T()

namespace nel
{

struct NoneT
{
};
constexpr NoneT None() {
    return NoneT {};
}


static constexpr NoneT const none = NoneT {};

template<typename T>
struct SomeT
{
    public:
        using Type = T;

    private:
        friend Optional<Type>;
        Type val_;

    public:
        constexpr SomeT(Type &&val)
            : val_(move(val))
        {
        }

    public:
        SomeT(SomeT &&rhs) = default;
        SomeT &operator=(SomeT &&rhs) = default;

        SomeT(SomeT const &rhs) = delete;
        SomeT &operator=(SomeT const &rhs) = default;

    private:
        // Having deref as public allows access to the value
        // contained so it can be moved/copied/re-assigned, which is not wanted.
        // The value needs to be hidden and when needed, unwrapped (thus invalidating the
        // SomeT.

        constexpr Type &deref()
        {
            return val_;
        }

        constexpr Type const &deref() const
        {
            return val_;
        }

    private:
        constexpr Type &operator*()
        {
            return deref();
        }

        constexpr Type const &operator*() const
        {
            return deref();
        }

#if 0
    // SomeT/SomeT<T>:
    // is a wrapper around T, preventing access.
    // so shouldn't need oper->().
    private:
        //  doesn't work if T = reference, so use a Ref<T>
        constexpr Type *operator->()
        {
            return &deref();
        }

        constexpr Type const *operator->() const
        {
            return &deref();
        }
#endif
    public:
        constexpr friend bool operator==(SomeT const &lhs, SomeT const &rhs)
        {
            return lhs.val_ == rhs.val_;
        }
        constexpr friend bool operator!=(SomeT const &lhs, SomeT const &rhs)
        {
            return lhs.val_ != rhs.val_;
        }
};

template<typename T>
constexpr inline Optional<T> Some(T &&t);

constexpr inline Optional<Void> Some(Void);

constexpr inline Optional<Void> Some();

/**
 * Optional<T>
 *
 * A non-exception throwing construct similar to std::optional.
 * Where std::optional throws on access for value is no value is present,
 * this construct panics.
 * c.f. rust Option.
 *
 * usage:
 *
 * ```c++
 *    Optional<int> v1; // compile error
 *
 *    Optional<int> v1 = Optional<int>::Some(1);
 *    Optional<int> v1 = Some(1);
 *    Optional<int> v2 = None;
 *
 *    bool b1 = v1.is_some(); // true
 *    bool b2 = v1.is_none(); // false
 *
 *    bool b3 = v2.is_some(); // false
 *    bool b4 = v2.is_none(); // true
 *
 *    bool b5 = v1 == v2; // false
 *
 *    bool b6 = v1 == Some(1); // true
 *    bool b7 = v1 == Some(2); // false
 *    bool b8 = v1 == Some("ha ha"); // fail to compile, type mismatch
 *    bool b9 = v1 == None; // false
 *
 *    bool b10 = v2 == Some(1); // false
 *    bool b11 = v2 == Some(2); // false
 *    bool b12 = v2 == Some("ha ha"); // fail to compile., type mismatch
 *    bool b13 = v2 == None; // true

 *    int i = v1.unwrap();
 *    // here: v1 is consumed (does not contain value any more)
 *    bool b6 = v1.is_some(); // false
 *    bool b7 = v1.is_none(); // false
 *    int j = v1.unwrap(); // would call abort.
 *
 *    int i = v2.unwrap(); // would call abort, std::optional would throw
 *
 * ```
 */
template<typename T>
struct Optional
{
    public:
        using Type = T;

    private:
        // Tagged enum thing.
        // not an enm struct so methods can be put on it.
        struct Tag
        {
            public:
                enum ETag {
                    inval = 0,
                    none,
                    some,
                } val_;

            public:
                // When moved, invalidate src.
                Tag(Tag &&rhs)
                    : val_(move(rhs.val_))
                {
                    rhs.val_ = inval;
                }

                Tag &operator=(Tag &&rhs)
                {
                    val_ = move(rhs.val_);
                    rhs.val_ = inval;
                    return *this;
                }

            public:
                Tag(Tag const &) = default;
                Tag &operator=(Tag const &) = default;

            public:
                constexpr Tag(ETag t)
                    : val_(t)
                {
                }

                constexpr operator ETag() const
                {
                    return val_;
                }
        } tag_;

        // Use union to disable certain default methods on T
        // Need to use class/struct as references cannot be used in unions,
        // so this is possible: Optional<Foo &>
        union {
                SomeT<Type> some_;
        };

    public:
        constexpr ~Optional(void)
        {
            switch (tag_) { // optional::dtor
                case Tag::some:
                    some_.~SomeT<Type>();
                    break;

                case Tag::none:
                    break;

                case Tag::inval:
                    break;

                default:
                    // The compile is to fail if an explicit enum case handler isn't present.
                    // (-Wenum-switch).
                    // Also, want to abort/panic if an unknown case is encountered
                    // at runtime, much how a default hander would work if it was
                    // present.

                    // For gcc/clang minsize:
                    // If this not nell::abort, then iteration may not collapse into a tight loop.
                    // It's the need for arguments that's doing it.
                    // For O3:
                    // Does not affect and iteration collapses into tight loop.
                    // TODO: look into stack trace dumper on fail.
                    // But would still like a message..
                    // For arm::minsize:
                    // If panic, then does not collapse (func with arg issue)

                    // For dtor + moves (but not usage) at runtime:
                    // Does it matter it's not one of the 'special' values?
                    // Yes, since it may have contained one or the others..
                    // If memory hardware is assumed to be incorruptible then can ignore ?
                    // (what of buffer overruns?) but might not want to for buffer overrun issues.
                    // If there is a memory corruption, then should have been detected/corrected by
                    // memory hardware (ECC mem).
                    // If not using ECC mem then that's the prob.
                    nel::panic("bad Optional");
                    break;
            }
        }

    private:
        // Don't want copy semantics here, use move instead.
        Optional(Optional const &o) = delete;
        Optional &operator=(Optional const &o) = delete;

    public:
        Optional(Optional &&rhs)
            : tag_(move(rhs.tag_))
        {
            switch (tag_) { // optional::move-ctor
                case Tag::some:
                    new (&some_) SomeT<Type>(move(rhs.some_));
                    break;
                case Tag::none:
                    break;
                case Tag::inval:
                    break;

                default:
                    // The compile is to fail if an explicit enum case handler isn't present.
                    // (-Wenum-switch).
                    // Also, want to abort/panic if an unknown case is encountered
                    // at runtime, much how a default hander would work if it was
                    // present.

                    // For gcc/clang minsize:
                    // If this not nell::abort, then iteration may not collapse into a tight loop.
                    // It's the need for arguments that's doing it.
                    // For O3:
                    // Does not affect and iteration collapses into tight loop.
                    // TODO: look into stack trace dumper on fail.
                    // But would still like a message..
                    // For arm::minsize:
                    // If panic, then does not collapse (func with arg issue)

                    // For dtor + moves (but not usage) at runtime:
                    // Does it matter it's not one of the 'special' values?
                    // Yes, since it may have contained one or the others..
                    // If memory hardware is assumed to be incorruptible then can ignore ?
                    // (what of buffer overruns?) but might not want to for buffer overrun issues.
                    // If there is a memory corruption, then should have been detected/corrected by
                    // memory hardware (ECC mem).
                    // If not using ECC mem then that's the prob.
                    nel::panic("bad Optional");
                    break;
            }
        }

        Optional &operator=(Optional &&rhs)
        {
            if (this != &rhs) {
                if (tag_ == rhs.tag_) {
                    switch (tag_) { // optional::move-ass
                        case Tag::inval:
                            break;
                        case Tag::some:
                            some_ = move(rhs.some_);
                            break;
                        case Tag::none:
                            break;
                        default:
                            // The compile is to fail if an explicit enum case handler
                            // isn't present (i.e. -Wenum-switch).
                            // Also, want to abort/panic if an unknown case is encountered
                            // at runtime, much how a default hander would work if it was
                            // present.

                            // For gcc/clang minsize:
                            // If this not nell::abort, then iteration may not collapse into
                            // a tight loop.
                            // It's the need for arguments that's doing it.
                            // For O3:
                            // Does not affect and iteration collapses into tight loop.
                            // TODO: look into stack trace dumper on fail.
                            // But would still like a message..
                            // For arm::minsize:
                            // If panic, then does not collapse (func with arg issue)

                            // For dtor + moves (but not usage) at runtime:
                            // Does it matter it's not one of the 'special' values?
                            // Yes, since it may have contained one or the others..
                            // If memory hardware is assumed to be incorruptible then can ignore ?
                            // (what of buffer overruns?) but might not want to for buffer overrun
                            // issues.
                            // If there is a memory corruption, then should have been
                            // detected/corrected by memory hardware (ECC mem).
                            // If not using ECC mem then that's the prob.
                            nel::panic("bad Optional");
                            break;
                    }
                    tag_ = move(rhs.tag_);
                } else {
                    this->~Optional();
                    new (this) Optional(move(rhs));
                }
            }
            return *this;
        }

    public:
        // Optional<T> t;
        /**
         * Default constructor.
         *
         */
        constexpr Optional(void)
            : tag_(Tag::inval)
        {
        }

        /**
         * Create an optional set to SomeT, moving existing value to hold.
         *
         * @returns an Optional 'wrapping' the value moved.
         */
        constexpr Optional(SomeT<Type> &&rhs)
            : tag_(Tag::some)
            , some_(move(rhs))
        {
        }

        // Optional<T> t = None;
        // Optional<T> t{None};
        // Optional<T> t(None);
        /**
         * Create an optional set to none.
         *
         * @returns an Optional 'wrapping' a None
         */
        constexpr Optional(NoneT const &)
            : tag_(Tag::none)
        {
        }

    private:
        // Don't use std::function.. it's bloatware..

        // TODO: create concepts to limit FNS, FNN to those func signatures.
        // If this is a some, call FNS and return value.
        // If this is a none, call FNN and return value.
        // If this is inval, panic.
        // After call this is untouched.
        template<typename V, typename FNS, typename FNN>
        // FNS: V (*)(SomeT<T> const &)
        // FNN: V (*)(void)
        constexpr V match(FNS &&on_some, FNN &&on_none, Context const &ctx = Context()) const
        {
            switch (tag_) { // optional::match
                case Tag::some:
                    // return on_some(*some_);
                    return on_some(some_);
                case Tag::none:
                    return on_none();
                case Tag::inval:
                    // Invalids are not values that should occur.
                    // If they do it's a use-after-move-from op so must panic,
                    // or a use-before-initialised so again must panic.
                    nel::panic("invalid Optional", ctx);
                    break;
                default:
                    // The compile is to fail if an explicit enum case handler isn't present.
                    // (-Wenum-switch).
                    // Also, want to abort/panic if an unknown case is encountered
                    // at runtime, much how a default hander would work if it was
                    // present.

                    // For gcc/clang minsize:
                    // If this not nell::abort, then iteration may not collapse into a tight loop.
                    // It's the need for arguments that's doing it.
                    // For O3:
                    // Does not affect and iteration collapses into tight loop.
                    // TODO: look into stack trace dumper on fail.
                    // But would still like a message..
                    // For arm::minsize:
                    // If panic, then does not collapse (func with arg issue)

                    // For dtor + moves (but not usage) at runtime:
                    // Does it matter it's not one of the 'special' values?
                    // Yes, since it may have contained one or the others..
                    // If memory hardware is assumed to be incorruptible then can ignore ?
                    // (what of buffer overruns?) but might not want to for buffer overrun issues.
                    // If there is a memory corruption, then should have been detected/corrected by
                    // memory hardware (ECC mem).
                    // If not using ECC mem then that's the prob.
                    nel::panic("bad Optional", ctx);
                    break;
            }
        }

    public:
        // Comparision operators
        // Implemented in terms of the operator on the type,
        // as some types may have more optimal implementations of that operation than the
        // it's negation.

        /**
         * Determine if this is equal to other by value.
         *
         * @param o The other optional to compare to.
         *
         * @returns true if this and o are equal by value.
         * @returns false if this and o are not equal by value.
         *
         * @note `this` is not consumed/changed by the operation.
         * @note `o` is not consumed/changed by the operation.
         */
        constexpr bool is_eq(Optional const &rhs, Context const &ctx = Context()) const
        {
            // return match<bool>(
            //     [&o, &ctx, this](T const &t) -> bool {
            //         return o.match<bool>([&t](T const &ot) -> bool { return t == ot; },
            //                              [](void) -> bool { return false; },
            //                              ctx);
            //     },
            //     [&o, &ctx](void) -> bool {
            //         return o.match<bool>([](T const &) -> bool { return false; },
            //                              [](void) -> bool { return true; },
            //                              ctx);
            //     },
            //     ctx);
            return match<bool>(
                [&rhs, &ctx](SomeT<T> const &s) -> bool {
                    return rhs.match<bool>([&s](SomeT<T> const &rhs_s)
                                               -> bool { return s == rhs_s; },
                                           [](void) -> bool { return false; },
                                           ctx);
                },
                [&rhs, &ctx](void) -> bool {
                    return rhs.match<bool>([](SomeT<T> const &) -> bool { return false; },
                                           [](void) -> bool { return true; },
                                           ctx);
                },
                ctx);
        }

        /**
         * Determine if this and other are equal by value.
         *
         * @param o The other optional to compare to.
         *
         * @returns true if o and this are equal by value.
         * @returns false if o and this are not equal by value.
         *
         * @note `this` is not consumed/changed by the operation.
         * @note `o` is not consumed/changed by the operation.
         */
        constexpr friend bool operator==(Optional const &lhs, Optional const &rhs)
        {
            return lhs.is_eq(rhs);
        }

        /**
         * Determine if this and other are not equal by value.
         *
         * @param o The other optional to compare to.
         *
         * @returns true if o and this are not equal by value.
         * @returns false if o and this are equal by value.
         *
         * @note `this` is not consumed/changed by the operation.
         * @note `o` is not consumed/changed by the operation.
         */
        constexpr bool is_ne(Optional const &rhs, Context const &ctx = Context()) const
        {
            return match<bool>(
                [&rhs, &ctx](SomeT<T> const &s) -> bool {
                    return rhs.match<bool>([&s](SomeT<T> const &rhs_s)
                                               -> bool { return s != rhs_s; },
                                           [](void) -> bool { return true; },
                                           ctx);
                },
                [&rhs, &ctx](void) -> bool {
                    return rhs.match<bool>([](SomeT<T> const &) -> bool { return true; },
                                           [](void) -> bool { return false; },
                                           ctx);
                },
                ctx);
        }

        /**
         * Determine if this and other are not equal by value.
         *
         * @param o The other optional to compare to.
         *
         * @returns true if o and this are not equal by value.
         * @returns false if o and this are equal by value.
         *
         * @note `this` is not consumed/changed by the operation.
         * @note `o` is not consumed/changed by the operation.
         */
        constexpr friend bool operator!=(Optional const &lhs, Optional const &rhs)
        {
            return lhs.is_ne(rhs);
        }

        constexpr friend bool operator==(Optional const &lhs, SomeT<T> const &rhs)
        {
            return lhs.match<bool>([&rhs](SomeT<T> const &some) { return some == rhs; },
                                   [](void) { return false; });
        }

        constexpr friend bool operator!=(Optional const &lhs, SomeT<T> const &rhs)
        {
            return lhs.match<bool>([&rhs](SomeT<T> const &some) { return some != rhs; },
                                   [](void) { return true; });
        }

        constexpr friend bool operator==(Optional const &lhs, NoneT const &/*rhs*/)
        {
            return lhs.match<bool>([](SomeT<T> const &) { return false; },
                                   [](void) { return true; });
        }

        constexpr friend bool operator!=(Optional const &lhs, NoneT const &/*rhs*/)
        {
            return lhs.match<bool>([](SomeT<T> const &) { return true; },
                                   [](void) { return false; });
        }

    public:
        /**
         * Determine if the container contains a SomeT.
         *
         * @returns true if container contains a SomeT.
         * @returns false if container does not contain a SomeT.
         *
         * @note `this` is not consumed/changed by the operation.
         */
        constexpr bool is_some(Context const &ctx = Context()) const
        {
            return match<bool>([](SomeT<T> const &) -> bool { return true; },
                               [](void) -> bool { return false; },
                               ctx);
        }

        /**
         * Determine if the container contains a None.
         *
         * @returns true if container contains a None.
         * @returns false if container does not contain a None.
         *
         * @note `this` is not consumed/changed by the operation.
         */
        constexpr bool is_none(Context const &ctx = Context()) const
        {
            return match<bool>([](SomeT<T> const &) -> bool { return false; },
                               [](void) -> bool { return true; },
                               ctx);
        }

#    if defined(TEST)
        // define a method to test optional is invalid for tests
        constexpr bool is_inval(void) const
        {
            return tag_ == Tag::inval;
        }
#    endif // defined(TEST)

    public:
        // Don't use std::function.. it's bloatware..

        // TODO: create concepts to limit FNS, FNN to those func signatures.
        // If this is a some, call FNS and return value.
        // If this is a none, call FNN and return value.
        // If this is inval, panic.
        // After call this is invalidated.
        template<typename V, typename FNS, typename FNN>
        // FNS: V (*)(T &&)
        // FNN: V (*)(void)
        constexpr V consume(FNS &&on_some, FNN &&on_none, Context const &ctx = Context())
        {
            // A consuming matcher..
            // That moves the value to the closures..
            switch (tag_) { // optional::consume
                case Tag::some:
                    tag_ = Tag::inval;
                    return on_some(move(*some_));

                case Tag::none:
                    tag_ = Tag::inval;
                    return on_none();

                case Tag::inval:
                    // Invalids are not values that should occur.
                    // If they do it's a use-after-move-from op so must panic,
                    // or a use-before-initialised so again must panic.
                    nel::panic("invalid Optional", ctx);
                    break;

                default:
                    nel::panic("bad Optional", ctx);
                    break;
            }
        }

    public:
        // Contained value extraction with consumption.
        // i.e. extracting invalidates the container.
        // Prob not expected C++ i.e. values being invalidated by use
        // and more access by const ref or copy-out.

        /**
         * Return the wrapped value or panic.
         *
         * If a some, returns wrapped value.
         * If a none, panics.
         *
         * @returns value contained by the Optional if it's a 'Some'.
         *
         * @warning this is consumed/invalidated.
         */
        constexpr T unwrap(Context const &ctx = Context())
        {
            return consume<T>([](T &&some) -> T { return some; },
                              [&](void) -> T { nel::panic("not a SomeT", ctx); },
                              ctx);
        }

        // TODO: what happens to o if this is a some?
        // destroyed?
        /**
         * Return wrapped or supplied value.
         *
         * If a some, then return the wrapped value.
         * If a none, return the other.
         *
         * @param o the other value
         *
         * @returns value contained by the Optional if it's a 'Some'.
         * @returns value passed in if it's a 'None'.
         *
         * @warning this is consumed/invalidated.
         */
        constexpr T unwrap_or(T &&o, Context const &ctx = Context())
        {
            return consume<T>([](T &&some) -> T { return some; },
                              [&o](void) -> T { return forward<T>(o); },
                              ctx);
        }

#    if 0
        // TODO: what happens to the args if this is a some?,
        // they have been moved,
        // are they destroyed, or ignored?
        /**
         * Return wrapped or create supplied value from .
         *
         * If a some, then return the wrapped value.
         * If a none, return the supplied value.
         * The optional is consumed regardless.
         *
         * @returns value contained by this if a 'Some'.
         * @returns value passed in if this is a 'None'.
         *
         * @warning this is consumed/invalidated.
         */
        template<typename... Args>
        T unwrap_or(Args &&...args, Context const &ctx = Context())
        {
            return consume<T>([](T &&some) -> T { return some; },
                              [&args...](void) -> T { return T(forward<Args>(args)...); },
                              ctx);
            // // TODO: are args destroyed if a some?
        }
#    endif
        // Why no access via references?
        // Because this allows copying of value out which I want to prevent.

    public:
        // TODO: create concepts to limit FN to that func signature.
        /**
         * Translate Optional<T> into Optional<U>.
         *
         * If this is a some, call Fn(T &&)->U.
         * If this is a none, return optional<U> = None.
         *
         * @return Optional<U>
         *
         * @warning If this is some, value is moved into func.
         * @warning this is invalidated/consumed.
         */
        template<typename U, typename FN>
        // Fn: U (*)(T &&)
        Optional<U> map(FN &&fn, Context const &ctx = Context())
        {
            return consume<Optional<U>>([&fn](T &&some) -> Optional<U> { return Some(fn(move(some))); },
                                        [](void)-> Optional<U> { return None(); },
                                        ctx);
        }

        // TODO: What happens to o if this is a some? destroyed/moved/left-alone?
        // destroyed
        /**
         * Return this optional or other value.
         *
         * @param o value to return if this is a none.
         *
         * @return If optional is a some, return it untouched.
         * @return If optional<T> is a none, return o.
         *
         * @warning If this is some, value is moved into result.
         * @warning this is invalidated/consumed.
         */
        Optional or_(Optional &&o, Context const &ctx = Context())
        {
            return consume<Optional>([](T &&some) { return Some(move(some)); },
                                     [&o](void) { return move(o); },
                                     ctx);
        }

        // TODO: create concepts to limit FN to that func signature.
        /**
         * Return this optional or other value from fn.
         *
         * @param fn returns a Optional<T> when called.
         *
         * @return If optional is a some, return it untouched.
         * @return If optional<T> is a none, return value from calling fn.
         *
         * @warning If this is some, value is moved into result.
         * @warning this is invalidated/consumed.
         */
        template<typename FN>
        // FN: T (*)(T &&)
        Optional or_else(FN &&fn, Context const &ctx = Context())
        {
            return consume<Optional>([](T &&some) { return Some(forward<T>(some)); },
                                     [&fn](void) { return fn(); },
                                     ctx);
        }

    public:
        friend Log &operator<<(Log &outs, Optional const &val)
        {
            outs << "Optional(";
            switch (val.tag_) {
                case Tag::none:
                    outs << "None";
                    break;
                case Tag::some:
                    outs << "Some(";
                    outs << *val.some_;
                    outs << ')';
                    break;
                case Tag::inval:
                    outs << "Inval";
                    break;
                default:
                    outs << "??";
                    break;
            }
            outs << ')';
            return outs;
        }
};

template<typename T>
constexpr inline Optional<T> Some(T &&t)
{
    return SomeT(move(t));
}

constexpr inline Optional<Void> Some(Void)
{
    return SomeT(Void());
}

constexpr inline Optional<Void> Some()
{
    return SomeT(Void());
}


} // namespace nel

#endif // !defined(NEL_OPTIONAL_HH)
