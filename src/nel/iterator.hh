// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#ifndef NEL_ITERATOR_HH
#define NEL_ITERATOR_HH

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

#include <nel/optional.hh>
#include <nel/log.hh>
#include <nel/defs.hh>

//#include <functional> //std::function

namespace nel
{

/**
 * A fluent-style/iterator 'trait'
 *
 * Use as a mixin to get functionals defined on it for all iterators
 */
template<typename ItT, typename IT, typename OT>
struct Iterator {
    public:
        typedef OT OutT;
        typedef IT InT;

    private:
        constexpr ItT &self(void) noexcept
        {
            ItT &it = static_cast<ItT &>(*this);
            return it;
        }

        constexpr ItT const &self(void) const noexcept
        {
            ItT const &it = static_cast<ItT const &>(*this);
            return it;
        }

    public:
        Optional<OutT> next(void);

    public:
        constexpr bool is_done(void) const noexcept;
        void inc(void) noexcept;
        OutT deref(void) noexcept;

    public:
        /**
         * Apply fn to each item in iterator
         *
         * @param fn func to apply to each item in iterator
         */
        // void for_each(std::function<void(OutT)> fn) noexcept
        template<typename F>
        void for_each(F &&fn) noexcept
        {
            while (true) {
                Optional<OutT> r = self().next();
                if (r.is_none()) { break; }
                // TODO: already checked r for none, but unwrap will do so again.. can this be
                // avoided?
                fn(r.unwrap());
            }
        }

        // void for_each2(std::function<void(OutT)> fn) noexcept
        template<typename F>
        void for_each2(F &&fn) noexcept
        {
            for (; !self().is_done(); self().inc()) {
                fn(self().deref());
                // Iter will be fn(self.deref()); // self.deref()->T const &
                // IterMut will be fn(self.deref()); // self.deref()->T &
                // IterOwn will be fn(self.deref()); self.deref()->T &&
            }
        }

        /**
         * fold/reduce each item to a single value
         *
         * @param fn func to apply to each item in iterator
         * @note for fn, acc is the folded value, e is the item to fold into it.
         */
        // template<typename U>
        // U fold(U &&initial, std::function<void(U &acc, OutT e)> fn) noexcept
        template<typename U, typename F>
        U fold(U &&initial, F &&fn) noexcept
        {
            U acc = std::move(initial);
            for_each([&acc, &fn](OutT v) { fn(acc, v); });
            return acc;
        }

        template<typename U, typename F>
        U fold2(U &&initial, F &&fn) noexcept
        {
            U acc = std::move(initial);
            for_each2([&acc, &fn](OutT v) { fn(acc, v); });
            return acc;
        }

    public:
        friend Log &operator<<(Log &outs, ItT const &it)
        {
            outs << '[';
            // copy/clone since want to mutate..
            ItT it2 = it;
#if 0
            OutT v = it2.next();
            if (v.is_some()) {
                outs << v.unwrap();
                //Index i = 0;
                //it2.for_each([&outs, &i](OutT const &e) {
                    //outs << '[' << i << "]:" << e << '\n';
                    //++i;
                //});
                it2.for_each([&outs](OutT const &e) {
                    outs << ',' << e;
                });
            }
#elif 1
            if (!it2.is_done()) {
                outs << it2.deref();
                it2.inc();
                // Index i = 0;
                // it2.for_each2([&outs, &i](OutT const &e) {
                //     outs << '[' << i << "]:" << e << '\n';
                //     ++i;
                // });
                it2.for_each2([&outs](OutT const &e) { outs << ',' << e; });
            }
#endif
            outs << ']';

            return outs;
        }

    public:
        // annoyingly, new iters need to be added to base iter for fluent style extensions
        FirstNIterator<ItT> first_n(Count const limit) noexcept
        {
            return FirstNIterator<ItT>(std::move(self()), limit);
        }

        // template<typename U>
        // MappingIterator<ItT, U> map(std::function<U(OutT &)> fn) noexcept
        // template<typename Fn, typename U>
        template<typename U, typename Fn>
        MappingIterator<ItT, U, Fn> map(Fn &&fn) noexcept
        {
            return MappingIterator<ItT, U, Fn>(std::move(self()), std::forward<Fn>(fn));
        }

        ChainIterator<ItT> chain(ItT &&other) noexcept
        {
            return ChainIterator<ItT>(std::move(self()), std::move(other));
        }
};

// x.zip(it2).for_each([&](??)->void{});
// x.enum().for_each([&](??)->void {..});

template<typename It, typename V, typename Fn>
struct MappingIterator: public Iterator<MappingIterator<It, V, Fn>, typename It::InT, V> {
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
        MappingIterator(It &&inner, FnT &&fn) noexcept
            : inner_(std::move(inner))
            , fn_(std::forward<FnT>(fn))
        {
        }

    public:
        /**
         * Return next item in iterator or None is no more.
         *
         * @returns Optional::Some if iterator still active.
         * @returns Optional::None if iterator exhausted.
         */
        Optional<OutT> next(void) noexcept
        {
            // c++ butt ugly language, giving butt ugly constrcts..
            // WTF should I need 'template' here..?
            return inner_.next().template map<OutT>([this](It::OutT &e) -> V { return fn_(e); });
        }

    public:
        constexpr bool is_done(void) const noexcept
        {
            return inner_.is_done();
        }

        void inc(void) noexcept
        {
            inner_.inc();
        }

        OutT deref(void) noexcept
        {
            return fn_(inner_.deref());
        }
};

/**
 * Return first n items in iterator, stop iteration if more.
 */
template<typename It>
struct FirstNIterator: public Iterator<FirstNIterator<It>, typename It::InT, typename It::OutT> {
    public:
        typedef typename It::OutT OutT;

    private:
        It inner_;
        Index current_;
        Length const limit_;

    public:
        FirstNIterator(It &&inner, Length limit) noexcept
            : inner_(std::move(inner))
            , current_(0)
            , limit_(limit)
        {
        }

    public:
        /**
         * Return next item in iterator or None is no more.
         *
         * @returns Optional::Some if iterator still active.
         * @returns Optional::None if iterator exhausted.
         */
        Optional<OutT> next(void) noexcept
        {
            return (current_ < limit_) ? ++current_, inner_.next() : None;
        }

    public:
        constexpr bool is_done(void) const noexcept
        {
            return (current_ >= limit_);
        }

        void inc(void) noexcept
        {
            inner_.inc();
            ++current_;
        }

        OutT deref(void) noexcept
        {
            return inner_.deref();
        }
};

template<typename It>
struct ChainIterator: public Iterator<ChainIterator<It>, typename It::InT, typename It::OutT> {
    public:
        typedef It::OutT OutT;

    private:
        It it1_;
        It it2_;

    public:
        ChainIterator(It &&it1, It &&it2) noexcept
            : it1_(std::move(it1))
            , it2_(std::move(it2))
        {
        }

    public:
        /**
         * Return next item in iterator or None is no more.
         *
         * @returns Optional::Some if iterator still active.
         * @returns Optional::None if iterator exhausted.
         */
        Optional<OutT> next(void) noexcept
        {
            return it1_.next().or_else([&](void) -> Optional<OutT> { return it2_.next(); });
        }

    public:
        constexpr bool is_done(void) const noexcept
        {
            return it1_.is_done() && it2_.is_done();
        }

        void inc(void) noexcept
        {
            if (!it1_.is_done()) {
                it1_.inc();
            } else {
                it2_.inc();
            }
        }

        OutT deref(void) noexcept
        {
            return (!it1_.is_done()) ? it1_.deref() : it2_.deref();
        }

    public:
        /**
         * Apply fn to each item in iterator
         * Consume each item iterated.
         *
         * @param fn func to apply to each item in iterator
         */
        // void for_each(std::function<void(OutT)> fn) noexcept
        template<typename F>
        void for_each(F &&fn) noexcept
        {
            it1_.for_each(fn);
            it2_.for_each(fn);
        }

        // void for_each2(std::function<void(OutT)> fn) noexcept
        template<typename F>
        void for_each2(F &&fn) noexcept
        {
            it1_.for_each2(fn);
            it2_.for_each2(fn);
        }
};

} // namespace nel

#endif // NEL_ITERATOR_HH
