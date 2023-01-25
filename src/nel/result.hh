// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
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
        constexpr ~Result(void)
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

        constexpr Result(Result &&o)
        {
            tag_ = o.tag_;
            o.tag_ = Tag::INVAL;
            switch (tag_) {
                case Tag::OK:
                    new (&ok_) Element<T>(move(o.ok_));
                    break;

                case Tag::ERR:
                    new (&err_) Element<E>(move(o.err_));
                    break;

                case Tag::INVAL:
                    break;

                default:
                    // std::cerr << "invalid Result: tag=" << this->tag_ << std::endl;
                    // std::abort();
                    nel_panic("invalid Result");
            }
        }

        Result &operator=(Result &&o)
        {
            if (this != &o) {
                tag_ = o.tag_;
                o.tag_ = Tag::INVAL;
                switch (tag_) {
                    case Tag::OK:
                        ok_ = move(o.ok_);
                        break;

                    case Tag::ERR:
                        err_ = move(o.err_);
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
        constexpr Result(void)
            : tag_(Tag::INVAL)
        {
        }

    private:
        constexpr Result(Phantom<Tag::OK> const, T &&v)
            : tag_(Tag::OK)
            , ok_(forward<T>(v))
        {
        }

        template<typename... Args>
        constexpr Result(Phantom<Tag::OK> const, Args &&...args)
            : tag_(Tag::OK)
            , ok_(forward<Args>(args)...)
        {
        }

        constexpr Result(Phantom<Tag::ERR> const, E &&v)
            : tag_(Tag::ERR)
            , err_(forward<E>(v))
        {
        }

        template<typename... Args>
        constexpr Result(Phantom<Tag::ERR> const, Args &&...args)
            : tag_(Tag::ERR)
            , err_(forward<Args>(args)...)
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
        constexpr static Result Ok(T &&val)
        {
            return Result(Phantom<Tag::OK>(), forward<T>(val));
        }

        /**
         * Create a result set to Ok value, using the values to construct the ok value.
         *
         * @param args The values to use to construct an Ok value inplace.
         *
         * @returns a Result 'wrapping' the values given to construct an ok value inplace.
         */
        template<typename... Args>
        constexpr static Result Ok(Args &&...args)
        {
            return Result(Phantom<Tag::OK>(), forward<Args>(args)...);
        }

        /**
         * Create a result set to Err value, using the value given.
         *
         * @param val The value to move into the result as an err value.
         *
         * @returns a Result 'wrapping' the value given as an err value.
         */
        constexpr static Result Err(E &&val)
        {
            return Result(Phantom<Tag::ERR>(), forward<E>(val));
        }

        /**
         * Create a result set to Err value, using the values to construct the err value.
         *
         * @param args The values to use to construct an Err value inplace.
         *
         * @returns a Result 'wrapping' the values given to construct an Err value inplace.
         */
        template<typename... Args>
        constexpr static Result Err(Args &&...args)
        {
            return Result(Phantom<Tag::ERR>(), forward<Args>(args)...);
        }

    private:
        //  don't use std::function, it's bloatware
        // template<typename V>
        // constexpr V match(Tag tag, std::function<V(void)> &&on_ok, std::function<V(void)>
        // &&on_err) const  {
        template<typename V, typename Fn1, typename Fn2>
        constexpr V match(Tag tag, Fn1 &&on_ok, Fn2 &&on_err) const
        {
            switch (tag) {
                case Tag::OK:
                    return on_ok();
                case Tag::ERR:
                    return on_err();
                case Tag::INVAL:
                // invalids are not values that should occur
                // if they do it's a use-after-move-from op so must panic.
                // or a use-before-initialised so again must panic.
                default:
                    nel_panic("invalid Result");
                    // std::abort();
            }
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
        constexpr bool operator==(Result const &o) const
        {
            if (this == &o) { return true; }
            if (tag_ == o.tag_) {
                return match<bool>(
                    tag_,
                    [this, &o](void) -> bool { return ok_ == o.ok_; },
                    [this, &o](void) -> bool { return err_ == o.err_; });
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
        constexpr bool operator!=(Result const &o) const
        {
            if (this == &o) { return false; }
            if (tag_ == o.tag_) {
                return match<bool>(
                    tag_,
                    [this, &o](void) -> bool { return ok_ != o.ok_; },
                    [this, &o](void) -> bool { return err_ != o.err_; });
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
        constexpr bool is_ok(void) const
        {
            return match<bool>(
                tag_,
                [this](void) -> bool { return true; },
                [this](void) -> bool { return false; });
        }

        /**
         * Determine if the container contains a Err.
         *
         * @returns true if container contains a Err, false otherwise.
         *
         * `this` is not consumed by this op.
         */
        constexpr bool is_err(void) const
        {
            return match<bool>(
                tag_,
                [this](void) -> bool { return false; },
                [this](void) -> bool { return true; });
        }

    private:
        // template<typename V>
        // V consume(std::function<V(void)> &&on_ok, std::function<V(void)> &&on_err) {
        template<typename V, typename Fn1, typename Fn2>
        V consume(Fn1 &&on_ok, Fn2 &&on_err)
        {
            auto tag = tag_;
            tag_ = Tag::INVAL;
            return match<V>(tag, forward<Fn1>(on_ok), forward<Fn2>(on_err));
        }

    public:
        /**
         * Return an optional containing the ok value or none.
         *
         * @returns if this is a Ok, returns Optional containing a Some with the ok value.
         * @returns if this is an Err, returns Optional containing a None.
         * Panics if invalid.
         * Panics if unknown.
         *
         * `this` is consumed and invalidated after.
         */
        Optional<T> ok(void)
        {
            return consume<Optional<T>>([this](void) -> Optional<T> { return Some(ok_.unwrap()); },
                                        [](void) -> Optional<T> { return None; });
        }

        /**
         * Return an optional containing the err value or none.
         *
         * @returns if this is a Err, returns Optional containing a Some with the err value.
         * @returns if this is not a Err, returns Optional containing a None.
         *
         * `this` is consumed and invalidated after.
         */
        Optional<E> err(void)
        {
            return consume<Optional<E>>([](void) -> Optional<E> { return None; },
                                        [this](void) -> Optional<E> {
                                            return Some(err_.unwrap());
                                        });
        }

        /**
         * Extract and return the contained value if an Ok, consuming the result.
         *
         * @return if Ok, value wrapped/contained.
         * @return if not Ok, calls abort, there is no return.
         *
         * `this` is consumed by the operation.
         */
        T unwrap(void)
        {
            return consume<T>([this](void) -> T { return ok_.unwrap(); },
                              [](void) -> T { nel_panic("not an OK"); });
        }

        /**
         * Extract and return the contained value if an Err, consuming `this`.
         *
         * @return if Err, value wrapped/contained.
         * @return if not Err, calls abort, there is no return.
         *
         * this is consumed by the operation.
         */
        E unwrap_err(void)
        {
            return consume<E>([](void) -> E { nel_panic("not an err"); },
                              [this](void) -> E { return err_.unwrap(); });
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
        T unwrap_or(T &&v)
        {
            return consume<T>([this](void) -> T { return ok_.unwrap(); },
                              [&v](void) -> T { return forward<T>(v); });
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
        T unwrap_or(Args &&...args)
        {
            return consume<T>([this](void) -> T { return ok_.unwrap(); },
                              [&args...](void) -> T { return T(forward<Args>(args)...); });
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
        E unwrap_err_or(E &&v)
        {
            return consume<E>([&v](void) -> E { return forward<E>(v); },
                              [this](void) -> E { return err_.unwrap(); });
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
        E unwrap_err_or(Args &&...args)
        {
            return consume<E>([&args...](void) -> E { return E(forward<Args>(args)...); },
                              [this](void) -> E { return err_.unwrap(); });
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
        // template<class U>
        // Result<U, E> map(std::function<U(T &&)> fn)
        template<class U, typename Fn>
        Result<U, E> map(Fn &&fn)
        {
            return consume<
                Result<U, E>>([this, &fn](void)
                                  -> Result<U, E> { return Result<U, E>::Ok(fn(ok_.unwrap())); },
                              [this](void) -> Result<U, E> {
                                  return Result<U, E>::Err(err_.unwrap());
                              });
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
        // template<class F>
        // Result<T, F> map_err(std::function<F(E &&)> fn)
        template<class F, typename Fn>
        Result<T, F> map_err(Fn &&fn)
        {
            // TODO: remove need to explicitly cast to result in each of the
            //       branches.. i.e. the Result<U,E>() bit, should be Ok(..) or Err(..)
            return consume<
                Result<T, F>>([this](
                                  void) -> Result<T, F> { return Result<T, F>::Ok(ok_.unwrap()); },
                              [this, &fn](void) -> Result<T, F> {
                                  return Result<T, F>::Err(fn(err_.unwrap()));
                              });
        }

    public:
        // friend std::ostream &operator<<(std::ostream &outs, Result const &val) {
        friend Log &operator<<(Log &outs, Result const &val)
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
        constexpr ~Result(void)
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
                    nel::abort();
                    // nel_panic("invalid Result");
            }
        }

        constexpr Result(Result &&o)
        {
            tag_ = o.tag_;
            o.tag_ = Tag::INVAL;
            switch (tag_) {
                case Tag::OK:
                    break;
                case Tag::ERR:
                    new (&err_) Element<E>(move(o.err_));
                    break;
                case Tag::INVAL:
                    break;
                default:
                    nel_panic("invalid Result");
            }
        }

        constexpr Result &operator=(Result &&o)
        {
            if (this != &o) {
                tag_ = o.tag_;
                o.tag_ = Tag::INVAL;
                switch (tag_) {
                    case Tag::OK:
                        break;
                    case Tag::ERR:
                        err_ = move(o.err_);
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
        constexpr Result(void)
            : tag_(Tag::INVAL)
        {
        }

    private:
        constexpr Result(Phantom<Tag::OK> const)
            : tag_(Tag::OK)
        {
        }

        constexpr Result(Phantom<Tag::ERR> const, E &&v)
            : tag_(Tag::ERR)
            , err_(forward<E>(v))
        {
        }

        template<typename... Args>
        constexpr Result(Phantom<Tag::ERR> const, Args &&...args)
            : tag_(Tag::ERR)
            , err_(forward<Args>(args)...)
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
        constexpr static Result Ok(void)
        {
            return Result(Phantom<Tag::OK>());
        }

        constexpr static Result Err(E &&v)
        {
            return Result(Phantom<Tag::ERR>(), forward<E>(v));
        }

        /**
         * Create a result set to Err value, using the values to construct the err value.
         *
         * @param args The values to use to construct an Err value inplace.
         *
         * @returns a Result 'wrapping' the values given to construct an Err value inplace.
         */
        template<typename... Args>
        constexpr static Result Err(Args &&...args)
        {
            return Result(Phantom<Tag::ERR>(), forward<Args>(args)...);
        }

    private:
        // template<typename V>
        // constexpr V
        // match(Tag tag, std::function<V(void)> on_ok, std::function<V(void)> on_err) const
        //
        template<typename V, typename Fn1, typename Fn2>
        constexpr V match(Tag tag, Fn1 &&on_ok, Fn2 &&on_err) const
        {
            switch (tag) {
                case Tag::OK:
                    return on_ok();
                case Tag::ERR:
                    return on_err();
                case Tag::INVAL:
                // invalids are not values that should occur
                // if they do it's a use-after-move-from op so must panic.
                // or a use-before-initialised so again must panic.
                default:
                    nel_panic("invalid Result");
                    // std::abort();
            }
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
        constexpr bool operator==(Result const &o) const
        {
            if (this == &o) { return true; }
            if (tag_ == o.tag_) {
                return match<bool>(
                    tag_,
                    [](void) -> bool { return true; },
                    [this, &o](void) -> bool { return err_ == o.err_; });
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
        constexpr bool operator!=(Result const &o) const
        {
            if (this == &o) { return false; }
            if (tag_ == o.tag_) {
                return match<bool>(
                    tag_,
                    [](void) -> bool { return false; },
                    [this, &o](void) -> bool { return err_ != o.err_; });
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
        constexpr bool is_ok(void) const
        {
            return match<bool>(
                tag_,
                [](void) -> bool { return true; },
                [](void) -> bool { return false; });
        }

        /**
         * Determine if the container contains a Err.
         *
         * @returns true if container contains a Err, false otherwise.
         *
         * `this` is not consumed by this op.
         */
        constexpr bool is_err(void) const
        {
            return match<bool>(
                tag_,
                [](void) -> bool { return false; },
                [](void) -> bool { return true; });
        }

    private:
        // template<typename V>
        // V consume(std::function<V(void)> on_ok, std::function<V(void)> on_err)
        template<typename V, typename Fn1, typename Fn2>
        V consume(Fn1 &&on_ok, Fn2 &&on_err)
        {
            auto tag = tag_;
            tag_ = Tag::INVAL;
            return match<V>(tag, forward<Fn1>(on_ok), forward<Fn2>(on_err));
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
        Optional<void> ok(void)
        {
            return consume<Optional<void>>([](void) -> Optional<void> { return Some(); },
                                           [](void) -> Optional<void> { return None; });
        }

        /**
         * Return an optional containing the err value or none.
         *
         * @returns if this is a Err, returns Optional containing a Some with the err value.
         * @returns if this is not a Err, returns Optional containing a None.
         *
         * `this` is consumed and invalidated after.
         */
        Optional<E> err(void)
        {
            return consume<Optional<E>>([](void) -> Optional<E> { return None; },
                                        [this](void) -> Optional<E> {
                                            return Some(err_.unwrap());
                                        });
        }

        /**
         * Extract and return the contained value if an Ok, consuming the result.
         *
         * @return if Ok, value wrapped/contained.
         * @return if not Ok, calls abort, there is no return.
         *
         * `this` is consumed by the operation.
         */
        void unwrap(void)
        {
            return consume<void>([](void) {}, [](void) { nel_panic("not an ok"); });
        }

        /**
         * Extract and return the contained value if an Err, consuming the result.
         *
         * @return if Err, value wrapped/contained.
         * @return if not Err, calls abort, there is no return.
         *
         * `this` is consumed by the operation.
         */
        E unwrap_err(void)
        {
            return consume<E>([](void) -> E { nel_panic("not an err"); },
                              [this](void) -> E { return err_.unwrap(); });
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
        void unwrap_or(void)
        {
            return consume<void>([](void) {}, [](void) {});
        }

        E unwrap_err_or(E &&v)
        {
            return consume<E>([&v](void) -> E { forward<E>(v); },
                              [this](void) -> E { return err_.unwrap(); });
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
        E unwrap_err_or(Args &&...args)
        {
            return consume<E>([&args...](void) -> E { return E(forward<Args>(args)...); },
                              [this](void) -> E { return err_.unwrap(); });
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
        // template<class U>
        // Result<U, E> map(std::function<U(void)> fn)
        template<typename U, typename Fn>
        Result<U, E> map(Fn &&fn)
        {
            // TODO: remove need to explicitly cast to result in each of the
            //       branches.. i.e. the Result<U,E>() bit, should be Ok(..) or Err(..)
            return consume<Result<U, E>>([&fn](void)
                                             -> Result<U, E> { return Result<U, E>::Ok(fn()); },
                                         [this](void) -> Result<U, E> {
                                             return Result<U, E>::Err(err_.unwrap());
                                         });
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
        // template<class F>
        // Result<void, F> map_err(std::function<F(E &&)> fn)
        template<typename F, typename Fn>
        Result<void, F> map_err(Fn &&fn)
        {
            // TODO: remove need to explicitly cast to result in each of the
            //       branches.. i.e. the Result<U,E>() bit, should be Ok(..), Err(..)
            return consume<Result<void, F>>([](void) -> E { return Result<void, F>::Ok(); },
                                            [this, &fn](void) -> E {
                                                return Result<void, F>::Err(fn(err_.unwrap()));
                                            });
        }

    public:
        friend Log &operator<<(Log &outs, Result const &val)
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
        auto v = move(f); \
        if (v.is_err()) { return v; } \
        v.unwrap(); \
    })

} // namespace nel

#endif // NEL_RESULT_HH
