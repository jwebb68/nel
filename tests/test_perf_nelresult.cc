#include "test.hh"

#include "result.hh"  // nel::Result<>

typedef int ok_type2;
typedef int err_type2;
typedef nel::ResultT<ok_type2, err_type2> return_type2;

// =======================================================================

noinline return_type2 _foo_r(int c, int d, int err)
{
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


// =======================================================================


noinline return_type2 _foo_rr(int c, int d, int err)
{
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
