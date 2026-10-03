// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(NEL_RESULT_HH)
#    define NEL_RESULT_HH

namespace nel
{

template<typename T, typename E>
struct Result;

} // namespace nel

#    include <nel/optional.hh>
#    include <nel/element.hh>
#    include <nel/log.hh>
#    include <nel/panic.hh>
#    include <nel/memory.hh> // move,forward
#    include <nel/new.hh> // new (p) T()

namespace nel
{

template<typename T>
struct OkT
{
    public:
        using Type = T;

    private:
        Type val_;

    public:
        constexpr OkT(Type &&val)
            : val_(move(val))
        {
        }

    public:
        OkT(OkT &&rhs) = default;
        OkT &operator=(OkT &&rhs) = default;

        OkT(OkT const &rhs) = delete;
        OkT &operator=(OkT const &rhs) = default;

    public:
    //protected:
        // Problem: Must not be able to access the wrapped value except via
        // Result<T,E> (if access is allowed then can move the contained value
        // in/out, which is something that only the result should be able to
        // do).
        // Usual fix is to make them private, and make the Result<T,E> a friend.
        // But for the OkT, E is unknown.

        constexpr Type &deref()
        {
            return val_;
        }

        constexpr Type const &deref() const
        {
            return val_;
        }

    public:
    //protected:
        constexpr Type &operator*()
        {
            return deref();
        }

        constexpr Type const &operator*() const
        {
            return deref();
        }

    protected:
        //  doesn't work if T = reference, so use a Ref<T>.
        constexpr Type *operator->()
        {
            return &deref();
        }

        constexpr Type const *operator->() const
        {
            return &deref();
        }

        // can't have a release(), since what would be the value of val_?
        // cannot be default as T may not have a default.
    public:
        constexpr bool operator==(OkT const &rhs) const {
            return val_ == rhs.val_;
        }
        constexpr bool operator!=(OkT const &rhs) const {
            return val_ != rhs.val_;
        }
};

template<typename T>
constexpr inline OkT<T> Ok(T &&t)
{
    return OkT(move(t));
}

// shorthand for Ok(Void())
// otherwise OkT could be renamed back to Ok.
constexpr inline OkT<Void> Ok()
{
    return OkT(Void());
}

template<typename T>
struct ErrT
{
    public:
        using Type = T;

    private:
        Type val_;
        Context raise_pt_;

        // TODO: Add location where created.
        // so can report where originally raised.
    public:
        constexpr ErrT(Type &&val, Context const &ctx = Context())
            : val_(move(val))
            , raise_pt_(ctx)
        {
        }

    public:
        ErrT(ErrT &&rhs) = default;
        ErrT &operator=(ErrT &&rhs) = default;

        ErrT(ErrT const &rhs) = delete;
        ErrT &operator=(ErrT const &rhs) = default;

    public:
    //protected:
        // Problem: Must not be able to access the wrapped value except via
        // Result<T,E> (if access is allowed then can move the contained value
        // in/out, which is something that only the result should be able to
        // do).
        // Usual fix is to make them private, and make the Result<T,E> a friend.
        // But for the ErrT, T is unknown.
        // Soln is to make them protected,, then inherit in the using class
        // and use move operator to move from this type into inherited class.

        constexpr Type &deref()
        {
            return val_;
        }

        constexpr Type const &deref() const
        {
            return val_;
        }

    public:
    //protected:
        constexpr Type &operator*()
        {
            return deref();
        }

        constexpr Type const &operator*() const
        {
            return deref();
        }

    protected:
        //  doesn't work if T = reference, so use a Ref<T>.
        constexpr Type *operator->()
        {
            return &deref();
        }

        constexpr Type const *operator->() const
        {
            return &deref();
        }

    protected:
        constexpr Context const &where() const {
            return raise_pt_;
        }

        // can't have a release(), since what would be the value of val_?
        // cannot be default as T may not have a default.

    public:
        constexpr bool operator==(ErrT const &rhs) const {
            return val_ == rhs.val_;
        }
        constexpr bool operator!=(ErrT const &rhs) const {
            return val_ != rhs.val_;
        }
};

// so can do: auto e = Err(1);
// Not really needed, only doing it for repeat pattern for Ok case.
template<typename E>
constexpr inline ErrT<E> Err(E &&e, Context const &ctx = Context())
{
    return ErrT(move(e), ctx);
}

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
struct [[nodiscard("unused Result that must be used")]] Result
{
    private:
        // Restrict accessor funcs in OkT/ErrT so they can
        // only be used by Result, and not by the public.
        using Ok = OkT<T>;

        using Err = ErrT<E>;

    private:
        // Tagged enum thing.
        // Similar to std::variant but without the exception throwing behaviour.
        // Maybe make into a nel::Variant ?
        struct Tag
        {
            public:
                enum ETag {
                    inval = 0,
                    ok,
                    err,
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

        union {
            Ok ok_;
            Err err_;
        };

    public:
        constexpr ~Result(void)
        {
            switch (tag_) { // result::dtor
                case Tag::ok:
                    ok_.~Ok();
                    break;

                case Tag::err:
                    err_.~Err();
                    break;

                case Tag::inval:
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
                    nel::panic("bad Result");
                    break;
            }
        }

        constexpr Result(Result &&rhs)
            : tag_(move(rhs.tag_))
        {
            switch (tag_) { // result::move-ctor
                case Tag::ok:
                    new (&ok_) Ok(move(rhs.ok_));
                    break;

                case Tag::err:
                    new (&err_) Err(move(rhs.err_));
                    break;

                case Tag::inval:
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
                    nel::panic("bad Result");
                    break;
            }
        }

        constexpr Result &operator=(Result &&rhs)
        {
            if (this != &rhs) {
                // Would use destroy+construct always, but want same types to use move-assgn.
                // Don't use { auto f = Result(move(rhs)); f.swap(*this); } as that results in 1+1.5 moves.
                if (tag_ == rhs.tag_) {
                    switch (tag_) { // result-move-ass
                        case Tag::ok:
                            ok_ = move(rhs.ok_);
                        break;
                        case Tag::err:
                            err_ = move(rhs.err_);
                        break;
                        case Tag::inval:
                        break;
                        default:
                        // for dtor + moves (but not usage) at runtime:
                        // does it matter it's not one of the 'special' values?
                        // yes, since it may have contained one or the others..
                        // if memory hardware is assumed to be incorruptible then can ignore ?
                        // (buffer overruns?) but might not want to for buffer overrun issues. if
                        // there is a memory corruption, then should have been detected by memory
                        // hardware (Ecc mem) if not using eec/ecc mem then that's the prob.
                            nel::panic("bad Result");
                        break;
                    }
                    tag_ = move(rhs.tag_);
                } else {
                    this->~Result();
                    new (this) Result(move(rhs));
                }
            }
            return *this;
        }

    private:
        // Not using copy semantics, results are only moveable..
        // because T may be expensive to copy, or may not have copy semantics.
        Result(Result const &) = delete;
        Result &operator=(Result const &) const = delete;

    public:
        // Don't really want default ctor, but will want to just declare an instance with no value.
        constexpr Result(void)
            : tag_(Tag::inval)
        {
        }

        constexpr Result(OkT<T> &&ok)
            : tag_(Tag::ok)
            , ok_(move(ok))
        {
        }

        constexpr Result(ErrT<E> &&err)
            : tag_(Tag::err)
            , err_(move(err))
        {
        }

    private:
        //  don't use std::function, it's bloatware
        // May be simpler to just expose the Tag value..
        // which mush panic if a non-enum value detected..

        // template<typename V>
        // constexpr V match(Tag tag, std::function<V(void)> &&on_ok, std::function<V(void)>
        // &&on_err) const  {
        template<typename V, typename Fn1, typename Fn2>
        constexpr V match(Fn1 &&on_ok, Fn2 &&on_err, Context const &ctx = Context()) const
        {
            // this match is non-consuming, for internal use only.
            switch (tag_) { // result-match
                case Tag::ok:
                    return on_ok(*ok_);
                case Tag::err:
                    return on_err(*err_);
                case Tag::inval:
                    // invalids are not values that should occur
                    // if they do it's a use-after-move-from op so must panic.
                    // or a use-before-initialised so again must panic.
                    nel::panic("invalid Result", ctx);
                    break;
                default:
                    nel::panic("bad Result", ctx);
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
         * @param rhs The right hand side of the expression.
         * @returns true if equal by value, false otherwise.
         *
         * `this` is not consumed by the operation.
         * `rhs` is not consumed by the operation.
         */
        constexpr bool operator==(Result const &rhs) const
        {
            switch (tag_) {
                case Tag::ok:
                    return ok_ == rhs;
                case Tag::err:
                    return err_ == rhs;
                case Tag::inval:
                    nel::panic("invalid Result");
                    break;
                default:
                    nel::panic("bad Result");
                    break;
            }
        }
        constexpr friend bool operator==(Result const &lhs, Ok const &rhs) {
            // bah, oper== can only take 1 arg..
            // TODO: fix this.
            switch (lhs.tag_) {
                case Tag::ok:
                    return lhs.ok_ == rhs;
                    break;
                case Tag::err:
                    return false;
                    break;
                case Tag::inval:
                    nel::panic("invalid Result");
                    break;
                default:
                    nel::panic("bad Result");
                    break;
            }
        }
        constexpr friend bool operator==(Ok const &lhs, Result const &rhs) {
            return rhs == lhs;
        }
        constexpr friend bool operator==(Result const &lhs, Err const &rhs) {
            // bah, oper== can only take 1 arg..
            // TODO: fix this.
            switch (lhs.tag_) {
                case Tag::ok:
                    return false;
                    break;
                case Tag::err:
                    return lhs.err_ == rhs;
                    break;
                case Tag::inval:
                    nel::panic("invalid Result");
                    break;
                default:
                    nel::panic("bad Result");
                    break;
            }
        }
        constexpr friend bool operator==(Err const &lhs, Result const &rhs) {
            return rhs == lhs;
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
        constexpr bool operator!=(Result const &rhs) const
        {
            switch (tag_) {
                case Tag::ok:
                    return ok_ != rhs;
                case Tag::err:
                    return err_ != rhs;
                case Tag::inval:
                    nel::panic("invalid Result");
                    break;
                default:
                    nel::panic("bad Result");
                    break;
            }
        }
        constexpr friend bool operator!=(Result const &lhs, Ok const &rhs) {
            // bah, != can only take 1 arg..
            // TODO: fix this.
            switch (lhs.tag_) {
                case Tag::ok:
                    return lhs.ok_ != rhs;
                    break;
                case Tag::err:
                    return true;
                    break;
                case Tag::inval:
                    nel::panic("invalid Result");
                    break;
                default:
                    nel::panic("bad Result");
                    break;
            }
        }
        constexpr friend bool operator!=(Ok const &lhs, Result const &rhs) {
            return rhs == lhs;
        }
        constexpr friend bool operator!=(Result const &lhs, Err const &rhs) {
            // bah, != can only take 1 arg..
            // TODO: fix this.
            switch (lhs.tag_) {
                case Tag::ok:
                    return true;
                    break;
                case Tag::err:
                    return lhs.err_ != rhs;
                    break;
                case Tag::inval:
                    nel::panic("invalid Result");
                    break;
                default:
                    nel::panic("bad Result");
                    break;
            }
        }
        constexpr friend bool operator!=(Err const &lhs, Result const &rhs) {
            return rhs == lhs;
        }

    public:
        /**
         * Determine if the container contains a Ok.
         *
         * @returns true if container contains a Ok, false otherwise.
         *
         * `this` is not consumed by this op.
         */
        constexpr bool is_ok(Context const &ctx = Context()) const
        {
            // min-size: gets optimised + inlined at call site.
            // release: gets optimised away..
            return match<bool>([](T const &) -> bool { return true; },
                               [](E const &) -> bool { return false; },
                               ctx);
        }

        /**
         * Determine if the container contains a Err.
         *
         * @returns true if container contains a Err, false otherwise.
         *
         * `this` is not consumed by this op.
         */
        constexpr bool is_err(Context const &ctx = Context()) const
        {
            return match<bool>([](T const &) -> bool { return false; },
                               [](E const &) -> bool { return true; },
                               ctx);
        }

#if defined(TEST)
        constexpr bool is_inval(Context const &ctx = Context()) const
        {
            switch (tag_) {
                case Tag::ok:
                    return false;
                case Tag::err:
                    return false;
                case Tag::inval:
                    return true;
                    break;
                default:
                    nel::panic("bad Result", ctx);
                    break;
            }
        }
#endif

    public:
        template<typename V, typename Fn1, typename Fn2>
        constexpr V consume(Fn1 &&on_ok, Fn2 &&on_err, Context const &ctx = Context())
        {
            // a consuming matcher..
            // that moves the value to the closures..
            auto tag = move(tag_);
            switch (tag) { // result-consume
                case Tag::ok:
                    return on_ok(move(*ok_));
                case Tag::err:
                    return on_err(move(*err_));
                case Tag::inval:
                    // invalids are not values that should occur
                    // if they do it's a use-after-move-from op so must panic.
                    // or a use-before-initialised so again must panic.
                    nel::panic("invalid Result", ctx);
                    break;
                default:
                    nel::panic("bad Result", ctx);
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
        constexpr Optional<T> ok(Context const &ctx = Context())
        {
            return consume<Optional<T>>([](T &&ok) -> Optional<T> { return Some(move(ok)); },
                                        [](E &&) -> Optional<T> { return None(); },
                                        ctx);
        }

        /**
         * Return an optional containing the err value or none.
         *
         * @returns if this is a Err, returns Optional containing a Some with the err value.
         * @returns if this is not a Err, returns Optional containing a None.
         *
         * `this` is consumed and invalidated after.
         */
        constexpr Optional<E> err(Context const &ctx = Context())
        {
            return consume<Optional<E>>([](T &&) -> Optional<E> { return None(); },
                                        [](E &&err) -> Optional<E> { return Some(move(err)); },
                                        ctx);
        }

        /**
         * Extract and return the contained value if an Ok, consuming the result.
         *
         * @return if Ok, value wrapped/contained.
         * @return if not Ok, calls abort, there is no return.
         *
         * `this` is consumed by the operation.
         */
        constexpr T unwrap(Context const &ctx = Context())
        {
            return consume<T>([](T &&ok) -> T { return move(ok); },
                              [&](E &&) -> T { nel::panic("not an ok", ctx); },
                              ctx);
        }

        /**
         * Extract and return the contained value if an Err, consuming `this`.
         *
         * @return if Err, value wrapped/contained.
         * @return if not Err, calls abort, there is no return.
         *
         * this is consumed by the operation.
         */
        constexpr E unwrap_err(Context const &ctx = Context())
        {
            return consume<E>([&](T &&) -> E { nel::panic("not an err", ctx); },
                              [](E &&err) -> E { return move(err); },
                              ctx);
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
        constexpr T unwrap_or(T &&v, Context const &ctx = Context())
        {
            return consume<T>([](T &&ok) -> T { return move(ok); },
                              [&v](E &&) -> T { return move(v); },
                              ctx);
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
        constexpr T unwrap_or(Args &&...args, Context const &ctx = Context())
        {
            return consume<T>([](T &&ok) -> T { return move(ok); },
                              [&args...](E &&) -> T { return T(forward<Args>(args)...); },
                              ctx);
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
        constexpr E unwrap_err_or(E &&v, Context const &ctx = Context())
        {
            return consume<E>([&v](T &&) -> E { return move(v); },
                              [](E &&err) -> E { return move(err); },
                              ctx);
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
        constexpr E unwrap_err_or(Args &&...args, Context const &ctx = Context())
        {
            return consume<E>([&args...](T &&) -> E { return E(forward<Args>(args)...); },
                              [](E &&err) -> E { return move(err); },
                              ctx);
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
        template<typename U, typename FN>
        constexpr Result<U, E> map(FN &&fn, Context const &ctx = Context())
        {
            using ReturnT = Result<U, E>;
            return consume<ReturnT>([&fn](
                                           T &&ok) -> ReturnT { return OkT<U>(fn(move(ok))); },
                                       [](E &&err) -> ReturnT { return Err(move(err)); },
                                       ctx);
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
        constexpr Result<T, F> map_err(Fn &&fn, Context const &ctx = Context())
        {
            // TODO: remove need to explicitly cast to result in each of the
            //       branches.. i.e. the Result<U,E>() bit, should be Ok(..) or Err(..)
            using ReturnT = Result<T, F>;
            return consume<ReturnT>([](T &&ok) -> ReturnT { return Ok(move(ok)); },
                                       [&fn](E &&err) -> ReturnT {
                                           return ErrT<F>(fn(move(err)));
                                       },
                                       ctx);
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
        constexpr Result<U, E> and_then(Fn &&fn, Context const &ctx = Context())
        {
            // TODO: remove need to explicitly cast to result in each of the
            //       branches.. i.e. the Result<U,E>() bit, should be Ok(..) or Err(..)
            typedef Result<U, E> ReturnType;
            return consume<ReturnType>([&fn](T &&ok) -> ReturnType { return OkT<U>(fn(move(ok))); },
                                       // TODO: check that this is a nop..
                                       [](E &&err) -> ReturnType { return Err(move(err)); },
                                       ctx);
        }

    public:
        friend Log &operator<<(Log &outs, Result const &val)
        {
            switch (val.tag_) { // result-dbgfmt
                case Tag::ok:
                    outs << "Result(Ok(" << *val.ok_ << "))";
                    return outs;
                    break;
                case Tag::err:
                    outs << "Result(Err(" << *val.err_ << "))";
                    return outs;
                    break;
                case Tag::inval:
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
