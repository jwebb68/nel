// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(NEL_ITERATOR_HH)
#    define NEL_ITERATOR_HH

namespace nel
{

template<typename It, typename InT, typename OutT>
struct Iterator;

template<typename It, typename V, typename Fn>
struct MappingIterator;

template<typename It>
struct ChainIterator;

template<typename It>
struct FirstNIterator;

} // namespace nel

#    include <nel/optional.hh>
#    include <nel/log.hh>
#    include <nel/defs.hh>

// #include <functional> //std::function

/**
 * A fluent-style/iterator 'trait'
 *
 * Can be created,
 * Can be iterated over using next(),
 * Maybe iterated using (is_done/incr/deref),
 */
#    define C_LIKE

// #define RUST_LIKE

namespace nel
{
template<typename ItT, typename IT, typename OT>
struct Iterator
{
    public:
        typedef OT OutT;
        typedef IT InT;

    private:
        constexpr ItT &self(void)
        {
            ItT &it = static_cast<ItT &>(*this);
            return it;
        }

        constexpr ItT const &self(void) const
        {
            ItT const &it = static_cast<ItT const &>(*this);
            return it;
        }

#    if defined(RUST_LIKE)
    public:
        Optional<OutT> next(void);
#    endif // defined(RUST_LIKE)

    public:
#    if defined(C_LIKE)
        constexpr bool is_done(void) const;
        void inc(void);
        constexpr OutT deref(void);

        constexpr operator bool(void) const
        {
            return !self().is_done();
        }

        Iterator &operator++(void)
        {
            self().inc();
            return *this;
        }

        constexpr OutT operator*(void)
        {
            return self().deref();
        }
#    endif // defined(C_LIKE)

    public:
        /**
         * Apply fn to each item in iterator
         *
         * @param fn func to apply to each item in iterator
         */
        // void for_each(std::function<void(OutT)> fn)
#    if defined(C_LIKE) || defined(RUST_LIKE)
        template<typename F>
        void for_each(F &&fn)
#    endif
#    if defined(RUST_LIKE)
        {
            while (true) {
                Optional<OutT> r = self().next();
                if (r.is_none()) { break; }
                // TODO: already checked r for none, but unwrap will do so again.. can this be
                // avoided?
                fn(r.unwrap());
            }
        }
#    endif

#    if defined(C_LIKE)
#        if defined(RUST_LIKE)
        // void for_each2(std::function<void(OutT)> fn)
        template<typename F>
        void for_each2(F &&fn)
#        endif
        {
#        if 1
            for (; !self().is_done(); self().inc()) {
                fn(self().deref());
                // Iter will be fn(self.deref()); // self.deref()->T const &
                // IterMut will be fn(self.deref()); // self.deref()->T &
                // IterOwn will be fn(self.deref()); self.deref()->T &&
            }
#        else
            goto enter;
loop:
            fn(self().deref());
            self().inc();
enter:
            if (!self().is_done()) { goto loop; }
#        endif
        }
#    endif

        /**
         * Iterate over self, call fn on each item, stop if fn returns false.
         *
         * @param: fn: fn to call on ieach item in order, returns true to continue, false to stop.
         * @return: true if iteration completed successfully
         * @return: false if iteration was interrupted.
         */
        template<typename F>
        bool try_for_each(F &&fn)
        {
            bool ok = true;
            for (; ok && self(); ++self()) {
                ok = fn(*self());
            }
            return ok;
        }

        /**
         * fold/reduce each item to a single value
         *
         * @param fn func to apply to each item in iterator
         * @note for fn, acc is the folded value, e is the item to fold into it.
         */
        // template<typename U>
        // U fold(U &&initial, std::function<void(U &acc, OutT e)> fn)
        template<typename U, typename F>
        U fold(U &&initial, F &&fn)
        {
            U acc = move(initial);
            self().for_each([&acc, &fn](OutT v) { fn(acc, v); });
            return acc;
        }

#    if defined(C_LIKE) && defined(RUST_LIKE)
        template<typename U, typename F>
        U fold2(U &&initial, F &&fn)
        {
            U acc = move(initial);
            self().for_each2([&acc, &fn](OutT v) { fn(acc, v); });
            return acc;
        }
#    endif

    public:
        friend Log &operator<<(Log &outs, ItT const &it)
        {
            outs << '[';
            // copy/clone since want to mutate..
            ItT it2 = it;
#    if defined(RUST_LIKE)
            OutT v = it2.next();
            if (v.is_some()) {
                outs << v.unwrap();
                // Index i = 0;
                // it2.for_each([&outs, &i](OutT const &e) {
                // outs << '[' << i << "]:" << e << '\n';
                //++i;
                // });
                it2.for_each([&outs](OutT const &e) { outs << ',' << e; });
            }
#    elif defined(C_LIKE)
            if (!it2.is_done()) {
                outs << it2.deref();
                it2.inc();
                // Index i = 0;
                // it2.for_each2([&outs, &i](OutT const &e) {
                //     outs << '[' << i << "]:" << e << '\n';
                //     ++i;
                // });
#        if defined(RUST_LIKE)
                it2.for_each2([&outs](OutT const &e) { outs << ',' << e; });
#        elif defined(C_LIKE)
                it2.for_each([&outs](OutT const &e) { outs << ',' << e; });
#        endif
            }
#    endif
            outs << ']';

            return outs;
        }

    public:
        // annoyingly, new iters need to be added to base iter for fluent style extensions
        constexpr FirstNIterator<ItT> first_n(Count const limit)
        {
            return FirstNIterator<ItT>(move(self()), limit);
        }

        // template<typename U>
        // MappingIterator<ItT, U> map(std::function<U(OutT &)> fn)
        // template<typename Fn, typename U>
        template<typename U, typename Fn>
        constexpr MappingIterator<ItT, U, Fn> map(Fn &&fn)
        {
            return MappingIterator<ItT, U, Fn>(move(self()), forward<Fn>(fn));
        }

        constexpr ChainIterator<ItT> chain(ItT &&other)
        {
            return ChainIterator<ItT>(move(self()), move(other));
        }
};

// x.zip(it2).for_each([&](??)->void{});
// x.enum().for_each([&](??)->void {..});

template<typename It, typename V, typename Fn>
struct MappingIterator: public Iterator<MappingIterator<It, V, Fn>, typename It::InT, V>
{
        // turns an InT into an OutT via a It::OutT

        // e.g. vec<int>.iter()
        //  .map([&](int &v)->float{ return v; })
        // is iter emitting float, but from int &.

        // e.g. vec<int>.iter() // iter of int &
        //    .map([&](int &v)->float{ return v; }) // iter of float
        //    .map([&](float v)->Complex<float>{ return Complex<float>(v,v); }) // iter of
        //    complex<float>
        // is iter emitting Complex, but from int &.

    public:
        typedef V OutT;
        // typedef std::function<V(typename It::OutT)> FnT;
        typedef Fn FnT;

    private:
        It inner_;
        FnT fn_;

    public:
        constexpr MappingIterator(It &&inner, FnT &&fn)
            : inner_(move(inner))
            , fn_(forward<FnT>(fn))
        {
        }

    public:
#    if defined(RUST_LIKE)
        /**
         * Return next item in iterator or None is no more.
         *
         * @returns Optional::Some if iterator still active.
         * @returns Optional::None if iterator exhausted.
         */
        Optional<OutT> next(void)
        {
            // c++ butt ugly language, giving butt ugly constructs..
            // WTF should I need 'template' here..?
            return inner_.next().template map<OutT>(
                [this](typename It::OutT &e) -> V { return fn_(e); });
        }
#    endif // defined(RUST_LIKE)

    public:
#    if defined(C_LIKE)
        constexpr bool is_done(void) const
        {
            return inner_.is_done();
        }

        void inc(void)
        {
            inner_.inc();
        }

        OutT deref(void)
        {
            return fn_(inner_.deref());
        }
#    endif
};

/**
 * Return first n items in iterator, stop iteration if more.
 */
template<typename It>
struct FirstNIterator: public Iterator<FirstNIterator<It>, typename It::InT, typename It::OutT>
{
    public:
        typedef typename It::OutT OutT;

    private:
        It inner_;
        Index current_;
        Length const limit_;

    public:
        constexpr FirstNIterator(It &&inner, Length limit)
            : inner_(move(inner))
            , current_(0)
            , limit_(limit)
        {
        }

    public:
#    if defined(RUST_LIKE)
        /**
         * Return next item in iterator or None is no more.
         *
         * @returns Optional::Some if iterator still active.
         * @returns Optional::None if iterator exhausted.
         */
        Optional<OutT> next(void)
        {
            return (current_ < limit_) ? ++current_, inner_.next() : None;
        }
#    endif // defined(RUST_LIKE)

    public:
#    if defined(C_LIKE)
        constexpr bool is_done(void) const
        {
            return (current_ >= limit_);
        }

        void inc(void)
        {
            inner_.inc();
            ++current_;
        }

        OutT deref(void)
        {
            return inner_.deref();
        }
#    endif
};

template<typename It>
struct ChainIterator: public Iterator<ChainIterator<It>, typename It::InT, typename It::OutT>
{
    public:
        typedef typename It::OutT OutT;

    private:
        It it1_;
        It it2_;

    public:
        // copying ok
        // moving ok.

    public:
        constexpr ChainIterator(It &&it1, It &&it2)
            : it1_(move(it1))
            , it2_(move(it2))
        {
        }

    public:
#    if defined(RUST_LIKE)
        /**
         * Return next item in iterator or None is no more.
         *
         * @returns Optional::Some if iterator still active.
         * @returns Optional::None if iterator exhausted.
         */
        Optional<OutT> next(void)
        {
            return it1_.next().or_else([&](void) -> Optional<OutT> { return it2_.next(); });
        }
#    endif // defined(RUST_LIKE)

    public:
#    if defined(C_LIKE)
        constexpr bool is_done(void) const
        {
            return it1_.is_done() && it2_.is_done();
        }

        void inc(void)
        {
            if (!it1_.is_done()) {
                it1_.inc();
            } else {
                it2_.inc();
            }
        }

        OutT deref(void)
        {
            return (!it1_.is_done()) ? it1_.deref() : it2_.deref();
        }
#    endif

    public:
        /**
         * Apply fn to each item in iterator
         * Consume each item iterated.
         *
         * @param fn func to apply to each item in iterator
         */
        // void for_each(std::function<void(OutT)> fn)
        template<typename F>
        void for_each(F &&fn)
        {
            it1_.for_each(fn);
            it2_.for_each(fn);
        }

#    if defined(C_LIKE) && defined(RUST_LIKE)
        // void for_each2(std::function<void(OutT)> fn)
        template<typename F>
        void for_each2(F &&fn)
        {
            it1_.for_each2(fn);
            it2_.for_each2(fn);
        }
#    endif // defined(C_LIKE)

        template<typename F>
        bool try_for_each(F &&fn)
        {
            return it1_.try_for_each(fn) && it2_.try_for_each(fn);
        }
};

} // namespace nel

#endif // !defined(NEL_ITERATOR_HH)
