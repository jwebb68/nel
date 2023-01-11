#ifndef NEL_OPTIONAL_HH
#define NEL_OPTIONAL_HH

namespace nel
{

template<typename T>
class Optional;

class NoneT;

} // namespace nel

#include <nel/element.hh>
#include <nel/log.hh>
#include <nel/panic.hh>

#include <new> // new (*) T(...)
#include <functional> // std::function
#include <utility> // std::move, std::forward

namespace nel
{

struct NoneT {
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
class Optional
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
        struct Phantom {
        };

        // Use union to disable certain default methods on T
        // Need to use class/struct as references cannot be used in unions,
        // so this is possible: Optional<Foo &>
        union {
                Element<T> some_;
        };

    public:
        constexpr ~Optional(void) noexcept
        {
            switch (tag_) {
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
                    nel::abort();
            }
        }

    private:
        // Don't want copy semantics here, use move instead.
        constexpr Optional(Optional const &o) = delete;
        constexpr Optional &operator=(Optional const &o) const = delete;

    public:
        constexpr Optional(Optional &&o) noexcept
        {
            tag_ = o.tag_;
            o.tag_ = Tag::INVAL;
            switch (tag_) {
                case Tag::SOME:
                    new (&some_) Element<T>(std::move(o.some_));
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
                    nel::abort();
            }
        }

        constexpr Optional &operator=(Optional &&o) noexcept
        {
            if (this != &o) {
                tag_ = o.tag_;
                o.tag_ = Tag::INVAL;
                switch (tag_) {
                    case Tag::SOME:
                        some_ = std::move(o.some_);
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
                        nel::abort();
                }
            }
            return *this;
        }

    private:
        constexpr Optional(Phantom<Tag::SOME> const, T &&v) noexcept
            : tag_(Tag::SOME)
            , some_(std::forward<T>(v))
        {
        }

        // template<typename... Args>
        // constexpr Optional(Phantom<SOME> const, Args &&...args) noexcept
        //     : tag_(SOME), some_(std::forward<Args>(args)...) {}

    public:
        // Default constructor.
        // Don't want this as there is no default for an optional.
        // i.e. require a value, even if it's a None (i.e. being explicit).
        // really? Default to None/Inval?
        // But use of move-ctor mandates an inval state, so can have a default.
        // Optional(void) = delete;
        constexpr Optional(void) noexcept
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
        constexpr Optional(NoneT const &) noexcept
            : tag_(Tag::NONE)
        {
        }

        /**
         * Create an optional set to Some, moving existing value to hold.
         *
         * @returns an Optional 'wrapping' the value moved.
         */
        constexpr static Optional Some(T &&v) noexcept
        {
            // using forward<>() instead of move() allows using T as references .
            return Optional(Phantom<Tag::SOME>(), std::forward<T>(v));
        }

        // /**
        //  * Create an optional set to Some, creating the value to use used inplace.
        //  *
        //  * @returns an Optional 'wrapping' the value created from the values given.
        //  */
        // template<typename... Args>
        // constexpr static Optional Some(Args &&...args) noexcept
        // {
        //     return Optional(Phantom<Tag::SOME>(), std::forward<Args>(args)...);
        // }

    public:
        /**
         * Determine if the container contains a Some.
         *
         * @returns true if container contains a Some, false otherwise.
         *
         * The optional is not consumed by the operation.
         */
        constexpr bool is_some(void) const noexcept
        {
            return tag_ == Tag::SOME;
        }

        /**
         * Determine if the container contains a None.
         *
         * @returns true if container contains a None, false otherwise.
         *
         * The optional is not consumed by the operation.
         */
        constexpr bool is_none(void) const noexcept
        {
            return tag_ == Tag::NONE;
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
        T unwrap(void) noexcept
        {
            nel_panic_if_not(is_some(), "not a Some");
            tag_ = Tag::INVAL;
            return some_.unwrap();
        }

        /**
         * Return wrapped or supplied value.
         *
         * If a some, then return the wrapped value.
         * If a none, return the suplied value.
         * The optional is consumed regardless.
         *
         * @returns value contained by the Optional if it's a 'Some'.
         * @returns value passed in if it's a 'None'.
         */
        T unwrap(T &&v) noexcept
        {
            // move into temp, will need to be moved or it'll be destroyed.
            auto t = std::forward<T>(v);
            bool const is_some = this->is_some();
            tag_ = Tag::INVAL;
            return is_some ? some_.unwrap() : std::forward<T>(t);
        }

        /**
         * Extract and return the contained value if a Some, consuming the optional.
         *
         * @returns if Some, consumes and returns the value contained by the Optional.
         * @returns if not Some, consumes args and creates value to return.
         */
        template<typename... Args>
        T unwrap(Args &&...args) noexcept
        {
            // TODO: are args destroyed if a some?
            bool const is_some = this->is_some();
            tag_ = Tag::INVAL;
            return is_some ? some_.unwrap() : T(std::forward<Args>(args)...);
        }

        // Why no access via references?
        // Because this allows copying of value out which I want to prevent.

    public:
        template<typename U>
        Optional<U> map(std::function<U(T &&)> fn) noexcept
        {
            return (this->is_none()) ? None
                                     : Optional<U>::Some(fn(std::forward<T>(some_.unwrap())));
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
        constexpr bool operator==(Optional const &o) const noexcept
        {
            if (this == &o) { return true; }
            if (tag_ == o.tag_) {
                switch (tag_) {
                    case Tag::SOME:
                        return some_ == o.some_;
                        break;
                    case Tag::NONE:
                        return true;
                        break;
                    case Tag::INVAL:
                        return true;
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
                        nel::abort();
                }
            }
            return false;
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
        constexpr bool operator!=(Optional const &o) const noexcept
        {
            if (this == &o) { return false; }
            if (tag_ == o.tag_) {
                switch (tag_) {
                    case Tag::SOME:
                        return some_ != o.some_;
                        break;
                    case Tag::NONE:
                        return false;
                        break;
                    case Tag::INVAL:
                        return false;
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
                        nel::abort();
                }
            }
            return true;
        }

    public:
        // friend std::ostream &operator<<(std::ostream &outs, Optional const &val) {
        friend Log &operator<<(Log &outs, Optional const &val) noexcept
        {
            switch (val.tag_) {
                case Tag::NONE:
                    outs << "Optional("
                         << "None"
                         << ")";
                    return outs;
                    break;
                case Tag::SOME:
                    outs << "Optional("
                         << "Some(" << val.some_.get() << ")"
                         << ")";
                    return outs;
                    break;
                case Tag::INVAL:
                    outs << "Optional("
                         << "Inval"
                         << ")";
                    return outs;
                    break;
                default:
                    outs << "Optional("
                         << "Unknown"
                         << ")";
            }
            return outs;
        }
};

template<typename T>
constexpr Optional<T> Some(T &&v) noexcept
{
    return Optional<T>::Some(std::forward<T>(v));
}

template<>
class Optional<void>
{
    private:
        // Tagged enum thing.
        // Similar to std::variant but without the exception throwing behaviour.
        // Maybe make into a nel::Variant ?
        enum Tag {
            INVAL = 0,
            NONE,
            SOME
        } tag_;

        template<enum Tag>
        struct Phantom {
        };

    public:
        constexpr ~Optional(void) noexcept
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
                    nel::abort();
            }
        }

    private:
        // Don't want copy semantics here, use move instead.
        constexpr Optional(Optional const &o) = delete;
        constexpr Optional &operator=(Optional const &o) const = delete;

    public:
        constexpr Optional(Optional &&o) noexcept
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
                    nel::abort();
            }
        }

        constexpr Optional &operator=(Optional &&o) noexcept
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
                        nel::abort();
                }
            }
            return *this;
        }

    private:
        constexpr Optional(Phantom<Tag::SOME> const) noexcept
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
        constexpr Optional(void) noexcept
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
        constexpr Optional(NoneT const &) noexcept
            : tag_(Tag::NONE)
        {
        }

        /**
         * Create an optional set to Some, creating the value to use used inplace.
         *
         * @returns an Optional 'wrapping' the value created from the values given.
         */
        // Cannot be constexpr since struct has non-trivial dtor..
        constexpr static Optional Some(void) noexcept
        {
            return Optional(Phantom<Tag::SOME>());
        }

    public:
        /**
         * Determine if the container contains a Some.
         *
         * @returns true if container contains a Some, false otherwise.
         *
         * The optional is not consumed by the operation.
         */
        constexpr bool is_some(void) const noexcept
        {
            return tag_ == Tag::SOME;
        }

        /**
         * Determine if the container contains a None.
         *
         * @returns true if container contains a None, false otherwise.
         *
         * The optional is not consumed by the operation.
         */
        constexpr bool is_none(void) const noexcept
        {
            return tag_ == Tag::NONE;
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
        void unwrap(void) noexcept
        {
            nel_panic_if_not(is_some(), "not a Some");
            tag_ = Tag::INVAL;
        }

        /**
         * Extract and return the contained value if a Some, consuming the optional.
         *
         * @param other The value to return if optional is not a Some.
         *
         * @returns if Some, consumes and returns the value contained by the Optional.
         * @returns if not Some, consumes and returns `other`.
         */
        void unwrap_or(void) noexcept
        {
            tag_ = Tag::INVAL;
        }

        // Why no access via references?
        // This infers copying of value which I want to prevent.

    public:
        template<typename U>
        Optional<U> map(std::function<U(void)> fn) noexcept
        {
            return (this->is_none()) ? None : Some(fn());
        }

    public:
        // Comparision operators
        // Implemented in terms of the operator on the type,
        // as some types may have more optimal implementations of thatoperationthan the
        // negation of it's opposite.
        /**
         * Is this equal by value to the optional given?
         *
         * @param o The other optional to compare to.
         * @returns true if equal by value, false otherwise.
         *
         * `this` is not consumed by the operation.
         * `o` is not consumed by the operation.
         */
        constexpr bool operator==(Optional const &o) const noexcept
        {
            if (this == &o) { return true; }
            if (tag_ == o.tag_) {
                switch (tag_) {
                    case Tag::SOME:
                        return true;
                        break;
                    case Tag::NONE:
                        return true;
                        break;
                    case Tag::INVAL:
                        return true;
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
                        nel::abort();
                }
            }
            return false;
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
        constexpr bool operator!=(Optional const &o) const noexcept
        {
            if (this == &o) { return false; }
            if (tag_ == o.tag_) {
                switch (tag_) {
                    case Tag::SOME:
                        return false;
                        break;
                    case Tag::NONE:
                        return false;
                        break;
                    case Tag::INVAL:
                        return false;
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
                        nel::abort();
                }
            }
            return true;
        }

    public:
        // friend std::ostream &operator<<(std::ostream &outs, Optional const &val) {
        friend Log &operator<<(Log &outs, Optional const &val) noexcept
        {
            switch (val.tag_) {
                case Tag::NONE:
                    outs << "Optional("
                         << "None"
                         << ")";
                    return outs;
                    break;
                case Tag::SOME:
                    outs << "Optional("
                         << "Some("
                         << ")"
                         << ")";
                    return outs;
                    break;
                case Tag::INVAL:
                    outs << "Optional("
                         << "Inval"
                         << ")";
                    return outs;
                    break;
                default:
                    outs << "Optional("
                         << "Unknown"
                         << ")";
            }
            return outs;
        }
};

constexpr Optional<void> Some(void)
{
    return Optional<void>::Some();
}

} // namespace nel

#endif // NEL_OPTIONAL_HH
