// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(NEL_RESULT_HH)
#    define NEL_RESULT_HH

namespace nel
{

template<typename T, typename E>
class Result;

} // namespace nel

#    include <nel/optional.hh>
#    include <nel/element.hh>
#    include <nel/log.hh>
#    include <nel/panic.hh>
#    include <nel/memory.hh> // move,forward
#    include <nel/new.hh> // new (p) T()

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
        struct Phantom
        {
        };

        union {
                Element<T> ok_;
                Element<E> err_;
        };

    public:
        constexpr ~Result(void)
        {
            switch (tag_) { // result::dtor
                case Tag::OK:
                    ok_.~Element<T>();
                    break;

                case Tag::ERR:
                    err_.~Element<E>();
                    break;

                case Tag::INVAL:
                    // dtor + move:
                    // if invalid, don't panic.. just means value was consumed earlier.
                    break;

                default:
                    // for dtor + moves (but not usage) at runtime:
                    // does it matter it's not one of the 'special' values?
                    // yes, since it may have contained one or the others..
                    // if memory hardware is assumed to be incorruptable then can ignore ? (buffer
                    // overruns?) but might not want to for buffer overrun issues. if there is a
                    // memory corruption, then should have been detected by memory hardware (Ecc
                    // mem) if not using eec/ecc mem then that's the prob. But, want to abort/panic
                    // if a unhandled case is encountered at runtime, much how a default hander
                    // would work if it was present.
                    nel_panic("invalid Result");
                    break;
            }
        }

        constexpr Result(Result &&o)
        {
            tag_ = o.tag_;
            o.tag_ = Tag::INVAL;

            switch (tag_) { // result::move-ctor
                case Tag::OK:
                    new (&ok_) Element(forward<Element<T>>(o.ok_));
                    break;

                case Tag::ERR:
                    new (&err_) Element(forward<Element<E>>(o.err_));
                    break;

                case Tag::INVAL:
                    // dtor + move:
                    // if invalid, don't panic.. just means value was consumed earlier.
                    break;

                default:
                    // for dtor + moves (but not usage) at runtime:
                    // does it matter it's not one of the 'special' values?
                    // yes, since it may have contained one or the others..
                    // if memory hardware is assumed to be incorruptable then can ignore ? (buffer
                    // overruns?) but might not want to for buffer overrun issues. if there is a
                    // memory corruption, then should have been detected by memory hardware (Ecc
                    // mem) if not using eec/ecc mem then that's the prob.
                    nel_panic("invalid Result");
                    break;
            }
        }

        constexpr Result &operator=(Result &&o)
        {
            if (this != &o) {
                tag_ = o.tag_;
                o.tag_ = Tag::INVAL;

                switch (tag_) { // result-move-ass
                    case Tag::OK:
                        ok_ = move(o.ok_);
                        break;

                    case Tag::ERR:
                        err_ = move(o.err_);
                        break;

                    case Tag::INVAL:
                        // dtor + move:
                        // if invalid, don't panic.. just means value was consumed earlier.
                        break;

                    default:
                        // for dtor + moves (but not usage) at runtime:
                        // does it matter it's not one of the 'special' values?
                        // yes, since it may have contained one or the others..
                        // if memory hardware is assumed to be incorruptable then can ignore ?
                        // (buffer overruns?) but might not want to for buffer overrun issues. if
                        // there is a memory corruption, then should have been detected by memory
                        // hardware (Ecc mem) if not using eec/ecc mem then that's the prob.
                        nel_panic("invalid Result");
                        break;
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

#    if 1
        // seems that the T&& variant is used and gets optimised to inplace create.
        // removing this prevents moving const objects (or implicit coping).
        // but if large >512bytes then does not get optimised out)
        template<typename... Args>
        constexpr Result(Phantom<Tag::OK> const, Args &&...args)
            : tag_(Tag::OK)
            , ok_(forward<Args>(args)...)
        {
        }
#    endif

        constexpr Result(Phantom<Tag::ERR> const, E &&v)
            : tag_(Tag::ERR)
            , err_(forward<E>(v))
        {
        }

#    if 1
        // seems that the T&& variant is used and gets optimised to inplace create.
        // removing this prevents moving const objects (or implicit coping).
        template<typename... Args>
        constexpr Result(Phantom<Tag::ERR> const, Args &&...args)
            : tag_(Tag::ERR)
            , err_(forward<Args>(args)...)
        {
        }
#    endif

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

#    if 1
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
#    endif

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

#    if 1
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
#    endif

    private:
        //  don't use std::function, it's bloatware
        // template<typename V>
        // constexpr V match(Tag tag, std::function<V(void)> &&on_ok, std::function<V(void)>
        // &&on_err) const  {
        template<typename V, typename Fn1, typename Fn2>
        constexpr V match(Fn1 &&on_ok, Fn2 &&on_err) const
        {
            // this match is non-consuming, for internal use only.
            switch (tag_) { // result-match
                case Tag::OK:
                    return on_ok(*ok_);
                case Tag::ERR:
                    return on_err(*err_);
                case Tag::INVAL:
                    // invalids are not values that should occur
                    // if they do it's a use-after-move-from op so must panic.
                    // or a use-before-initialised so again must panic.
                    nel_panic("invalid Result");
                    break;
                default:
                    nel_panic("invalid Result");
                    break;
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
                return match<bool>([&o](T const &ok) -> bool { return ok == *o.ok_; },
                                   [&o](E const &err) -> bool { return err == *o.err_; });
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
                return match<bool>([&o](T const &ok) -> bool { return ok != *o.ok_; },
                                   [&o](E const &err) -> bool { return err != *o.err_; });
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
            // min-size: gets optimised + inlined at call site.
            // release: gets optimised away..
            return match<bool>([](T const &) -> bool { return true; },
                               [](E const &) -> bool { return false; });
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
            return match<bool>([](T const &) -> bool { return false; },
                               [](E const &) -> bool { return true; });
        }

    public:
        template<typename V, typename Fn1, typename Fn2>
        constexpr V consume(Fn1 &&on_ok, Fn2 &&on_err)
        {
            // a consuming matcher..
            // that moves the value to the closures..
            auto tag = tag_;
            tag_ = Tag::INVAL;
            switch (tag) { // result-consume
                case Tag::OK:
                    return on_ok(forward<T>(*ok_));
                case Tag::ERR:
                    return on_err(forward<E>(*err_));
                case Tag::INVAL:
                    // invalids are not values that should occur
                    // if they do it's a use-after-move-from op so must panic.
                    // or a use-before-initialised so again must panic.
                    nel_panic("invalid Result");
                    break;
                default:
                    nel_panic("invalid Result");
                    break;
            }
        }

        template<typename Fn1, typename Fn2>
        constexpr void consumev(Fn1 &&on_ok, Fn2 &&on_err)
        {
            auto tag = tag_;
            tag_ = Tag::INVAL;
            switch (tag) { // result-consumev
                case Tag::OK:
                    on_ok(forward<T>(*ok_));
                    break;
                case Tag::ERR:
                    on_err(forward<E>(*err_));
                    break;
                case Tag::INVAL:
                    // invalids are not values that should occur
                    // if they do it's a use-after-move-from op so must panic.
                    // or a use-before-initialised so again must panic.
                    // fall through to default handler
                    nel_panic("invalid Result");
                    break;
                default:
                    nel_panic("invalid Result");
                    break;
            }
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
        constexpr Optional<T> ok(void)
        {
            return consume<Optional<T>>([](T &&ok) -> Optional<T> { return Some(forward<T>(ok)); },
                                        [](E &&) -> Optional<T> { return None; });
        }

        /**
         * Return an optional containing the err value or none.
         *
         * @returns if this is a Err, returns Optional containing a Some with the err value.
         * @returns if this is not a Err, returns Optional containing a None.
         *
         * `this` is consumed and invalidated after.
         */
        constexpr Optional<E> err(void)
        {
            return consume<Optional<E>>([](T &&) -> Optional<E> { return None; },
                                        [](E &&err) -> Optional<E> {
                                            return Some(forward<E>(err));
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
        constexpr T unwrap(void)
        {
            return consume<T>([](T &&ok) -> T { return forward<T>(ok); },
                              [](E &&) -> T { nel_panic("not an OK"); });
        }

        /**
         * Extract and return the contained value if an Err, consuming `this`.
         *
         * @return if Err, value wrapped/contained.
         * @return if not Err, calls abort, there is no return.
         *
         * this is consumed by the operation.
         */
        constexpr E unwrap_err(void)
        {
            return consume<E>([](T &&) -> E { nel_panic("not an err"); },
                              [](E &&err) -> E { return err; });
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
        constexpr T unwrap_or(T &&v)
        {
            return consume<T>([](T &&ok) -> T { return ok; },
                              [&v](E &&) -> T { return forward<T>(v); });
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
        constexpr T unwrap_or(Args &&...args)
        {
            return consume<T>([](T &&ok) -> T { return ok; },
                              [&args...](E &&) -> T { return T(forward<Args>(args)...); });
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
        constexpr E unwrap_err_or(E &&v)
        {
            return consume<E>([&v](T &&) -> E { return forward<E>(v); },
                              [](E &&err) -> E { return err; });
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
        constexpr E unwrap_err_or(Args &&...args)
        {
            return consume<E>([&args...](T &&) -> E { return E(forward<Args>(args)...); },
                              [](E &&err) -> E { return err; });
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
        constexpr Result<U, E> map(Fn &&fn)
        {
            typedef Result<U, E> ReturnType;
            return consume<ReturnType>(
                [&fn](T &&ok) -> ReturnType {
                    return ReturnType::Ok(forward<U>(fn(forward<T>(ok))));
                },
                [](E &&err) -> ReturnType { return ReturnType::Err(forward<E>(err)); });
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
        constexpr Result<T, F> map_err(Fn &&fn)
        {
            // TODO: remove need to explicitly cast to result in each of the
            //       branches.. i.e. the Result<U,E>() bit, should be Ok(..) or Err(..)
            typedef Result<T, F> ReturnType;
            return consume<ReturnType>([](T &&ok)
                                           -> ReturnType { return ReturnType::Ok(forward<T>(ok)); },
                                       [&fn](E &&err) -> ReturnType {
                                           return ReturnType::Err(forward<F>(fn(forward<E>(err))));
                                       });
        }

        /**
         * Map the result::ok to a different result::ok.
         *
         * If self is ok, calls fn and returns that instead.
         * If self is err, returns self.
         *
         * @param fn A fn that takes a T and returns a Result<U,E>
         * @returns if ok, result with ok value after applying fn to ok value.
         * @returns if err, result with err of same value.
         *
         * `this` is consumed by the operation.
         */
        template<typename U, typename Fn>
        // Fn: Result<U,E> fn(T&&)
        constexpr Result<U, E> and_then(Fn &&fn)
        {
            // TODO: remove need to explicitly cast to result in each of the
            //       branches.. i.e. the Result<U,E>() bit, should be Ok(..) or Err(..)
            typedef Result<U, E> ReturnType;
            return consume<ReturnType>([&fn](T &&ok) -> ReturnType { return fn(forward<T>(ok)); },
                                       // TODO: check that this is a nop..
                                       [](E &&err) -> ReturnType {
                                           return ReturnType::Err(forward<E>(err));
                                       });
        }

    public:
        friend Log &operator<<(Log &outs, Result const &val)
        {
            switch (val.tag_) { // result-dbgfmt
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
        struct Phantom
        {
        };

        union {
                Element<E> err_;
        };

    public:
        constexpr ~Result(void)
        {
            switch (tag_) { // result<void>-dtor
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

                    // for dtor + moves (but not usage) at runtime:
                    // does it matter it's not one of the 'special' values?
                    // yes, since it may have contained one or the others..
                    // if memory hardware is assumed to be incorruptable then can ignore ? (buffer
                    // overruns?) but might not want to for buffer overrun issues. if there is a
                    // memory corruption, then should have been detected by memory hardware (Ecc
                    // mem) if not using eec/ecc mem then that's the prob. But, want to abort/panic
                    // if a unhandled case is encountered at runtime, much how a default hander
                    // would work if it was present.
                    nel_panic("invalid Result");
                    break;
            }
        }

        constexpr Result(Result &&o)
        {
            tag_ = o.tag_;
            o.tag_ = Tag::INVAL;
            switch (tag_) { // result<void>-move-ctor
                case Tag::OK:
                    break;
                case Tag::ERR:
                    new (&err_) Element(move(o.err_));
                    break;
                case Tag::INVAL:
                    break;

                    // No default as I want compile to fail if an enum case handler
                    // isn't present.

                default:
                    // But, want to abort/panic if a unhandled case is encountered
                    // at runtime, much how a default hander would work if it was
                    // present.

                    // for dtor + moves (but not usage) at runtime:
                    // does it matter it's not one of the 'special' values?
                    // yes, since it may have contained one or the others..
                    // if memory hardware is assumed to be incorruptable then can ignore ? (buffer
                    // overruns?) but might not want to for buffer overrun issues. if there is a
                    // memory corruption, then should have been detected by memory hardware (Ecc
                    // mem) if not using eec/ecc mem then that's the prob.
                    nel_panic("invalid Result");
                    break;
            }
        }

        constexpr Result &operator=(Result &&o)
        {
            if (this != &o) {
                tag_ = o.tag_;
                o.tag_ = Tag::INVAL;
                switch (tag_) { // result<void>-move-assn
                    case Tag::OK:
                        break;
                    case Tag::ERR:
                        err_ = move(o.err_);
                        break;
                    case Tag::INVAL:
                        break;

                        // No default as I want compile to fail if an enum case handler
                        // isn't present.

                    default:
                        // But, want to abort/panic if a unhandled case is encountered
                        // at runtime, much how a default hander would work if it was
                        // present.

                        // for dtor + moves (but not usage) at runtime:
                        // does it matter it's not one of the 'special' values?
                        // yes, since it may have contained one or the others..
                        // if memory hardware is assumed to be incorruptable then can ignore ?
                        // (buffer overruns?) but might not want to for buffer overrun issues. if
                        // there is a memory corruption, then should have been detected by memory
                        // hardware (Ecc mem) if not using eec/ecc mem then that's the prob.
                        nel_panic("invalid Result");
                        break;
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
        constexpr V match(Fn1 &&on_ok, Fn2 &&on_err) const
        {
            switch (tag_) { // result<void>-match
                case Tag::OK:
                    return on_ok();
                case Tag::ERR:
                    return on_err(*err_);
                case Tag::INVAL:
                    // invalids are not values that should occur
                    // if they do it's a use-after-move-from op so must panic.
                    // or a use-before-initialised so again must panic.
                    nel_panic("invalid Result");
                    break;
                default:
                    nel_panic("invalid Result");
                    break;
            }
        }

    public:
        // Comparision operators
        // Implemented in terms of the operator on the type,
        // as some types may have more optimal implementations of that
        // operation than the negation of it's opposite.
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
                return match<bool>([](void) -> bool { return true; },
                                   [&o](E const &err) -> bool { return err == *o.err_; });
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
                return match<bool>([](void) -> bool { return false; },
                                   [&o](E const &err) -> bool { return err != *o.err_; });
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
            return match<bool>([](void) -> bool { return true; },
                               [](E const &) -> bool { return false; });
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
            return match<bool>([](void) -> bool { return false; },
                               [](E const &) -> bool { return true; });
        }

    public:
        template<typename V, typename Fn1, typename Fn2>
        constexpr V consume(Fn1 &&on_ok, Fn2 &&on_err)
        {
            auto tag = tag_;
            tag_ = Tag::INVAL;
            switch (tag) { // result<void>-consume
                case Tag::OK:
                    return on_ok();
                case Tag::ERR:
                    return on_err(nel::forward<E>(*err_));
                case Tag::INVAL:
                    // invalids are not values that should occur
                    // if they do it's a use-after-move-from op so must panic.
                    // or a use-before-initialised so again must panic.
                    nel_panic("invalid Result");
                    break;
                default:
                    nel_panic("invalid Result");
                    break;
            }
        }

        template<typename Fn1, typename Fn2>
        constexpr void consumev(Fn1 &&on_ok, Fn2 &&on_err)
        {
            auto tag = tag_;
            tag_ = Tag::INVAL;
            switch (tag) { // result<void>-consume
                case Tag::OK:
                    on_ok();
                    break;
                case Tag::ERR:
                    on_err(nel::forward<E>(*err_));
                    break;
                case Tag::INVAL:
                    // invalids are not values that should occur
                    // if they do it's a use-after-move-from op so must panic.
                    // or a use-before-initialised so again must panic.
                    nel_panic("invalid Result");
                    break;
                default:
                    nel_panic("invalid Result");
                    break;
            }
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
        constexpr Optional<void> ok(void)
        {
            return consume<Optional<void>>([](void) -> Optional<void> { return Some(); },
                                           [](E &&) -> Optional<void> { return None; });
        }

        /**
         * Return an optional containing the err value or none.
         *
         * @returns if this is a Err, returns Optional containing a Some with the err value.
         * @returns if this is not a Err, returns Optional containing a None.
         *
         * `this` is consumed and invalidated after.
         */
        constexpr Optional<E> err(void)
        {
            return consume<Optional<E>>([](void) -> Optional<E> { return None; },
                                        [](E &&err) -> Optional<E> {
                                            return Some(nel::forward<E>(err));
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
        constexpr void unwrap(void)
        {
            return consumev([](void) {}, [](E &&) { nel_panic("not an ok"); });
        }

        /**
         * Extract and return the contained value if an Err, consuming the result.
         *
         * @return if Err, value wrapped/contained.
         * @return if not Err, calls abort, there is no return.
         *
         * `this` is consumed by the operation.
         */
        constexpr E unwrap_err(void)
        {
            return consume<E>([](void) -> E { nel_panic("not an err"); },
                              [](E &&err) -> E { return err; });
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
        constexpr void unwrap_or(void)
        {
            return consumev([](void) {}, [](E &&) {});
        }

        constexpr E unwrap_err_or(E &&v)
        {
            return consume<E>([&v](void) -> E { forward<E>(v); }, [](E &&err) -> E { return err; });
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
        constexpr E unwrap_err_or(Args &&...args)
        {
            return consume<E>([&args...](void) -> E { return E(forward<Args>(args)...); },
                              [](E &&err) -> E { return err; });
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
        constexpr Result<U, E> map(Fn &&fn)
        {
            // TODO: remove need to explicitly cast to result in each of the
            //       branches.. i.e. the Result<U,E>() bit, should be Ok(..) or Err(..)
            typedef Result<U, E> ReturnType;
            return consume<
                ReturnType>([&fn](void) -> ReturnType { return ReturnType::Ok(forward<U>(fn())); },
                            [](E &&err) -> ReturnType { return ReturnType::Err(forward<E>(err)); });
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
        constexpr Result<void, F> map_err(Fn &&fn)
        {
            // TODO: remove need to explicitly cast to result in each of the
            //       branches.. i.e. the Result<U,E>() bit, should be Ok(..), Err(..)
            typedef Result<void, F> ReturnType;
            return consume<ReturnType>([](void) -> ReturnType { return ReturnType::Ok(); },
                                       [&fn](E &&err) -> ReturnType {
                                           return ReturnType::Err(forward<F>(fn(forward<E>(err))));
                                       });
        }

        template<typename U, typename Fn>
        // Fn: Result<U,E> op(T&&)
        constexpr Result<U, E> and_then(Fn &&fn)
        {
            // TODO: remove need to explicitly cast to result in each of the
            //       branches.. i.e. the Result<U,E>() bit, should be Ok(..) or Err(..)
            typedef Result<U, E> ReturnType;
            return consume<ReturnType>([&fn](void) -> ReturnType { return fn(); },
                                       // TODO: check that this is a nop..
                                       [](E &&err) -> ReturnType {
                                           return ReturnType::Err(forward<E>(err));
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
#    define NEL_RESULT_TRY(f) \
        __extension__({ \
            auto v = move(f); \
            if (v.is_err()) { return v; } \
            v.unwrap(); \
        })

} // namespace nel

#endif // !defined(NEL_RESULT_HH)
