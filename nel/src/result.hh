#ifndef NEL_RESULT_HH
#define NEL_RESULT_HH

namespace nel {

template<typename T, typename E>
class Result;

}

#include "optional.hh" //nel::OptionalT

#include <functional> //std::function
//#include <iostream>
#include "log.hh"
#include <utility> // std::move

namespace nel {

/**
 * Result<T,E>
 *
 * A construct containing a Ok or Err value, where the Ok aspect holds a T, and
 * the Error holds a E c.f. Rust's Result<>
 *
 * usage:
 * ```c++
 *      Result<T, E> r1;// should be compile error, but move semantics allow.
 *
 *      Result<T,E> r1 = Result<T,E>::Ok(T());
 *      Result<T,E> r2 = Result<T,E>::Ok(E()); // won't compile
 *      Result<T,E> r3 = Result<T,E>::Err(T()); // won't compile
 *      Result<T,E> r4 = Result<T,E>::Err(E()); //
 *
 *      bool b1 = r1.is_ok(); // true
 *      bool b2 = r1.is_err(); // false
 *
 *      bool b3 = r4.is_ok(); // false
 *      bool b4 = r4.is_err(); // true
 *
 *      bool b5 = r1 == r4; // false
 *      bool b6 = r1 == Result<T,E>::Ok(T()); // true
 *      bool b7 = r4 == Result<T,E>::Err(E()); // true
 *
 *      T t1 = r1.unwrap();
 *      E e1 = r1.unwrap_err(); // aborts.
 *
 *      T t2 = r4.unwrap(); // aborts
 *      E e2 = r4.unwrap_err();
 * ```
 * etc.
 *
 */
template<typename T, typename E>
class Result {
    public:
        // A tag class for the Ok case for Result.
        // May be moved to outside the Result class as all Ok<T> are the same
        // and not really specialised to Result.
        // Only in Result to be in it's namespace.
        // Ah, maybe not, Ok in one namespace may be different in another..
        // but not within nel.
        class Ok {
            private:
                T value_;

            public:
                // No defaults please.. what would be the wrapped value? default of
                // wrapped type? Then be explicit in the code, which is covered by the
                // move oper.
                Ok(void) = delete;

                // don't want copy-, but need move- instead
                Ok(Ok const &) = delete;
                Ok &operator=(Ok const &) const = delete;

                constexpr Ok(Ok &&o) noexcept
                    : value_(std::move(o.value_))
                {
                }
                constexpr Ok &operator=(Ok &&o) noexcept
                {
                    value_ = std::move(o.value_);
                    return *this;
                }

            public:
                // No implicit conversions.. if you want an Ok, then specify.
                explicit constexpr Ok(T &&val) noexcept
                    : value_(std::move(val))
                {
                }

                // inplace contruct..
                template<typename ...Args>
                explicit constexpr Ok(Args &&...args) noexcept
                    : value_(std::forward<Args>(args)...)
                {
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
                constexpr bool operator==(const Ok &other) const noexcept
                {
                    return this->value_ == other.value_;
                }
                constexpr bool operator!=(const Ok &other) const noexcept
                {
                    return this->value_ != other.value_;
                }

            public:
                //friend std::ostream &operator<<(std::ostream &outs, Ok const &val) {
                friend Log &operator<<(Log &outs, Ok const &val)
                {
                    outs << "Ok(" << val.value_ << ")";
                    return outs;
                }
        };

        // A tag class for the Err case for Result.
        // May be moved to outside the Result class as all Err<T> are the same
        // and not really specialised to Result.
        // Only in Result to be in it's namespace.
        // Ah, maybe not, Err in one namespace may be different in another..
        // but not within nel.
        class Err {
            private:
                E value_;

            public:
                // No defaults please.. what would be the wrapped value? default of
                // wrapped type? Then be explicit in the code, which is covered by the
                // move oper.
                Err(void) = delete;

                // don't want these, but need moves instead
                Err(Err const &) = delete;
                Err &operator=(Err const &) const = delete;

                constexpr Err(Err &&o) noexcept
                    : value_(std::move(o.value_))
                {
                }
                constexpr Err &operator=(Err &&o) noexcept
                {
                    value_ = std::move(o.value_);
                    return *this;
                }

            public:
                // No implicit conversions.. if you want an Err, then specify.
                explicit constexpr Err(E &&val) noexcept
                    : value_(std::move(val))
                {
                }

                // inplace construct
                template<typename ...Args>
                explicit constexpr Err(Args &&...args) noexcept
                    : value_(std::forward<Args>(args)...)
                {
                }

            public:
                // Contained value access
                // Note no return via copy, as members may not be copyable.
                // note no return via copy, copy may be expensive.
                // Note no access via reference, accessing the contained must invalidate
                // the container. i.e value extract/unwrap invalidates the container.
                constexpr E &&unwrap(void) noexcept
                {
                    return std::move(this->value_);
                }

                // Why no access via references?
                // this allows copying of value which I want to prevent.

            public:
                // Comparision operators
                // Implemented in terms of the operator on the type,
                // as some types may have more optimal impls of that oper than the
                // negation of it's opposite.
                constexpr bool operator==(const Err &other) const noexcept
                {
                    return this->value_ == other.value_;
                }
                constexpr bool operator!=(const Err &other) const noexcept
                {
                    return this->value_ != other.value_;
                }

            public:
                //friend std::ostream &operator<<(std::ostream &outs, Err const &val) {
                friend Log &operator<<(Log &outs, Err const &val)
                {
                    outs << "Err(" << val.value_ << ")";
                    return outs;
                }
        };

    private:
        // Tagged enum thing
        // Similar to std::variant but without the exception throwing behaviour.
        // maybe make into a nel::Variant ?
        struct Tag {
            public:
                typedef enum { INVAL = 0, OK, ERR } Values;
            public:
                constexpr Tag(void) noexcept
                    : value_(INVAL) {}
                constexpr Tag(Values const v) noexcept
                    : value_(v) {}
                constexpr Tag &operator=(Values const &v) noexcept
                {
                    // simple type, does not need swapping..
                    value_ = v;
                    return *this;
                }
                constexpr Tag(Tag &&o) noexcept
                    : value_(std::move(o.value_))
                {
                    o.value_ = INVAL;
                }
                constexpr Tag &operator=(Tag &&o) noexcept
                {
                    value_ = std::move(o.value_);
                    o.value_ = INVAL;
                    return *this;
                }
            public:
                constexpr bool operator==(Tag const &o) const noexcept
                {
                    return value_ == o.value_;
                }
            public:
                Values value_;
        };
        Tag tag_;

        union {
            Ok ok_;
            Err err_;
        };

    public:
        ~Result(void) noexcept
        {
            switch (this->tag_.value_) {
                case Tag::OK:
                    this->ok_.~Ok();
                    return;
                    break;

                case Tag::ERR:
                    this->err_.~Err();
                    return;
                    break;

                case Tag::INVAL:
                    return;
                    break;
            }
            // No default as I want compile to fail if an enum case handler
            // isn't present.
            // But, want to abort/panic if a unhandled case is encountered
            // at runtime, much how a default hander would work if it was
            // present.
            //std::cerr << "invalid Result: tag=" << this->tag_ << std::endl;
            std::abort();
        }

        constexpr Result(void) noexcept {}

        // don't want copy/assign but needed if no move/assign impls.
        // as there's a non-trivial union involved, need to add impl.
#if 1
        Result(Result const &) = delete;
        Result &operator=(Result const &) const = delete;
#else
        constexpr Result(Result const &o):
            tag_(o.tag_)
        {
            switch (o.tag_.value_) {
                case Tag::OK:
                    new (&this->ok_) Ok(o.ok_);
                    return;
                    break;
                case Tag::ERR:
                    new (&this->err_) Err(o.err_);
                    return;
                    break;
                case Tag::INVAL:
                    return;
                    break;
            }
            //std::cerr << "invalid Result: tag=" << this->tag_ << std::endl;
            std::abort();
        };
        constexpr Result &operator=(Result const &o)
        {
            //#if 0
            if (this != &o) {
                if (tag_ == o.tag_) {
                    tag_ = o.tag_;
                    switch (tag_.value_) {
                        case Tag::OK:
                            ok_ = o.ok_;
                            return *this;
                            break;
                        case Tag::ERR:
                            err_ = o.err_;
                            return *this;
                            break;
                        case Tag::INVAL:
                            return *this;
                            break;
                    }
                    std::abort();
                } else {
                    // only safe if copy-ctor guaranteed to not throw
                    this->~Result();
                    new (this) Result(o);
                }
                return *this;
            }
            return *this;
            // #else
            //             if (this != &o) {
            //                 Result t(o);
            //                 swap(t);
            //             }
            //             return *this;
            // #endif
        }
#endif
        // void swap(Result &o) {
        //     // yeah, hacky, not by-member, so bite me.
        //     mymemswap((uint8_t *)this, (uint8_t *)&o, sizeof(*this));
        // }

        constexpr Result(Result &&o) noexcept
            : tag_(std::move(o.tag_))
        {
            switch (this->tag_.value_) {
                case Tag::OK:
                    new (&this->ok_) Ok(std::move(o.ok_));
                    return;
                    break;
                case Tag::ERR:
                    new (&this->err_) Err(std::move(o.err_));
                    return;
                    break;
                case Tag::INVAL:
                    return;
                    break;
            }
            //std::cerr << "invalid Result: tag=" << this->tag_ << std::endl;
            std::abort();
        }
        constexpr Result &operator=(Result &&o) noexcept
        {
            //#if 1
            if (tag_ == o.tag_) {
                tag_ = std::move(o.tag_);
                switch (tag_.value_) {
                    case Tag::OK:
                        ok_ = std::move(o.ok_);
                        return *this;
                        break;
                    case Tag::ERR:
                        err_ = std::move(o.err_);
                        return *this;
                        break;
                    case Tag::INVAL:
                        return *this;
                        break;
                }
                std::abort();
                // copy O(N), zap O(N) => O(2N)
            } else {
                // only safe if move-ctor guaranteed to not throw
                this->~Result();
                new (this) Result(std::move(o));
                // copy O(N), zap O(N) => O(2N)
            }
            return *this;
            // #else
            //             Result t(std::move(o));
            //             // copy O(N), zap O(N) => O(2N)
            //             swap(t);
            //             // byteswap O(N)
            //             // total = O(3N)
            //             return *this;
            // #endif
        }


    public:
        // // Consume and wrap the given ok value.
        constexpr Result(Ok &&v) noexcept
            : tag_(Tag::OK), ok_(std::move(v))
        {
        }
        // Result needs to construct Ok inplace as well..
        // prob better to not have OK as sep type then, merely a create func.

        // Consume and wrap the given err value.
        constexpr Result(Err &&v) noexcept
            : tag_(Tag::ERR), err_(std::move(v))
        {
        }
        // Result needs to construct Err inplace as well..

    public:
        // Comparision operators
        // Implemented in terms of the operator on the type,
        // as some types may have more optimal impls of that oper than the
        // negation of it's opposite.
        constexpr bool operator==(const Result<T, E> &o) const noexcept
        {
            if (this->tag_ == o.tag_) {
                switch (this->tag_.value_) {
                    case Tag::OK:
                        return this->ok_ == o.ok_;
                        break;
                    case Tag::ERR:
                        return this->err_ == o.err_;
                        break;
                    case Tag::INVAL:
                        return true;
                        break;
                }
                std::abort();
            }
            return false;
        }

        constexpr bool operator!=(const Result<T, E> &o) const noexcept
        {
            if (this->tag_ == o.tag_) {
                switch (this->tag_.value_) {
                    case Tag::OK:
                        return this->ok_ != o.ok_;
                        break;
                    case Tag::ERR:
                        return this->err_ != o.err_;
                        break;
                    case Tag::INVAL:
                        return false;
                        break;
                }
                std::abort();
            }
            return true;
        }

        constexpr bool operator==(const Ok &val) const noexcept
        {
            return this->is_ok() && this->ok_ == val;
        }

        constexpr bool operator!=(const Ok &val) const noexcept
        {
            return !this->is_ok() || this->ok_ != val;
        }

        constexpr bool operator==(const Err &val) const noexcept
        {
            return this->is_err() && this->err_ == val;
        }

        constexpr bool operator!=(const Err &val) const noexcept
        {
            return !this->is_err() || this->err_ != val;
        }

    public:
        /**
         * Determine if the container contains a Ok.
         *
         * @returns true if container contains a Ok, false otherwise.
         *
         * value is not consumed by this op.
         */
        constexpr bool is_ok(void) const noexcept
        {
            return this->tag_.value_ == Tag::OK;
        }

        /**
         * Determine if the container contains a Err.
         *
         * @returns true if container contains a Err, false otherwise.
         *
         * value is not consumed by this op.
         */
        bool is_err(void) const noexcept
        {
            return this->tag_.value_ == Tag::ERR;
        }

    public:
        /**
         * If result is an Ok, return optional::Some<val>, else return
         * optional::None.
         * The result is consumed and invalidated after.
         */
        constexpr Optional<T> ok(void) noexcept
        {
            bool const is_ok = this->is_ok();
            tag_ = Tag::INVAL;
            return is_ok ? Some(this->ok_.unwrap()) :
                   // TODO: remove the T in None..
                   None<T>();
        }

        /**
         * If result is an Err, return optional::Some<err>, else return
         * optional::None.
         * The result is consumed and invalidated after.
         */
        constexpr Optional<E> err(void) noexcept
        {
            bool const is_err = this->is_err();
            tag_ = Tag::INVAL;
            return is_err ? Some(this->err_.unwrap()) :
                   // TODO: remove the E in None..
                   None<E>();
        }

        /**
         * Extract and return the contained value if an Ok, consuming the result.
         *
         * If the result does not contain an Ok, then abort/panic.
         */
        constexpr T unwrap(void) noexcept
        {
            if (!this->is_ok()) {
                std::abort();
            };
            tag_ = Tag::INVAL;
            return ok_.unwrap();
        }

        /**
         * Extract and return the contained value if an Err, consuming the result.
         *
         * If the result does not contain an Err, then abort/panic.
         */
        constexpr E unwrap_err(void) noexcept
        {
            if (!this->is_err()) {
                std::abort();
            };
            tag_ = Tag::INVAL;
            return err_.unwrap();
        }

        /**
         * Extract and return the contained value if an Ok, consuming the optional.
         *
         * If the result does not contain an Ok, then return `other`, which will
         * also be consumed.
         */
        constexpr T unwrap_or(T &&other) noexcept
        {
            bool const is_ok = this->is_ok();
            tag_ = Tag::INVAL;
            return is_ok ? this->ok_.unwrap() : std::move(other);
        }
        template<typename ...Args>
        constexpr T unwrap_or(Args &&...args) noexcept
        {
            bool const is_ok = this->is_ok();
            tag_ = Tag::INVAL;
            return is_ok ? this->ok_.unwrap() : T(std::forward<Args>(args)...);
        }

        /**
         * Extract and return the contained value if an Err, consuming the optional.
         *
         * If the result does not contain an Err, then return `other`, which will
         * also be consumed.
         */
        constexpr E unwrap_err_or(E &&other) noexcept
        {
            bool const is_err = this->is_err();
            tag_ = Tag::INVAL;
            return is_err ? this->err_.unwrap() : std::move(other);
        }
        template<typename ...Args>
        constexpr T unwrap_err_or(Args &&...args) noexcept
        {
            bool const is_err = this->is_err();
            tag_ = Tag::INVAL;
            return is_err ? this->err_.unwrap() : E(std::forward<Args>(args)...);
        }


        // ptr/ref access accesses value, and leaves it in the variable.
        // which we don't want.


        /**
         * Map the result to another result.
         *
         * If result contains an err then return a new result containin it
         * (passthrough). If result contains an ok, then return a new result with
         * value remapped by fn.
         */
        // would this be better as a free func?
        template<class U>
        constexpr Result<U, E> map(std::function < U(T &&) > fn) noexcept
        {
            // TODO: remove need to explictly cast to result in each of the
            //       branches.. i.e. the Result<U,E>() bit.
            // i.e.:
            // return this->is_ok()? Ok(fn(ok_.unwrap())) : Err(err_.unwrap());
            switch (tag_.value_) {
                case Tag::OK:
                    return typename Result<U, E>::Ok(fn(unwrap()));
                    break;
                case Tag::ERR:
                    return typename Result<U, E>::Err(unwrap_err());
                    break;
                case Tag::INVAL:
                    return Result<U, E>();
                    break;
            }
            std::abort();
        }

        /**
         * Map the result to another result.
         *
         * If result contains an ok then return a new result containin it
         * (passthrough). If result contains an err, then return a new result with
         * value remapped by fn.
         */
        // would this be better as a free func?
        template<class F>
        constexpr Result<T, F> map_err(std::function < F(E &&) > fn) noexcept
        {
            // TODO: remove need to explictly cast to result in each of the
            //       branches.. i.e. the Result<U,E>() bit.
            // i.e.
            // return this->is_err()? Err(fn(err_.unwrap())) : Ok(ok_.unwrap());
            switch (tag_.value_) {
                case Tag::OK:
                    return typename Result<T, F>::Ok(unwrap());
                    break;
                case Tag::ERR:
                    return typename Result<T, F>::Err(fn(unwrap_err()));
                    break;
                case Tag::INVAL:
                    return Result<T, F>();
                    break;
            }
            std::abort();
        }

    public:
        //friend std::ostream &operator<<(std::ostream &outs, Result const &val) {
        friend Log &operator<<(Log &outs, Result const &val)
        {
            outs << "Result(";
            switch (val.tag_) {
                case Tag::OK:
                    outs << val.ok_;
                    break;
                case Tag::ERR:
                    outs << val.err_;
                    break;
                case Tag::INVAL:
                    outs << "Inval";
                    break;
            }
            outs << ")";
            return outs;
        }
};

// Helper function to allow creating an Ok without need to prefix with
// Result<T,E>.
// Only present as Ok is defined in Result.
template<typename T, typename E>
constexpr Result<T, E> Ok(T &&v)
{
    return typename Result<T, E>::Ok(std::forward<T>(v));
}

// Helper function to allow creating a Err without need to prefix with
// Result<T,E>.
// Only present as Err is defined in Result.
template<typename T, typename E>
constexpr Result<T, E> Err(E &&v)
{
    return typename Result<T, E>::Err(std::forward<E>(v));
}

// calc v as a temp to keep f single eval-ed
#define NEL_RESULT_TRY(f) \
    ({ \
        auto v = std::move(f); \
        if (v.is_err()) { \
            return v; \
        } \
        v.unwrap(); \
    })

} // namespace nel

#endif // NEL_RESULT_HH
