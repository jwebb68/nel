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
#    include <nel/panic.hh>
#    include <nel/memory.hh> // move,forward
#    include <nel/new.hh> // new (p) T()

namespace nel
{

struct NoneT
{
};

static constexpr NoneT None = NoneT {};

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
    private:
        // Tagged enum thing.
        struct Tag
        {
            public:
                enum ETag {
                    INVAL = 0,
                    NONE,
                    SOME
                } val_;

            public:
                // When moved, invalidate src.
                Tag(Tag &&rhs)
                    : val_(nel::move(rhs.val_))
                {
                    rhs.val_ = INVAL;
                }

                Tag &operator=(Tag &&rhs)
                {
                    val_ = nel::move(rhs.val_);
                    rhs.val_ = INVAL;
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

        template<Tag::ETag const TAG>
        struct Phantom
        {
        };

        // Use union to disable certain default methods on T
        // Need to use class/struct as references cannot be used in unions,
        // so this is possible: Optional<Foo &>
        union {
                Element<T> some_;
        };

    public:
        constexpr ~Optional(void)
        {
            switch (tag_) { // optional::dtor
                case Tag::SOME:
                    some_.~Element<T>();
                    break;

                case Tag::NONE:
                    break;

                case Tag::INVAL:
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
                    // If memory hardware is assumed to be incorruptable then can ignore ?
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
            : tag_(nel::move(rhs.tag_))
        {
            switch (tag_) { // optional::move-ctor
                case Tag::SOME:
                    new (&some_) Element<T>(move(rhs.some_));
                    break;
                case Tag::NONE:
                    break;
                case Tag::INVAL:
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
                    // If memory hardware is assumed to be incorruptable then can ignore ?
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
                        case Tag::INVAL:
                            break;
                        case Tag::SOME:
                            some_ = nel::move(rhs.some_);
                            break;
                        case Tag::NONE:
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
                            // If memory hardware is assumed to be incorruptable then can ignore ?
                            // (what of buffer overruns?) but might not want to for buffer overrun
                            // issues.
                            // If there is a memory corruption, then should have been
                            // detected/corrected by memory hardware (ECC mem).
                            // If not using ECC mem then that's the prob.
                            nel::panic("invalid Optional");
                            nel::panic("bad Optional");
                            break;
                    }
                    tag_ = nel::move(rhs.tag_);
                } else {
                    this->~Optional();
                    new (this) Optional(nel::move(rhs));
                }
            }
            return *this;
        }

    private:
        constexpr Optional(Phantom<Tag::SOME> const, T &&v)
            : tag_(Tag::SOME)
            , some_(forward<T>(v))
        {
        }

        template<typename... Args>
        constexpr Optional(Phantom<Tag::SOME> const, Args &&...args)
            : tag_(Tag::SOME)
            , some_(forward<Args>(args)...)
        {
        }

    public:
        // Optional<T> t;
        /**
         * Default constructor.
         *
         */
        constexpr Optional(void)
            : tag_(Tag::INVAL)
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
            : tag_(Tag::NONE)
        {
        }

        /**
         * Create an optional set to Some, moving existing value to hold.
         *
         * @returns an Optional 'wrapping' the value moved.
         */
        constexpr static Optional Some(T &&v)
        {
            // using forward<>() instead of move() allows using T as references .
            return Optional(Phantom<Tag::SOME>(), forward<T>(v));
        }

        /**
         * Create an optional set to Some, creating the value to use used inplace.
         *
         * @returns an Optional 'wrapping' the value created from the values given.
         */
        template<typename... Args>
        constexpr static Optional Some(Args &&...args)
        {
            return Optional(Phantom<Tag::SOME>(), forward<Args>(args)...);
        }

    private:
        // Don't use std::function.. it's bloatware..

        // TODO: create concepts to limit FNS, FNN to those func signatures.
        // If this is a some, call FNS and return value.
        // If this is a none, call FNN and return value.
        // If this is inval, panic.
        // After call this is untouched.
        template<typename V, typename FNS, typename FNN>
        // FNS: V (*)(T const &)
        // FNN: V (*)(void)
        constexpr V match(FNS &&on_some, FNN &&on_none, Context const &ctx = Context()) const
        {
            switch (tag_) { // optional::match
                case Tag::SOME:
                    return on_some(*some_);
                case Tag::NONE:
                    return on_none();
                case Tag::INVAL:
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
                    // If memory hardware is assumed to be incorruptable then can ignore ?
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
        constexpr bool is_eq(Optional const &o, Context const &ctx = Context()) const
        {
            return match<bool>(
                [&o, &ctx, this](T const &t) -> bool {
                    return o.match<bool>([&t](T const &ot) -> bool { return t == ot; },
                                         [](void) -> bool { return false; },
                                         ctx);
                },
                [&o, &ctx](void) -> bool {
                    return o.match<bool>([](T const &) -> bool { return false; },
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
        constexpr bool operator==(Optional const &o) const
        {
            return is_eq(o);
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
        constexpr bool is_ne(Optional const &o, Context const &ctx = Context()) const
        {
            return match<bool>(
                [&o, &ctx, this](T const &t) -> bool {
                    return o.match<bool>([&t](T const &ot) -> bool { return t != ot; },
                                         [](void) -> bool { return true; },
                                         ctx);
                },
                [&o, &ctx](void) -> bool {
                    return o.match<bool>([](T const &) -> bool { return true; },
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
        constexpr bool operator!=(Optional const &o) const
        {
            return is_ne(o);
        }

    public:
        /**
         * Determine if the container contains a Some.
         *
         * @returns true if container contains a Some.
         * @returns false if container does not contain a Some.
         *
         * @note `this` is not consumed/changed by the operation.
         */
        constexpr bool is_some(Context const &ctx = Context()) const
        {
            return match<bool>([](T const &) -> bool { return true; },
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
            return match<bool>([](T const &) -> bool { return false; },
                               [](void) -> bool { return true; },
                               ctx);
        }

#    if defined(TEST)
        // define a method to test optional is invalid for tests
        constexpr bool is_inval(void) const
        {
            return tag_ == Tag::INVAL;
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
            auto tag = nel::move(tag_);
            switch (tag) { // optional::consume
                case Tag::SOME:
                    return on_some(forward<T>(*some_));

                case Tag::NONE:
                    return on_none();

                case Tag::INVAL:
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
                              [&](void) -> T { nel::panic("not a Some", ctx); },
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
            typedef Optional<U> ReturnType;
            return consume<
                ReturnType>([&fn](T &&some)
                                -> ReturnType { return ReturnType::Some(fn(forward<T>(some))); },
                            [](void) -> ReturnType { return None; },
                            ctx);
        }

        // TODO: What happens to o if this is a some? destroyed/moved/left-alone?
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
            return consume<Optional>([](T &&some)
                                         -> Optional { return Optional::Some(forward<T>(some)); },
                                     [&o](void) -> Optional { return o; },
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
            return consume<Optional>([](T &&some)
                                         -> Optional { return Optional::Some(forward<T>(some)); },
                                     [&fn](void) -> Optional { return fn(); },
                                     ctx);
        }

    public:
        friend Log &operator<<(Log &outs, Optional const &val)
        {
            outs << "Optional(";
            switch (val.tag_) {
                case Tag::NONE:
                    outs << "None";
                    break;
                case Tag::SOME:
                    outs << "Some(";
                    outs << *val.some_;
                    outs << ')';
                    break;
                case Tag::INVAL:
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

// auto v = Some(1);
/**
 * Create an Optional<T>::Some from a T.
 */
template<typename T>
constexpr Optional<T> Some(T &&v)
{
    return Optional<T>::Some(forward<T>(v));
}

// auto v = Some<int>(1);
/**
 * Create an Optional<T>::Some from t create args, creating T inplace in the Optional.
 */
template<typename T, typename... Args>
constexpr Optional<T> Some(Args &&...args)
{
    return Optional<T>::Some(forward<Args>(args)...);
}

template<>
struct Optional<void>
{
    private:
        // Tagged enum thing.
        struct Tag
        {
            public:
                enum ETag {
                    INVAL = 0,
                    NONE,
                    SOME
                } val_;

            public:
                // When moved, invalidate src.
                Tag(Tag &&rhs)
                    : val_(nel::move(rhs.val_))
                {
                    rhs.val_ = INVAL;
                }

                Tag &operator=(Tag &&rhs)
                {
                    val_ = nel::move(rhs.val_);
                    rhs.val_ = INVAL;
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

        template<Tag::ETag const TAG>
        struct Phantom
        {
        };

    public:
        constexpr ~Optional(void)
        {
            switch (tag_) { // optional::~
                case Tag::SOME:
                    break;
                case Tag::NONE:
                    break;
                case Tag::INVAL:
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
                    // If memory hardware is assumed to be incorruptable then can ignore ?
                    // (what of buffer overruns?) but might not want to for buffer overrun issues.
                    // If there is a memory corruption, then should have been detected/corrected by
                    // memory hardware (ECC mem).
                    // If not using ECC mem then that's the prob.
                    nel::panic("invalid Optional");
                    break;
            }
        }

    private:
        // Don't want copy semantics here, use move instead.
        Optional(Optional const &o) = delete;
        Optional &operator=(Optional const &o) = delete;

    public:
        Optional(Optional &&rhs)
            : tag_(nel::move(rhs.tag_))
        {
            switch (tag_) {
                case Tag::SOME:
                    break;
                case Tag::NONE:
                    break;
                case Tag::INVAL:
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
                    // If memory hardware is assumed to be incorruptable then can ignore ?
                    // (what of buffer overruns?) but might not want to for buffer overrun issues.
                    // If there is a memory corruption, then should have been detected/corrected by
                    // memory hardware (ECC mem).
                    // If not using ECC mem then that's the prob.
                    nel::panic("invalid Optional");
                    break;
            }
        }

        Optional &operator=(Optional &&rhs)
        {
            if (this != &rhs) {
                if (tag_ == rhs.tag_) {
                    switch (tag_) { // optional::move-ass
                        case Tag::INVAL:
                            break;
                        case Tag::SOME:
                            break;
                        case Tag::NONE:
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
                            // If memory hardware is assumed to be incorruptable then can ignore ?
                            // (what of buffer overruns?) but might not want to for buffer overrun
                            // issues.
                            // If there is a memory corruption, then should have been
                            // detected/corrected by memory hardware (ECC mem).
                            // If not using ECC mem then that's the prob.
                            nel::panic("invalid Optional");
                            nel::panic("bad Optional");
                            break;
                    }
                    tag_ = nel::move(rhs.tag_);
                } else {
                    this->~Optional();
                    new (this) Optional(nel::move(rhs));
                }
            }
            return *this;
        }

    private:
        constexpr Optional(Phantom<Tag::SOME> const)
            : tag_(Tag::SOME)
        {
        }

    public:
        // Optional<void> o;
        /**
         * Default constructor.
         *
        */
        constexpr Optional(void)
            : tag_(Tag::INVAL)
        {
        }

        // Optional<void> t = None;
        // Optional<void> t{None};
        // Optional<void> t(None);
        /**
         * Create an optional set to none.
         *
         * @returns an Optional 'wrapping' a None
         */
        constexpr Optional(NoneT const &)
            : tag_(Tag::NONE)
        {
        }

        /**
         * Create an optional set to Some.
         *
         */
        constexpr static Optional Some(void)
        {
            return Optional(Phantom<Tag::SOME>());
        }

    private:
        // Don't use std::function.. it's bloatware..

        // TODO: create concepts to limit FNS, FNN to those func signatures.
        // If this is a some, call FNS and return value.
        // If this is a none, call FNN and return value.
        // If this is inval, panic.
        // After call this is untouched.
        template<typename V, typename FNS, typename FNN>
        // FNS: V (*)(void)
        // FNN: V (*)(void)
        constexpr V match(FNS &&on_some, FNN &&on_none, Context const &ctx = Context()) const
        {
            switch (tag_) {
                case Tag::SOME:
                    return on_some();
                case Tag::NONE:
                    return on_none();
                case Tag::INVAL:
                    // Invalids are not values that should occur.
                    // If they do it's a use-after-move-from op so must panic,
                    // or a use-before-initialised so again must panic.
                    nel::panic("invalid Optional", ctx);
                    break;
                default:
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
                    // If memory hardware is assumed to be incorruptable then can ignore ?
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
        // as some types may have more optimal implementations of that operation
        // than the negation of it's opposite.

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
        constexpr bool is_eq(Optional const &o, Context const &ctx = Context()) const
        {
            return match<bool>(
                [&o, &ctx](void) -> bool {
                    return o.match<bool>([](void) -> bool { return true; },
                                         [](void) -> bool { return false; },
                                         ctx);
                },
                [&o, &ctx](void) -> bool {
                    return o.match<bool>([](void) -> bool { return false; },
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
        constexpr bool operator==(Optional const &o) const
        {
            return is_eq(o);
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
        constexpr bool is_ne(Optional const &o, Context const &ctx = Context()) const
        {
            return match<bool>(
                [&o, &ctx](void) -> bool {
                    return o.match<bool>([](void) -> bool { return false; },
                                         [](void) -> bool { return true; },
                                         ctx);
                },
                [&o, &ctx](void) -> bool {
                    return o.match<bool>([](void) -> bool { return true; },
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
        constexpr bool operator!=(Optional const &o) const
        {
            return is_ne(o);
        }

    public:
        /**
         * Determine if the container contains a Some.
         *
         * @returns true if container contains a Some.
         * @returns false if container does not contain a Some.
         *
         * @note `this` is not consumed/changed by the operation.
         */
        constexpr bool is_some(Context const &ctx = Context()) const

        {
            return match<bool>([](void) -> bool { return true; },
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
            return match<bool>([](void) -> bool { return false; },
                               [](void) -> bool { return true; },

                               ctx);
        }

#    if defined(TEST)
        // define a method to test optional is invalid for tests
        constexpr bool is_inval(void) const
        {
            return tag_ == Tag::INVAL;
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
        // FNS: V (*)(void)
        // FNN: V (*)(void)
        constexpr V consume(FNS &&on_some, FNN &&on_none, Context const &ctx = Context())
        {
            // A consuming matcher..
            // That moves the value to the closures..
            auto tag = nel::move(tag_);
            switch (tag) {
                case Tag::SOME:
                    return on_some();

                case Tag::NONE:
                    return on_none();

                case Tag::INVAL:
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
         * Which becomes, return if a some, panic if not.
         *
         * @returns if a some, returns wrapped value.
         *
         * @warning this is consumed/invalidated.
         */
        constexpr void unwrap(Context const &ctx = Context())
        {
            return consume<void>([](void) {}, [&](void) { nel::panic("not a Some", ctx); }, ctx);
        }

        /**
         * Return wrapped or supplied value.
         *
         * If a some, then return the wrapped value.
         * If a none, return the other.
         *
         * Which becomes, return if a some, return if not, i.e. null op.
         *
         * @warning this is consumed/invalidated.
         */
        constexpr void unwrap_or(Context const &ctx = Context())
        {
            return consume<void>([](void) {}, [](void) {}, ctx);
        }

        // Why no access via references?
        // This infers copying of value which I want to prevent.

    public:
        // TODO: create concepts to limit FN to that func signature.
        /**
         * Translate Optional<void> into Optional<U>.
         *
         * If this is a some, call Fn()->U.
         * If this is a none, return Optional<U> = None.
         *
         * @return Optional<U>
         *
         * @warning If this is some, value is moved into func.
         * @warning this is invalidated/consumed.
         */
        template<typename U, typename FN>
        // Fn: U (*)(void)
        Optional<U> map(FN &&fn, Context const &ctx = Context())
        {
            typedef Optional<U> ReturnType;
            return consume<ReturnType>([&fn](void) -> ReturnType { return ReturnType::Some(fn()); },
                                       [](void) -> ReturnType { return None; },
                                       ctx);
        }

    public:
        friend Log &operator<<(Log &outs, Optional const &val)
        {
            outs << "Optional(";
            switch (val.tag_) {
                case Tag::NONE:
                    outs << "None";
                    break;
                case Tag::SOME:
                    outs << "Some()";
                    break;
                case Tag::INVAL:
                    outs << "Inval";
                    break;
                default:
                    outs << "??";
            }
            outs<< ')';
            return outs;
        }
};

/**
 * Create an Optional<void>::Some.
 *
 * Saves having to use the scope prefix.
 */
inline Optional<void> Some(void)
{
    return Optional<void>::Some();
}

} // namespace nel

#endif // !defined(NEL_OPTIONAL_HH)
