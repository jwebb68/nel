// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
// #include "largestruct1.hh"
#include "error.hh"

#include <nel/result.hh>
#include <nel/log.hh>
#include <nel/defs.hh> // NEL_WARN_UNUSED_RESULT
#include "libnosys_stubs.cx"

template<class T, nel::USize const N>
struct Array
{
        T elems_[N];

        ~Array()
        {
            for (T *d = &elems_[0]; d != &elems_[N]; ++d) {
                d->~T();
            }
        }

        template<typename... Us>
        constexpr Array(Us &&...us)
            : elems_(nel::forward<Us>(us)...)
        {
        }

    public:
        // maybe default for T=Pod
#define MAN_MOVE_COPY 0
#if MAN_MOVE_COPY == 0
        // array will use per-item moving? and copying?
        constexpr Array(Array &&) = default;
        constexpr Array &operator=(Array &&) = default;
#elif MAN_MOVE_COPY == 1
        // maybe by-member for T=UDT
        constexpr Array(Array &&o)
        {
            for (T *d = &elems_[0], *s = &o.elems_[0]; d != &elems_[N]; ++s, ++d) {
                new (d) T(nel::forward<T>(*s));
            }
        }

        constexpr Array &operator=(Array &&o)
        {
            for (T *d = &elems_[0], *s = &o.elems_[0]; d != &elems_[N]; ++s, ++d) {
                *d = nel::forward<T>(*s);
            }
        }
#endif

        // private:
        //  copy-ctor/assn needs to be public for init from const/const arrays.
#if MAN_MOVE_COPY == 0
        constexpr Array(Array const &) = default;
        constexpr Array &operator=(Array const &) = default;
#elif MAN_MOVE_COPY == 1
        constexpr Array(Array const &o)
        {
            T const *s = &o.elems_[0];
            for (T *d = &elems_[0]; d != &elems_[N]; ++d) {
                new (d) T(*s);
                s += 1;
            }
        }

        constexpr Array &operator=(Array const &o)
        {
            T const *s = &o.elems_[0];
            for (T *d = &elems_[0], *s = &o.elems_[0]; d != &elems_[N]; ++d) {
                *d = *s;
                s += 1;
            }
        }
#endif

    public:
        // constexpr Array clone() const {
        //     return Array {*this};
        // }

        nel::Log &dbgfmt(nel::Log &outs) const
        {
            outs << "Array<" << N << ">{";
            if (N > 0) {
                outs << elems_[0];
                for (nel::USize i = 1; i < N; ++i) {
                    outs << ',' << elems_[i];
                }
            }
            outs << '}';
            return outs;
        }

        friend nel::Log &operator<<(nel::Log &outs, Array const &v)
        {
            return v.dbgfmt(outs);
        }
};

typedef Array<uint8_t, 256> OkT;
typedef Error ErrT;
typedef nel::Result<OkT, ErrT> ResultT;

#if 0
//only if T has a default ctor..
ResultT NEL_WARN_UNUSED_RESULT ok1(void)
{
    return ResultT::Ok();
}
#endif

ResultT NEL_WARN_UNUSED_RESULT ok2(void)
{
    // leaves a temporary that isn't optimised away when called..
    return ResultT::Ok(OkT());
}

ResultT NEL_WARN_UNUSED_RESULT ok3(void)
{
    // leaves a temporary that isn't optimised away when called..
    return ResultT::Ok(OkT((uint8_t)0x30));
}

ResultT NEL_WARN_UNUSED_RESULT ok4(void)
{
    // does not leave a temp when called..
    return ResultT::Ok((uint8_t)0x40);
}

ResultT NEL_WARN_UNUSED_RESULT fail3(void)
{
    return ResultT::Err(ErrT(ErrorCode::Failed, __FILE__, __LINE__));
}

ResultT NEL_WARN_UNUSED_RESULT fail4(void)
{
    return ResultT::Err(ErrorCode::Failed, __FILE__, __LINE__);
}

ResultT NEL_WARN_UNUSED_RESULT passok3(void)
{
    return ok3();
}

ResultT NEL_WARN_UNUSED_RESULT passfail4(void)
{
    return fail4();
}

ResultT NEL_WARN_UNUSED_RESULT passok3_1(void)
{
    auto res = ok3();
    if (res.is_ok()) {
        // only one thing to move-> better than err+bool separate
        return res;
    }
    return ResultT::Ok((uint8_t)0x31);
}

ResultT NEL_WARN_UNUSED_RESULT passok3_2(void)
{
    auto res = ok3();
    if (res.is_err()) {
        // only one thing to move-> better than err+bool separate
        return res;
    }
    return ResultT::Ok((uint8_t)0x32);
}

ResultT NEL_WARN_UNUSED_RESULT passok3_3(void)
{
    auto res = ok3();
    return res.consume<ResultT>([](OkT &&) -> ResultT { return ResultT::Ok((uint8_t)0x33); },
                                [](ErrT &&v) -> ResultT { return ResultT::Err(v); });
}

ResultT NEL_WARN_UNUSED_RESULT passok3_4(void)
{
    auto res = ok3();
    NEL_RESULT_TRY(res);
    return ResultT::Ok((uint8_t)0x33);
}

ResultT NEL_WARN_UNUSED_RESULT passfail4_1(void)
{
    auto res = fail4();
    if (res.is_ok()) { return res; }
    return ResultT::Ok((uint8_t)0x41);
}

ResultT NEL_WARN_UNUSED_RESULT passfail4_2(void)
{
    auto res = fail4();
    if (res.is_err()) { return res; }
    return ResultT::Ok((uint8_t)0x42);
}

ResultT NEL_WARN_UNUSED_RESULT passfail4_3(void)
{
    auto res = fail4();
    return res.consume<ResultT>([](OkT &&) -> ResultT { return ResultT::Ok((uint8_t)0x43); },
                                [](ErrT &&v) -> ResultT { return ResultT::Err(v); });
}

ResultT NEL_WARN_UNUSED_RESULT passfail4_4(void)
{
    ResultT res = fail4();
    NEL_RESULT_TRY(res);
    return ResultT::Ok((uint8_t)0x43);
}

void __attribute__((noinline)) handleok3_1(void)
{
    auto res = ok3();
    res.consume<void>([](OkT &&v) { nel::log << v << '\n'; },
                      [](ErrT &&v) { nel::log << v << '\n'; });
}

void __attribute__((noinline)) handleok3_2(void)
{
    auto res = ok3();
    res.consume<void>([](OkT &&v) { nel::log << v << '\n'; },
                      [](ErrT &&v) { nel::log << v << '\n'; });
}

void __attribute__((noinline)) handleok4_1(void)
{
    auto res = ok4();
    if (res.is_ok()) {
        auto v = res.unwrap();
        nel::log << v << '\n';
    }
}

void __attribute__((noinline)) handleok4_11(void)
{
    auto res = ok4();
    if (res.is_ok()) { nel::log << res.unwrap() << '\n'; }
}

void __attribute__((noinline)) handleok4_2(void)
{
    auto res = ok4();
    if (res.is_err()) {
        auto v = res.unwrap_err();
        nel::log << v << '\n';
    }
}

void __attribute__((noinline)) handleok4_21(void)
{
    auto res = ok4();
    if (res.is_err()) { nel::log << res.unwrap_err() << '\n'; }
}

void __attribute__((noinline)) handleok4_3(void)
{
    auto res = ok4();
    res.consume<void>([](OkT &&v) { nel::log << v << '\n'; },
                      [](ErrT &&v) { nel::log << v << '\n'; });
}

void __attribute__((noinline)) handleok4_31(void)
{
    ok4().consume<void>([](OkT &&v) { nel::log << v << '\n'; },
                        [](ErrT &&v) { nel::log << v << '\n'; });
}

void __attribute__((noinline)) handlefail4_1(void)
{
    auto res = fail4();
    if (res.is_ok()) {
        auto v = res.unwrap();
        nel::log << v << '\n';
    }
}

void __attribute__((noinline)) handlefail4_2(void)
{
    auto res = fail4();
    if (res.is_err()) {
        auto v = res.unwrap_err();
        nel::log << v << '\n';
    }
}

void __attribute__((noinline)) handlefail4_3(void)
{
    auto res = fail4();
    res.consume<void>([](OkT &&v) { nel::log << v << '\n'; },
                      [](ErrT &&v) { nel::log << v << '\n'; });
}

int main()
{
    handleok3_1();
    //    handlefail1();
    handleok3_2();
    //    handlefail2();

    handleok4_1();
    handleok4_2();
    handleok4_3();
    handlefail4_1();
    handlefail4_2();
    handlefail4_3();
}
