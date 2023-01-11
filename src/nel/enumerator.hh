#ifndef NEL_ENUMERATOR_HH
#define NEL_ENUMERATOR_HH

namespace nel
{

template<typename T>
struct Enumerator;

template<typename I, typename U>
struct MappingEnumerator;

template<typename I>
struct FirstNEnumerator;

} // namespace nel

#include <nel/defs.hh>

#include <functional> // std::function

namespace nel
{

template<typename T>
struct Enumerator {
    public:
        typedef T ItemT;
        typedef T &OutT;

    private:
        // shared ?
        //  won't pick up changes if realloc'd
        T *const content_;
        Index idx_;
        Length len_;

    public:
        Enumerator(T p[], Length const len) noexcept
            : content_(p)
            , idx_(0)
            , len_(len)
        {
        }

        constexpr bool is_done(void) const noexcept
        {
            return (idx_ >= len_);
        }

        constexpr operator bool(void) const noexcept
        {
            return (idx_ < len_);
        }

        void inc(void) noexcept
        {
            ++idx_;
        }

        void operator++(void) noexcept
        {
            ++idx_;
        }

        constexpr OutT get(void) const noexcept
        {
            return content_[idx_];
        }

        constexpr OutT operator*(void) const noexcept
        {
            return content_[idx_];
        }
};

template<typename I, typename U>
struct MappingEnumerator {
    public:
        typedef U ItemT;
        typedef U OutT;
        typedef std::function<U(typename I::ItemT &&)> FnT;

    private:
        I inner_;
        FnT fn_;
        // F fn_; // U fn(I::T &&)

    public:
        // MappingEnumerator(I &inner, F fn)
        MappingEnumerator(I &inner, FnT fn) noexcept
            : inner_(inner)
            , fn_(fn)
        {
        }

        constexpr bool is_done(void) const noexcept
        {
            return inner_.is_done();
        }

        constexpr operator bool(void) const noexcept
        {
            return inner_;
        }

        void inc(void) noexcept
        {
            inner_.inc();
        }

        void operator++(void) noexcept
        {
            ++inner_;
        }

        // TODO: what happens if T cannot be mapped into U?
        // then fn returns a result, pass it on.
        // if fn args are move, then cannot repeat the get..
        // but if inner get returns a T or T &, then can repeat.
        constexpr OutT get(void) const noexcept
        {
            return fn_(inner_.get());
        }

        constexpr OutT operator*(void) const noexcept
        {
            return fn_(*inner_);
        }
};

template<typename I, typename U>
MappingEnumerator<I, U> map_en(I en, std::function<U(typename I::ItemT &&)> fn) noexcept
{
    return MappingEnumerator<I, U>(en, fn);
}

template<typename I>
struct FirstNEnumerator {
    public:
        typedef typename I::ItemT ItemT;
        typedef typename I::OutT OutT;

    private:
        I inner_;
        Index current_;
        Length limit_;

    public:
        FirstNEnumerator(I &inner, Length limit) noexcept
            : inner_(inner)
            , current_(0)
            , limit_(limit)
        {
        }

        constexpr bool is_done(void) const noexcept
        {
            return current_ >= limit_ || inner_.is_done();
        }

        constexpr operator bool(void) const noexcept
        {
            return current_ < limit_ && inner_.is_done();
        }

        void inc(void) noexcept
        {
            ++current_;
            inner_.inc();
        }

        void operator++(void) noexcept
        {
            ++current_;
            ++inner_;
        }

        constexpr OutT get(void) const noexcept
        {
            return inner_.get();
        }

        constexpr OutT operator*(void) const noexcept
        {
            return *inner_;
        }
};

template<typename I>
FirstNEnumerator<I> first_n_en(I en, Length limit) noexcept
{
    return FirstNEnumerator<I>(en, limit);
}

} // namespace nel

#endif // NEL_ENUMERATOR_HH
