#include "test.hh"

#include <optional> // std::nullopt
#include <tuple> // std::tuple<>, std::make_tuple, std::tie

typedef typename std::optional<int> err_type1;
typedef typename std::optional<int> ok_type1;
typedef typename std::tuple<ok_type1, err_type1> return_type1;

// =======================================================================

noinline return_type1 _foo_t(int c, int d, int err)
{
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


// =======================================================================

noinline return_type1 _foo_ts(int c, int d, int err)
{
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


// =======================================================================


noinline return_type1 _foo_tr(int c, int d, int err)
{
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
