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

#include "optional.hh"

#include <functional> //std::function

namespace nel
{

template<typename T>
struct Iterator {
    public:
        typedef T ItemT;
        typedef T &OutT;

    private:
        // Won't pick up changes if realloc'd.
        ItemT *const content_;
        size_t idx_;
        size_t len_;

    public:
        constexpr Iterator(ItemT p[], size_t const len) noexcept: content_(p), idx_(0), len_(len) {}

        Optional<OutT> next(void) noexcept
        {
            return (idx_ < len_) ? Optional<OutT>::Some(content_[idx_++]) : Optional<OutT>::None();
        }

    public:
        // nasty, now cannot add new ops as class is not extensible.
        // and has to be implemented on each new iterator type.
        void for_each(std::function<void(T &)> fn) noexcept
        {
            for (idx_ = 0; idx_ < len_; ++idx_) {
                fn(content_[idx_]);
            }
            // for (T *it = content_, *e = content_ + len_; it != e; ++it) {
            //     fn(*it);
            // }
        }
};

// x.map(mapfn).enum().for_each(..);
// x.chain(MapIt(mapfn)).chain(EnumIt).for_each([](size_t idx, T &e) {});

template<typename I, typename U>
// template<typename I, typename F, typename U>
struct MappingIterator {
    public:
        typedef U ItemT;
        typedef U OutT;
        typedef std::function<U(typename I::ItemT &&)> FnT;
        // typedef F FnT;

    private:
        I inner_;
        FnT fn_;

    public:
        MappingIterator(I inner, FnT fn) noexcept: inner_(inner), fn_(fn) {}

        // TODO: What happens if T cannot be mapped into U?
        // Then fn returns a result, pass it on.
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

template<typename I>
struct FirstNIterator {
    public:
        typedef typename I::ItemT ItemT;
        typedef typename I::OutT OutT;

    private:
        I inner_;
        size_t current_;
        size_t limit_;

    public:
        FirstNIterator(I inner, size_t limit) noexcept: inner_(inner), current_(0), limit_(limit) {}

        Optional<OutT> next(void) noexcept
        {
            // size_t i = current_++;
            // return (i < limit_)
            //     ? inner_.next()
            //     : Optional<typename I::ItemT>::None();
            return (current_ < limit_) ? ++current_, inner_.next() : Optional<OutT>::None();
        }
};

template<typename I>
FirstNIterator<I> first_n_it(I it, size_t limit) noexcept
{
    return FirstNIterator<I>(it, limit);
}

} // namespace nel

#endif // NEL_ITERATOR_HH
