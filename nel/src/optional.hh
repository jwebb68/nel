#ifndef NEL_OPTIONAL_HH
#define NEL_OPTIONAL_HH

namespace nel {

template<typename T>
class Optional;

}

#include <cstdlib> // abort
#include "log.hh"
#include <utility> // std::move

#define ARG_UNUSED(arg) ((void)(arg))

namespace nel {

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
 *    bool b8 = v1 == Some("haha"); // fail to compile, type mismatch
 *    bool b9 = v1 == None(); // false
 *
 *    bool b10 = v2 == Some(1); // false
 *    bool b11 = v2 == Some(2); // false
 *    bool b12 = v2 == Some("haha"); // fail to compile., type mismatch
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
class Optional {
    public:
        // A tag class for the Some case for optional.
        // May be moved to outside the optional class as all Some<T> are the same
        // and not really specialised to Optional.
        // Only in Optional to be in it's namespace.
        // Ah, maybe not, Some in one namespace may be different in another..
        // but not within nel.
        class Some {
            private:
                T value_;

            public:
                // Default constructor.
                // Don't want this as there is no default for a Some.
                // You must specify a value.
                Some(void) = delete;

                // shouldn't need these but..
                // Some(Some const &) = delete;
                // Some &operator=(Some const &) const = delete;


                // shouldn't these be defaulted
#if 1
                constexpr Some(Some &&) noexcept = default;
                constexpr Some &operator=(Some &&) noexcept = default;
#else
                constexpr Some(Some &&o) noexcept
                    : value_(std::move(o.value_))
                {
                }
                constexpr Some &operator=(Some &&o) noexcept
                {
                    value_ = std::move(o.value_);
                    return *this;
                }
#endif
                // Wrapping constructor.
                // Must specify if it's to be wrapped in a Some.
                // i.e. being explicit.
                explicit constexpr  Some(T &&val) noexcept: value_(std::move(val))
                {
                }
                template<typename ...Args>
                explicit constexpr Some(Args &&...args) noexcept: value_(std::forward<Args>
                            (args)...)
                {
                }


                // do I need to define the ass-move? will compiler not just use the
                // ctor-move?
                constexpr Some &operator=(T &&val) const noexcept
                {
                    this->value_ = std::move(val);
                    return *this;
                }

            public:
                // Contained value access
                // Note no return via copy, as members may not be copyable.
                // note no return via copy, copy may be expensive.
                // Note no access via reference, accessing the contained must invalidate
                // the container. i.e value extract/unwrap invalidates the container.
                constexpr T &&unwrap(void) noexcept
                {
                    return std::move(this->value_);
                }

            public:
                // Comparision operators
                // Implemented in terms of the operator on the type,
                // as some types may have more optimal impls of that oper than the
                // negation of it's opposite.
                constexpr bool operator==(Some const &o) const
                {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
                    return this->value_ == o.value_;
#pragma GCC diagnostic pop
                }
                constexpr bool operator!=(Some const &o) const
                {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
                    return this->value_ != o.value_;
#pragma GCC diagnostic pop
                }

            public:
                // friend std::ostream &operator<<(std::ostream &outs, Some const &val) {
                //     outs << "Some(" << val.value_ << ")";
                //     return outs;
                // }
                friend Log &operator<<(Log &outs, Some const &val)
                {
                    outs << "Some(" << val.value_ << ")";
                    return outs;
                }
        };

        // A tag class for the Some case for optional.
        // May be moved to outside the optional class as all None<T> are the same
        // and not really specialised to Optional.
        // Also, could be a simpleton.. c.f. nullptr, nullopt
        // Only in Optional to be in it's namespace.
        class None {
            public:
                constexpr None(void)
                {
                }

                // shouldn't these be defaulted
#if 1
                constexpr None(None &&) noexcept = default;
                constexpr None &operator=(None &&) noexcept = default;
#else
                constexpr None(None &&o) noexcept {}
                constexpr None &operator=(None &&o) noexcept
                {
                    return *this;
                }
#endif

            public:
                // // Comparision operators
                // bool operator==(None const &o) const {
                //     ARG_UNUSED(o);
                //     return true;
                // }
                // bool operator!=(None const &o) const {
                //     ARG_UNUSED(o);
                //     return false;
                // }

            public:
                // friend std::ostream &operator<<(std::ostream &outs, None const &val) {
                //     ARG_UNUSED(val);
                //     outs << "None";
                //     return outs;
                // }
                friend Log &operator<<(Log &outs, None const &val)
                {
                    ARG_UNUSED(val);
                    outs << "None";
                    return outs;
                }
        };

    private:
        // Tagged enum thing
        // Similar to std::variant but without the exception throwing behaviour.
        // maybe make into a nel::Variant ?
        enum Tag { INVAL = 0, NONE, SOME } tag_;
        union {
            Some some_;
            None none_;
        };

    public:
        ~Optional(void) noexcept
        {
            switch (this->tag_) {
                case SOME:
                    this->some_.~Some();
                    return;
                    break;

                case NONE:
                    this->none_.~None();
                    return;
                    break;

                case INVAL:
                    return;
                    break;

            }
            // No default as I want compile to fail if an enum case handler
            // isn't present.
            // But, want to abort/panic if a unhandled case is encountered
            // at runtime, much how a default hander would work if it was
            // present.
            nel::log << "invalid  Optional: tag=" << this->tag_ << "\n";
            std::abort();
        }

        // Default constructor.
        // Don't want this as there is no default for an optional.
        // You must specify a value, even if it's a None (i.e. being explicit).
        // really? Default to None?
        //Optional(void) = delete;
        constexpr Optional(void) noexcept: tag_(INVAL) {}

        constexpr Optional(None &&) noexcept: tag_(NONE), none_()
        {
        }

        constexpr Optional(Some &&v) noexcept: tag_(SOME), some_(std::move(v))
        {
        }

#if 1
        Optional(Optional const &other) = delete;
        Optional const &operator=(Optional const &other) const noexcept = delete;
#else
        constexpr Optional(Optional const &other) noexcept: tag_(other.tag_)
        {
            switch (this->tag_) {
                case SOME:
                    this->some_ = other.some_;
                    return;
                    break;
                case NONE:
                    this->none_ = None();
                    return;
                    break;
                case INVAL:
                    return;
                    break;
            }
            //std::cerr << "not  Optional: " << this->tag_ << std::endl;
            nel::log << "invalid  Optional: tag=" << this->tag_ << "\n";
            std::abort();
        }
        constexpr Optional const &operator=(Optional const &other) const noexcept
        {
            if (this != &other) {
                Optional t(other);
                std::swap(*this, t);
            }
            return *this;
        }
#endif

        constexpr Optional(Optional &&other) noexcept
        {
            new (&this->tag_) Tag(std::move(other.tag_));
            other.tag_ = INVAL;
            // at -Og: compiler is having issues with union
            // thinks that all values in union must be inited, which is incorrect
            // only 1 must be.
            // it's complaining that some_ is not initialised when none_ is being initialised.
            // initing all would cause overwrites..
            switch (this->tag_) {
                case SOME:
                    // compiler reports some_ may be uninitialised
                    // it is correct, some_ IS uninitialised
                    // I'm calling placement new to initialise it.
                    // and no, initialising it to a value before calling pl.new is
                    // incorrect.. pl new is meant to operate on uninit values so to init them.
                    // as tag_ wasn't picked up then 1. poss error 2.maybe that it's a union...
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
                    new (&this->some_) Some(std::move(other.some_));
#pragma GCC diagnostic pop
                    return;
                    break;
                case NONE:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
                    new (&this->none_) None(std::move(other.none_));
#pragma GCC diagnostic pop
                    return;
                    break;
                case INVAL:
                    return;
                    break;
            }
            //std::cerr << "invalid Optional: tag=" << this->tag_ << std::endl;
            nel::log << "invalid  Optional: tag=" << this->tag_ << "\n";
            std::abort();
        }

        constexpr Optional &operator=(Optional &&other) noexcept
        {
#if 0
            // destruct self, cleaning up/releasing any resources in union.
            this->~Optional();
            // move-construct other into self?
            // is this exception safe?
            // move shouldn't be allocing any resources so should be a byteswap.
            // maybe explcitly do that?
            new (this) Optional(std::move(other));
#else
            // prob better than dtuct+move-ctuct as no point where this is uninit.
            // also, ass-move of type uses ass-move of members.
            Optional t(std::move(*this));
            this->tag_ = std::move(other.tag_);
            other.tag_ = INVAL;
            switch (this->tag_) {
                case SOME:
                    this->some_ = std::move(other.some_);
                    return *this;
                    break;
                case NONE:
                    this->none_ = std::move(other.none_);
                    return *this;
                    break;
                case INVAL:
                    return *this;
                    break;
            }
            //std::cerr << "invalid Optional: tag=" << this->tag_ << std::endl;
            nel::log << "invalid  Optional: tag=" << this->tag_ << "\n";
            std::abort();
#endif
            return *this;
        }

    public:
        // wrapped value checking without consuming the value.
        /**
         * Determine if the container contains a Some.
         *
         * @returns true if container contains a Some, false otherwise.
         */
        constexpr bool is_some(void) const noexcept
        {
            return this->tag_ == SOME;
        }

        /**
         * Determine if the container contains a None.
         *
         * @returns true if container contains a None, false otherwise.
         */
        constexpr bool is_none(void) const noexcept
        {
            return this->tag_ == NONE;
        }

    public:
        // contained value extraction with consumption
        // i..e extracting invalidates the container.
        // prob not expected C++ i.e. values being invalidated by use
        // and more acess by const ref or copy-out.

        /**
         * Extract and return the contained value if a Some, consuming the optional.
         *
         * If the optional does not contain a Some, then abort/panic.
         */
        constexpr T unwrap(void) noexcept
        {
            if (!this->is_some()) {
                abort();
            };
            tag_ = INVAL;
            return this->some_.unwrap();
        }

        /**
         * Extract and return the contained value if a Some, consuming the optional.
         *
         * If the optional does not contain a Some, then return `other`, which will
         * also be consumed.
         */
        constexpr T unwrap_or(T &&other) noexcept
        {
            bool const is_some = this->is_some();
            tag_ = INVAL;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
            return is_some ? this->some_.unwrap() : other;
#pragma GCC diagnostic pop
        }
        template<typename ...Args>
        constexpr T unwrap_or(Args &&...args) noexcept
        {
            bool const is_some = this->is_some();
            tag_ = INVAL;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
            return is_some ? this->some_.unwrap() : T(std::forward<Args>(args)...);
#pragma GCC diagnostic pop
        }

        // Why no access via references?
        // this allows copying of value which I want to prevent.

    public:
        // Comparision operators
        // Implemented in terms of the operator on the type,
        // as some types may have more optimal impls of that oper than the
        // negation of it's opposite.
        constexpr bool operator==(Optional const &o) const noexcept
        {
            if (this->tag_ == o.tag_) {
                switch (this->tag_) {
                    case SOME:
                        return this->some_ == o.some_;
                        break;
                    case NONE:
                        return true;
                        break;
                    case INVAL:
                        return true;
                        break;
                }
                std::abort();
            }
            return false;
        }

        constexpr bool operator!=(Optional const &o) const noexcept
        {
            if (this->tag_ == o.tag_) {
                switch (this->tag_) {
                    case SOME:
                        return this->some_ != o.some_;
                        break;
                    case NONE:
                        return false;
                        break;
                    case INVAL:
                        return false;
                        break;
                }
                std::abort();
            }
            return true;
        }

        constexpr bool operator==(const Some &val) const noexcept
        {
            return this->is_some() && this->some_ == val;
        }

        constexpr bool operator!=(const Some &val) const noexcept
        {
            return !this->is_some() || this->some_ != val;
        }

        constexpr bool operator==(const None &val) const noexcept
        {
            ((void)(val));
            return this->is_none();
        }

        constexpr bool operator!=(const None &val) const noexcept
        {
            ((void)(val));
            return !this->is_none();
        }

    public:
        //friend std::ostream &operator<<(std::ostream &outs, Optional const &val) {
        friend Log &operator<<(Log &outs, Optional const &val)
        {
            outs << "Optional(";
            switch (val.tag_) {
                case NONE:
                    outs << val.none_;
                    break;
                case SOME:
                    outs << val.some_;
                    break;
                case INVAL:
                    outs << "Inval";
                    break;
            }
            outs << ")";
            return outs;
        }
};

// Helper function to allow creating a Some without need to prefix with
// Optional<T>.
// Only present as Some is defined in Optional.
template<typename T>
constexpr Optional<T> Some(T &&v)
{
    return typename Optional<T>::Some(std::move(v));
}
template<typename T, typename ...Args>
constexpr Optional<T> Some(Args &&...v)
{
    return typename Optional<T>::Some(std::forward<Args>(v)...);
}

// Helper function to allow creating a None without need to prefix with
// Optional<T>.
// Only present as None is defined in Optional.
template<typename T>
constexpr Optional<T> None(void)
{
    return typename Optional<T>::None();
}

} // namespace nel

#endif // NEL_OPTIONAL_HH
