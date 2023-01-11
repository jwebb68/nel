#ifndef NEL_ITERATOR_HH
#define NEL_ITERATOR_HH

namespace nel
{

template<typename It, typename InT, typename OutT>
struct Iterator;

template<typename It, typename V>
struct MappingIterator;

template<typename It>
struct FirstNIterator;

} // namespace nel

#include <nel/optional.hh>
#include <nel/defs.hh>

#include <functional> //std::function

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
        /**
         * Apply fn to each item in iterator
         *
         * @param fn func to apply to each item in iterator
         */
        // void for_each(std::function<void(OutT)> fn) noexcept
        template<typename F>
        void for_each(F fn) noexcept
        {
            while (true) {
                Optional<OutT> r = self().next();
                if (r.is_none()) { break; }
                // TODO: already checked r for none, but unwrap will do so again.. can this be
                // avoided?
                fn(r.unwrap());
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
        U fold(U &&initial, F fn) noexcept
        {
            U acc = std::move(initial);
            for_each([&](OutT &v) { fn(acc, v); });
            return acc;
        }

    public:
        // annoyingly, new iters need to be added to base iter for fluent style extensions
        FirstNIterator<ItT> first_n(Count const limit) noexcept
        {
            return FirstNIterator<ItT>(self(), limit);
        }

        template<typename U>
        MappingIterator<ItT, U> map(std::function<U(OutT &&)> fn) noexcept
        {
            return MappingIterator<ItT, U>(self(), fn);
        }
};

// x.map(mapfn).enum().for_each(..);
// x.chain(MapIt(mapfn)).chain(EnumIt).for_each([](Index idx, T &e) {});

template<typename It, typename V>
struct MappingIterator: public Iterator<MappingIterator<It, V>, typename It::InT, V> {
    public:
        typedef V OutT;
        typedef std::function<V(typename It::OutT)> FnT;

    private:
        It inner_;
        FnT fn_;

    public:
        MappingIterator(It inner, FnT fn) noexcept
            : inner_(inner)
            , fn_(fn)
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
            auto r = inner_.next();
            // // if (r.is_none()) { return Optional<OutT>::None(); }
            // // auto t = r.unwrap();
            // // auto u = fn_(std::move(t));
            // // return Optional<OutT>::Some(u);
            return (r.is_none()) ? None : Some(fn_(std::move(r.unwrap())));

            // auto r = inner_.next();
            // return r.map1(fn_);
            // return inner_.next();
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
        FirstNIterator(It inner, Length limit) noexcept
            : inner_(inner)
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
};

} // namespace nel

#endif // NEL_ITERATOR_HH
