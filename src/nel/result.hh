#ifndef NEL_RESULT_HH
#define NEL_RESULT_HH

namespace nel
{

template<typename T, typename E>
class Result;

} // namespace nel

#include <nel/optional.hh>
#include <nel/element.hh>
#include <nel/log.hh>
#include <nel/panic.hh>

#include <functional> // std::function
#include <utility> // std::move, std::forward

namespace nel
{

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
// T must implement move semantics
// T may not have implement copy semantics
// E must implement move semantics
// E may not have implement copy semantics
// T must implement operator==, operator!=
// E must implement operator==, operator!=
template<typename T, typename E>
class Result
{
    public:
    private:
        // Tagged enum thing.
        // Similar to std::variant but without the exception throwing behaviour.
        // Maybe make into a nel::Variant ?
        enum class Tag {
            INVAL = 0,
            OK,
            ERR
        } tag_;

        template<enum Tag>
        struct Phantom {
        };

        union {
                Element<T> ok_;
                Element<E> err_;
        };

    public:
        constexpr ~Result(void) noexcept
        {
            switch (tag_) {
                case Tag::OK:
                    ok_.~Element<T>();
                    break;

                case Tag::ERR:
                    err_.~Element<E>();
                    break;

                case Tag::INVAL:
                    break;

                // I want compile to fail if an enum case handler isn't present.
                default:
                    // But, want to abort/panic if a unhandled case is encountered
                    // at runtime, much how a default hander would work if it was
                    // present.
                    // std::cerr << "invalid Result: tag=" << tag_ << std::endl;
                    // std::abort();
                    nel_panic("invalid Result");
            }
        }

        constexpr Result(Result &&o) noexcept
        {
            tag_ = o.tag_;
            o.tag_ = Tag::INVAL;
            switch (tag_) {
                case Tag::OK:
                    new (&ok_) Element<T>(std::move(o.ok_));
                    return;
                    break;
                case Tag::ERR:
                    new (&err_) Element<E>(std::move(o.err_));
                    return;
                    break;
                case Tag::INVAL:
                    return;
                    break;
                default:
                    // std::cerr << "invalid Result: tag=" << this->tag_ << std::endl;
                    // std::abort();
                    nel_panic("invalid Result");
            }
        }

        Result &operator=(Result &&o) noexcept
        {
            if (this != &o) {
                tag_ = o.tag_;
                o.tag_ = Tag::INVAL;
                switch (tag_) {
                    case Tag::OK:
                        ok_ = std::move(o.ok_);
                        break;
                    case Tag::ERR:
                        err_ = std::move(o.err_);
                        break;
                    case Tag::INVAL:
                        break;
                    default:
                        // std::cerr << "invalid Result: tag=" << this->tag_ << std::endl;
                        // std::abort();
                        nel_panic("invalid Result");
                }
            }
            return *this;
        }

    private:
        // Not using copy semantics, results are only moveable..
        Result(Result const &) = delete;
        Result &operator=(Result const &) const = delete;

    public:
        // Don't really want default ctor, but move semantics brought in a
        // inval state, which can be used for it.
        constexpr Result(void) noexcept
            : tag_(Tag::INVAL)
        {
        }

    private:
        constexpr Result(Phantom<Tag::OK> const, T &&v) noexcept
            : tag_(Tag::OK)
            , ok_(std::forward<T>(v))
        {
        }

        template<typename... Args>
        constexpr Result(Phantom<Tag::OK> const, Args &&...args) noexcept
            : tag_(Tag::OK)
            , ok_(std::forward<Args>(args)...)
        {
        }

        constexpr Result(Phantom<Tag::ERR> const, E &&v) noexcept
            : tag_(Tag::ERR)
            , err_(std::forward<E>(v))
        {
        }

        template<typename... Args>
        constexpr Result(Phantom<Tag::ERR> const, Args &&...args) noexcept
            : tag_(Tag::ERR)
            , err_(std::forward<Args>(args)...)
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
        constexpr static Result Ok(T &&val) noexcept
        {
            return Result(Phantom<Tag::OK>(), std::forward<T>(val));
        }

        /**
         * Create a result set to Ok value, using the values to construct the ok value.
         *
         * @param args The values to use to construct an Ok value inplace.
         *
         * @returns a Result 'wrapping' the values given to construct an ok value inplace.
         */
        template<typename... Args>
        constexpr static Result Ok(Args &&...args) noexcept
        {
            return Result(Phantom<Tag::OK>(), std::forward<Args>(args)...);
        }

        /**
         * Create a result set to Err value, using the value given.
         *
         * @param val The value to move into the result as an err value.
         *
         * @returns a Result 'wrapping' the value given as an err value.
         */
        constexpr static Result Err(E &&val) noexcept
        {
            return Result(Phantom<Tag::ERR>(), std::forward<E>(val));
        }

        /**
         * Create a result set to Err value, using the values to construct the err value.
         *
         * @param args The values to use to construct an Err value inplace.
         *
         * @returns a Result 'wrapping' the values given to construct an Err value inplace.
         */
        template<typename... Args>
        constexpr static Result Err(Args &&...args) noexcept
        {
            return Result(Phantom<Tag::ERR>(), std::forward<Args>(args)...);
        }

    public:
        // Comparision operators
        // Implemented in terms of the operator on the type,
        // as some types may have more optimal implementations of that operation than the
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
            if (this == &o) { return true; }
            if (tag_ == o.tag_) {
                switch (tag_) {
                    case Tag::OK:
                        return ok_ == o.ok_;
                        break;
                    case Tag::ERR:
                        return err_ == o.err_;
                        break;
                    case Tag::INVAL:
                        return true;
                        break;
                    default:
                        nel_panic("invalid Result");
                        // std::abort();
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
        constexpr bool operator!=(Result const &o) const noexcept
        {
            if (this == &o) { return false; }
            if (tag_ == o.tag_) {
                switch (tag_) {
                    case Tag::OK:
                        return ok_ != o.ok_;
                        break;
                    case Tag::ERR:
                        return err_ != o.err_;
                        break;
                    case Tag::INVAL:
                        return false;
                        break;
                    default:
                        nel_panic("invalid Result");
                        // std::abort();
                }
            }
            return true;
        }

    public:
        /**
         * Determine if the container contains a Ok.
         *
         * @returns true if container contains a Ok, false otherwise.
         *
         * `this` is not consumed by this op.
         */
        constexpr bool is_ok(void) const noexcept
        {
            return tag_ == Tag::OK;
        }

        /**
         * Determine if the container contains a Err.
         *
         * @returns true if container contains a Err, false otherwise.
         *
         * `this` is not consumed by this op.
         */
        constexpr bool is_err(void) const noexcept
        {
            return tag_ == Tag::ERR;
        }

    public:
        /**
         * Return an optional containing the ok value or none.
         *
         * @returns if this is a Ok, returns Optional containing a Some with the ok value.
         * @returns if this is not a Ok, returns Optional containing a None.
         *
         * `this` is consumed and invalidated after.
         */
        Optional<T> ok(void) noexcept
        {
            bool const is_ok = this->is_ok();
            tag_ = Tag::INVAL;
            // TODO: remove need to explicitly cast to Optional in each of the
            //       branches.. i.e. the Optional<T>:: bit.
            return is_ok ? Some(ok_.unwrap()) : None;
        }

        /**
         * Return an optional containing the err value or none.
         *
         * @returns if this is a Err, returns Optional containing a Some with the err value.
         * @returns if this is not a Err, returns Optional containing a None.
         *
         * `this` is consumed and invalidated after.
         */
        Optional<E> err(void) noexcept
        {
            bool const is_err = this->is_err();
            tag_ = Tag::INVAL;
            // TODO: remove need to explicitly cast to Optional in each of the
            //       branches.. i.e. the Optional<E>:: bit.
            return is_err ? Some(err_.unwrap()) : None;
        }

        /**
         * Extract and return the contained value if an Ok, consuming the result.
         *
         * @return if Ok, value wrapped/contained.
         * @return if not Ok, calls abort, there is no return.
         *
         * `this` is consumed by the operation.
         */
        T unwrap(void) noexcept
        {
            nel_panic_if_not(is_ok(), "not an ok");
            tag_ = Tag::INVAL;
            return ok_.unwrap();
        }

        /**
         * Extract and return the contained value if an Err, consuming `this`.
         *
         * @return if Err, value wrapped/contained.
         * @return if not Err, calls abort, there is no return.
         *
         * this is consumed by the operation.
         */
        E unwrap_err(void) noexcept
        {
            nel_panic_if_not(is_err(), "not an err");
            tag_ = Tag::INVAL;
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
         * `this` is consumed by the operation.
         * `v` is consumed by the operation.
         */
        T unwrap_or(T &&v) noexcept
        {
            bool const is_ok = this->is_ok();
            tag_ = Tag::INVAL;
            return is_ok ? ok_.unwrap() : std::forward<T>(v);
        }

        /**
         * Extract and return the contained value if an Ok, consuming the result.
         *
         * @param args The value to construct an Ok if this is not an Ok.
         *
         * @return if Ok, value wrapped/contained.
         * @return if not Ok, consume and return an Ok value constructed by args.
         *
         * `this` is consumed by the operation.
         * `args` are consumed by the operation.
         */
        template<typename... Args>
        T unwrap_or(Args &&...args) noexcept
        {
            bool const is_ok = this->is_ok();
            tag_ = Tag::INVAL;
            return is_ok ? ok_.unwrap() : T(std::forward<Args>(args)...);
        }

        /**
         * Extract and return the contained value if an Err, consuming the result.
         *
         * @param v The value to return if this is not an Err.
         *
         * @return if Err, value wrapped/contained.
         * @return if not Err, consume and return `v`.
         *
         * `this` is consumed by the operation.
         * `v` is consumed by the operation.
         */
        E unwrap_err_or(E &&v) noexcept
        {
            bool const is_err = this->is_err();
            tag_ = Tag::INVAL;
            return is_err ? err_.unwrap() : std::forward<E>(v);
        }

        /**
         * Extract and return the contained value if an Err, consuming the result.
         *
         * @param args The value to construct an Err if this is not an Err.
         *
         * @return if Err, value wrapped/contained.
         * @return if not Err, consume and return an Err value constructed by args.
         *
         * `this` is consumed by the operation.
         * `args` are consumed by the operation.
         */
        template<typename... Args>
        E unwrap_err_or(Args &&...args) noexcept
        {
            bool const is_err = this->is_err();
            tag_ = Tag::INVAL;
            return is_err ? err_.unwrap() : E(std::forward<Args>(args)...);
        }

        /**
         * Map the result::ok to a different result::ok.
         *
         * @param fn A fn that takes a T and returns a U
         * @returns if ok, result with ok value after applying fn to ok value.
         * @returns if err, result with err of same value.
         *
         * `this` is consumed by the operation.
         */
        // would this be better as a free func?
        template<class U>
        Result<U, E> map(std::function<U(T &&)> fn) noexcept
        {
            // TODO: remove need to explicitly cast to result in each of the
            //       branches.. i.e. the Result<U,E>() bit.
            switch (tag_) {
                case Tag::OK:
                    return Result<U, E>::Ok(fn(unwrap()));
                    break;
                case Tag::ERR:
                    return Result<U, E>::Err(unwrap_err());
                    break;
                case Tag::INVAL:
                    return Result<U, E>();
                    break;
                default:
                    nel_panic("invalid Result");
                    // std::abort();
            }
        }

        /**
         * Map the result::err to a different result::err.
         *
         * @param fn A fn that takes a E and returns a F
         * @returns if err, result with err value after applying fn to err value.
         * @returns if ok, result with ok of same value.
         *
         * `this` is consumed by the operation.
         */
        // would this be better as a free func?
        template<class F>
        Result<T, F> map_err(std::function<F(E &&)> fn) noexcept
        {
            // TODO: remove need to explicitly cast to result in each of the
            //       branches.. i.e. the Result<U,E>() bit.
            switch (tag_) {
                case Tag::OK:
                    return Result<T, F>::Ok(unwrap());
                    break;
                case Tag::ERR:
                    return Result<T, F>::Err(fn(unwrap_err()));
                    break;
                case Tag::INVAL:
                    return Result<T, F>();
                    break;
                default:
                    // std::abort();
                    nel_panic("invalid Result");
            }
        }

    public:
        // friend std::ostream &operator<<(std::ostream &outs, Result const &val) {
        friend Log &operator<<(Log &outs, Result const &val) noexcept
        {
            switch (val.tag_) {
                case Tag::OK:
                    outs << "Result(Ok(" << *val.ok_ << "))";
                    return outs;
                    break;
                case Tag::ERR:
                    outs << "Result(Err(" << *val.err_ << "))";
                    return outs;
                    break;
                case Tag::INVAL:
                    outs << "Result(Inval)";
                    return outs;
                    break;
                default:
                    outs << "Result(Unknown)";
            }
            return outs;
        }
};

template<typename E>
class Result<void, E>
{
    public:
    private:
        // Tagged enum thing.
        // Similar to std::variant but without the exception throwing behaviour.
        // Maybe make into a nel::Variant ?
        enum class Tag {
            INVAL = 0,
            OK,
            ERR
        } tag_;

        template<enum Tag>
        struct Phantom {
        };

        union {
                Element<E> err_;
        };

    public:
        constexpr ~Result(void) noexcept
        {
            switch (tag_) {
                case Tag::OK:
                    break;

                case Tag::ERR:
                    err_.~Element<E>();
                    break;

                case Tag::INVAL:
                    break;

                // No default as I want compile to fail if an enum case handler
                // isn't present.
                default:
                    // But, want to abort/panic if a unhandled case is encountered
                    // at runtime, much how a default hander would work if it was
                    // present.
                    std::abort();
                    // nel_panic("invalid Result");
            }
        }

        constexpr Result(Result &&o) noexcept
        {
            tag_ = o.tag_;
            o.tag_ = Tag::INVAL;
            switch (tag_) {
                case Tag::OK:
                    break;
                case Tag::ERR:
                    new (&err_) Element<E>(std::move(o.err_));
                    break;
                case Tag::INVAL:
                    break;
                default:
                    nel_panic("invalid Result");
            }
        }

        constexpr Result &operator=(Result &&o) noexcept
        {
            if (this != &o) {
                tag_ = o.tag_;
                o.tag_ = Tag::INVAL;
                switch (tag_) {
                    case Tag::OK:
                        break;
                    case Tag::ERR:
                        err_ = std::move(o.err_);
                        break;
                    case Tag::INVAL:
                        break;
                    default:
                        nel_panic("invalid Result");
                }
            }
            return *this;
        }

        // not using copy semantics, results are only moveable..
        Result(Result const &) = delete;
        Result &operator=(Result const &) const = delete;

        // Don't really want default ctor, but move semantics brought in a
        // inval state, which can be used for it.
        constexpr Result(void) noexcept
            : tag_(Tag::INVAL)
        {
        }

    private:
        constexpr Result(Phantom<Tag::OK> const) noexcept
            : tag_(Tag::OK)
        {
        }

        constexpr Result(Phantom<Tag::ERR> const, E &&v) noexcept
            : tag_(Tag::ERR)
            , err_(std::forward<E>(v))
        {
        }

        template<typename... Args>
        constexpr Result(Phantom<Tag::ERR> const, Args &&...args) noexcept
            : tag_(Tag::ERR)
            , err_(std::forward<Args>(args)...)
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
        constexpr static Result Ok(void) noexcept
        {
            return Result(Phantom<Tag::OK>());
        }

        constexpr static Result Err(E &&v) noexcept
        {
            return Result(Phantom<Tag::ERR>(), std::forward<E>(v));
        }

        /**
         * Create a result set to Err value, using the values to construct the err value.
         *
         * @param args The values to use to construct an Err value inplace.
         *
         * @returns a Result 'wrapping' the values given to construct an Err value inplace.
         */
        template<typename... Args>
        constexpr static Result Err(Args &&...args) noexcept
        {
            return Result(Phantom<Tag::ERR>(), std::forward<Args>(args)...);
        }

    public:
        // Comparision operators
        // Implemented in terms of the operator on the type,
        // as some types may have more optimal implementations of thatoperationthan the
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
            if (this == &o) { return true; }
            if (tag_ == o.tag_) {
                switch (tag_) {
                    case Tag::OK:
                        return true;
                        break;
                    case Tag::ERR:
                        return err_ == o.err_;
                        break;
                    case Tag::INVAL:
                        return true;
                        break;
                    default:
                        nel_panic("invalid Result");
                        // std::abort();
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
        constexpr bool operator!=(Result const &o) const noexcept
        {
            if (this == &o) { return false; }
            if (tag_ == o.tag_) {
                switch (tag_) {
                    case Tag::OK:
                        return false;
                        break;
                    case Tag::ERR:
                        return err_ != o.err_;
                        break;
                    case Tag::INVAL:
                        return false;
                        break;
                    default:
                        nel_panic("invalid Result");
                        // std::abort();
                }
            }
            return true;
        }

    public:
        /**
         * Determine if the container contains a Ok.
         *
         * @returns true if container contains a Ok, false otherwise.
         *
         * `this` is not consumed by this op.
         */
        constexpr bool is_ok(void) const noexcept
        {
            return tag_ == Tag::OK;
        }

        /**
         * Determine if the container contains a Err.
         *
         * @returns true if container contains a Err, false otherwise.
         *
         * `this` is not consumed by this op.
         */
        constexpr bool is_err(void) const noexcept
        {
            return tag_ == Tag::ERR;
        }

    public:
        /**
         * Return an optional containing the ok value or none.
         *
         * @returns if this is a Ok, returns Optional containing a Some with the ok value.
         * @returns if this is not a Ok, returns Optional containing a None.
         *
         * `this` is consumed and invalidated after.
         */
        Optional<void> ok(void) noexcept
        {
            bool const is_ok = this->is_ok();
            tag_ = Tag::INVAL;
            // TODO: remove need to explicitly cast to Optional in each of the
            //       branches.. i.e. the Optional<T>:: bit.
            return is_ok ? Some() : None;
        }

        /**
         * Return an optional containing the err value or none.
         *
         * @returns if this is a Err, returns Optional containing a Some with the err value.
         * @returns if this is not a Err, returns Optional containing a None.
         *
         * `this` is consumed and invalidated after.
         */
        Optional<E> err(void) noexcept
        {
            bool const is_err = this->is_err();
            tag_ = Tag::INVAL;
            // TODO: remove need to explicitly cast to Optional in each of the
            //       branches.. i.e. the Optional<E>:: bit.
            return is_err ? Some(err_.unwrap()) : None;
        }

        /**
         * Extract and return the contained value if an Ok, consuming the result.
         *
         * @return if Ok, value wrapped/contained.
         * @return if not Ok, calls abort, there is no return.
         *
         * `this` is consumed by the operation.
         */
        void unwrap(void) noexcept
        {
            nel_panic_if_not(is_ok(), "not an ok");
            tag_ = Tag::INVAL;
        }

        /**
         * Extract and return the contained value if an Err, consuming the result.
         *
         * @return if Err, value wrapped/contained.
         * @return if not Err, calls abort, there is no return.
         *
         * `this` is consumed by the operation.
         */
        E unwrap_err(void) noexcept
        {
            nel_panic_if_not(is_err(), "not an err");
            tag_ = Tag::INVAL;
            return err_.unwrap();
        }

        /**
         * Extract and return the contained value if an Ok, consuming the result.
         *
         * @param o The value to return if this is not an Ok.
         *
         * @return if Ok, value wrapped/contained.
         * @return if not Ok, consume and return `o`.
         *
         * `this` is consumed by the operation.
         * `o` is consumed by the operation.
         */
        void unwrap_or(void) noexcept
        {
            tag_ = Tag::INVAL;
        }

        E unwrap_err_or(E &&v) noexcept
        {
            bool const is_err = this->is_err();
            tag_ = Tag::INVAL;
            return is_err ? err_.unwrap() : std::forward<E>(v);
        }

        /**
         * Extract and return the contained value if an Err, consuming the result.
         *
         * @param args The value to construct an Err if this is not an Err.
         *
         * @return if Err, value wrapped/contained.
         * @return if not Err, consume and return an Err value constructed by args.
         *
         * `this` is consumed by the operation.
         * `args` are consumed by the operation.
         */
        template<typename... Args>
        E unwrap_err_or(Args &&...args) noexcept
        {
            bool const is_err = this->is_err();
            tag_ = Tag::INVAL;
            return is_err ? err_.unwrap() : E(std::forward<Args>(args)...);
        }

        /**
         * Map the result::ok to a different result::ok.
         *
         * @param fn A fn that takes a T and returns a U
         * @returns if ok, result with ok value after applying fn to ok value.
         * @returns if err, result with err of same value.
         *
         * `this` is consumed by the operation.
         */
        // would this be better as a free func?
        template<class U>
        Result<U, E> map(std::function<U(void)> fn) noexcept
        {
            // TODO: remove need to explicitly cast to result in each of the
            //       branches.. i.e. the Result<U,E>() bit.
            switch (tag_) {
                case Tag::OK:
                    return Result<U, E>::Ok(fn());
                    break;
                case Tag::ERR:
                    return Result<U, E>::Err(unwrap_err());
                    break;
                case Tag::INVAL:
                    return Result<U, E>();
                    break;
                default:
                    nel_panic("invalid Result");
                    // std::abort();
            }
        }

        /**
         * Map the result::err to a different result::err.
         *
         * @param fn A fn that takes a E and returns a F
         * @returns if err, result with err value after applying fn to err value.
         * @returns if ok, result with ok of same value.
         *
         * `this` is consumed by the operation.
         */
        // would this be better as a free func?
        template<class F>
        Result<void, F> map_err(std::function<F(E &&)> fn) noexcept
        {
            // TODO: remove need to explicitly cast to result in each of the
            //       branches.. i.e. the Result<U,E>() bit.
            switch (tag_) {
                case Tag::OK:
                    return Result<void, F>::Ok();
                    break;
                case Tag::ERR:
                    return Result<void, F>::Err(fn(unwrap_err()));
                    break;
                case Tag::INVAL:
                    return Result<void, F>();
                    break;
                default:
                    // std::abort();
                    nel_panic("invalid Result");
            }
        }

    public:
        // friend std::ostream &operator<<(std::ostream &outs, Result const &val) {
        friend Log &operator<<(Log &outs, Result const &val) noexcept
        {
            switch (val.tag_) {
                case Tag::OK:
                    outs << "Result(Ok())";
                    return outs;
                    break;
                case Tag::ERR:
                    outs << "Result(Err(" << *val.err_ << "))";
                    return outs;
                    break;
                case Tag::INVAL:
                    outs << "Result(Inval)";
                    return outs;
                    break;
                default:
                    outs << "Result(Unknown)";
            }
            return outs;
        }
};

// Calc v as a temp to keep f single eval-ed
#define NEL_RESULT_TRY(f) \
    __extension__({ \
        auto v = std::move(f); \
        if (v.is_err()) { return v; } \
        v.unwrap(); \
    })

} // namespace nel

#endif // NEL_RESULT_HH
