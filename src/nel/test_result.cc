// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include <catch2/catch.hpp>

#include <nel/result.hh> //nel::ResultT
#include <nel/defs.hh> // NEL_UNUSED

namespace nel
{
namespace test
{
namespace result
{

typedef enum _Error {
    NOERROR = 0,
    FAIL,
    NOENT
} Error;

nel::Log &operator<<(nel::Log &outs, Error const &v)
{
    switch (v) {
        case NOERROR:
            outs << "NOERROR";
            return outs;
            break;
        case FAIL:
            outs << "FAIL";
            return outs;
            break;
        case NOENT:
            outs << "NOENT";
            return outs;
            break;
        default:
            std::abort();
    }
    return outs;
}

typedef nel::Result<int, Error> TestResult;

TEST_CASE("Result<>::Ok", "[result]")
{
    auto res = TestResult::Ok(1);

    SECTION("must produce a res containing the val")
    {
        REQUIRE(res.unwrap() == 1);
    }
}

TEST_CASE("Result<>::Err", "[result]")
{
    auto res = TestResult::Err(FAIL);

    SECTION("must produce a res containing the val")
    {
        REQUIRE(res.unwrap_err() == FAIL);
    }
}

TEST_CASE("Result<>(ok).move-ctor", "[result]")
{
    auto src = TestResult::Ok(1);

    auto dst = move(src);

    SECTION("must move value to new loc")
    {
        REQUIRE(dst.unwrap() == 1);
    }
    SECTION("must invalidate src")
    {
        REQUIRE(src.is_inval());
    }
}

TEST_CASE("Result<>(err).move-ctor", "[result]")
{
    auto src = TestResult::Err(FAIL);

    auto dst = move(src);

    SECTION("must move value to new loc")
    {
        REQUIRE(dst.unwrap_err() == FAIL);
    }
    SECTION("must invalidate src")
    {
        REQUIRE(src.is_inval());
    }
}

TEST_CASE("Result<>(inval).move-ctor", "[result]")
{
    auto src = TestResult::Err(FAIL);
    auto tmp1 = move(src);
    nel::unused(tmp1);

    auto dst = move(src);

    SECTION("must move value to new loc")
    {
        // move must not panic
        REQUIRE(dst.is_inval());
    }
    SECTION("must invalidate src")
    {
        REQUIRE(src.is_inval());
    }
}

TEST_CASE("Result<>(ok).move-ass", "[result]")
{
    auto src = TestResult::Ok(1);

    auto dst = TestResult::Ok(2);
    dst = move(src);

    SECTION("must move value to new loc")
    {
        REQUIRE(dst.unwrap() == 1);
    }
    SECTION("must invalidate src")
    {
        REQUIRE(src.is_inval());
    }
}

TEST_CASE("Result<>(err).move-ass: must move err val to dest", "[result]")
{
    auto src = TestResult::Err(FAIL);

    auto dst = TestResult::Ok(2);
    dst = move(src);

    SECTION("must move value to new loc")
    {
        REQUIRE(dst.unwrap_err() == FAIL);
    }
    SECTION("must invalidate src")
    {
        REQUIRE(src.is_inval());
    }
}

TEST_CASE("Result<>(inval).move-ass", "[result]")
{
    auto src = TestResult::Err(FAIL);
    auto tmp1 = move(src);

    auto dst = TestResult::Ok(3);
    dst = move(src);

    SECTION("must move value to new loc")
    {
        // move must not panic
        REQUIRE(dst.is_inval());
    }
    SECTION("must invalidate src")
    {
        REQUIRE(src.is_inval());
    }
}

TEST_CASE("Result<>(ok).ok()", "[result]")
{
    auto src = TestResult::Ok(1);

    auto val = src.ok();

    SECTION("must produce the wrapped value wrapped in an optional")
    {
        REQUIRE(val == nel::Optional<int>::Some(1));
    }
    SECTION("must invalidate src")
    {
        REQUIRE(src.is_inval());
    }
}

TEST_CASE("Result<>(err).ok(): must produce none", "[result]")
{
    auto src = TestResult::Err(FAIL);
    auto val = src.ok();

    SECTION("must produce a none")
    {
        REQUIRE(val == None);
    }
    SECTION("must invalidate src")
    {
        REQUIRE(src.is_inval());
    }
}

TEST_CASE("Result<>(inval).ok(): must panic", "[result]")
{
    auto src = TestResult::Err(FAIL);
    auto tmp1 = move(src);
    nel::unused(tmp1);

    SECTION("must panic")
    {
        // use after-free case.
        // value is invalid, so cannot use.
        REQUIRE_PANIC(src.ok());
    }
}

TEST_CASE("Result<>(ok).err(): must produce none", "[result]")
{
    auto src = TestResult::Ok(1);
    auto val = src.err();

    SECTION("must produce a none")
    {
        REQUIRE(val == None);
    }
    SECTION("must invalidate src")
    {
        REQUIRE(src.is_inval());
    }
}

TEST_CASE("Result<>(err).err(): must produce err val", "[result]")
{
    auto src = TestResult::Err(FAIL);
    auto val = src.err();

    SECTION("must produce optional holding err value")
    {
        REQUIRE(val == nel::Some(FAIL));
    }
    SECTION("must invalidate src")
    {
        REQUIRE(src.is_inval());
    }
}

TEST_CASE("Result<>(inval).err(): must panic", "[result]")
{
    auto src = TestResult::Ok(1);
    auto tmp1 = move(src);
    nel::unused(tmp1);

    SECTION("must panic")
    {
        // use after-free case.
        // value is invalid, so cannot use.
        REQUIRE_PANIC(src.err());
    }
}

TEST_CASE("Result<>(ok).is_ok()", "[result]")
{
    auto src = TestResult::Ok(1);

    bool is_ok = src.is_ok();

    SECTION("must return true")
    {
        REQUIRE(is_ok);
    }
    SECTION("must not change value of src")
    {
        REQUIRE(src.unwrap() == 1);
    }
}

TEST_CASE("Result<>(err).is_ok()", "[result]")
{
    auto src = TestResult::Err(FAIL);

    bool is_ok = src.is_ok();

    SECTION("must return false")
    {
        REQUIRE(!is_ok);
    }
    SECTION("must not change value of src")
    {
        REQUIRE(src.unwrap_err() == FAIL);
    }
}

TEST_CASE("Result<>(inval).is_ok(): must panic", "[result]")
{
    auto src = TestResult::Ok(1);
    auto tmp = move(src);
    nel::unused(tmp);

    SECTION("must panic")
    {
        REQUIRE_PANIC(src.is_ok());
    }
}

TEST_CASE("Result<>(ok).is_err()", "[result]")
{
    auto src = TestResult::Ok(1);

    SECTION("must return false")
    {
        REQUIRE(!src.is_err());
    }
    SECTION("must not change value of src")
    {
        REQUIRE(src.unwrap() == 1);
    }
}

TEST_CASE("Result<>(err).is_err()", "[result]")
{
    auto src = TestResult::Err(FAIL);

    SECTION("must return true")
    {
        REQUIRE(src.is_err());
    }
    SECTION("must not change value of src")
    {
        REQUIRE(src.unwrap_err() == FAIL);
    }
}

TEST_CASE("Result<>(inval).is_err()", "[result]")
{
    auto src = TestResult::Ok(1);
    auto tmp = move(src);
    nel::unused(tmp);

    SECTION("must panic")
    {
        REQUIRE_PANIC(src.is_err());
    }
}


TEST_CASE("Result<>(ok).unwrap()", "[result]")
{
    auto src = TestResult::Ok(1);

    auto val = src.unwrap();

    SECTION("must return value") {
        REQUIRE(val == 1);
    }
    SECTION("must invalidate src") {
        REQUIRE(src.is_inval());
    }
}

TEST_CASE("Result<>(err).unwrap()", "[result]")
{
    auto res = TestResult::Err(FAIL);

    SECTION("must panic") {
        REQUIRE_PANIC(res.unwrap());
    }
}

TEST_CASE("Result<>(inval).unwrap()", "[result]")
{
    auto res = TestResult::Ok(1);
    auto res2 = move(res);

    SECTION("must panic") {
        REQUIRE_PANIC(res.unwrap());
    }
}

// disabled until panic detection is available.
TEST_CASE("Result<>(ok).unwrap_err()", "[result]")
{
    auto res = TestResult::Ok(1);

    SECTION("must panic") {
        REQUIRE_PANIC(res.unwrap_err());
    }
}

TEST_CASE("Result<>(err).unwrap_err()", "[result]")
{
    auto src = TestResult::Err(FAIL);

    auto val = src.unwrap_err();

    SECTION("must return value") {
        REQUIRE(val == FAIL);
    }
    SECTION("must invalidate src") {
        REQUIRE(src.is_inval());
    }
}

TEST_CASE("Result<>(inval).unwrap_err()", "[result]")
{
    auto res = TestResult::Ok(1);
    auto res2 = move(res);

    SECTION("must panic") {
        REQUIRE_PANIC(res.unwrap());
    }
}


TEST_CASE("Result<>(ok).unwrap_or(ok2)", "[result]")
{
    auto src = TestResult::Ok(1);

    auto val = src.unwrap_or(2);

    SECTION("must return wrapped value") {
        REQUIRE(val == 1);
    }
    SECTION("must invalidate src") {
        REQUIRE(src.is_inval());
    }
}

TEST_CASE("Result<>(err).unwrap_or(ok2)", "[result]")
{
    auto src = TestResult::Err(FAIL);

    auto val = src.unwrap_or(2);

    SECTION("must return supplied value") {
        REQUIRE(val == 2);
    }
    SECTION("must invalidate src") {
        REQUIRE(src.is_inval());
    }
}

TEST_CASE("Result<>(inval).unwrap_or(ok2)", "[result]")
{
    auto src = TestResult::Err(FAIL);
    auto res2 = move(src);

    SECTION("must panic") {
        REQUIRE_PANIC(src.unwrap_or(2));
    }
}

TEST_CASE("Result<>(ok).unwrap_err_or(err2)", "[result]")
{
    auto src = TestResult::Ok(1);

    auto val = src.unwrap_err_or(FAIL);

    SECTION("must return the supplied value") {
        REQUIRE(val == FAIL);
    }
    SECTION("must invalidate src") {
        REQUIRE(src.is_inval());
    }
}

TEST_CASE("Result.unwrap_err_or for err must give err value", "[result]")
{
    auto src = TestResult::Err(FAIL);

    auto val = src.unwrap_err_or(NOENT);

    SECTION("must return the wrapped value") {
        REQUIRE(val == FAIL);
    }
    SECTION("must invalidate src") {
        REQUIRE(src.is_inval());
    }
}

TEST_CASE("Result<>(inval).unwrap_err_or()", "[result]")
{
    auto res = TestResult::Err(FAIL);
    auto res2 = move(res);

    SECTION("must panic") {
        REQUIRE_PANIC(res.unwrap_err_or(NOENT));
    }
}


bool map1(int const &e)
{
    NEL_UNUSED(e);
    return true;
}

TEST_CASE("Result<>(ok).map()", "[result]")
{
    auto src = TestResult::Ok(1);

    // TODO: nasty, want bool to be auto-inferred as it's the return type of
    // map1, i.e. want res.map(map1).unwrap();
    nel::Result<bool, _Error> res2 = src.map<bool>(map1);

    SECTION("remaps value") {
        auto val = res2.unwrap();
        REQUIRE(val == true);
    }
    SECTION("invalidates src") {
        REQUIRE(src.is_inval());
    }
    // auto res3 = res.map<bool>([](int const &e)->bool {return e==1;});
    // auto val2 = res3.unwrap();
    // REQUIRE(val2 == true);
}

TEST_CASE("Result<>(err).map()", "[result]")
{
    auto src = TestResult::Err(FAIL);

    // TODO: nasty, want bool to be auto-inferred as it's the return type of
    // map1, i.e. want res.map(map1).unwrap();
    auto res2 = src.map<bool>(map1);

    SECTION("does not change value") {
        auto val = res2.unwrap_err();
        REQUIRE(val == FAIL);
    }
    SECTION("invalidates src") {
        REQUIRE(src.is_inval());
    }
}

TEST_CASE("Result<>(inval).map()", "[result]")
{
    auto src = TestResult::Err(FAIL);
    auto res2 = move(src);

    SECTION("must panic") {
        // TODO: nasty, want bool to be auto-inferred as it's the return type of
        // map1, i.e. want res.map(map1).unwrap();
        REQUIRE_PANIC(src.map<bool>(map1));
    }
}

const char *bar = "bar";

const char *map2(const Error &e)
{
    NEL_UNUSED(e);
    return bar;
}

TEST_CASE("Result<>(ok).map_err() for ok must not change ok val", "[result]")
{
    auto src = TestResult::Ok(1);

    // TODO: nasty, want bool to be auto-inferred as it's the return type of
    // map1, i.e. want res.map(map1).unwrap();
    auto res2 = src.map_err<const char *>(map2);

    SECTION("must not change value") {
        auto val = res2.unwrap();
        REQUIRE(val == 1);
    }
    SECTION("invalidates src") {
        REQUIRE(src.is_inval());
    }
}

TEST_CASE("Result<>(err).map_err() for err must change err val", "[result]")
{
    auto src = TestResult::Err(FAIL);

    // TODO: nasty, want bool to be auto-inferred as it's the return type of
    // map1, i.e. want res.map(map1).unwrap();
    auto res2 = src.map_err<const char *>(map2);

    SECTION("remaps value") {
        auto val = res2.unwrap_err();
        REQUIRE(val == bar);
    }
    SECTION("invalidates src") {
        REQUIRE(src.is_inval());
    }
}

TEST_CASE("Result<>(inval).map_err for inval must stay inval", "[result]")
{
    auto src = TestResult::Err(FAIL);
    auto res2 = move(src);

    SECTION("must panic") {
        // TODO: nasty, want bool to be auto-inferred as it's the return type of
        // map1, i.e. want res.map(map1).unwrap();
        REQUIRE_PANIC(src.map_err<const char *>(map2));
    }
}


nel::Result<int, Error> foo_ok(void)
{
    return TestResult::Ok(1);
}

TEST_CASE("Result::Ok auto convert to Result", "[result]")
{
    auto res = foo_ok();

    REQUIRE(res.is_ok());
}

nel::Result<int, Error> foo_err(void)
{
    return TestResult::Err(FAIL);
}

TEST_CASE("Result::Err auto convert to Result", "[result]")
{
    auto res = foo_err();

    REQUIRE(res.is_err());
}

struct Foo
{
    public:
        ~Foo(void)
        {
            this->dtor_called = true;
        }

        Foo(bool &dtor_called)
            : dtor_called(dtor_called)
        {
        }

    private:
        bool &dtor_called;
};

TEST_CASE("result::~dtor for ok, must call ok dtor", "[result]")
{
    bool dtor_called = false;
    {
        nel::Result<Foo, Error> res1 = nel::Result<Foo, Error>::Ok(Foo(dtor_called));

        // REQUIRE(!bar);
        dtor_called = false;
    }
    REQUIRE(dtor_called);
}

#if 0
TEST_CASE("result::~dtor for ok, err dtor not called", "[result]")
{
// how do I do this?
}
#endif

TEST_CASE("result::~dtor for err, must call err dtor", "[result]")
{
    bool dtor_called = false;
    {
        nel::Result<int, Foo> res1 = nel::Result<int, Foo>::Err(Foo(dtor_called));

        // REQUIRE(!bar);
        dtor_called = false;
    }
    REQUIRE(dtor_called);
}

#if 0
TEST_CASE("result::~dtor for err ok dtor not called", "[result]")
{
// how do I do this?
}

TEST_CASE("result::~dtor for inval, ok not dtor called", "[result]")
{
// how do I do this?
}

TEST_CASE("result::~dtor for inval, err not dtor called", "[result]")
{
// how do I do this?
}
#endif

TEST_CASE("Result<>(ok).is_eq()", "[result]")
{
    auto src = TestResult::Ok(2);

    SECTION("must return true if eq to wrapped") {
        REQUIRE(src == TestResult::Ok(2));
        SECTION("must not alter src") {
            REQUIRE(src.unwrap() == 2);
        }
    }
    SECTION("must return false if not eq to wrapped") {
        REQUIRE(!(src == TestResult::Ok(3)));
        SECTION("must not alter src") {
            REQUIRE(src.unwrap() == 2);
        }
    }
    SECTION("must return false if comparing to err") {
        REQUIRE(!(src == TestResult::Err(FAIL)));
        SECTION("must not alter src") {
            REQUIRE(src.unwrap() == 2);
        }
    }
    SECTION("must panic if compared to an inval") {
        auto res = TestResult::Ok(2);
        auto res2 = move(res);
        nel::unused(res2);

        REQUIRE_PANIC(src == res);
    }
}

TEST_CASE("Result<>(err).is_eq()", "[result]")
{
    auto src = TestResult::Err(FAIL);

    SECTION("must return true if eq to wrapped") {
        REQUIRE(src == TestResult::Err(FAIL));
        SECTION("must not alter src") {
            REQUIRE(src.unwrap_err() == FAIL);
        }
    }
    SECTION("must return false if not eq to wrapped") {
        REQUIRE(!(src == TestResult::Err(NOENT)));
        SECTION("must not alter src") {
            REQUIRE(src.unwrap_err() == FAIL);
        }
    }
    SECTION("must return false if comparing to ok") {
        REQUIRE(!(src == TestResult::Ok(1)));
        SECTION("must not alter src") {
            REQUIRE(src.unwrap_err() == FAIL);
        }
    }
    SECTION("must panic if compared to an inval") {
        auto res = TestResult::Ok(2);
        auto res2 = move(res);
        nel::unused(res2);

        REQUIRE_PANIC(src == res);
    }
}

TEST_CASE("Result<>(inval).is_eq()", "[result]")
{
    auto src = TestResult::Err(FAIL);
    auto tmp1 = move(src);
    nel::unused(tmp1);

    SECTION("must panic if compared to an erer") {
        REQUIRE_PANIC(src == TestResult::Err(FAIL));
    }
    SECTION("must panic if compared to an ok") {
        REQUIRE_PANIC(src == TestResult::Ok(1));
    }
    SECTION("must panic if compared to an inval") {
        auto res = TestResult::Ok(2);
        auto res2 = move(res);
        nel::unused(res2);

        REQUIRE_PANIC(src == res);
    }
}


TEST_CASE("Result<>(ok).is_ne()", "[result]")
{
    auto src = TestResult::Ok(2);

    SECTION("must return false if eq to wrapped") {
        REQUIRE(!(src != TestResult::Ok(2)));
        SECTION("must not alter src") {
            REQUIRE(src.unwrap() == 2);
        }
    }
    SECTION("must return true if not eq to wrapped") {
        REQUIRE(src != TestResult::Ok(3));
        SECTION("must not alter src") {
            REQUIRE(src.unwrap() == 2);
        }
    }
    SECTION("must return true if comparing to err") {
        REQUIRE(src != TestResult::Err(FAIL));
        SECTION("must not alter src") {
            REQUIRE(src.unwrap() == 2);
        }
    }
    SECTION("must panic if compared to an inval") {
        auto res = TestResult::Ok(2);
        auto res2 = move(res);
        nel::unused(res2);

        REQUIRE_PANIC(src != res);
    }
}


TEST_CASE("Result<>(err).is_ne()", "[result]")
{
    auto src = TestResult::Err(FAIL);

    SECTION("must return false if eq to wrapped") {
        REQUIRE(!(src != TestResult::Err(FAIL)));
        SECTION("must not alter src") {
            REQUIRE(src.unwrap_err() == FAIL);
        }
    }
    SECTION("must return true if not eq to wrapped") {
        REQUIRE(src != TestResult::Err(NOENT));
        SECTION("must not alter src") {
            REQUIRE(src.unwrap_err() == FAIL);
        }
    }
    SECTION("must return true if comparing to ok") {
        REQUIRE(src != TestResult::Ok(1));
        SECTION("must not alter src") {
            REQUIRE(src.unwrap_err() == FAIL);
        }
    }
    SECTION("must panic if compared to an inval") {
        auto res = TestResult::Ok(2);
        auto res2 = move(res);
        nel::unused(res2);

        REQUIRE_PANIC(src != res);
    }
}

TEST_CASE("Result<>(inval).is_ne()", "[result]")
{
    auto src = TestResult::Err(FAIL);
    auto tmp1 = move(src);
    nel::unused(tmp1);

    SECTION("must panic if compared to an err") {
        REQUIRE_PANIC(src != TestResult::Err(FAIL));
    }
    SECTION("must panic if compared to an ok") {
        REQUIRE_PANIC(src != TestResult::Ok(1));
    }
    SECTION("must panic if compared to an inval") {
        auto res = TestResult::Ok(2);
        auto res2 = move(res);
        nel::unused(res2);

        REQUIRE_PANIC(src != res);
    }
}


}; // namespace result
}; // namespace test
}; // namespace nel
