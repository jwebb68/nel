// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include "largestruct1.hh"
#include "error.hh"

#include <nel/result.hh>
#include <nel/log.hh>
#include "libnosys_stubs.cx"

typedef nel::Result<U8Buf<256>, Error> Result_t;

#if 0
Result_t ok1(void)
{
    return Result_t::Ok();
}
#endif

Result_t ok2(void)
{
    return nel::Ok(U8Buf<256>());
}

Result_t ok3(void)
{
    return nel::Ok(U8Buf<256>((uint8_t)0x22));
}

#if 0
Result_t ok4(void)
{
    return nel::Ok((uint8_t)0x33);
}
#endif

//Result_t fail2(void)
Result_t fail4(void)
{
    return nel::Err(Error(ErrorCode::Failed, __FILE__, __LINE__));
}

Result_t passok3(void)
{
    return ok3();
}

Result_t passfail4(void)
{
    return fail4();
}

Result_t passok31(void)
{
    Result_t res = ok3();
    if (res.is_ok()) {
        // only one thing to move-> better than err+bool separate
        return res;
    }
    return nel::Ok(U8Buf<256>(0x31));
}

Result_t passok32(void)
{
    Result_t res = ok3();
    if (res.is_err()) {
        // only one thing to move-> better than err+bool separate
        return res;
    }
    return nel::Ok(U8Buf<256>(0x32));
}

Result_t passfail31(void)
{
    Result_t res = fail4();
    if (res.is_ok()) { return res; }
    return nel::Ok(U8Buf<256>(0x41));
}

Result_t passfail32(void)
{
    Result_t res = fail4();
    if (res.is_err()) { return res; }
    return nel::Ok(U8Buf<256>(0x42));
}

Result_t passok33(void)
{
    Result_t res = ok3();
    NEL_RESULT_TRY(res);
    return nel::Ok(U8Buf<256>(0x33));
}

#if 0
Result_t passfail33(void)
{
    Result_t res = fail4();
    NEL_RESULT_TRY(res);
    return nel::Ok((uint8_t)0x43);
}
#endif

void handleok1(void)
{
    Result_t res = ok3();
    if (res.is_ok()) {
        U8Buf<256> v = res.unwrap();
        nel::log << v << '\n';
    }
}

void handleok2(void)
{
    Result_t res = ok3();
    if (res.is_err()) {
        Error v = res.unwrap_err();
        nel::log << v << '\n';
    }
}

void handlefail1(void)
{
    Result_t res = fail4();
    if (res.is_ok()) {
        U8Buf<256> v = res.unwrap();
        nel::log << v << '\n';
    }
}

void handlefail2(void)
{
    Result_t res = fail4();
    if (res.is_err()) {
        Error v = res.unwrap_err();
        nel::log << v << '\n';
    }
}

int main()
{
    handleok1();
    handlefail1();
    handleok2();
    handlefail2();

    return 0;
}
