#ifndef NEL_ITERATOR_HH
#define NEL_ITERATOR_HH

namespace nel
{

template<typename T>
struct Iterator;

template<typename I, typename U>
// template<typename I, typename F, typename U>
struct MappingIterator;

template<typename I>
struct FirstNIterator;

} // namespace nel

#include <nel/optional.hh>
#include <nel/defs.hh>

#include <functional> //std::function

namespace nel
{

/**
 * An Iterator over a range of T
 *
 * Iterator does not own what it's iterating over, so is invalidated if that goes out of scope.
 * Cannot be reset once exhausted.
 * Is non-consuming, items are not moved out of original container.
 */
// TODO: maybe make length a template param..
template<typename T>
struct Iterator {
    public:
        typedef T ItemT;
        typedef T &OutT;

    private:
        // Won't pick up changes if realloc'd.
        ItemT *const content_;
        Index idx_;
        Length len_;

    public:
        constexpr Iterator(ItemT p[], Length const len) noexcept: content_(p), idx_(0), len_(len) {}

        // default copy is ok.
        // default move is ok.

    public:
        /**
         * Return next item in iterator or None is no more.
         *
         * @returns Optional::Some if iterator still active.
         * @returns Optional::None if iterator exhausted.
         */
        Optional<OutT> next(void) noexcept
        {
            return (idx_ < len_) ? Optional<OutT>::Some(content_[idx_++]) : Optional<OutT>::None();
        }

    public:
        // nasty, now cannot add new ops as class is not extensible.
        // and has to be implemented on each new iterator type.
        /**
         * Apply mutating fn to each item in iterator
         *
         * @param fn func to apply to each item in iterator
         */
        void for_each(std::function<void(T &)> fn) noexcept
        {
            for (idx_ = 0; idx_ < len_; ++idx_) {
                fn(content_[idx_]);
            }
        }

        /**
         * fold/reduce each item to a single value
         *
         * @param fn func to apply to each item in iterator
         * @note for fn, acc is the folded value, e is the item to fold into it.
         */
        template<typename U>
        U fold(U &&initial, std::function<void(U &acc, T &e)> fn) noexcept
        {
            U acc = std::move(initial);
            for (idx_ = 0; idx_ < len_; ++idx_) {
                fn(acc, content_[idx_]);
            }
            return acc;
        }
};

// x.map(mapfn).enum().for_each(..);
// x.chain(MapIt(mapfn)).chain(EnumIt).for_each([](Index idx, T &e) {});

/**
 * Apply fn to each value in iterator, returning the result for each item
 */
template<typename I, typename U>
struct MappingIterator {
    public:
        typedef U ItemT;
        typedef U OutT;
        typedef std::function<U(typename I::ItemT &&)> FnT;

    private:
        I inner_;
        FnT fn_;

    public:
        MappingIterator(I inner, FnT fn) noexcept: inner_(inner), fn_(fn) {}

    public:
        /**
         * Return next item in iterator or None is no more.
         *
         * @returns Optional::Some if iterator still active.
         * @returns Optional::None if iterator exhausted.
         */
        Optional<OutT> next(void) noexcept
        {
            auto v = inner_.next();
            return v.is_none() ? v : Optional<OutT>(fn_(v.unwrap()));
        }
};
template<typename I, typename U>
MappingIterator<I, U> map_it(I it, std::function<U(typename I::ItemT &&)> fn) noexcept
{
    return MappingIterator<I, U>(it, fn);
}

/**
 * Return first n items in iterator, stop iteration if more.
 */
template<typename I>
struct FirstNIterator {
    public:
        typedef typename I::ItemT ItemT;
        typedef typename I::OutT OutT;

    private:
        I inner_;
        Index current_;
        Length limit_;

    public:
        FirstNIterator(I inner, Length limit) noexcept: inner_(inner), current_(0), limit_(limit) {}

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
template<typename I>
FirstNIterator<I> first_n_it(I it, Length limit) noexcept
{
    return FirstNIterator<I>(it, limit);
}

/**
 * fold values in iterator into single value.
 *
 * @param it The iterator to operate on
 * @param initial the initial value to start the fold with
 * @param fn The fn to apply to each element to fold.
 * @returns folded value..
 */
template<typename I, typename U>
U fold(I it, U &&initial, std::function<void(U &acc, typename I::ItemT &e)> fn) noexcept
{
    U acc = std::move(initial);
    while (true) {
        auto v = it.next();
        if (v.is_none()) { break; }
        // TODO: want better way to unwrap that does not check after already checked
        // or returns value pair<bool, T> ? then could use unpacking without checking twice.
        auto u = v.unwrap();
        fn(acc, u);
    }
    return acc;
}

} // namespace nel

#endif // NEL_ITERATOR_HH
