#include "largestruct1.hh"
#include "error.hh"

#include <nel/result.hh>
#include <nel/log.hh>
#include <nel/defs.hh>

typedef nel::Result<U8Buf<256>, Error> Result_t;

Result_t NEL_WARN_UNUSED ok1(void) {
    return Result_t::Ok();
}
Result_t NEL_WARN_UNUSED ok2(void) {
    return Result_t::Ok(U8Buf<256>());
}
Result_t NEL_WARN_UNUSED ok3(void) {
    return Result_t::Ok(U8Buf<256>((uint8_t)0x22));
}
Result_t NEL_WARN_UNUSED ok4(void) {
    return Result_t::Ok((uint8_t)0x33);
}

Result_t NEL_WARN_UNUSED fail2(void) {
    return Result_t::Err(Error(ErrorCode::Failed, __FILE__, __LINE__));
}
Result_t NEL_WARN_UNUSED fail4(void) {
    return Result_t::Err(ErrorCode::Failed, __FILE__, __LINE__);
}

Result_t NEL_WARN_UNUSED passok3(void) {
    return ok3();
}
Result_t NEL_WARN_UNUSED passfail4(void) {
    return fail4();
}

Result_t NEL_WARN_UNUSED passok31(void) {
    Result_t res = ok3();
    if (res.is_ok()) {
        // only one thing to move-> better than err+bool separate
        return res;
    }
    return Result_t::Ok((uint8_t)0x31);
}
Result_t NEL_WARN_UNUSED passok32(void) {
    Result_t res = ok3();
    if (res.is_err()) {
        // only one thing to move-> better than err+bool separate
        return res;
    }
    return Result_t::Ok((uint8_t)0x32);
}

Result_t NEL_WARN_UNUSED passfail31(void) {
    Result_t res = fail4();
    if (res.is_ok()) {
        return res;
    }
    return Result_t::Ok((uint8_t)0x41);
}
Result_t NEL_WARN_UNUSED passfail32(void) {
    Result_t res = fail4();
    if (res.is_err()) {
        return res;
    }
    return Result_t::Ok((uint8_t)0x42);
}

Result_t NEL_WARN_UNUSED passok33(void) {
    Result_t res = ok3();
    NEL_RESULT_TRY(res);
    return Result_t::Ok((uint8_t)0x33);
}
Result_t NEL_WARN_UNUSED passfail33(void) {
    Result_t res = fail4();
    NEL_RESULT_TRY(res);
    return Result_t::Ok((uint8_t)0x43);
}

void handleok1(void) {
    Result_t res = ok3();
    if (res.is_ok()) {
        U8Buf<256> v = res.unwrap();
        nel::log << v << '\n';
    }
}
void handleok2(void) {
    Result_t res = ok3();
    if (res.is_err()) {
        Error v = res.unwrap_err();
        nel::log << v << '\n';
    }
}

void handlefail1(void) {
    Result_t res = fail4();
    if (res.is_ok()) {
        U8Buf<256> v = res.unwrap();
        nel::log << v << '\n';
    }
}
void handlefail2(void) {
    Result_t res = fail4();
    if (res.is_err()) {
        Error v = res.unwrap_err();
        nel::log << v << '\n';
    }
}


int main() {
    handleok1();
    handlefail1();
    handleok2();
    handlefail2();
}
