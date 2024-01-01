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
 * c.f. rust Option (but then, if you want rust-like, use rust?)
 *
 * usage:
 * ```c++
 *    Optional<int> v1; // compile error
 *
 *    Optional<int> v1 = Optional<int>::Some(1);
 *    Optional<int> v2 = Optional<int>::None();
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
 *    bool b9 = v1 == None(); // false
 *
 *    bool b10 = v2 == Some(1); // false
 *    bool b11 = v2 == Some(2); // false
 *    bool b12 = v2 == Some("ha ha"); // fail to compile., type mismatch
 *    bool b13 = v2 == None(); // true

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
        // Similar to std::variant but without the exception throwing behaviour.
        // Maybe make into a nel::Variant ?
        enum class Tag {
            INVAL = 0,
            NONE,
            SOME
        } tag_;

        template<enum Tag>
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

                    // I want compile to fail if an explicit enum case handler isn't present.
                    // But, want to abort/panic if a unhandled case is encountered
                    // at runtime, much how a default hander would work if it was
                    // present.

                default:
                    // For gcc/clang minsize: if this not nell:abort,
                    // then iteration may not collapse into a tight loop.
                    // It's the  need for arguments that's doing it.
                    // For O3: does not affect and iteration collapses into tight loop
                    // TODO: look into stack trace dumper on fail.
                    // But would still like a message..
                    // For arm:minsize: if panic, then does not collapse (func with arg issue)
                    // nel_panic("invalid optional");

                    // for dtor + moves (but not usage) at runtime:
                    // does it matter it's not one of the 'special' values?
                    // yes, since it may have contained one or the others..
                    // if memory hardware is assumed to be incorruptable then can ignore ? (buffer
                    // overruns?) but might not want to for buffer overrun issues. if there is a
                    // memory corruption, then should have been detected by memory hardware (Ecc
                    // mem) if not using eec/ecc mem then that's the prob.
                    nel::panic("bad Optional");
                    break;
            }
        }

    private:
        // Don't want copy semantics here, use move instead.
        Optional(Optional const &o) = delete;
        Optional &operator=(Optional const &o) = delete;

    public:
        Optional(Optional &&o)
        {
            tag_ = o.tag_;
            o.tag_ = Tag::INVAL;
            switch (tag_) { // optional::move-ctor
                case Tag::SOME:
                    new (&some_) Element<T>(move(o.some_));
                    break;
                case Tag::NONE:
                    break;
                case Tag::INVAL:
                    break;

                default:
                    // For gcc/clang minsize: if this not nell:abort,
                    // then iteration may not collapse into a tight loop.
                    // It's the  need for arguments that's doing it.
                    // For O3: does not affect and iteration collapses into tight loop
                    // TODO: look into stack trace dumper on fail.
                    // But would still like a message..
                    // For arm:minsize: if panic, then does not collapse (func with arg issue)
                    // nel_panic("invalid optional");

                    // for dtor + moves (but not usage) at runtime:
                    // does it matter it's not one of the 'special' values?
                    // yes, since it may have contained one or the others..
                    // if memory hardware is assumed to be incorruptable then can ignore ? (buffer
                    // overruns?) but might not want to for buffer overrun issues. if there is a
                    // memory corruption, then should have been detected by memory hardware (Ecc
                    // mem) if not using eec/ecc mem then that's the prob.
                    nel::panic("bad Optional");
                    break;
            }
        }

        Optional &operator=(Optional &&o)
        {
            if (this != &o) {
                tag_ = o.tag_;
                o.tag_ = Tag::INVAL;
                switch (tag_) { // optional::move-ass
                    case Tag::SOME:
                        some_ = move(o.some_);
                        break;
                    case Tag::NONE:
                        break;
                    case Tag::INVAL:
                        break;

                    default:
                        // For gcc/clang minsize: if this not nell:abort,
                        // then iteration may not collapse into a tight loop.
                        // It's the  need for arguments that's doing it.
                        // For O3: does not affect and iteration collapses into tight loop
                        // TODO: look into stack trace dumper on fail.
                        // But would still like a message..
                        // For arm:minsize: if panic, then does not collapse (func with arg issue)
                        // nel_panic("invalid optional");

                        // for dtor + moves (but not usage) at runtime:
                        // does it matter it's not one of the 'special' values?
                        // yes, since it may have contained one or the others..
                        // if memory hardware is assumed to be incorruptable then can ignore ?
                        // (buffer overruns?) but might not want to for buffer overrun issues. if
                        // there is a memory corruption, then should have been detected by memory
                        // hardware (Ecc mem) if not using eec/ecc mem then that's the prob.
                        nel::panic("invalid Optional");
                        break;
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

        // template<typename... Args>
        // constexpr Optional(Phantom<SOME> const, Args &&...args)
        //     : tag_(SOME), some_(forward<Args>(args)...) {}

    public:
        // Default constructor.
        // Don't want this as there is no default for an optional.
        // i.e. require a value, even if it's a None (i.e. being explicit).
        // really? Default to None/Inval?
        // But use of move-ctor mandates an inval state, so can have a default.
        // Optional(void) = delete;
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

        // /**
        //  * Create an optional set to Some, creating the value to use used inplace.
        //  *
        //  * @returns an Optional 'wrapping' the value created from the values given.
        //  */
        // template<typename... Args>
        // constexpr static Optional Some(Args &&...args)
        // {
        //     return Optional(Phantom<Tag::SOME>(), forward<Args>(args)...);
        // }

    private:
        // don't use std::function.. it's bloatware..
        // template<typename V>
        // constexpr V match(Tag tag, std::function<V(void)> on_some, std::function<V(void)>
        // on_none) const  {
        template<typename V, typename Fn1, typename Fn2>
        // Fn1: V (*)(T const &)
        // Fn2: V (*)(void)
        constexpr V match(Fn1 &&on_some, Fn2 &&on_none, Context const &ctx = Context()) const
        {
            switch (tag_) { // optional::match
                case Tag::SOME:
                    return on_some(*some_);
                case Tag::NONE:
                    return on_none();
                case Tag::INVAL:
                    // invalids are not values that should occur
                    // if they do it's a use-after-move-from op so must panic.
                    // or a use-before-initialised so again must panic.
                    // fall through to default handler
                    nel::panic("invalid Optional", ctx);
                    break;
                default:
                    // For gcc/clang minsize: if this not nell:abort,
                    // then iteration may not collapse into a tight loop.
                    // It's the  need for arguments that's doing it.
                    // For O3: does not affect and iteration collapses into tight loop
                    // TODO: look into stack trace dumper on fail.
                    // But would still like a message..
                    // For arm:minsize: if panic, then does not collapse (func with arg issue)
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
         * Is this equal by value to the optional given?
         *
         * @param o The other optional to compare to.
         * @returns true if equal by value, false otherwise.
         *
         * `this` is not consumed by the operation.
         * `o` is not consumed by the operation.
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
         * Is this equal by value to the optional given?
         *
         * @param o The other optional to compare to.
         * @returns true if equal by value, false otherwise.
         *
         * `this` is not consumed by the operation.
         * `o` is not consumed by the operation.
         */
        constexpr bool operator==(Optional const &o) const
        {
            return is_eq(o);
        }

        /**
         * Is this not equal by value to the result given?
         *
         * @param o The other result to compare to.
         * @returns true if not equal by value, false otherwise.
         *
         * `this` is not consumed by the operation.
         * `o` is not consumed by the operation.
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
         * Is this not equal by value to the result given?
         *
         * @param o The other result to compare to.
         * @returns true if not equal by value, false otherwise.
         *
         * `this` is not consumed by the operation.
         * `o` is not consumed by the operation.
         */
        constexpr bool operator!=(Optional const &o) const
        {
            return is_ne(o);
        }

    public:
        /**
         * Determine if the container contains a Some.
         *
         * @returns true if container contains a Some, false otherwise.
         *
         * The optional is not consumed by the operation.
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
         * @returns true if container contains a None, false otherwise.
         *
         * The optional is not consumed by the operation.
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
        // don't use std::function.. it's bloatware..
        // template<typename V>
        // V consume(std::function<V(void)> on_some, std::function<V(void)> on_none)
        template<typename V, typename Fn1, typename Fn2>
        // Fn1: V (*)(T &&)
        // Fn2: V (*)(void)
        constexpr V consume(Fn1 &&on_some, Fn2 &&on_none, Context const &ctx = Context())
        {
            // a consuming matcher..
            // that moves the value to the closures..
            auto tag = tag_;
            tag_ = Tag::INVAL;
            switch (tag) { // optional::consume
                case Tag::SOME:
                    return on_some(forward<T>(*some_));
                case Tag::NONE:
                    return on_none();
                case Tag::INVAL:
                    // invalids are not values that should occur
                    // if they do it's a use-after-move-from op so must panic.
                    // or a use-before-initialised so again must panic.
                    nel::panic("invalid Optional", ctx);
                    break;
                default:
                    nel::panic("bad Optional", ctx);
                    break;
            }
        }

        template<typename Fn1, typename Fn2>
        // Fn1: void (*)(T &&)
        // Fn2: void (*)(void)
        constexpr void consumev(Fn1 &&on_some, Fn2 &&on_none, Context const &ctx = Context())
        {
            // a consuming matcher..
            // that moves the value to the closures..
            auto tag = tag_;
            tag_ = Tag::INVAL;
            switch (tag) { // optional::consumev
                case Tag::SOME:
                    on_some(forward<T>(*some_));
                    break;
                case Tag::NONE:
                    on_none();
                    break;
                case Tag::INVAL:
                    // invalids are not values that should occur
                    // if they do it's a use-after-move-from op so must panic.
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
         * Extract and return the contained value if a Some, consuming the optional.
         *
         * @returns value contained by the Optional if it's a 'Some'.
         *
         * @warning If the optional does not contain a Some, then abort/panic.
         */
        T unwrap(Context const &ctx = Context())
        {
            return consume<T>([](T &&some) -> T { return some; },
                              [&](void) -> T { nel::panic("not a Some", ctx); },
                              ctx);
        }

        /**
         * Return wrapped or supplied value.
         *
         * If a some, then return the wrapped value.
         * If a none, return the supplied value.
         * The optional is consumed regardless.
         *
         * @returns value contained by the Optional if it's a 'Some'.
         * @returns value passed in if it's a 'None'.
         */
        T unwrap_or(T &&v, Context const &ctx = Context())
        {
            return consume<T>([](T &&some) -> T { return some; },
                              [&v](void) -> T { return forward<T>(v); },
                              ctx);
        }

        /**
         * Extract and return the contained value if a Some, consuming the optional.
         *
         * @returns if Some, consumes and returns the value contained by the Optional.
         * @returns if not Some, consumes args and creates value to return.
         */
        template<typename... Args>
        T unwrap(Args &&...args, Context const &ctx = Context())
        {
            return consume<T>([](T &&some) -> T { return some; },
                              [&args...](void) -> T { return T(forward<Args>(args)...); },
                              ctx);
            // // TODO: are args destroyed if a some?
        }

        // Why no access via references?
        // Because this allows copying of value out which I want to prevent.

    public:
        // template<typename U>
        // Optional<U> map(std::function<U(T &)> fn)
        template<typename U, typename Fn>
        // Fn: U (*)(T &&)
        Optional<U> map(Fn &&fn, Context const &ctx = Context())
        {
            // optional must be consumed.
            // contained type must be moved.
            // if contained is reference, then moving reference.
            // if contained is ptr, then move ptr.
            // if contained is value, then move value.
            // so, fn must always be T (*)(auto &&)
            typedef Optional<U> ReturnType;
            return consume<
                ReturnType>([&fn](T &&some)
                                -> ReturnType { return ReturnType::Some(fn(forward<T>(some))); },
                            [](void) -> ReturnType { return None; },
                            ctx);
        }

        Optional or_(Optional &&o, Context const &ctx = Context())
        {
            return consume<Optional>([](T &&some)
                                         -> Optional { return Optional::Some(forward<T>(some)); },
                                     [&o](void) -> Optional { return o; },
                                     ctx);
        }

        template<typename Fn>
        // Fn: T (*)(T &&)
        Optional or_else(Fn &&fn, Context const &ctx = Context())
        {
            return consume<Optional>([](T &&some)
                                         -> Optional { return Optional::Some(forward<T>(some)); },
                                     [&fn](void) -> Optional { return fn(); },
                                     ctx);
        }

    public:
        friend Log &operator<<(Log &outs, Optional const &val)
        {
            switch (val.tag_) {
                case Tag::NONE:
                    outs << "Optional("
                         << "None)";
                    return outs;
                    break;
                case Tag::SOME:
                    outs << "Optional("
                         << "Some(";
                    outs << *val.some_;
                    outs << "))";
                    return outs;
                    break;
                case Tag::INVAL:
                    outs << "Optional("
                         << "Inval)";
                    return outs;
                    break;
                default:
                    outs << "Optional("
                         << "Unknown)";
            }
            return outs;
        }
};

template<typename T>
constexpr Optional<T> Some(T &&v)
{
    return Optional<T>::Some(forward<T>(v));
}

template<>
struct Optional<void>
{
    private:
        // Tagged enum thing.
        // Similar to std::variant but without the exception throwing behaviour.
        // Maybe make into a nel::Variant ?
        enum class Tag {
            INVAL = 0,
            NONE,
            SOME
        } tag_;

        template<enum Tag>
        struct Phantom
        {
        };

    public:
        constexpr ~Optional(void)
        {
            switch (tag_) {
                case Tag::SOME:
                    break;
                case Tag::NONE:
                    break;
                case Tag::INVAL:
                    break;

                default:
                    // For gcc/clang minsize: if this not nell:abort,
                    // then iteration may not collapse into a tight loop.
                    // It's the  need for arguments that's doing it.
                    // For O3: does not affect and iteration collapses into tight loop
                    // TODO: look into stack trace dumper on fail.
                    // But would still like a message..
                    // For arm:minsize: if panic, then does not collapse (func with arg issue)
                    // nel_panic("invalid optional");

                    // for dtor + moves (but not usage) at runtime:
                    // does it matter it's not one of the 'special' values?
                    // yes, since it may have contained one or the others..
                    // if memory hardware is assumed to be incorruptable then can ignore ? (buffer
                    // overruns?) but might not want to for buffer overrun issues. if there is a
                    // memory corruption, then should have been detected by memory hardware (Ecc
                    // mem) if not using eec/ecc mem then that's the prob.
                    nel::panic("invalid Optional");
                    break;
            }
        }

    private:
        // Don't want copy semantics here, use move instead.
        Optional(Optional const &o) = delete;
        Optional &operator=(Optional const &o) = delete;

    public:
        Optional(Optional &&o)
        {
            tag_ = o.tag_;
            o.tag_ = Tag::INVAL;
            switch (tag_) {
                case Tag::SOME:
                    break;
                case Tag::NONE:
                    break;
                case Tag::INVAL:
                    break;

                default:
                    // For gcc/clang minsize: if this not nell:abort,
                    // then iteration may not collapse into a tight loop.
                    // It's the  need for arguments that's doing it.
                    // For O3: does not affect and iteration collapses into tight loop
                    // TODO: look into stack trace dumper on fail.
                    // But would still like a message..
                    // For arm:minsize: if panic, then does not collapse (func with arg issue)
                    // nel_panic("invalid optional");

                    // for dtor + moves (but not usage) at runtime:
                    // does it matter it's not one of the 'special' values?
                    // yes, since it may have contained one or the others..
                    // if memory hardware is assumed to be incorruptable then can ignore ? (buffer
                    // overruns?) but might not want to for buffer overrun issues. if there is a
                    // memory corruption, then should have been detected by memory hardware (Ecc
                    // mem) if not using eec/ecc mem then that's the prob.
                    nel::panic("invalid Optional");
                    break;
            }
        }

        Optional &operator=(Optional &&o)
        {
            if (this != &o) {
                tag_ = o.tag_;
                o.tag_ = Tag::INVAL;
                switch (tag_) {
                    case Tag::SOME:
                        break;
                    case Tag::NONE:
                        break;
                    case Tag::INVAL:
                        break;

                    default:
                        // For gcc/clang minsize: if this not nell:abort,
                        // then iteration may not collapse into a tight loop.
                        // It's the  need for arguments that's doing it.
                        // For O3: does not affect and iteration collapses into tight loop
                        // TODO: look into stack trace dumper on fail.
                        // But would still like a message..
                        // For arm:minsize: if panic, then does not collapse (func with arg issue)
                        // nel_panic("invalid optional");

                        // for dtor + moves (but not usage) at runtime:
                        // does it matter it's not one of the 'special' values?
                        // yes, since it may have contained one or the others..
                        // if memory hardware is assumed to be incorruptable then can ignore ?
                        // (buffer overruns?) but might not want to for buffer overrun issues. if
                        // there is a memory corruption, then should have been detected by memory
                        // hardware (Ecc mem) if not using eec/ecc mem then that's the prob.
                        nel::panic("invalid Optional");
                        break;
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
        // Default constructor.
        // Don't want this as there is no default for an optional.
        // i.e. require a value, even if it's a None (i.e. being explicit).
        // really? Default to None/Inval?
        // But use of move-ctor mandates an inval state, so can have a default.
        // Optional(void) = delete;
        constexpr Optional(void)
            : tag_(Tag::INVAL)
        {
        }

        // assign/create from None
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
         * Create an optional set to Some, creating the value to use used inplace.
         *
         * @returns an Optional 'wrapping' the value created from the values given.
         */
        // Cannot be constexpr since struct has non-trivial dtor..
        static Optional Some(void)
        {
            return Optional(Phantom<Tag::SOME>());
        }

    private:
        // don't use std::function.. it's bloatware..
        // template<typename V>
        // constexpr V match(Tag tag, std::function<V(void)> on_some, std::function<V(void)>
        // on_none) const  {
        template<typename V, typename Fn1, typename Fn2>
        // Fn1: V (*)(void)
        // Fn2: V (*)(void)
        constexpr V match(Fn1 &&on_some, Fn2 &&on_none, Context const &ctx = Context()) const
        {
            switch (tag_) {
                case Tag::SOME:
                    return on_some();
                case Tag::NONE:
                    return on_none();
                case Tag::INVAL:
                    // invalids are not values that should occur
                    // if they do it's a use-after-move-from op so must panic.
                    // or a use-before-initialised so again must panic.
                    nel::panic("invalid Optional", ctx);
                    break;
                default:
                    // For gcc/clang minsize: if this not nell:abort,
                    // then iteration may not collapse into a tight loop.
                    // It's the  need for arguments that's doing it.
                    // For O3: does not affect and iteration collapses into tight loop
                    // TODO: look into stack trace dumper on fail.
                    // But would still like a message..
                    // For arm:minsize: if panic, then does not collapse (func with arg issue)
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
         * Is this equal by value to the optional given?
         *
         * @param o The other optional to compare to.
         * @returns true if equal by value, false otherwise.
         *
         * `this` is not consumed by the operation.
         * `o` is not consumed by the operation.
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
         * Is this equal by value to the optional given?
         *
         * @param o The other optional to compare to.
         * @returns true if equal by value, false otherwise.
         *
         * `this` is not consumed by the operation.
         * `o` is not consumed by the operation.
         */
        constexpr bool operator==(Optional const &o) const
        {
            return is_eq(o);
        }

        /**
         * Is this not equal by value to the result given?
         *
         * @param o The other result to compare to.
         * @returns true if not equal by value, false otherwise.
         *
         * `this` is not consumed by the operation.
         * `o` is not consumed by the operation.
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
         * Is this not equal by value to the result given?
         *
         * @param o The other result to compare to.
         * @returns true if not equal by value, false otherwise.
         *
         * `this` is not consumed by the operation.
         * `o` is not consumed by the operation.
         */
        constexpr bool operator!=(Optional const &o) const
        {
            return is_ne(o);
        }

    public:
        /**
         * Determine if the container contains a Some.
         *
         * @returns true if container contains a Some, false otherwise.
         *
         * The optional is not consumed by the operation.
         */
        bool is_some(Context const &ctx = Context()) const

        {
            return match<bool>([](void) -> bool { return true; },
                               [](void) -> bool { return false; },
                               ctx);
        }

        /**
         * Determine if the container contains a None.
         *
         * @returns true if container contains a None, false otherwise.
         *
         * The optional is not consumed by the operation.
         */
        bool is_none(Context const &ctx = Context()) const
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
        // don't use std::function.. it's bloatware..
        // template<typename V>
        // V consume(std::function<V(void)> on_some, std::function<V(void)> on_none)
        template<typename V, typename Fn1, typename Fn2>
        // Fn1: V (*)(void)
        // Fn2: V (*)(void)
        constexpr V consume(Fn1 &&on_some, Fn2 &&on_none, Context const &ctx = Context())
        {
            // a consuming matcher..
            // that moves the value to the closures..
            auto tag = tag_;
            tag_ = Tag::INVAL;
            switch (tag) {
                case Tag::SOME:
                    return on_some();
                case Tag::NONE:
                    return on_none();
                case Tag::INVAL:
                    // invalids are not values that should occur
                    // if they do it's a use-after-move-from op so must panic.
                    // or a use-before-initialised so again must panic.
                    nel::panic("invalid Optional", ctx);
                    break;
                default:
                    nel::panic("bad Optional", ctx);
                    break;
            }
        }

        template<typename Fn1, typename Fn2>
        // Fn1: void (*)(void)
        // Fn2: void (*)(void)
        constexpr void consumev(Fn1 &&on_some, Fn2 &&on_none, Context const &ctx = Context())
        {
            // a consuming matcher..
            // that moves the value to the closures..
            auto tag = tag_;
            tag_ = Tag::INVAL;
            switch (tag) {
                case Tag::SOME:
                    on_some();
                    break;
                case Tag::NONE:
                    on_none();
                    break;
                case Tag::INVAL:
                    // invalids are not values that should occur
                    // if they do it's a use-after-move-from op so must panic.
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
         * Extract and return the contained value if a Some, consuming the optional.
         *
         * @returns value contained by the Optional if it's a 'Some'.
         *
         * If the optional does not contain a Some, then abort/panic.
         */
        void unwrap(Context const &ctx = Context())
        {
            return consumev([](void) {}, [&](void) { nel::panic("not a Some", ctx); }, ctx);
        }

        /**
         * Extract and return the contained value if a Some, consuming the optional.
         *
         * @param other The value to return if optional is not a Some.
         *
         * @returns if Some, consumes and returns the value contained by the Optional.
         * @returns if not Some, consumes and returns `other`.
         */
        void unwrap_or(Context const &ctx = Context())
        {
            return consumev([](void) {}, [](void) {}, ctx);
        }

        // Why no access via references?
        // This infers copying of value which I want to prevent.

    public:
        // template<typename U>
        // Optional<U> map(std::function<U(void)> fn)
        template<typename U, typename Fn>
        // Fn: U (*)(void)
        Optional<U> map(Fn &&fn, Context const &ctx = Context())
        {
            typedef Optional<U> ReturnType;
            return consume<ReturnType>([&fn](void) -> ReturnType { return ReturnType::Some(fn()); },
                                       [](void) -> ReturnType { return None; },
                                       ctx);
        }

    public:
        friend Log &operator<<(Log &outs, Optional const &val)
        {
            switch (val.tag_) {
                case Tag::NONE:
                    outs << "Optional("
                         << "None" << ')';
                    return outs;
                    break;
                case Tag::SOME:
                    outs << "Optional("
                         << "Some(" << ')' << ')';
                    return outs;
                    break;
                case Tag::INVAL:
                    outs << "Optional("
                         << "Inval" << ')';
                    return outs;
                    break;
                default:
                    outs << "Optional("
                         << "Unknown" << ')';
            }
            return outs;
        }
};

// Optional<void> Some(void)
inline Optional<void> Some(void)
{
    return Optional<void>::Some();
}

} // namespace nel

#endif // !defined(NEL_OPTIONAL_HH)
