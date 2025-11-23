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

TEST_CASE("Result()  must produce inval", "[result]")
{
    auto res = TestResult();

    auto is = res.is_inval();

    REQUIRE(is);
}

TEST_CASE("Result(Ok) for ok must produce ok", "[result]")
{
    auto res = TestResult(Ok(1));

    auto is_ok = res.is_ok();
    auto is_err = res.is_err();
    REQUIRE(is_ok);
    REQUIRE_FALSE(is_err);

    auto v = res.unwrap();
    REQUIRE(v == 1);
}

TEST_CASE("Result::Err for err must produce err", "[result]")
{
    auto res = TestResult(Err(FAIL));

    auto is_ok = res.is_ok();
    auto is_err = res.is_err();
    REQUIRE_FALSE(is_ok);
    REQUIRE(is_err);

    auto v = res.unwrap_err();
    REQUIRE(v == FAIL);
}

TEST_CASE("move(Result)-ctor for ok must move ok to dest", "[result]")
{
    auto res = TestResult(Ok(1));

    auto res2 = move(res);

    auto is_ok = res2.is_ok();
    auto is_err = res2.is_err();
    REQUIRE(is_ok);
    REQUIRE_FALSE(is_err);

    auto val = res2.unwrap();
    REQUIRE(val == 1);
}

TEST_CASE("move(Result)-ctor for err must move err to dest", "[result]")
{
    auto res = TestResult(Err(FAIL));

    auto res2 = move(res);

    auto is_ok = res2.is_ok();
    auto is_err = res2.is_err();
    REQUIRE_FALSE(is_ok);
    REQUIRE(is_err);

    auto val = res2.unwrap_err();
    REQUIRE(val == FAIL);
}

TEST_CASE("move(Result)-ctor for inval must move inval to dest", "[result]")
{
    auto res = TestResult();

    auto res2 = move(res);

    auto is = res2.is_inval();
    REQUIRE(is);
}

TEST_CASE("move(Result)-ctor for ok must invalidate src", "[result]")
{
    auto res = TestResult(Ok(1));

    auto res2 = move(res);

    auto is = res.is_inval();
    REQUIRE(is);
}

TEST_CASE("move(Result)-ctor for err must invalidate src", "[result]")
{
    auto res = TestResult(Err(FAIL));

    auto res2 = move(res);

    auto is = res.is_inval();
    REQUIRE(is);
}

TEST_CASE("move(Result)-ctor for inval must invalidate src", "[result]")
{
    auto res = TestResult();

    auto res2 = move(res);

    auto is = res.is_inval();
    REQUIRE(is);
}

TEST_CASE("move(Result)-ass for ok must move ok val to dest", "[result]")
{
    auto res = TestResult(Ok(1));

    auto res2 = TestResult(Ok(2));

    res2 = move(res);

    auto is_ok = res2.is_ok();
    auto is_err = res2.is_err();
    REQUIRE(is_ok);
    REQUIRE_FALSE(is_err);

    auto val = res2.unwrap();
    REQUIRE(val == 1);
}

TEST_CASE("move(Result)-ass for err must move err val to dest", "[result]")
{
    auto res = TestResult(Err(FAIL));

    auto res2 = TestResult(Ok(2));

    res2 = move(res);

    auto is_ok = res2.is_ok();
    auto is_err = res2.is_err();
    REQUIRE_FALSE(is_ok);
    REQUIRE(is_err);

    auto val = res2.unwrap_err();
    REQUIRE(val == FAIL);
}

TEST_CASE("move(Result)-ass for inval must move inval to dest", "[result]")
{
    // this propagation of inval, which is ok since it's not being
    // compared against.
    auto res1 = TestResult();
    auto res2 = TestResult(Ok(2));

    res2 = move(res1);

    auto is = res2.is_inval();
    REQUIRE(is);
}

TEST_CASE("move(Result)-ass for ok must invalidate src", "[result]")
{
    auto res = TestResult(Ok(1));
    auto res2 = TestResult(Ok(2));

    res2 = move(res);

    auto is = res.is_inval();
    REQUIRE(is);
}

TEST_CASE("move(Result)-ass for err must invalidate src", "[result]")
{
    auto res = TestResult(Err(FAIL));
    auto res2 = TestResult(Ok(2));

    res2 = move(res);

    auto is = res.is_inval();
    REQUIRE(is);
}

TEST_CASE("move(Result)-ass for inval must invalidate src", "[result]")
{
    // this propagation of inval, which is ok since it's not being
    // compared against.
    auto res = TestResult();
    auto res2 = TestResult(Ok(2));

    res2 = move(res);

    auto is = res.is_inval();
    REQUIRE(is);
}


TEST_CASE("Result.ok for ok must produce some containing ok val", "[result]")
{
    auto res = TestResult(Ok(1));
    auto val = res.ok();

    auto is_some = val.is_some();
    REQUIRE(is_some);

    auto v = val.unwrap();
    REQUIRE(v == 1);
}

TEST_CASE("Result.ok for err must produce none", "[result]")
{
    auto res = TestResult(Err(FAIL));
    auto val = res.ok();

    auto is_none = val.is_none();
    REQUIRE(is_none);
}

TEST_CASE("Result.ok for ok must invalidate src", "[result]")
{
    auto res = TestResult(Ok(1));

    auto val = res.ok();
    NEL_UNUSED(val);

    auto is = res.is_inval();
    REQUIRE(is);
}

TEST_CASE("Result.ok for err must invalidate src", "[result]")
{
    auto res = TestResult(Err(FAIL));

    auto val = res.ok();
    NEL_UNUSED(val);

    auto is = res.is_inval();
    REQUIRE(is);
}

TEST_CASE("Result.ok for inval must panic", "[result]")
{
    auto res = TestResult();

    REQUIRE_PANIC(res.ok());
}

TEST_CASE("Result.err for ok must produce none", "[result]")
{
    auto res = TestResult(Ok(1));

    auto val = res.err();

    auto is_none = val.is_none();
    REQUIRE(is_none);
}

TEST_CASE("Result.err for err must produce some containing err val", "[result]")
{
    auto res = TestResult(Err(FAIL));

    auto val = res.err();

    auto is_some = val.is_some();
    REQUIRE(is_some);

    auto v = val.unwrap();
    REQUIRE(v == FAIL);
}

TEST_CASE("Result.err for ok must invalidate src", "[result]")
{
    auto res = TestResult(Ok(1));

    auto val = res.err();
    NEL_UNUSED(val);

    auto is = res.is_inval();
    REQUIRE(is);
}

TEST_CASE("Result.err for err must invalidate src", "[result]")
{
    auto res = TestResult(Err(FAIL));

    auto val = res.err();
    NEL_UNUSED(val);

    auto is = res.is_inval();
    REQUIRE(is);
}

TEST_CASE("Result.err for inval must panic", "[result]")
{
    auto res = TestResult();

    REQUIRE_PANIC(res.err());
}

TEST_CASE("Result.is_ok for ok must give true", "[result]")
{
    auto res = TestResult(Ok(1));

    auto is = res.is_ok();
    REQUIRE(is);
}

TEST_CASE("Result.is_ok for err must give false", "[result]")
{
    auto res = TestResult(Err(FAIL));

    auto is = res.is_ok();
    REQUIRE_FALSE(is);
}

TEST_CASE("Result.is_ok for inval must panic", "[result]")
{
    auto res = TestResult();

    // must panic here..
    // why?
    REQUIRE_PANIC(res.is_ok());
}

TEST_CASE("Result.is_ok for ok must not alter src", "[result]")
{
    auto res = TestResult(Ok(1));

    auto is_ok = res.is_ok();
    NEL_UNUSED(is_ok);

    auto v = res.unwrap();
    REQUIRE(v == 1);
}

TEST_CASE("Result.is_ok for err must not alter src", "[result]")
{
    auto res = TestResult(Err(FAIL));

    auto is_ok = res.is_ok();
    NEL_UNUSED(is_ok);

    auto v = res.unwrap_err();
    REQUIRE(v == FAIL);
}

TEST_CASE("Result.is_err for ok must give false", "[result]")
{
    auto res = TestResult(Ok(1));

    auto is = res.is_err();
    REQUIRE_FALSE(is);
}

TEST_CASE("Result.is_err for err must give true", "[result]")
{
    auto res = TestResult(Err(FAIL));

    auto is = res.is_err();
    REQUIRE(is);
}

TEST_CASE("Result.is_err for inval must panic", "[result]")
{
    auto res = TestResult();

    REQUIRE_PANIC(res.is_err());

}

TEST_CASE("Result.unwrap for Ok must give ok value", "[result]")
{
    auto res = TestResult(Ok(1));

    auto val = res.unwrap();

    REQUIRE(val == 1);
}

TEST_CASE("Result.unwrap for Ok must invalidate src", "[result]")
{
    auto res = TestResult(Ok(1));

    auto val = res.unwrap();
    NEL_UNUSED(val);

    REQUIRE(res.is_inval());
}

TEST_CASE("Result.unwrap for err must panic", "[result]")
{
    auto res = TestResult(Err(FAIL));

    REQUIRE_PANIC(res.unwrap());
}

TEST_CASE("Result.unwrap for inval must panic", "[result]")
{
    auto res = TestResult();

    REQUIRE_PANIC(res.unwrap());
}

TEST_CASE("Result.unwrap_err for err must give err value", "[result]")
{
    auto res = TestResult(Err(FAIL));

    auto val = res.unwrap_err();

    REQUIRE(val == FAIL);
}

TEST_CASE("Result.unwrap_err for err must invalidate src", "[result]")
{
    auto res = TestResult(Err(FAIL));

    auto val = res.unwrap_err();
    NEL_UNUSED(val);

    auto is = res.is_inval();
    REQUIRE(is);
}

TEST_CASE("Result.unwrap_err for ok must panic", "[result]")
{
    auto res = TestResult(Ok(1));

    REQUIRE_PANIC(res.unwrap_err());
}


TEST_CASE("Result.unwrap_err for inval must panic", "[result]")
{
    auto res = TestResult();

    REQUIRE_PANIC(res.unwrap());
}

TEST_CASE("Result.unwrap_or for ok must give ok value", "[result]")
{
    auto res = TestResult(Ok(1));

    auto val = res.unwrap_or(2);

    REQUIRE(val == 1);
}

TEST_CASE("Result.unwrap_or for err must give or value", "[result]")
{
    auto res = TestResult(Err(FAIL));

    auto val = res.unwrap_or(2);

    REQUIRE(val == 2);
}

TEST_CASE("Result.unwrap_or for invalid must panic", "[result]")
{
    auto res = TestResult();

    REQUIRE_PANIC(res.unwrap_or(2));
}

TEST_CASE("Result.unwrap_or for ok must invalidate src", "[result]")
{
    auto res = TestResult(Ok(1));

    auto val = res.unwrap_or(2);
    NEL_UNUSED(val);

    auto is = res.is_inval();
    REQUIRE(is);
}

TEST_CASE("Result.unwrap_or for err must invalidate src", "[result]")
{
    auto res = TestResult(Err(FAIL));

    auto val = res.unwrap_or(2);
    NEL_UNUSED(val);

    auto is = res.is_inval();
    REQUIRE(is);
}

TEST_CASE("Result.unwrap_err_or for ok must give or value", "[result]")
{
    auto res = TestResult(Ok(1));

    auto val = res.unwrap_err_or(FAIL);

    REQUIRE(val == FAIL);
}

TEST_CASE("Result.unwrap_err_or for err must give err value", "[result]")
{
    auto res = TestResult(Err(FAIL));

    auto val = res.unwrap_err_or(NOENT);

    REQUIRE(val == FAIL);
}

TEST_CASE("Result.unwrap_err_or for invalid must panic", "[result]")
{
    auto res = TestResult();

    REQUIRE_PANIC(res.unwrap_err_or(NOENT));
}

TEST_CASE("Result.unwrap_err_or for ok must invalidate src", "[result]")
{
    auto res = TestResult(Ok(1));

    auto val = res.unwrap_err_or(FAIL);
    NEL_UNUSED(val);

    auto is = res.is_inval();
    REQUIRE(is);
}

TEST_CASE("Result.unwrap_err_or for err must invalidate src", "[result]")
{
    auto res = TestResult(Err(FAIL));

    auto val = res.unwrap_err_or(NOENT);
    NEL_UNUSED(val);

    auto is = res.is_inval();
    REQUIRE(is);
}

bool map1(int const &e)
{
    NEL_UNUSED(e);
    return true;
}

TEST_CASE("Result.map for ok must change ok val", "[result]")
{

    // TODO: nasty, want bool to be auto-inferred as it's the return type of
    // map1, i.e. want res.map(map1).unwrap();

    // using ptr-to-func
    SECTION("using ptr-to-func") {
        auto res = TestResult(Ok(1));
        //nel::Result<bool, _Error> res2 = res.map<bool>(map1);
        auto res2 = res.map<bool>(map1);
        auto val = res2.unwrap();
        REQUIRE(val);
    }

    SECTION("using closure") {
        // using closure
        auto res22 = TestResult(Ok(1));
        auto res23 = res22.map<bool>([](int const &e)->bool {return e == 1;});
        auto val22 = res23.unwrap();
        REQUIRE(val22);
    }
}

TEST_CASE("Result.map for err must not change err val", "[result]")
{

    // TODO: nasty, want bool to be auto-inferred as it's the return type of
    // map1, i.e. want res.map(map1).unwrap();

    SECTION("using ptr-to-func") {
        auto res = TestResult(Err(FAIL));
        auto res2 = res.map<bool>(map1);
        auto val = res2.unwrap_err();
        REQUIRE(val == FAIL);
    }

    SECTION("using closure") {
        auto res22 = TestResult(Err(FAIL));
        auto res23 = res22.map<bool>([](int const &e)->bool {return e == 1;});
        auto val22 = res23.unwrap_err();
        REQUIRE(val22 == FAIL);
    }
}

TEST_CASE("Result.map for inval must panic", "[result]")
{
    auto res = TestResult();

    // TODO: nasty, want bool to be auto-inferred as it's the return type of
    // map1, i.e. want res.map(map1).unwrap();
    REQUIRE_PANIC(res.map<bool>(map1));
}

TEST_CASE("Result.map for ok must invalidate src", "[result]")
{
    auto res = TestResult(Ok(1));

    // TODO: nasty, want bool to be auto-inferred as it's the return type of
    // map1, i.e. want res.map(map1).unwrap();
    auto res2 = res.map<bool>(map1);
    NEL_UNUSED(res2);

    auto is = res.is_inval();
    REQUIRE(is);
}

TEST_CASE("Result.map for err must invalidate src", "[result]")
{
    auto res = TestResult(Err(FAIL));

    // TODO: nasty, want bool to be auto-inferred as it's the return type of
    // map1, i.e. want res.map(map1).unwrap();
    auto res2 = res.map<bool>(map1);
    NEL_UNUSED(res2);

    auto is = res.is_inval();
    REQUIRE(is);
}

const char *bar = "bar";

const char *map2(const Error &e)
{
    NEL_UNUSED(e);
    return bar;
}

TEST_CASE("Result.map_err for ok must not change ok val", "[result]")
{
    auto res = TestResult(Ok(1));

    // TODO: nasty, want bool to be auto-inferred as it's the return type of
    // map1, i.e. want res.map(map1).unwrap();
    auto res2 = res.map_err<const char *>(map2);

    auto val = res2.unwrap();

    REQUIRE(val == 1);
}

TEST_CASE("Result.map_err for err must change err val", "[result]")
{
    auto res = TestResult(Err(FAIL));

    // TODO: nasty, want bool to be auto-inferred as it's the return type of
    // map1, i.e. want res.map(map1).unwrap();
    auto res2 = res.map_err<const char *>(map2);

    auto val = res2.unwrap_err();

    REQUIRE(val == bar);
}

TEST_CASE("Result.map_err for inval must panic", "[result]")
{
    auto res = TestResult();

    // TODO: nasty, want bool to be auto-inferred as it's the return type of
    // map1, i.e. want res.map(map1).unwrap();
    REQUIRE_PANIC(res.map_err<const char *>(map2));
}

TEST_CASE("Result.map_err for ok must invalidate src", "[result]")
{
    auto res = TestResult(Ok(1));

    // TODO: nasty, want bool to be auto-inferred as it's the return type of
    // map1, i.e. want res.map(map1).unwrap();
    auto res2 = res.map_err<const char *>(map2);
    NEL_UNUSED(res2);

    auto is = res.is_inval();
    REQUIRE(is);
}

TEST_CASE("Result.map_err for err must invalidate src", "[result]")
{
    auto res = TestResult(Err(FAIL));

    // TODO: nasty, want bool to be auto-inferred as it's the return type of
    // map1, i.e. want res.map(map1).unwrap();
    auto res2 = res.map_err<const char *>(map2);
    NEL_UNUSED(res2);

    auto is = res.is_inval();
    REQUIRE(is);
}


nel::Result<int, Error> foo_ok(void)
{
    return Ok(1);
}

TEST_CASE("Result::Ok auto convert to Result", "[result]")
{
    auto res = foo_ok();

    auto is = res.is_ok();
    REQUIRE(is);
}

nel::Result<int, Error> foo_err(void)
{
    return Err(FAIL);
}

TEST_CASE("Result::Err auto convert to Result", "[result]")
{
    auto res = foo_err();

    auto is = res.is_err();
    REQUIRE(is);
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
        nel::Result<Foo, Error> res1 = nel::Ok(Foo(dtor_called));

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
        nel::Result<int, Foo> res1 = Err(Foo(dtor_called));

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

TEST_CASE("Result::eq for ok with ok must give true", "[result]")
{
    auto res1 = TestResult(Ok(2));

    auto is = res1 == Ok(2);
    REQUIRE(is);
}


TEST_CASE("Result::eq for ok with diff ok must give false", "[result]")
{
    auto res1 = TestResult(Ok(2));

    auto is = res1 == Ok(3);
    REQUIRE_FALSE(is);
}


TEST_CASE("Result::eq for ok with err must give false", "[result]")
{
    auto res1 = TestResult(Ok(2));

    auto is = res1 == Err(FAIL);
    REQUIRE_FALSE(is);
}


TEST_CASE("Result::eq for ok with inval must panic", "[result]")
{
    auto res1 = TestResult(Ok(2));

    auto res2 = TestResult();

    REQUIRE_PANIC(res1 == res2);
}


TEST_CASE("Result::eq for ok with ok must not alter src", "[result]")
{
    auto res1 = TestResult(Ok(2));

    auto is_eq = res1 == Ok(2);
    NEL_UNUSED(is_eq);

    auto v = res1.unwrap();
    REQUIRE(v == 2);
}


TEST_CASE("Result::eq for ok with diff ok must not alter src", "[result]")
{
    auto res1 = TestResult(Ok(2));

    auto is_eq = res1 == Ok(3);
    NEL_UNUSED(is_eq);

    auto v = res1.unwrap();
    REQUIRE(v == 2);
}


TEST_CASE("Result::eq for ok with err must not alter src", "[result]")
{
    auto res1 = TestResult(Ok(2));

    auto is_eq = res1 == Err(FAIL);
    NEL_UNUSED(is_eq);

    auto v = res1.unwrap();
    REQUIRE(v == 2);
}



TEST_CASE("Result::eq for err with ok must give false", "[result]")
{
    auto res1 = TestResult(Err(FAIL));

    auto is = res1 == Ok(2);
    REQUIRE_FALSE(is);
}


TEST_CASE("Result::eq for err with err must give true", "[result]")
{
    auto res1 = TestResult(Err(FAIL));

    auto is = res1 == Err(FAIL);
    REQUIRE(is);
}


TEST_CASE("Result::eq for err with diff err must give false", "[result]")
{
    auto res1 = TestResult(Err(NOENT));

    auto is = res1 == Err(FAIL);
    REQUIRE_FALSE(is);
}


TEST_CASE("Result::eq for err with inval must panic", "[result]")
{
    auto res1 = TestResult(Err(FAIL));

    auto res2 = TestResult();

    REQUIRE_PANIC(res1 == res2);
}


TEST_CASE("Result::eq for err with ok must not alter src", "[result]")
{
    auto res1 = TestResult(Err(FAIL));

    auto is_eq = res1 == Ok(1);
    NEL_UNUSED(is_eq);

    auto v = res1.unwrap_err();
    REQUIRE(v == FAIL);
}


TEST_CASE("Result::eq for err with err must not alter src", "[result]")
{
    auto res1 = TestResult(Err(FAIL));

    auto is_eq = res1 == Err(FAIL);
    NEL_UNUSED(is_eq);

    auto v = res1.unwrap_err();
    REQUIRE(v == FAIL);
}


TEST_CASE("Result::eq for err with diff err must not alter src", "[result]")
{
    auto res1 = TestResult(Err(FAIL));

    auto is_eq = res1 == Err(NOENT);
    NEL_UNUSED(is_eq);

    auto v = res1.unwrap_err();
    REQUIRE(v == FAIL);
}


TEST_CASE("Result::eq for inval with ok must panic", "[result]")
{
    auto res1 = TestResult();

    REQUIRE_PANIC(res1 == Ok(1));
}


TEST_CASE("Result::eq for inval with err must panic", "[result]")
{
    auto res1 = TestResult();

    REQUIRE_PANIC(res1 == Err(FAIL));
}


TEST_CASE("Result::eq for inval with inval must panic", "[result]")
{
    auto res1 = TestResult();

    auto res2 = TestResult();

    REQUIRE_PANIC(res1 == res2);
}

TEST_CASE("Result::neq for ok with ok must give false", "[result]")
{
    auto res1 = TestResult(Ok(2));

    auto is = res1 != Ok(2);
    REQUIRE_FALSE(is);
}


TEST_CASE("Result::neq for ok with diff ok must give true", "[result]")
{
    auto res1 = TestResult(Ok(2));

    auto is = res1 != Ok(3);
    REQUIRE(is);
}


TEST_CASE("Result::neq for ok with err must give true", "[result]")
{
    auto res1 = TestResult(Ok(2));

    auto is = res1 != Err(FAIL);
    REQUIRE(is);
}


TEST_CASE("Result::neq for ok with inval must panic", "[result]")
{
    auto res1 = TestResult(Ok(2));

    auto res2 = TestResult();

    REQUIRE_PANIC(res1 != res2);
}


TEST_CASE("Result::neq for ok with ok must not alter src", "[result]")
{
    auto res1 = TestResult(Ok(2));

    auto is_neq = res1 != Ok(2);
    NEL_UNUSED(is_neq);

    auto v = res1.unwrap();
    REQUIRE(v == 2);
}


TEST_CASE("Result::neq for ok with diff ok must not alter src", "[result]")
{
    auto res1 = TestResult(Ok(2));

    auto is_neq = res1 != Ok(3);
    NEL_UNUSED(is_neq);

    auto v = res1.unwrap();
    REQUIRE(v == 2);
}


TEST_CASE("Result::neq for ok with err must not alter src", "[result]")
{
    auto res1 = TestResult(Ok(2));

    auto is_neq = res1 != Err(FAIL);
    NEL_UNUSED(is_neq);

    auto v = res1.unwrap();
    REQUIRE(v == 2);
}


TEST_CASE("Result::neq for err with ok must give true", "[result]")
{
    auto res1 = TestResult(Err(FAIL));

    auto is = res1 != Ok(1);
    REQUIRE(is);
}


TEST_CASE("Result::neq for err with err must give false", "[result]")
{
    auto res1 = TestResult(Err(FAIL));

    auto is = res1 != Err(FAIL);
    REQUIRE_FALSE(is);
}


TEST_CASE("Result::neq for err with diff err must give true", "[result]")
{
    auto res1 = TestResult(Err(NOENT));

    auto is = res1 != Err(FAIL);
    REQUIRE(is);
}


TEST_CASE("Result::neq for err with inval must panic", "[result]")
{
    auto res1 = TestResult(Err(FAIL));

    auto res2 = TestResult();

    REQUIRE_PANIC(res1 != res2);
}


TEST_CASE("Result::neq for err with ok must not alter src", "[result]")
{
    auto res1 = TestResult(Err(FAIL));

    auto is_neq = res1 != Ok(1);
    NEL_UNUSED(is_neq);

    auto v = res1.unwrap_err();
    REQUIRE(v == FAIL);
}


TEST_CASE("Result::neq for err with err must not alter src", "[result]")
{
    auto res1 = TestResult(Err(FAIL));

    auto is_neq = res1 != Err(FAIL);
    NEL_UNUSED(is_neq);

    auto v = res1.unwrap_err();
    REQUIRE(v == FAIL);
}


TEST_CASE("Result::neq for err with diff err must not alter src", "[result]")
{
    auto res1 = TestResult(Err(FAIL));

    auto is_neq = res1 != Err(NOENT);
    NEL_UNUSED(is_neq);

    auto v = res1.unwrap_err();
    REQUIRE(v == FAIL);
}


TEST_CASE("Result::neq for inval with ok must panic", "[result]")
{
    auto res1 = TestResult();

    REQUIRE_PANIC(res1 != Ok(1));
}


TEST_CASE("Result::neq for inval with err must panic", "[result]")
{
    auto res1 = TestResult();

    REQUIRE_PANIC(res1 != Err(FAIL));
}


TEST_CASE("Result::neq for inval with inval must give false", "[result]")
{
    auto res1 = TestResult();

    auto res2 = TestResult();

    REQUIRE_PANIC(res1 != res2);
}

}; // namespace result
}; // namespace test
}; // namespace nel
