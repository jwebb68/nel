#ifndef NEL_RESULT_HH
#define NEL_RESULT_HH

namespace nel {

template<typename T, typename E>
class Result;

}

#include "element.hh"
#include "optional.hh"
#include "log.hh"
#include "panic.hh"

#include <utility> // std::move, std::forward
#include <functional> //std::function

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
// T must implement move semanitcs
// T may not have implement copy semanitcs
// E must implement move semanitcs
// E may not have implement copy semanitcs
// T must implement operator==, operator!=
// E must implement operator==, operator!=
template<typename T, typename E>
class Result {
    public:
    private:
        typedef T OkT;
        typedef E ErrT;

        // Tagged enum thing
        // Similar to std::variant but without the exception throwing behaviour.
        // maybe make into a nel::Variant ?
        enum Tag { INVAL = 0, OK, ERR } tag_;
        template<enum Tag>
        struct Phantom {};

        union {
            Element<OkT> ok_;
            Element<ErrT> err_;
        };

        Result(Phantom<OK> const, OkT &&v) noexcept
            : tag_(OK)
            , ok_(std::move(v))
        {}

        template<typename ...Args>
        Result(Phantom<OK> const, Args &&...args) noexcept
            : tag_(OK)
            , ok_(std::forward<Args>(args)...)
        {}

        Result(Phantom<ERR> const, ErrT &&v) noexcept
            : tag_(ERR)
            , err_(std::move(v))
        {}

        template<typename ...Args>
        Result(Phantom<ERR> const, Args &&...args) noexcept
            : tag_(ERR)
            , err_(std::forward<Args>(args)...)
        {}

    public:
        ~Result(void) noexcept
        {
            switch (tag_) {
                case OK:
                    ok_.~Element<OkT>();
                    return;
                    break;

                case ERR:
                    err_.~Element<ErrT>();
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
            //std::cerr << "invalid Result: tag=" << tag_ << std::endl;
            //std::abort();
            nel_panic("invalid Result");
        }

        Result(Result &&o) noexcept
            : tag_(std::move(o.tag_))
        {
            o.tag_ = INVAL;
            switch (tag_) {
                case OK:
                    new (&ok_) Element<OkT>(std::move(o.ok_));
                    return;
                    break;
                case ERR:
                    new (&err_) Element<ErrT>(std::move(o.err_));
                    return;
                    break;
                case INVAL:
                    return;
                    break;
            }
            //std::cerr << "invalid Result: tag=" << this->tag_ << std::endl;
            //std::abort();
            nel_panic("invalid Result");
        }
        Result &operator=(Result &&o) noexcept
        {
#if 1
            if (tag_ == o.tag_) {
                o.tag_ = INVAL;
                switch (tag_) {
                    case OK:
                        ok_ = std::move(o.ok_);
                        return *this;
                        break;
                    case ERR:
                        err_ = std::move(o.err_);
                        return *this;
                        break;
                    case INVAL:
                        return *this;
                        break;
                }
                //std::abort();
                nel_panic("invalid Result");
            } else {
                // only safe if move-ctor guaranteed to not throw
                this->~Result();
                new (this) Result(std::move(o));
            }
#else
            // can be very inefficient.
            // depends on impl of swap and dtor and m-ctor
            Result t(std::move(o));
            swap(t);
#endif
            return *this;
        }

#if 0
        void swap(Result &o) noexcept
        {
            // yeah, hacky, not by-member, so bite me.
            mymemswap((uint8_t *)this, (uint8_t *)&o, sizeof(*this));
        }
#endif

        // not using copy semanics, results are only moveable..
        Result(Result const &) = delete;
        Result &operator=(Result const &) const = delete;

        // don't really want default ctor, but move semanitcs brought in a
        // inval state, which can be used for it.
        constexpr Result(void) noexcept
            : tag_(INVAL)
        {
        }

    public:
        /**
         * Create a result set to Ok value, using the value given.
         *
         * @param val The value to move into the result as an ok value.
         *
         * @returns a Result 'wrapping' the value given as an ok value.
         */
        static Result Ok(T &&val) noexcept
        {
            return Result(Phantom<OK>(), std::move(val));
        }

        /**
         * Create a result set to Ok value, using the values to construct the ok value.
         *
         * @param args The values to use to construct an Ok value inplace.
         *
         * @returns a Result 'wrapping' the values given to construct an ok value inplace.
         */
        template<typename ...Args>
        static Result Ok(Args &&...args) noexcept
        {
            return Result(Phantom<OK>(), std::forward<Args>(args)...);
        }

        /**
         * Create a result set to Err value, using the value given.
         *
         * @param val The value to move into the result as an err value.
         *
         * @returns a Result 'wrapping' the value given as an err value.
         */
        static Result Err(E &&val) noexcept
        {
            return Result(Phantom<ERR>(), std::move(val));
        }

        /**
         * Create a result set to Err value, using the values to construct the err value.
         *
         * @param args The values to use to construct an Err value inplace.
         *
         * @returns a Result 'wrapping' the values given to construct an Err value inplace.
         */
        template<typename ...Args>
        static Result Err(Args &&...args) noexcept
        {
            return Result(Phantom<ERR>(), std::forward<Args>(args)...);
        }

    public:
        // Comparision operators
        // Implemented in terms of the operator on the type,
        // as some types may have more optimal impls of that oper than the
        // negation of it's opposite.
        /**
         * Is this equal by value to the result given?
         *
         * @param o The other result to compare to.
         * @returns true if equal by value, false otherwise.
         *
         * `this` is not consumed by the operation.
         * `o` is not consumed by the operation.
         */
        constexpr bool operator==(Result const &o) const noexcept
        {
            if (tag_ == o.tag_) {
                switch (tag_) {
                    case OK:
                        return ok_ == o.ok_;
                        break;
                    case ERR:
                        return err_ == o.err_;
                        break;
                    case INVAL:
                        return true;
                        break;
                }
                nel_panic("invalid Result");
                //std::abort();
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
        constexpr bool operator!=(Result const &o) const noexcept
        {
            if (tag_ == o.tag_) {
                switch (tag_) {
                    case OK:
                        return ok_ != o.ok_;
                        break;
                    case ERR:
                        return err_ != o.err_;
                        break;
                    case INVAL:
                        return false;
                        break;
                }
                nel_panic("invalid Result");
                //std::abort();
            }
            return true;
        }

    public:
        /**
         * Determine if the container contains a Ok.
         *
         * @returns true if container contains a Ok, false otherwise.
         *
         * this is not consumed by this op.
         */
        constexpr bool is_ok(void) const noexcept
        {
            return tag_ == OK;
        }

        /**
         * Determine if the container contains a Err.
         *
         * @returns true if container contains a Err, false otherwise.
         *
         * this is not consumed by this op.
         */
        constexpr bool is_err(void) const noexcept
        {
            return tag_ == ERR;
        }

    public:
        /**
         * Return an optional containing the ok value or none.
         *
         * @returns if this is a Ok, returns Optional containing a Some with the ok value.
         * @returns if this is not a Ok, returns Optional containing a None.
         *
         * this is consumed and invalidated after.
         */
        Optional<OkT> ok(void) noexcept
        {
            bool const is_ok = this->is_ok();
            tag_ = INVAL;
            // TODO: remove need to explictly cast to Optional in each of the
            //       branches.. i.e. the Optional<T>:: bit.
            return is_ok
                   ? Optional<OkT>::Some(ok_.unwrap())
                   : Optional<OkT>::None();
        }

        /**
         * Return an optional containing the err value or none.
         *
         * @returns if this is a Err, returns Optional containing a Some with the err value.
         * @returns if this is not a Err, returns Optional containing a None.
         *
         * this is consumed and invalidated after.
         */
        Optional<ErrT> err(void) noexcept
        {
            bool const is_err = this->is_err();
            tag_ = INVAL;
            // TODO: remove need to explictly cast to Optional in each of the
            //       branches.. i.e. the Optional<E>:: bit.
            return is_err
                   ? Optional<ErrT>::Some(err_.unwrap())
                   : Optional<ErrT>::None();
        }

        /**
         * Extract and return the contained value if an Ok, consuming the result.
         *
         * @return if Ok, value wrapped/contained.
         * @return if not Ok, calls abort, there is no return.
         *
         * this is consumed by the operation.
         */
        OkT unwrap(void) noexcept
        {
            if (!is_ok()) {
                nel_panic("not an ok");
                //std::abort();
            };
            tag_ = INVAL;
            return ok_.unwrap();
        }

        /**
         * Extract and return the contained value if an Err, consuming the result.
         *
         * @return if Err, value wrapped/contained.
         * @return if not Err, calls abort, there is no return.
         *
         * this is consumed by the operation.
         */
        ErrT unwrap_err(void) noexcept
        {
            if (!is_err()) {
                nel_panic("not an err");
                //std::abort();
            };
            tag_ = INVAL;
            return err_.unwrap();
        }

        /**
         * Extract and return the contained value if an Ok, consuming the result.
         *
         * @param v The value to return if this is not an Ok.
         *
         * @return if Ok, value wrapped/contained.
         * @return if not Ok, consume and return `v`.
         *
         * this is consumed by the operation.
         * `v` is consumed by the operation if this is an ok.
         */
        OkT unwrap_or(OkT &&v) noexcept
        {
            bool const is_ok = this->is_ok();
            tag_ = INVAL;
            return is_ok
                   ? ok_.unwrap()
                   : std::move(other);
        }

        /**
         * Extract and return the contained value if an Ok, consuming the result.
         *
         * @param args The value to construct an Ok if this is not an Ok.
         *
         * @return if Ok, value wrapped/contained.
         * @return if not Ok, consume and return an Ok value constructed by args.
         *
         * this is consumed by the operation.
         * args are consumed by the operation if this is an ok.
         */
        template<typename ...Args>
        OkT unwrap_or(Args &&...args) noexcept
        {
            bool const is_ok = this->is_ok();
            tag_ = INVAL;
            return is_ok
                   ? ok_.unwrap()
                   : OkT(std::forward<Args>(args)...);
        }

        /**
         * Extract and return the contained value if an Err, consuming the result.
         *
         * @param v The value to return if this is not an Err.
         *
         * @return if Err, value wrapped/contained.
         * @return if not Err, consume and return `v`.
         *
         * this is consumed by the operation.
         * `v` is consumed by the operation if this is an err.
         */
        ErrT unwrap_err_or(ErrT &&v) noexcept
        {
            bool const is_err = this->is_err();
            tag_ = INVAL;
            return is_err
                   ? err_.unwrap()
                   : std::move(o);
        }

        /**
         * Extract and return the contained value if an Err, consuming the result.
         *
         * @param args The value to construct an Err if this is not an Err.
         *
         * @return if Err, value wrapped/contained.
         * @return if not Err, consume and return an Err value constructed by args.
         *
         * this is consumed by the operation.
         * args are consumed by the operation if this is an Err.
         */
        template<typename ...Args>
        ErrT unwrap_err_or(Args &&...args) noexcept
        {
            bool const is_err = this->is_err();
            tag_ = INVAL;
            return is_err
                   ? err_.unwrap()
                   : ErrT(std::forward<Args>(args)...);
        }

        /**
         * Map the result::ok to a different result::ok.
         *
         * @param fn A fn that takes a T and returns a U
         * @returns if ok, result with ok value after applying fn to ok value.
         * @returns if err, result with err of same value.
         *
         * this is consumed by the operation.
         */
        // would this be better as a free func?
        template<class U>
        Result<U, E> map(std::function < U(OkT &&) > fn) noexcept
        {
            // TODO: remove need to explictly cast to result in each of the
            //       branches.. i.e. the Result<U,E>() bit.
            switch (tag_) {
                case OK:
                    return Result<U, ErrT>::Ok(fn(unwrap()));
                    break;
                case ERR:
                    return Result<U, ErrT>::Err(unwrap_err());
                    break;
                case INVAL:
                    return Result<U, ErrT>();
                    break;
            }
            nel_panic("invalid Result");
            //std::abort();
        }

        /**
         * Map the result::err to a different result::err.
         *
         * @param fn A fn that takes a E and returns a F
         * @returns if err, result with err value after applying fn to err value.
         * @returns if ok, result with ok of same value.
         *
         * this is consumed by the operation.
         */
        // would this be better as a free func?
        template<class F>
        Result<OkT, F> map_err(std::function < F(ErrT &&) > fn) noexcept
        {
            // TODO: remove need to explictly cast to result in each of the
            //       branches.. i.e. the Result<U,E>() bit.
            switch (tag_) {
                case OK:
                    return Result<OkT, F>::Ok(unwrap());
                    break;
                case ERR:
                    return Result<OkT, F>::Err(fn(unwrap_err()));
                    break;
                case INVAL:
                    return Result<OkT, F>();
                    break;
            }
            //std::abort();
            nel_panic("invalid Result");
        }

    public:
        //friend std::ostream &operator<<(std::ostream &outs, Result const &val) {
        friend Log &operator<<(Log &outs, Result const &val)  noexcept
        {
            switch (val.tag_) {
                case OK:
                    outs << "Result(Ok(" << *val.ok_ << "))";
                    return outs;
                    break;
                case ERR:
                    outs << "Result(Err(" << *val.err_ << "))";
                    return outs;
                    break;
                case INVAL:
                    outs << "Result(Inval)";
                    return outs;
                    break;
            }
            outs << "Result(Unknown)";
            return outs;
        }
};

// calc v as a temp to keep f single eval-ed
#define NEL_RESULT_TRY(f) \
    __extension__ ({ \
        auto v = std::move(f); \
        if (v.is_err()) { \
            return v; \
        } \
        v.unwrap(); \
    })

} // namespace nel

#endif // NEL_RESULT_HH
