
#include "result.hh" //nel::ResultT

#include <catch2/catch.hpp>

typedef enum _Error { NOERROR = 0, EFAIL, EENOENT } Error;

bool map1(const int &e) {
    return true;
}
TEST_CASE("Result::Ok works", "[optional]") {
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Ok(1));

    REQUIRE(res.is_ok());
    REQUIRE(!res.is_err());
    REQUIRE(res.ok().is_some());
    REQUIRE(res.err().is_none());
    REQUIRE(res.unwrap() == 1);
    REQUIRE(res.unwrap_or(2) == 1);
    // TODO: nasty, want bool to be auto-inferred.
    REQUIRE(res.map<bool>(map1).unwrap());
}

const char *bar = "bar";
const char *map2(const Error &e) {
    return bar;
}
TEST_CASE("Result::Err works", "[optional]") {
    auto res = nel::Result<int, Error>(nel::Result<int, Error>::Err(EFAIL));

    REQUIRE(!res.is_ok());
    REQUIRE(res.is_err());
    REQUIRE(res.ok().is_none());
    REQUIRE(res.err().is_some());
    REQUIRE(res.unwrap_err() == EFAIL);
    REQUIRE(res.unwrap_or(2) == 2);
    // TODO: nasty, want const char * to be auto-inferred.
    REQUIRE(res.map_err<const char *>(map2).unwrap_err() == bar);
}

struct Foo {
  public:
    ~Foo(void) {
        *this->dtor_called = true;
    }
    Foo(bool *dtor_called): dtor_called(dtor_called) {}

  private:
    bool *dtor_called;
};

TEST_CASE("result::ok dtor called", "[optional]") {
    bool bar = false;
    {
        nel::Result<Foo, Error> res1 = nel::Result<Foo, Error>::Ok(Foo(&bar));

        // REQUIRE(!bar);
        bar == false;
    }
    REQUIRE(bar);
}

TEST_CASE("result::err dtor called", "[optional]") {
    bool bar = false;
    {
        nel::Result<int, Foo> res1 = nel::Result<int, Foo>::Err(Foo(&bar));

        // REQUIRE(!bar);
        bar == false;
    }
    REQUIRE(bar);
}

TEST_CASE("Result::Ok equality works", "[optional]") {
    auto res1 = nel::Result<int, Error>(nel::Result<int, Error>::Ok(2));
    auto res2 = nel::Result<int, Error>(nel::Result<int, Error>::Ok(2));
    auto res3 = nel::Result<int, Error>(nel::Result<int, Error>::Ok(3));

    REQUIRE(res1.unwrap() == 2);
    REQUIRE(res2.unwrap() == 2);
    REQUIRE(res3.unwrap() == 3);

    REQUIRE(res1 == nel::Result<int, Error>::Ok(2));
    REQUIRE(res1 != nel::Result<int, Error>::Ok(3));
}

TEST_CASE("Result::Err equality works", "[optional]") {
    auto res1 = nel::Result<bool, int>(nel::Result<bool, int>::Err(2));
    auto res2 = nel::Result<bool, int>(nel::Result<bool, int>::Err(2));
    auto res3 = nel::Result<bool, int>(nel::Result<bool, int>::Err(3));

    REQUIRE(res1.unwrap_err() == 2);
    REQUIRE(res2.unwrap_err() == 2);
    REQUIRE(res3.unwrap_err() == 3);

    REQUIRE(res1 == nel::Result<bool, int>::Err(2));
    REQUIRE(res1 != nel::Result<bool, int>::Err(3));
}

TEST_CASE("Result equality works for ok", "[optional]") {
    auto res1 = nel::Result<bool, int>(nel::Result<bool, int>::Ok(true));
    auto res2 = nel::Result<bool, int>(nel::Result<bool, int>::Ok(true));
    auto res3 = nel::Result<bool, int>(nel::Result<bool, int>::Ok(false));

    REQUIRE(res1.unwrap());
    REQUIRE(res2.unwrap());
    REQUIRE(!res3.unwrap());

    REQUIRE(res1 == res2);
    REQUIRE(res1 != res3);
}

TEST_CASE("Result equality works for err", "[optional]") {
    auto res1 = nel::Result<bool, int>(nel::Result<bool, int>::Err(2));
    auto res2 = nel::Result<bool, int>(nel::Result<bool, int>::Err(2));
    auto res3 = nel::Result<bool, int>(nel::Result<bool, int>::Err(3));

    REQUIRE(res1.unwrap_err() == 2);
    REQUIRE(res2.unwrap_err() == 2);
    REQUIRE(res3.unwrap_err() == 3);

    REQUIRE(res1 == res2);
    REQUIRE(res1 != res3);
}
