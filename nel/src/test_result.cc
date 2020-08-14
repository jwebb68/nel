
#include "result.hh" //nel::ResultT

#include <catch2/catch.hpp>

#define UNUSED(arg) ((void)(arg))

typedef enum _Error { NOERROR = 0, FAIL, NOENT } Error;

TEST_CASE("Result::Ok for ok must produce ok", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Ok(1));

    REQUIRE(res.is_ok());
    REQUIRE(!res.is_err());

    REQUIRE(res.unwrap() == 1);
}
TEST_CASE("Result::Err for err must produce err", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));

    REQUIRE(!res.is_ok());
    REQUIRE(res.is_err());

    REQUIRE(res.unwrap_err() == FAIL);
}


TEST_CASE("std::move(Result)-ctor for ok must move ok to dest", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Ok(1));

    auto res2 = nel::Result<int, Error>(std::move(res));

    REQUIRE(res2.is_ok());
    REQUIRE(!res2.is_err());

    auto val = res2.unwrap();

    REQUIRE(val == 1);
}

TEST_CASE("std::move(Result)-ctor for err must move err to dest", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));

    auto res2 = nel::Result<int, Error>(std::move(res));

    REQUIRE(!res2.is_ok());
    REQUIRE(res2.is_err());

    auto val = res2.unwrap_err();

    REQUIRE(val == FAIL);
}

TEST_CASE("std::move(Result)-ctor for inval must move inval to dest",
          "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));
    auto res2 = nel::Result<int, Error>(std::move(res));
    auto res3 = nel::Result<int, Error>(std::move(res));

    REQUIRE(!res3.is_ok());
    REQUIRE(!res3.is_err());
}


TEST_CASE("std::move(Result)-ctor for ok must invalidate src", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Ok(1));
    auto res2 = nel::Result<int, Error>(std::move(res));

    REQUIRE(!res.is_ok());
    REQUIRE(!res.is_err());
}

TEST_CASE("std::move(Result)-ctor for err must invalidate src", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));
    auto res2 = nel::Result<int, Error>(std::move(res));

    REQUIRE(!res.is_ok());
    REQUIRE(!res.is_err());
}

TEST_CASE("std::move(Result)-ctor for inval must invalidate src", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));
    auto res2 = nel::Result<int, Error>(std::move(res));
    auto res3 = nel::Result<int, Error>(std::move(res));

    REQUIRE(!res.is_ok());
    REQUIRE(!res.is_err());
}


TEST_CASE("std::move(Result)-ass for ok must move ok val to dest", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Ok(1));
    auto res2 = nel::Result<int, Error>(nel::Result<int, Error>::Ok(2));

    res2 = std::move(res);

    REQUIRE(res2.is_ok());
    REQUIRE(!res2.is_err());

    auto val = res2.unwrap();
    REQUIRE(val == 1);
}

TEST_CASE("std::move(Result)-ass for err must move err val to dest",
          "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));
    auto res2 = nel::Result<int, Error>(nel::Result<int, Error>::Ok(2));

    res2 = std::move(res);

    REQUIRE(!res2.is_ok());
    REQUIRE(res2.is_err());

    auto val = res2.unwrap_err();
    REQUIRE(val == FAIL);
}

TEST_CASE("std::move(Result)-ass for inval must move inval to dest",
          "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));
    auto res2 = nel::Result<int, Error>(nel::Result<int, Error>::Ok(2));
    auto res3 = nel::Result<int, Error>(nel::Result<int, Error>::Ok(3));

    res2 = std::move(res);
    res3 = std::move(res);

    REQUIRE(!res.is_ok());
    REQUIRE(!res.is_err());
}


TEST_CASE("std::move(Result)-ass for ok must invalidate src", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Ok(1));
    auto res2 = nel::Result<int, Error>(nel::Result<int, Error>::Ok(2));

    res2 = std::move(res);

    REQUIRE(!res.is_ok());
    REQUIRE(!res.is_err());
}

TEST_CASE("std::move(Result)-ass for err must invalidate src", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));
    auto res2 = nel::Result<int, Error>(nel::Result<int, Error>::Ok(2));

    res2 = std::move(res);

    REQUIRE(!res.is_ok());
    REQUIRE(!res.is_err());
}

TEST_CASE("std::move(Result)-ass for inval must invalidate src", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));
    auto res2 = nel::Result<int, Error>(nel::Result<int, Error>::Ok(2));
    auto res3 = nel::Result<int, Error>(nel::Result<int, Error>::Ok(3));

    res2 = std::move(res);
    res3 = std::move(res);

    REQUIRE(!res.is_ok());
    REQUIRE(!res.is_err());
}


TEST_CASE("Result.ok for ok must produce ok val", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Ok(1));
    auto val = res.ok();

    REQUIRE(val.unwrap() == 1);
}

TEST_CASE("Result.ok for ok must not produce none", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Ok(1));
    auto val = res.ok();

    REQUIRE(val.is_some());
    REQUIRE(!val.is_none());
}

TEST_CASE("Result.ok for err must produce none", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));
    auto val = res.ok();

    REQUIRE(!val.is_some());
    REQUIRE(val.is_none());
}

TEST_CASE("Result.ok for inval must produce none", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));
    auto res2 = std::move(res);
    auto val = res.ok();

    REQUIRE(!val.is_some());
    REQUIRE(val.is_none());
}

TEST_CASE("Result.ok for ok must invalidate src", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Ok(1));

    auto val = res.ok();
    UNUSED(val);


    REQUIRE(!res.is_ok());
    REQUIRE(!res.is_err());
}

TEST_CASE("Result.ok for err must invalidate src", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));

    auto val = res.ok();
    UNUSED(val);


    REQUIRE(!res.is_ok());
    REQUIRE(!res.is_err());
}

TEST_CASE("Result.ok for inval must invalidate src", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));
    auto res2 = std::move(res);

    auto val = res.ok();
    UNUSED(val);


    REQUIRE(!res.is_ok());
    REQUIRE(!res.is_err());
}


TEST_CASE("Result.err for ok must produce none", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Ok(1));
    auto val = res.err();

    REQUIRE(val.is_none());
    REQUIRE(!val.is_some());
}

TEST_CASE("Result.err for err must produce err val", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));
    auto val = res.err();

    REQUIRE(val.unwrap() == FAIL);
}

TEST_CASE("Result.err for err must not produce none", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));
    auto val = res.err();

    REQUIRE(!val.is_none());
    REQUIRE(val.is_some());
}

TEST_CASE("Result.err for inval must produce none", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Ok(1));
    auto res2 = std::move(res);
    auto val = res.err();

    REQUIRE(val.is_none());
    REQUIRE(!val.is_some());
}

TEST_CASE("Result.err for ok must invalidate src", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Ok(1));

    auto val = res.err();
    UNUSED(val);

    REQUIRE(!res.is_ok());
    REQUIRE(!res.is_err());
}

TEST_CASE("Result.err for err must invalidate src", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));

    auto val = res.err();
    UNUSED(val);

    REQUIRE(!res.is_ok());
    REQUIRE(!res.is_err());
}

TEST_CASE("Result.err for inval must invalidate src", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));
    auto res2 = std::move(res);

    auto val = res.err();
    UNUSED(val);

    REQUIRE(!res.is_ok());
    REQUIRE(!res.is_err());
}


TEST_CASE("Result.is_ok for ok must give true", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Ok(1));

    REQUIRE(res.is_ok());
}

TEST_CASE("Result.is_ok for err must give false", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));

    REQUIRE(!res.is_ok());
}

TEST_CASE("Result.is_ok for inval must give false", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Ok(1));
    auto res2 = std::move(res);

    REQUIRE(!res.is_ok());
}


TEST_CASE("Result.is_ok for ok must not alter src", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Ok(1));

    auto is_ok = res.is_ok();
    UNUSED(is_ok);

    REQUIRE(res.unwrap() == 1);
}

TEST_CASE("Result.is_ok for err must not alter src", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));

    auto is_ok = res.is_ok();
    UNUSED(is_ok);

    REQUIRE(res.unwrap_err() == FAIL);
}

TEST_CASE("Result.is_ok for inval must not alter src", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Ok(1));
    auto res2 = std::move(res);

    auto is_ok = res.is_ok();
    UNUSED(is_ok);

    REQUIRE(!res.is_ok());
    REQUIRE(!res.is_err());
}




TEST_CASE("Result.is_err for ok must give false", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Ok(1));

    REQUIRE(!res.is_err());
}

TEST_CASE("Result.is_err for err must give true", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));

    REQUIRE(res.is_err());
}

TEST_CASE("Result.is_err for inval must give false", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Ok(1));
    auto res2 = std::move(res);

    REQUIRE(!res.is_err());
}


TEST_CASE("Result.unwrap for Ok must give ok value", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Ok(1));

    auto val = res.unwrap();

    REQUIRE(val == 1);
}

TEST_CASE("Result.unwrap for Ok must invalidate src", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Ok(1));

    auto val = res.unwrap();
    UNUSED(val);

    REQUIRE(!res.is_ok());
    REQUIRE(!res.is_err());
}

// TEST_CASE("Result.unwrap for err aborts", "[optional]")
// {
//     auto res = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));
//     auto val = res.unwrap();

//     // don't know how to test aborts
//     REQUIRE(true?);
// }

// TEST_CASE("Result.unwrap for inval aborts", "[optional]")
// {
//     auto res = nel::Result<int, Error>(nel::Result<int, Error>::Ok(1));
//     auto res2 = std::move(res);

//     auto val = res.unwrap();

//     // don't know how to test aborts
//     REQUIRE(true?);
// }

TEST_CASE("Result.unwrap_err for err must give err value", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));

    auto val = res.unwrap_err();

    REQUIRE(val == FAIL);
}

TEST_CASE("Result.unwrap_err for err must invalidate src", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));

    auto val = res.unwrap_err();
    UNUSED(val);

    REQUIRE(!res.is_ok());
    REQUIRE(!res.is_err());
}

// TEST_CASE("Result.unwrap_err for ok aborts", "[optional]")
// {
//     auto res = nel::Result<int, Error>(nel::Result<int, Error>::Ok(1));

//     auto val = res.unwrap_err();

//     // don't know how to test aborts
//     REQUIRE(true?);
// }

// TEST_CASE("Result.unwrap_err for inval aborts", "[optional]")
// {
//     auto res = nel::Result<int, Error>(nel::Result<int, Error>::Ok(1));
//     auto res2 = std::move(res);

//     auto val = res.unwrap();

//     // don't know how to test aborts
//     REQUIRE(true?);
// }

TEST_CASE("Result.unwrap_or for ok must give ok value", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Ok(1));

    auto val = res.unwrap_or(2);

    REQUIRE(val == 1);
}

TEST_CASE("Result.unwrap_or for err must give or value", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));

    auto val = res.unwrap_or(2);

    REQUIRE(val == 2);
}

TEST_CASE("Result.unwrap_or for invalid must give or value", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));
    auto res2 = std::move(res);

    auto val = res.unwrap_or(2);

    REQUIRE(val == 2);
}

TEST_CASE("Result.unwrap_or for ok must invalidate src", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Ok(1));

    auto val = res.unwrap_or(2);
    UNUSED(val);

    REQUIRE(!res.is_ok());
    REQUIRE(!res.is_err());
}

TEST_CASE("Result.unwrap_or for err must invalidate src", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));

    auto val = res.unwrap_or(2);
    UNUSED(val);

    REQUIRE(!res.is_ok());
    REQUIRE(!res.is_err());
}

TEST_CASE("Result.unwrap_or for invalid must stay invalid", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));
    auto res2 = std::move(res);

    auto val = res.unwrap_or(2);
    UNUSED(val);

    REQUIRE(!res.is_ok());
    REQUIRE(!res.is_err());
}


TEST_CASE("Result.unwrap_err_or for ok must give or value", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Ok(1));

    auto val = res.unwrap_err_or(FAIL);

    REQUIRE(val == FAIL);
}

TEST_CASE("Result.unwrap_err_or for err must give err value", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));

    auto val = res.unwrap_err_or(NOENT);

    REQUIRE(val == FAIL);
}

TEST_CASE("Result.unwrap_err_or for invalid must give or value", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));
    auto res2 = std::move(res);

    auto val = res.unwrap_err_or(NOENT);

    REQUIRE(val == NOENT);
}

TEST_CASE("Result.unwrap_err_or for ok must invalidate src", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Ok(1));

    auto val = res.unwrap_err_or(FAIL);
    UNUSED(val);

    REQUIRE(!res.is_ok());
    REQUIRE(!res.is_err());
}

TEST_CASE("Result.unwrap_err_or for err must invalidate src", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));

    auto val = res.unwrap_err_or(NOENT);
    UNUSED(val);

    REQUIRE(!res.is_ok());
    REQUIRE(!res.is_err());
}

TEST_CASE("Result.unwrap_err_or for invalid must invalidate src", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));
    auto res2 = std::move(res);

    auto val = res.unwrap_err_or(NOENT);
    UNUSED(val);

    REQUIRE(!res.is_ok());
    REQUIRE(!res.is_err());
}



bool map1(const int &e)
{
    UNUSED(e);
    return true;
}

TEST_CASE("Result.map for ok must change ok val", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Ok(1));

    // TODO: nasty, want bool to be auto-inferred as it's the return type of
    // map1, i.e. want res.map(map1).unwrap();
    auto res2 = res.map<bool>(map1);

    auto val = res2.unwrap();

    REQUIRE(val == true);
}

TEST_CASE("Result.map for err must not change err val", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));

    // TODO: nasty, want bool to be auto-inferred as it's the return type of
    // map1, i.e. want res.map(map1).unwrap();
    auto res2 = res.map<bool>(map1);

    auto val = res2.unwrap_err();

    REQUIRE(val == FAIL);
}

TEST_CASE("Result.map for inval must stay inval", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));
    auto res2 = std::move(res);

    // TODO: nasty, want bool to be auto-inferred as it's the return type of
    // map1, i.e. want res.map(map1).unwrap();
    auto res3 = res.map<bool>(map1);

    REQUIRE(!res3.is_ok());
    REQUIRE(!res3.is_err());
}


TEST_CASE("Result.map for ok must invalidate src", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Ok(1));

    // TODO: nasty, want bool to be auto-inferred as it's the return type of
    // map1, i.e. want res.map(map1).unwrap();
    auto res2 = res.map<bool>(map1);
    UNUSED(res2);

    REQUIRE(!res.is_ok());
    REQUIRE(!res.is_err());
}

TEST_CASE("Result.map for err must invalidate src", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));

    // TODO: nasty, want bool to be auto-inferred as it's the return type of
    // map1, i.e. want res.map(map1).unwrap();
    auto res2 = res.map<bool>(map1);
    UNUSED(res2);

    REQUIRE(!res.is_ok());
    REQUIRE(!res.is_err());
}

TEST_CASE("Result.map for inval must invalidate src", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));
    auto res2 = std::move(res);

    // TODO: nasty, want bool to be auto-inferred as it's the return type of
    // map1, i.e. want res.map(map1).unwrap();
    auto res3 = res.map<bool>(map1);
    UNUSED(res3);

    REQUIRE(!res.is_ok());
    REQUIRE(!res.is_err());
}



const char *bar = "bar";
const char *map2(const Error &e)
{
    UNUSED(e);
    return bar;
}


TEST_CASE("Result.map_err for ok must not change ok val", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Ok(1));

    // TODO: nasty, want bool to be auto-inferred as it's the return type of
    // map1, i.e. want res.map(map1).unwrap();
    auto res2 = res.map_err<const char *>(map2);

    auto val = res2.unwrap();

    REQUIRE(val == 1);
}

TEST_CASE("Result.map_err for err must change err val", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));

    // TODO: nasty, want bool to be auto-inferred as it's the return type of
    // map1, i.e. want res.map(map1).unwrap();
    auto res2 = res.map_err<const char *>(map2);

    auto val = res2.unwrap_err();

    REQUIRE(val == bar);
}

TEST_CASE("Result.map_err for inval must stay inval", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));
    auto res2 = std::move(res);

    // TODO: nasty, want bool to be auto-inferred as it's the return type of
    // map1, i.e. want res.map(map1).unwrap();
    auto res3 = res.map_err<const char *>(map2);

    REQUIRE(!res3.is_ok());
    REQUIRE(!res3.is_err());
}

TEST_CASE("Result.map_err for ok must invalidate src", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Ok(1));

    // TODO: nasty, want bool to be auto-inferred as it's the return type of
    // map1, i.e. want res.map(map1).unwrap();
    auto res2 = res.map_err<const char *>(map2);
    UNUSED(res2);

    REQUIRE(!res.is_ok());
    REQUIRE(!res.is_err());
}

TEST_CASE("Result.map_err for err must invalidate src", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));

    // TODO: nasty, want bool to be auto-inferred as it's the return type of
    // map1, i.e. want res.map(map1).unwrap();
    auto res2 = res.map_err<const char *>(map2);
    UNUSED(res2);

    REQUIRE(!res.is_ok());
    REQUIRE(!res.is_err());
}

TEST_CASE("Result.map_err for inval must invalidate src", "[optional]")
{
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));
    auto res2 = std::move(res);

    // TODO: nasty, want bool to be auto-inferred as it's the return type of
    // map1, i.e. want res.map(map1).unwrap();
    auto res3 = res.map_err<const char *>(map2);
    UNUSED(res3);

    REQUIRE(!res.is_ok());
    REQUIRE(!res.is_err());
}



nel::Result<int, Error> foo_ok(void)
{
    return nel::Result<int, Error>::Ok(1);
}
TEST_CASE("Result::Ok auto convert to Result", "[optional]")
{
    auto res = foo_ok();

    REQUIRE(res.is_ok());
}

nel::Result<int, Error> foo_err(void)
{
    return nel::Result<int, Error>::Err(FAIL);
}
TEST_CASE("Result::Err auto convert to Result", "[optional]")
{
    auto res = foo_err();

    REQUIRE(res.is_err());
}


struct Foo {
    public:
        ~Foo(void) {
            this->dtor_called = true;
        }
        Foo(bool &dtor_called): dtor_called(dtor_called) {
        }

    private:
        bool &dtor_called;
};

TEST_CASE("result::~dtor for ok, must call ok dtor", "[optional]")
{
    bool dtor_called = false;
    {
        nel::Result<Foo, Error> res1 = nel::Result<Foo, Error>::Ok(Foo(dtor_called));

        // REQUIRE(!bar);
        dtor_called = false;
    }
    REQUIRE(dtor_called);
}

// TEST_CASE("result::~dtor for ok, err dtor not called", "[optional]")
// {
// how do I do this?
// }

TEST_CASE("result::~dtor for err, must call err dtor", "[optional]")
{
    bool dtor_called = false;
    {
        nel::Result<int, Foo> res1 = nel::Result<int, Foo>::Err(Foo(dtor_called));

        // REQUIRE(!bar);
        dtor_called = false;
    }
    REQUIRE(dtor_called);
}

// TEST_CASE("result::~dtor for err ok dtor not called", "[optional]")
// {
// how do I do this?
// }

// TEST_CASE("result::~dtor for inval, ok not dtor called", "[optional]")
// {
// // how do I do this?
// }

// TEST_CASE("result::~dtor for inval, err not dtor called", "[optional]")
// {
// // how do I do this?
// }


TEST_CASE("Result::eq for ok with ok must give true", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Ok(2));

    REQUIRE(res1 == nel::Result<int, Error>::Ok(2));
}

TEST_CASE("Result::eq for ok with diff ok must give false", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Ok(2));

    REQUIRE(!(res1 == nel::Result<int, Error>::Ok(3)));
}

TEST_CASE("Result::eq for ok with err must give false", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Ok(2));

    REQUIRE(!(res1 == nel::Result<int, Error>::Err(FAIL)));
}

TEST_CASE("Result::eq for ok with inval must give false", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Ok(1));

    auto res2 = nel::Result<int, Error>(nel::Result<int, Error>::Ok(2));
    auto res3 = std::move(res2);

    REQUIRE(!(res1 == res2));
}


TEST_CASE("Result::eq for ok with ok must not alter src", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Ok(2));

    auto is_eq = res1 == nel::Result<int, Error>::Ok(2);
    UNUSED(is_eq);

    REQUIRE(res1.unwrap() == 2);
}

TEST_CASE("Result::eq for ok with diff ok must not alter src", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Ok(2));

    auto is_eq = res1 == nel::Result<int, Error>::Ok(3);
    UNUSED(is_eq);

    REQUIRE(res1.unwrap() == 2);
}

TEST_CASE("Result::eq for ok with err must not alter src", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Ok(2));

    auto is_eq = res1 == nel::Result<int, Error>::Err(FAIL);
    UNUSED(is_eq);

    REQUIRE(res1.unwrap() == 2);
}

TEST_CASE("Result::eq for ok with inval must not alter src", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Ok(2));

    auto res2 = nel::Result<int, Error>(nel::Result<int, Error>::Ok(1));
    auto res3 = std::move(res2);

    auto is_eq = res1 == res2;
    UNUSED(is_eq);

    REQUIRE(res1.unwrap() == 2);
}



TEST_CASE("Result::eq for err with ok must give false", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));

    REQUIRE(!(res1 == nel::Result<int, Error>::Ok(1)));
}

TEST_CASE("Result::eq for err with err must give true", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));

    REQUIRE(res1 == nel::Result<int, Error>::Err(FAIL));
}

TEST_CASE("Result::eq for err with diff err must give false", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Err(NOENT));

    REQUIRE(!(res1 == nel::Result<int, Error>::Err(FAIL)));
}

TEST_CASE("Result::eq for err with inval must give false", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));

    auto res2 = nel::Result<int, Error>(nel::Result<int, Error>::Err(NOENT));
    auto res3 = std::move(res2);

    REQUIRE(!(res1 == res2));
}


TEST_CASE("Result::eq for err with ok must not alter src", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));

    auto is_eq = res1 == nel::Result<int, Error>::Ok(1);
    UNUSED(is_eq);

    REQUIRE(res1.unwrap_err() == FAIL);
}

TEST_CASE("Result::eq for err with err must not alter src", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));

    auto is_eq = res1 == nel::Result<int, Error>::Err(FAIL);
    UNUSED(is_eq);

    REQUIRE(res1.unwrap_err() == FAIL);
}

TEST_CASE("Result::eq for err with diff err must not alter src", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));

    auto is_eq = res1 == nel::Result<int, Error>::Err(NOENT);
    UNUSED(is_eq);

    REQUIRE(res1.unwrap_err() == FAIL);
}

TEST_CASE("Result::eq for err with inval must not alter src", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));

    auto res2 = nel::Result<int, Error>(nel::Result<int, Error>::Err(NOENT));
    auto res3 = std::move(res2);

    auto is_eq = res1 == res2;
    UNUSED(is_eq);

    REQUIRE(res1.unwrap_err() == FAIL);
}



TEST_CASE("Result::eq for inval with ok must give false", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));
    auto res2 = std::move(res1);

    REQUIRE(!(res1 == nel::Result<int, Error>::Ok(1)));
}

TEST_CASE("Result::eq for inval with err must give false", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));
    auto res2 = std::move(res1);

    REQUIRE(!(res1 == nel::Result<int, Error>::Err(FAIL)));
}

TEST_CASE("Result::eq for inval with inval must give true", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Ok(2));
    auto res2 = std::move(res1);

    auto res3 = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));
    auto res4 = std::move(res3);

    REQUIRE(res1 == res3);
}


TEST_CASE("Result::eq for inval with ok must not alter src", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));
    auto res2 = std::move(res1);

    auto is_eq = res1 == nel::Result<int, Error>::Ok(1);
    UNUSED(is_eq);

    REQUIRE(!res1.is_ok());
    REQUIRE(!res1.is_err());
}

TEST_CASE("Result::eq for inval with err must not alter src", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));
    auto res2 = std::move(res1);

    auto is_eq = res1 == nel::Result<int, Error>::Err(FAIL);
    UNUSED(is_eq);

    REQUIRE(!res1.is_ok());
    REQUIRE(!res1.is_err());
}

TEST_CASE("Result::eq for inval with inval must not alter src", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Ok(2));
    auto res2 = std::move(res1);

    auto res3 = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));
    auto res4 = std::move(res3);

    auto is_eq = res1 == res3;
    UNUSED(is_eq);

    REQUIRE(!res1.is_ok());
    REQUIRE(!res1.is_err());
}


TEST_CASE("Result::neq for ok with ok must give false", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Ok(2));

    REQUIRE(!(res1 != nel::Result<int, Error>::Ok(2)));
}

TEST_CASE("Result::neq for ok with diff ok must give true", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Ok(2));

    REQUIRE(res1 != nel::Result<int, Error>::Ok(3));
}

TEST_CASE("Result::neq for ok with err must give true", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Ok(2));

    REQUIRE(res1 != nel::Result<int, Error>::Err(FAIL));
}

TEST_CASE("Result::neq for ok with inval must give true", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Ok(2));

    auto res2 = nel::Result<int, Error>(nel::Result<int, Error>::Ok(2));
    auto res3 = std::move(res2);

    REQUIRE(res1 != res2);
}


TEST_CASE("Result::neq for ok with ok must not alter src", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Ok(2));

    auto is_neq = res1 != nel::Result<int, Error>::Ok(2);
    UNUSED(is_neq);

    REQUIRE(res1.unwrap() == 2);
}

TEST_CASE("Result::neq for ok with diff ok must not alter src", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Ok(2));

    auto is_neq = res1 != nel::Result<int, Error>::Ok(3);
    UNUSED(is_neq);

    REQUIRE(res1.unwrap() == 2);
}

TEST_CASE("Result::neq for ok with err must not alter src", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Ok(2));

    auto is_neq = res1 != nel::Result<int, Error>::Err(FAIL);
    UNUSED(is_neq);

    REQUIRE(res1.unwrap() == 2);
}

TEST_CASE("Result::neq for ok with inval must not alter src", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Ok(2));

    auto res2 = nel::Result<int, Error>(nel::Result<int, Error>::Ok(3));
    auto res3 = std::move(res2);

    auto is_neq = res1 != res2;
    UNUSED(is_neq);

    REQUIRE(res1.unwrap() == 2);
}


TEST_CASE("Result::neq for err with ok must give true", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));

    REQUIRE(res1 != nel::Result<int, Error>::Ok(1));
}

TEST_CASE("Result::neq for err with err must give false", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));

    REQUIRE(!(res1 != nel::Result<int, Error>::Err(FAIL)));
}

TEST_CASE("Result::neq for err with diff err must give true", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Err(NOENT));

    REQUIRE(res1 != nel::Result<int, Error>::Err(FAIL));
}

TEST_CASE("Result::neq for err with inval must give true", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));

    auto res2 = nel::Result<int, Error>(nel::Result<int, Error>::Ok(2));
    auto res3 = std::move(res2);

    REQUIRE(res1 != res2);
}


TEST_CASE("Result::neq for err with ok must not alter src", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));

    auto is_neq = res1 != nel::Result<int, Error>::Ok(1);
    UNUSED(is_neq);

    REQUIRE(res1.unwrap_err() == FAIL);
}

TEST_CASE("Result::neq for err with err must not alter src", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));

    auto is_neq = res1 != nel::Result<int, Error>::Err(FAIL);
    UNUSED(is_neq);

    REQUIRE(res1.unwrap_err() == FAIL);
}

TEST_CASE("Result::neq for err with diff err must not alter src", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));

    auto is_neq = res1 != nel::Result<int, Error>::Err(NOENT);
    UNUSED(is_neq);

    REQUIRE(res1.unwrap_err() == FAIL);
}

TEST_CASE("Result::neq for err with inval must not alter src", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));

    auto res2 = nel::Result<int, Error>(nel::Result<int, Error>::Ok(2));
    auto res3 = std::move(res2);

    auto is_neq = res1 != res2;
    UNUSED(is_neq);

    REQUIRE(res1.unwrap_err() == FAIL);
}


TEST_CASE("Result::neq for inval with ok must give true", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Ok(1));
    auto res2 = std::move(res1);

    REQUIRE(res1 != nel::Result<int, Error>::Ok(1));
}

TEST_CASE("Result::neq for inval with err must give true", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));
    auto res2 = std::move(res1);

    REQUIRE(res1 != nel::Result<int, Error>::Err(FAIL));
}

TEST_CASE("Result::neq for inval with inval must give false", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Ok(2));
    auto res2 = std::move(res1);

    auto res3 = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));
    auto res4 = std::move(res3);

    REQUIRE(!(res1 != res3));
}


TEST_CASE("Result::neq for inval with ok must not alter src", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));
    auto res2 = std::move(res1);

    auto is_neq = res1 != nel::Result<int, Error>::Ok(1);
    UNUSED(is_neq);

    REQUIRE(!res1.is_ok());
    REQUIRE(!res1.is_err());
}

TEST_CASE("Result::neq for inval with err must not alter src", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));
    auto res2 = std::move(res1);

    auto is_neq = res1 != nel::Result<int, Error>::Err(FAIL);
    UNUSED(is_neq);

    REQUIRE(!res1.is_ok());
    REQUIRE(!res1.is_err());
}

TEST_CASE("Result::neq for inval with inval must not alter src", "[optional]")
{
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Ok(2));
    auto res2 = std::move(res1);

    auto res3 = nel::Result<int, Error>(nel::Result<int, Error>::Err(FAIL));
    auto res4 = std::move(res3);

    auto is_neq = res1 != res4;
    UNUSED(is_neq);

    REQUIRE(!res1.is_ok());
    REQUIRE(!res1.is_err());
}
