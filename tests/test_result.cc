
#include "result.hh" //nel::ResultT


typedef enum
{
    NOERROR,
    EFAIL,
    EENOENT
} fail_type;


nel::Result<int, fail_type> foo(int v)
{
    return nel::Result<int, fail_type>::Ok(v);
}

nel::Result<int, fail_type> bar(fail_type w)
{
    return nel::Result<int, fail_type>::Err(w);
}

#define UNUSED(x) ((void)(x))

extern "C"
void test_ok_ctor()
{
    auto ret = nel::Result<int, fail_type>::Ok(1);
    UNUSED(ret);
}

extern "C"
void test_err_ctor()
{
    auto ret = nel::Result<int, fail_type>::Err(EENOENT);
    UNUSED(ret);
}

extern "C"
int test_ok_is_ok()
{
    auto ret = foo(1);
    auto got = ret.is_ok();
    return got;
}

extern "C"
int test_err_unwrap_or()
{
    auto ret = bar(EENOENT);
    auto got = ret.unwrap_or(5);
    return got;
}

int main(int argc, char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    test_ok_is_ok();

    test_err_unwrap_or();

    return 0;
}
