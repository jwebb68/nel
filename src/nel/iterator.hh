#ifndef NEL_ITERATOR_HH
#define NEL_ITERATOR_HH

namespace nel
{

template<typename It, typename InT, typename OutT>
struct Iterator;

template<typename It, typename InT, typename OutT>
struct MappingIterator;

template<typename It, typename InT, typename OutT>
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
template<typename ItT, typename InT, typename OutT>
struct Iterator {
    public:
    private:
        constexpr ItT &self(void) noexcept
        {
            ItT &it = static_cast<ItT &>(*this);
            return it;
        }

    public:
        /**
         * Apply fn to each item in iterator
         *
         * @param fn func to apply to each item in iterator
         */
        void for_each(std::function<void(InT &&)> fn) noexcept
        {
            // It &it = static_cast<It &>(*this);
            while (true) {
                auto r = self().next();
                if (r.is_none()) { break; }
                // auto & v = r.unwrap();
                auto v = r.unwrap();
                fn(std::move(v));
            }
        }

        /**
         * fold/reduce each item to a single value
         *
         * @param fn func to apply to each item in iterator
         * @note for fn, acc is the folded value, e is the item to fold into it.
         */
        template<typename U>
        U fold(U &&initial, std::function<void(U &acc, InT &&e)> fn) noexcept
        {
            U acc = std::move(initial);
            while (true) {
                auto r = self().next();
                if (r.is_none()) { break; }
                // auto & v = r.unwrap();
                auto v = r.unwrap();
                fn(acc, std::move(v));
            }
            return acc;
        }

        FirstNIterator<ItT, InT, OutT> first_n(Count const limit) noexcept
        {
            return FirstNIterator<ItT, InT, OutT>(self(), limit);
        }

        template<typename U>
        MappingIterator<ItT, InT, U> map(std::function<U(InT &&)> fn) noexcept
        {
            return MappingIterator<ItT, InT, U>(self(), fn);
        }
};

// x.map(mapfn).enum().for_each(..);
// x.chain(MapIt(mapfn)).chain(EnumIt).for_each([](Index idx, T &e) {});

template<typename It, typename InT, typename OutT>
struct MappingIterator: Iterator<MappingIterator<It, InT, OutT>, InT, OutT> {
    public:
        typedef std::function<OutT(InT &&)> FnT;

    private:
        It inner_;
        FnT fn_;

    public:
        MappingIterator(It inner, FnT fn) noexcept: inner_(inner), fn_(fn) {}

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
            return (r.is_none()) ? Optional<OutT>::None()
                                 : Optional<OutT>::Some(fn_(std::move(r.unwrap())));

            // auto r = inner_.next();
            // return r.map1(fn_);
            // return inner_.next();
        }
};

/**
 * Return first n items in iterator, stop iteration if more.
 */
template<typename It, typename InT, typename OutT>
struct FirstNIterator: Iterator<FirstNIterator<It, InT, OutT>, InT, OutT> {
    public:
    private:
        It inner_;
        Index current_;
        Length const limit_;

    public:
        FirstNIterator(It inner, Length limit) noexcept: inner_(inner), current_(0), limit_(limit)
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
            return (current_ < limit_) ? ++current_, inner_.next() : Optional<OutT>::None();
        }
};

} // namespace nel

#endif // NEL_ITERATOR_HH
