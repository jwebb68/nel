// test performance of nel result against:
// 1. exceptions,
// 2. simple int return code,
// 3. tuple of err,ok (cf. erlang)
// 4. nel::Result
// over n stack frames...
#include "optional.hh"
#include "result.hh"

#include <optional>
#include <tuple>

#define UNUSED(x) ((void)(x))

// to prevent clang optimising through function pointers
// to prevent clang doing tail-call-optimisations prevent recursive
// functions being TCOd.
#define noinline __attribute__ ((noinline))

// prevent clang from inlining the control/timeref functions even
// though called by function pointer.
volatile int a = 0;

noinline int timeref(int c, int d, int err)
{
    UNUSED(c);
    UNUSED(d);
    UNUSED(err);
    ++a;
    return 0;
}

noinline int control(int c, int d, int err)
{
    ++a;
    if (c == err) return -1;
    if (c == d) return c + 1;
    return control(c + 1, d, err);
}

// ======================================================
noinline int _foo_e(int c, int d, int err)
{
    ++a;
    if (c == err) throw std::runtime_error("foo");
    if (c == d) return c + 1;
    return _foo_e(c + 1, d, err);
}
noinline int foo_e(int c, int d, int err)
{
    try
    {
        ++a;
        if (c == err) throw std::runtime_error("foo");
        if (c == d) return c + 1;
        return _foo_e(c + 1, d, err);
    }
    catch (const std::runtime_error &exc)
    {
        return -1;
    }
}

// ========================================================

noinline int _foo_es(int c, int d, int err)
{
    try
    {
        ++a;
        if (c == err) throw std::runtime_error("foo");
        if (c == d) return c + 1;
        return _foo_es(c + 1, d, err);
    }
    catch (const std::runtime_error &exc)
    {
        throw;
    }
}
noinline int foo_es(int c, int d, int err)
{
    try
    {
        ++a;
        if (c == err) throw std::runtime_error("foo");
        if (c == d) return c + 1;
        return _foo_es(c + 1, d, err);
    }
    catch (const std::runtime_error &exc)
    {
        return -1;
    }
}

// =========================================================
#if 0
typedef typename nel::OptionalT<int> err_type0;
typedef typename nel::OptionalT<int> ok_type0;
typedef typename std::tuple<ok_type0, err_type0> return_type0;
return_type0 _foo0_t(int c, int d, int err)
{
    if (c == err) return std::make_tuple(err_type0(nel::Some(err)),
                                             ok_type0(nel::None()));
    if (c == d) return std::make_tuple(err_type0(nel::None()),
                                           ok_type0(nel::Some(c + 1)));
    return _foo0_t(c + 1, d, err);
}
int foo0_t(int c, int d, int err)
{
    auto v = _foo0_t(c, d, err);
    err_type0 err_;
    ok_type0 val;
    std::tie(err_, val) = v;
    if (err_.is_some()) return -1;
    return 0;
}
return_type0 _foo0_ts(int c, int d, int err)
{
    if (c == err) return std::make_tuple(err_type0(nel::Some(err)),
                                             ok_type0(nel::None()));
    if (c == d) return std::make_tuple(err_type0(nel::None()),
                                           ok_type0(nel::Some(c + 1)));
    err_type0 err_;
    ok_type0 val;
    std::tie(err_, val) = _foo0_ts(c + 1, d, err);
    if (err_.is_some()) return std::make_tuple(err_, val);
    return std::make_tuple(err_, val);
}
int foo0_ts(int c, int d, int err)
{
    auto v = _foo0_ts(c, d, err);
    err_type0 err_;
    ok_type0 val;
    std::tie(err_, val) = v;
    if (err_.is_some()) return -1;
    return 0;
}

return_type0 _foo0_tr(int c, int d, int err)
{
    if (c == err) return std::make_tuple(err_type0(nel::Some(err)),
                                             ok_type0(nel::None()));
    if (c == d) return std::make_tuple(err_type0(nel::None()),
                                           ok_type0(nel::Some(c + 1)));
    err_type0 err_;
    ok_type0 val;
    auto res = _foo0_ts(c + 1, d, err);
    std::tie(err_, val) = res;
    if (err_.is_some()) return res;
    return res;
}
int foo0_tr(int c, int d, int err)
{
    auto v = _foo0_tr(c, d, err);
    err_type0 err_;
    ok_type0 val;
    std::tie(err_, val) = v;
    if (err_.is_some()) return -1;
    return 0;
}
#endif//0

// =========================================================
typedef typename std::optional<int> err_type1;
typedef typename std::optional<int> ok_type1;
typedef typename std::tuple<ok_type1, err_type1> return_type1;
noinline return_type1 _foo_t(int c, int d, int err)
{
    ++a;
    if (c == err) return std::make_tuple(err, std::nullopt);
    if (c == d) return std::make_tuple(std::nullopt, c + 1);
    return _foo_t(c + 1, d, err);
}
noinline int foo_t(int c, int d, int err)
{
    auto v = _foo_t(c, d, err);

    err_type1 err_;
    ok_type1 val;
    std::tie(err_, val) = v;
    if (err_.has_value()) return -1;
    return 0;
}

noinline return_type1 _foo_ts(int c, int d, int err)
{
    ++a;
    if (c == err) return std::make_tuple(err, std::nullopt);
    if (c == d) return std::make_tuple(std::nullopt, c + 1);

    err_type1 err_;
    ok_type1 val;
    std::tie(err_, val) = _foo_ts(c + 1, d, err);
    if (err_.has_value()) return std::make_tuple(err_, val);
    return std::make_tuple(err_, val);
}
noinline int foo_ts(int c, int d, int err)
{
    auto v = _foo_ts(c, d, err);

    err_type1 err_;
    ok_type1 val;
    std::tie(err_, val) = v;
    if (err_.has_value()) return -1;
    return 0;
}

noinline return_type1 _foo_tr(int c, int d, int err)
{
    ++a;
    if (c == err) return std::make_tuple(err, std::nullopt);
    if (c == d) return std::make_tuple(std::nullopt, c + 1);

    err_type1 err_;
    ok_type1 val;
    auto res = _foo_tr(c + 1, d, err);
    std::tie(err_, val) = res;
    if (err_.has_value()) return res;
    return res;
}
noinline int foo_tr(int c, int d, int err)
{
    auto v = _foo_tr(c, d, err);

    err_type1 err_;
    ok_type1 val;
    std::tie(err_, val) = v;
    if (err_.has_value()) return -1;
    return 0;
}

// =============================================================
typedef int ok_type2;
typedef int err_type2;
typedef nel::ResultT<ok_type2, err_type2> return_type2;

noinline return_type2 _foo_r(int c, int d, int err)
{
    ++a;
    if (c == err) return nel::Err(err);
    if (c == d) return nel::Ok(c + 1);
    return _foo_r(c + 1, d, err);
}
noinline int foo_r(int c, int d, int err)
{
    auto v = _foo_r(c, d, err);
    return v.unwrap_or(-1);
}

noinline return_type2 _foo_rs(int c, int d, int err)
{
    ++a;
    if (c == err) return nel::Err(err);
    if (c == d) return nel::Ok(c + 1);

    auto res = _foo_rs(c + 1, d, err);
    if (res.is_err()) return res;
    auto v = res.unwrap();
    return nel::Ok(v);
}
noinline int foo_rs(int c, int d, int err)
{
    auto v = _foo_rs(c, d, err);
    return v.unwrap_or(-1);
}

noinline return_type2 _foo_rr(int c, int d, int err)
{
    ++a;
    if (c == err) return nel::Err(err);
    if (c == d) return nel::Ok(c + 1);

    auto res = _foo_rr(c + 1, d, err);
    if (res.is_err()) return res;
    return res;
}
noinline int foo_rr(int c, int d, int err)
{
    auto v = _foo_rr(c, d, err);
    return v.unwrap_or(-1);
}

#include <chrono>

class Timer
{
    public:
        Timer() : beg_(clock_type::now()) {}
        void reset()
        {
            this->beg_ = clock_type::now();
        }
        double elapsed() const
        {
            return std::chrono::duration_cast<second_type>
                   (clock_type::now() - this->beg_).count();
        }

    private:
        //typedef std::chrono::high_resolution_clock clock_type;
        typedef std::chrono::steady_clock clock_type;
        //typedef std::chrono::duration<double, std::ratio<1> > second_type;
        typedef std::chrono::duration<double> second_type;
        std::chrono::time_point<clock_type> beg_;
};

//extern "C"
double perf(int (*fn)(int, int, int), int reps, int d, int err)
{
    Timer t1;
    for (int i = 0; i < reps; ++i)
    {
        (*fn)(0, d, err);
    }
    return t1.elapsed();
}

double perf2(int (*fn)(int, int, int), int reps, int d, int err)
{
    auto beg = std::chrono::steady_clock::now();
    for (int i = 0; i < reps; ++i)
    {
        (*fn)(0, d, err);
    }
    auto end = std::chrono::steady_clock::now();
    auto diff = end-beg;
    return std::chrono::duration<double>(diff).count();
}

#if 1
#include <iostream>
#include <iomanip>
int main(int argc, char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    const int reps = 100000;
    auto ref = perf(timeref, reps, 0, 0);
    std::cout << std::setw(10) << "timeref" << ",";
    std::cout << std::fixed << std::setw(5+1+9) << std::setprecision(9) << ref << ", ";
    std::cout << std::fixed << std::setw(5+1+9) << std::setprecision(12) << ref / reps << std::endl;

    auto ref2 = perf2(timeref, reps, 0, 0);
    std::cout << std::setw(10) << "timeref" << ",";
    std::cout << std::fixed << std::setw(5+1+9) << std::setprecision(9) << ref2 << ", ";
    std::cout << std::fixed << std::setw(5+1+9) << std::setprecision(12) << ref2 / reps << std::endl;

    struct cases_type
    {
        const char *name;
        int (*fn)(int c, int d, int err);
    } cases[] =
    {
        {"control", control},
#if 0
        {"foo0_t", foo0_t},
        {"foo0_ts", foo0_ts},
        {"foo0_tr", foo0_tr},
#endif//0
        {"foo_t", foo_t},
        {"foo_ts", foo_ts},
        {"foo_tr", foo_tr},
        {"foo_r", foo_r},
        {"foo_rs", foo_rs},
        {"foo_rr", foo_rr},
        {"foo_e", foo_e},
        {"foo_es", foo_es},
    };

    int frames[] = {1, 5, 10, 100};
    for (unsigned int frames_idx = 0;
         frames_idx < sizeof(frames) / sizeof(frames[0]);
         ++frames_idx)
    {
        auto frm = frames[frames_idx];
        for (unsigned int idx = 0;
             idx < sizeof(cases) / sizeof(cases[0]);
             ++idx)
        {
            auto ok = perf(cases[idx].fn, reps, frm, frm + 1);
            std::cout << std::setw(10) << cases[idx].name << ",";
            std::cout << std::setw(3) << frm << ",";
            std::cout << std::setw(3) << "ok" << ", ";
            std::cout << std::fixed << std::setw(8+1+12) << std::setprecision(12) << ok << ",";
            std::cout << std::fixed << std::setw(8+1+12) << std::setprecision(12) << ok / reps << ",";
            std::cout << std::fixed << std::setw(8+1+12) << std::setprecision(12) << ok / reps / static_cast<double>(frm) << ", ";
            std::cout << std::fixed << std::setw(8+1+12) << std::setprecision(12) << ok / ref << ",";
            std::cout << std::fixed << std::setw(8+1+12) << std::setprecision(12) << ok / ref / static_cast<double>(frm) << ",";
            std::cout << std::endl;

            auto err = perf(cases[idx].fn, reps, frm, frm);
            std::cout << std::setw(10) << cases[idx].name << ",";
            std::cout << std::setw(3) << frm << ",";
            std::cout << std::setw(3) << "err" << ", ";
            std::cout << std::fixed << std::setw(8+1+12) << std::setprecision(12) << err << ",";
            std::cout << std::fixed << std::setw(8+1+12) << std::setprecision(12) << err / reps << ",";
            std::cout << std::fixed << std::setw(8+1+12) << std::setprecision(12) << err / reps/ static_cast<double>(frm) << ", ";
            std::cout << std::fixed << std::setw(8+1+12) << std::setprecision(12) << err / ref << ",";
            std::cout << std::fixed << std::setw(8+1+12) << std::setprecision(12) << err / ref / static_cast<double>(frm) << ",";
            std::cout << std::endl;
        }

    }
    return 0;
}
#else
#include <iostream>
#include <chrono>
#include <iomanip>

using namespace std;

int main()
{
#if 0
    cout << "system_clock" << endl;
    cout << chrono::system_clock::period::num << endl;
    cout << chrono::system_clock::period::den << endl;
    cout << "steady = " << boolalpha << chrono::system_clock::is_steady << endl <<
         endl;

    cout << "high_resolution_clock" << endl;
    cout << chrono::high_resolution_clock::period::num << endl;
    cout << chrono::high_resolution_clock::period::den << endl;
    cout << "steady = " << boolalpha << chrono::high_resolution_clock::is_steady <<
         endl << endl;

    cout << "steady_clock" << endl;
    cout << chrono::steady_clock::period::num << endl;
    cout << chrono::steady_clock::period::den << endl;
    cout << "steady = " << boolalpha << chrono::steady_clock::is_steady << endl <<
         endl;

#endif//0
    auto diff1 = perf(&timeref, 10000, 0, 0);
    cout << fixed << setw(1+1+9) << setprecision(9) << chrono::duration<double>(diff1).count() << "s" << std::endl;

    auto diff2 = perf2(&timeref, 10000, 0, 0);
    cout << fixed << setw(1+1+9) << setprecision(9) << chrono::duration<double>(diff2).count() << "s" << std::endl;

    //cout << fixed << setw(1+1+9) << setprecision(6) << chrono::duration<double, milli>(diff).count() << "ms" << std::endl;
    //cout << fixed << setw(1+1+9) << setprecision(3) << chrono::duration<double, micro>(diff).count() << "us" << std::endl;
    //cout << fixed << setw(1+1+9) << setprecision(0) << chrono::duration<double, nano>(diff).count() << "ns" << std::endl;
    return 0;
}
#endif//0
