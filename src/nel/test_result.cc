// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include "result.hh" //nel::ResultT

#include <nel/defs.hh> // unused

#include <iostream>

namespace nel
{
namespace test
{
namespace result
{
struct Error;
std::ostream &operator<<(std::ostream &outs, Error const &v);
}
}
}
// because need Error declaring before including catch2 headers.
#include <catch2/catch.hpp>
#include <catch2/catch2_panic.hh>


namespace nel
{
namespace test
{
namespace result
{

struct Error {
    public:
        enum Tag {
            NOERROR =0,
            FAIL,
            NOENT,
        };
    private:
        Tag v_;
    public:
        constexpr Error(Tag const &t): v_(t) {}
        constexpr operator Tag() const {return v_;}
    public:
        constexpr char const *to_str() const {
            switch (v_) {
                case NOERROR:
                    return "Error::NOERROR";
                    break;
                case FAIL:
                    return "Error::FAIL";
                    break;
                case NOENT:
                    return "Error::NOENT";
                    break;
                default:
                    nel::panic();
            }
            return "??";
        }

        friend nel::Log &operator<<(nel::Log &outs, Error const &v) {
            outs << v.to_str();
            return outs;
        }
};

// addition for catch2.
std::ostream &operator<<(std::ostream &outs, Error const &v) {
    outs << v.to_str();
    return outs;
}

using TestResult = nel::Result<int, Error>;

TEST_CASE("Result() must produce inval", "[result]")
{
    auto res = TestResult();

    auto is = res.is_inval();

    REQUIRE(is);
}

TEST_CASE("Result(Ok) for ok must produce ok", "[result]")
{
    auto res = TestResult(Ok(1));

    SECTION("must mark as ok") {
        auto is_ok = res.is_ok();
        REQUIRE(is_ok);
    }

    SECTION("must produce value given") {
        auto v = res.unwrap();
        REQUIRE(v == 1);
    }
}

TEST_CASE("Result(Err) for err must produce err", "[result]")
{
    auto res = TestResult(Err(Error(Error::FAIL)));

    SECTION("must mark as err") {
        auto is_ok = res.is_ok();
        REQUIRE_FALSE(is_ok);
    }

    SECTION("must produce value given") {
        auto v = res.unwrap_err();
        REQUIRE(v == Error(Error::FAIL));
    }
}

TEST_CASE("move(Result)-ctor for ok must move ok to dest", "[result]")
{
    auto res = TestResult(Ok(1));

    auto res2 = move(res);

    SECTION("dest must now be an ok") {
        auto is_ok = res2.is_ok();
        REQUIRE(is_ok);
    }

    SECTION("value must be moved") {
        auto val = res2.unwrap();
        REQUIRE(val == 1);
    }

    SECTION("src must be invalidated") {
        auto is_inval = res.is_inval();
        REQUIRE(is_inval);
    }

    // TODO: value wrapped must not be destructed.
}

TEST_CASE("move(Result)-ctor for err must move err to dest", "[result]")
{
    auto res = TestResult(Err(Error(Error::FAIL)));

    auto res2 = move(res);

    SECTION("dest must now be an err") {
        auto is_ok = res2.is_ok();
        REQUIRE_FALSE(is_ok);
    }

    SECTION("value must be moved") {
        auto val = res2.unwrap_err();
        REQUIRE(val == Error(Error::FAIL));
    }

    SECTION("src must be invalidated") {
        auto is_inval = res.is_inval();
        REQUIRE(is_inval);
    }

    // TODO: value wrapped must not be destructed.
}

TEST_CASE("move(Result)-ctor for inval must move inval to dest", "[result]")
{
    auto res = TestResult();

    auto res2 = move(res);

    SECTION("dest must now be an inval") {
        auto is = res2.is_inval();
        REQUIRE(is);
    }

    SECTION("src must (still) be invalidated") {
        auto is_inval = res.is_inval();
        REQUIRE(is_inval);
    }
}

TEST_CASE("move(Result)-ass for ok must move ok val to dest", "[result]")
{
    auto res = TestResult(Ok(1));

    // TODO: test dest has ok,err,inval
    auto res2 = TestResult();

    res2 = move(res);

    SECTION("dest must now be an ok") {
        auto is_ok = res2.is_ok();
        REQUIRE(is_ok);
    }

    SECTION("dest must have src old value") {
        auto val = res2.unwrap();
        REQUIRE(val == 1);
    }

    SECTION("src must be invalidated") {
        auto is_inval = res.is_inval();
        REQUIRE(is_inval);
    }

    // TODO: value wrapped must not be destructed.
}

TEST_CASE("move(Result)-ass for err must move err val to dest", "[result]")
{
    auto res = TestResult(Err(Error(Error::FAIL)));

    auto res2 = TestResult();

    res2 = move(res);

    SECTION("dest must now be an err") {
        auto is_ok = res2.is_ok();
        REQUIRE_FALSE(is_ok);
    }

    SECTION("dest must have src old value") {
        auto val = res2.unwrap_err();
        REQUIRE(val == Error(Error::FAIL));
    }

    SECTION("src must be invalidated") {
        auto is_inval = res.is_inval();
        REQUIRE(is_inval);
    }

    // TODO: value wrapped must be moved once (into res2).
}

TEST_CASE("move(Result)-ass for inval must move inval to dest", "[result]")
{
    // this propagation of inval, which is ok since it's not being
    // compared against.
    auto res1 = TestResult();

    auto res2 = TestResult(Ok(2));

    res2 = move(res1);

    SECTION("dest must now be an inval") {
        auto is = res2.is_inval();
        REQUIRE(is);
    }

    SECTION("src must be invalidated") {
        auto is_inval = res1.is_inval();
        REQUIRE(is_inval);
    }
}

TEST_CASE("Result.ok for ok must produce some containing ok val", "[result]")
{
    auto res = TestResult(Ok(1));
    auto val = res.ok();

    SECTION("must produce an Optional(Some())") {
        auto is_some = val.is_some();
        REQUIRE(is_some);
    }

    SECTION("result must hold result's value") {
        // TODO: value wrapped must be moved once (into val).

        auto v = val.unwrap();
        REQUIRE(v == 1);
    }

    SECTION("src must be invalidated") {
        auto is_inval = res.is_inval();
        REQUIRE(is_inval);
    }
}

TEST_CASE("Result.ok for err must produce none", "[result]")
{
    auto res = TestResult(Err(Error(Error::FAIL)));

    auto val = res.ok();

    SECTION("must produce a None") {
        auto is_none = val.is_none();
        REQUIRE(is_none);
    }

    SECTION("src must be invalidated") {
        auto is_inval = res.is_inval();
        REQUIRE(is_inval);
    }

    // TODO: value wrapped must be destructed.
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

    SECTION("must produce an Optional(None)") {
        auto is_none = val.is_none();
        REQUIRE(is_none);
    }

    SECTION("src must be invalidated") {
        auto is_inval = res.is_inval();
        REQUIRE(is_inval);
    }

    // TODO: value wrapped must be destructed.
}

TEST_CASE("Result.err for err must produce some containing err val", "[result]")
{
    auto res = TestResult(Err(Error(Error::FAIL)));

    auto val = res.err();

    SECTION("must produce a Some()") {
        auto is_some = val.is_some();
        REQUIRE(is_some);
    }

    SECTION("result must contain original value") {
        // TODO: value wrapped must be moved once (into val).

        auto v = val.unwrap();
        REQUIRE(v == Error(Error::FAIL));
    }

    SECTION("src must be invalidated") {
        auto is_inval = res.is_inval();
        REQUIRE(is_inval);
    }
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

    SECTION("must give true") {
        REQUIRE(is);
    }

    SECTION("must stay an ok") {
        auto is_ok = res.is_ok();
        REQUIRE(is_ok);
    }

    SECTION("must not alter the value") {
        auto v = res.unwrap();
        REQUIRE(v == 1);
    }
}

TEST_CASE("Result.is_ok for err must give false", "[result]")
{
    auto res = TestResult(Err(Error(Error::FAIL)));

    auto is = res.is_ok();

    SECTION("must give false") {
        REQUIRE_FALSE(is);
    }

    SECTION("must stay an err") {
        auto is_err = res.is_err();
        REQUIRE(is_err);
    }

    SECTION("must not alter the value") {
        auto v = res.unwrap_err();
        REQUIRE(v == Error(Error::FAIL));
    }
}

TEST_CASE("Result.is_ok for inval must panic", "[result]")
{
    auto res = TestResult();

    // must panic here..
    // why?
    REQUIRE_PANIC(res.is_ok());
}

TEST_CASE("Result.is_err for ok must give false", "[result]")
{
    auto res = TestResult(Ok(1));

    auto is = res.is_err();

    SECTION("must give false") {
        REQUIRE_FALSE(is);
    }

    SECTION("must stay an ok") {
        auto is_ok = res.is_ok();
        REQUIRE(is_ok);
    }

    SECTION("must not alter the value") {
        auto v = res.unwrap();
        REQUIRE(v == 1);
    }
}

TEST_CASE("Result.is_err for err must give true", "[result]")
{
    auto res = TestResult(Err(Error(Error::FAIL)));

    auto is = res.is_err();

    SECTION("must give true") {
        REQUIRE(is);
    }

    SECTION("must stay an err") {
        auto is_err = res.is_err();
        REQUIRE(is_err);
    }

    SECTION("must not alter the value") {
        auto v = res.unwrap_err();
        REQUIRE(v == Error(Error::FAIL));
    }
}

TEST_CASE("Result.is_err for inval must panic", "[result]")
{
    auto res = TestResult();

    // must panic here..
    // why?
    REQUIRE_PANIC(res.is_err());
}

TEST_CASE("Result.is_inval for ok must return false", "[result]")
{
    auto res = TestResult(Ok(1));

    auto is = res.is_inval();

    SECTION("must give false") {
        REQUIRE_FALSE(is);
    }

    SECTION("must stay an ok") {
        auto is_ok = res.is_ok();
        REQUIRE(is_ok);
    }

    SECTION("must not alter the value") {
        auto v = res.unwrap();
        REQUIRE(v == 1);
    }

}

TEST_CASE("Result.is_inval for err must return false", "[result]")
{
    auto res = TestResult(Err(Error(Error::FAIL)));

    auto is = res.is_inval();

    SECTION("must give false") {
        REQUIRE_FALSE(is);
    }

    SECTION("must stay an err") {
        auto is_err = res.is_err();
        REQUIRE(is_err);
    }

    SECTION("must not alter the value") {
        auto v = res.unwrap_err();
        REQUIRE(v == Error(Error::FAIL));
    }
}

TEST_CASE("Result.is_inval for inval must return true", "[result]")
{
    auto res = TestResult();

    auto is = res.is_inval();

    SECTION("must give true") {
        REQUIRE(is);
    }

    SECTION("must stay an inval") {
        auto is_inval = res.is_inval();
        REQUIRE(is_inval);
    }
}


TEST_CASE("Result.unwrap for Ok must give ok value", "[result]")
{
    auto res = TestResult(Ok(1));

    auto val = res.unwrap();

    SECTION("must return wrapped value") {
        REQUIRE(val == 1);
    }

    SECTION("must invalidate src") {
        auto is_inval = res.is_inval();
        REQUIRE(is_inval);
    }

    // TODO: value wrapped must be moved once (into val).
}

TEST_CASE("Result.unwrap for err must panic", "[result]")
{
    auto res = TestResult(Err(Error(Error::FAIL)));

    REQUIRE_PANIC(res.unwrap());
}

TEST_CASE("Result.unwrap for inval must panic", "[result]")
{
    auto res = TestResult();

    REQUIRE_PANIC(res.unwrap());
}

TEST_CASE("Result.unwrap_err for err must give err value", "[result]")
{
    auto res = TestResult(Err(Error(Error::FAIL)));

    auto val = res.unwrap_err();

    SECTION("must give wrapped value") {
        REQUIRE(val == Error(Error::FAIL));
    }

    SECTION("must invalidate src") {
        auto is_inval = res.is_inval();
        REQUIRE(is_inval);
    }

    // TODO: value wrapped must be moved once (into val).
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

    SECTION("must give wrapped value") {
        REQUIRE(val == 1);
    }

    SECTION("must invalidate src") {
        auto is_inval = res.is_inval();
        REQUIRE(is_inval);
    }

    // TODO: value wrapped must be moved once (into val).

}

TEST_CASE("Result.unwrap_or for err must give or value", "[result]")
{
    auto res = TestResult(Err(Error(Error::FAIL)));

    auto val = res.unwrap_or(2);

    SECTION("must give the 'or' value") {
        REQUIRE(val == 2);
    }

    SECTION("must invalidate src") {
        auto is_inval = res.is_inval();
        REQUIRE(is_inval);
    }

    // TODO: value wrapped must be destructed.
}

TEST_CASE("Result.unwrap_or for invalid must panic", "[result]")
{
    auto res = TestResult();

    // panics since it's a use of an invalid.
    REQUIRE_PANIC(res.unwrap_or(2));
}

TEST_CASE("Result.unwrap_err_or for ok must give or value", "[result]")
{
    auto res = TestResult(Ok(1));

    auto val = res.unwrap_err_or(Error::FAIL);

    SECTION("must give 'or' value") {
        REQUIRE(val == Error(Error::FAIL));
    }

    SECTION("must invalidate src") {
        auto is_inval = res.is_inval();
        REQUIRE(is_inval);
    }

    // TODO: value wrapped must be destructed.
}

TEST_CASE("Result.unwrap_err_or for err must give err value", "[result]")
{
    auto res = TestResult(Err(Error(Error::FAIL)));

    auto val = res.unwrap_err_or(Error::NOENT);

    SECTION("must give wrapped value") {
        REQUIRE(val == Error(Error::FAIL));
    }

    SECTION("must invalidate src") {
        auto is_inval = res.is_inval();
        REQUIRE(is_inval);
    }

    // TODO: value wrapped must be moved once.
}

TEST_CASE("Result.unwrap_err_or for invalid must panic", "[result]")
{
    auto res = TestResult();

    REQUIRE_PANIC(res.unwrap_err_or(Error::NOENT));
}


bool map1(int const &e)
{
    nel::unused(e);
    return true;
}

TEST_CASE("Result.map for ok must change ok val", "[result]")
{
    auto res = TestResult(Ok(1));

    SECTION("using ptr-to-func") {
        // TODO: nasty, want bool to be auto-inferred as it's the return type of
        // map1, i.e. want res.map(map1).unwrap();
        auto res2 = res.map<bool>(map1);

        SECTION("must invalidate src") {
            auto is_inval = res.is_inval();
            REQUIRE(is_inval);
        }

        SECTION("value must be mapped to expected value") {
            auto val = res2.unwrap();
            REQUIRE(val);
        }
    }

    SECTION("using closure") {
        // TODO: nasty, want bool to be auto-inferred as it's the return type of
        // map1, i.e. want res.map(map1).unwrap();
        auto res2 = res.map<bool>([](int const &e)->bool {return e == 1;});

        SECTION("must invalidate src") {
            auto is_inval = res.is_inval();
            REQUIRE(is_inval);
        }

        SECTION("value must be mapped to expected value") {
            auto val = res2.unwrap();
            REQUIRE(val);
        }
    }
}

TEST_CASE("Result.map for err must not change err val", "[result]")
{
    auto res = TestResult(Err(Error(Error::FAIL)));

    SECTION("using ptr-to-func") {
        // TODO: nasty, want bool to be auto-inferred as it's the return type of
        // map1, i.e. want res.map(map1).unwrap();
        auto res2 = res.map<bool>(map1);

        SECTION("must invalidate src") {
            auto is_inval = res.is_inval();
            REQUIRE(is_inval);
        }

        SECTION("value must not be changed") {
            auto val = res2.unwrap_err();
            REQUIRE(val == Error(Error::FAIL));
        }
    }

    SECTION("using closure") {
        // TODO: nasty, want bool to be auto-inferred as it's the return type of
        // map1, i.e. want res.map(map1).unwrap();
        auto res2 = res.map<bool>([](int const &e)->bool {return e == 1;});

        SECTION("must invalidate src") {
            auto is_inval = res.is_inval();
            REQUIRE(is_inval);
        }

        SECTION("value must not be changed") {
            auto val = res2.unwrap_err();
            REQUIRE(val == Error(Error::FAIL));
        }
    }
}

TEST_CASE("Result.map for inval must panic", "[result]")
{
    auto res = TestResult();

    REQUIRE_PANIC(res.map<bool>(map1));
}


const char *bar = "bar";

const char *map2(const Error &e)
{
    nel::unused(e);
    return bar;
}

TEST_CASE("Result.map_err for ok must not change ok val", "[result]")
{
    auto res = TestResult(Ok(1));

    SECTION("using ptr-to-func") {
        // TODO: nasty, want bool to be auto-inferred as it's the return type of
        // map1, i.e. want res.map(map1).unwrap();
        auto res2 = res.map_err<char const *>(map2);

        SECTION("must invalidate src") {
            auto is_inval = res.is_inval();
            REQUIRE(is_inval);
        }

        SECTION("value must not be changed") {
            auto val = res2.unwrap();
            REQUIRE(val == 1);
        }
    }

    SECTION("using closure") {
        // TODO: nasty, want bool to be auto-inferred as it's the return type of
        // map1, i.e. want res.map(map1).unwrap();
        auto res2 = res.map_err<char const *>([](Error const &/*e*/)->char const * {return bar;});

        SECTION("must invalidate src") {
            auto is_inval = res.is_inval();
            REQUIRE(is_inval);
        }

        SECTION("value must not be changed") {
            auto val = res2.unwrap();
            REQUIRE(val == 1);
        }
    }
}

TEST_CASE("Result.map_err for err must change err val", "[result]")
{
    auto res = TestResult(Err(Error(Error::FAIL)));

    SECTION("using ptr-to-func") {
        // TODO: nasty, want bool to be auto-inferred as it's the return type of
        // map1, i.e. want res.map(map1).unwrap();
        auto res2 = res.map_err<char const *>(map2);

        SECTION("must invalidate src") {
            auto is_inval = res.is_inval();
            REQUIRE(is_inval);
        }

        SECTION("value must change value") {
            auto val = res2.unwrap_err();
            REQUIRE(val == bar);
        }
    }

    SECTION("using closure") {
        // TODO: nasty, want bool to be auto-inferred as it's the return type of
        // map1, i.e. want res.map(map1).unwrap();
        auto res2 = res.map_err<char const *>([](Error const &/*e*/)->char const * {return bar;});

        SECTION("must invalidate src") {
            auto is_inval = res.is_inval();
            REQUIRE(is_inval);
        }

        SECTION("value must change value") {
            auto val = res2.unwrap_err();
            REQUIRE(val == bar);
        }
    }
}

TEST_CASE("Result.map_err for inval must panic", "[result]")
{
    auto res = TestResult();

    // TODO: nasty, want bool to be auto-inferred as it's the return type of
    // map1, i.e. want res.map(map1).unwrap();
    REQUIRE_PANIC(res.map_err<const char *>(map2));
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
    return Err(Error(Error::FAIL));
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

    SECTION("must give true") {
        REQUIRE(is);
    }

    SECTION("must not invalidate lhs") {
        auto is_inval = res1.is_inval();
        REQUIRE_FALSE(is_inval);
    }

    SECTION("must not change lhs value") {
        auto v = res1.unwrap();
        REQUIRE(v == 2);
    }
}


TEST_CASE("Result::eq for ok with diff ok must give false", "[result]")
{
    auto res1 = TestResult(Ok(2));

    auto is = res1 == Ok(3);

    SECTION("must give false") {
        REQUIRE_FALSE(is);
    }

    SECTION("must not invalidate lhs") {
        auto is_inval = res1.is_inval();
        REQUIRE_FALSE(is_inval);
    }

    SECTION("must not change lhs value") {
        auto v = res1.unwrap();
        REQUIRE(v == 2);
    }
}


TEST_CASE("Result::eq for ok with err must give false", "[result]")
{
    auto res1 = TestResult(Ok(2));

    auto is = res1 == Err(Error(Error::FAIL));

    SECTION("must give false") {
        REQUIRE_FALSE(is);
    }

    SECTION("must not invalidate lhs") {
        auto is_inval = res1.is_inval();
        REQUIRE_FALSE(is_inval);
    }

    SECTION("must not change lhs value") {
        auto v = res1.unwrap();
        REQUIRE(v == 2);
    }
}


TEST_CASE("Result::eq for ok with inval must panic", "[result]")
{
    auto res1 = TestResult(Ok(2));

    auto res2 = TestResult();

    REQUIRE_PANIC(res1 == res2);

    // although lhs is required to not change, panicking is a non-recoverable
    // error so program execution stops, and so res1 changing or not is moot.
}


TEST_CASE("Result::eq for err with ok must give false", "[result]")
{
    auto res1 = TestResult(Err(Error(Error::FAIL)));

    auto is = res1 == Ok(2);

    SECTION("must give false") {
        REQUIRE_FALSE(is);
    }

    SECTION("must not invalidate lhs") {
        auto is_inval = res1.is_inval();
        REQUIRE_FALSE(is_inval);
    }

    SECTION("must not change lhs value") {
        auto v = res1.unwrap_err();
        REQUIRE(v == Error(Error::FAIL));
    }
}


TEST_CASE("Result::eq for err with err must give true", "[result]")
{
    auto res1 = TestResult(Err(Error(Error::FAIL)));

    auto is = res1 == Err(Error(Error::FAIL));

    SECTION("must give true") {
        REQUIRE(is);
    }

    SECTION("must not invalidate lhs") {
        auto is_inval = res1.is_inval();
        REQUIRE_FALSE(is_inval);
    }

    SECTION("must not change lhs value") {
        auto v = res1.unwrap_err();
        REQUIRE(v == Error(Error::FAIL));
    }
}


TEST_CASE("Result::eq for err with diff err must give false", "[result]")
{
    auto res1 = TestResult(Err(Error(Error::NOENT)));

    auto is = res1 == Err(Error(Error::FAIL));

    SECTION("must give false") {
        REQUIRE_FALSE(is);
    }

    SECTION("must not invalidate lhs") {
        auto is_inval = res1.is_inval();
        REQUIRE_FALSE(is_inval);
    }

    SECTION("must not change lhs value") {
        auto v = res1.unwrap_err();
        REQUIRE(v == Error::NOENT);
    }
}


TEST_CASE("Result::eq for err with inval must panic", "[result]")
{
    auto res1 = TestResult(Err(Error(Error::FAIL)));

    auto res2 = TestResult();

    REQUIRE_PANIC(res1 == res2);
    // although lhs is required to not change, panicking is a non-recoverable
    // error so program execution stops, and so res1 changing or not is moot.
}

TEST_CASE("Result::eq for inval with ok must panic", "[result]")
{
    auto res1 = TestResult();

    REQUIRE_PANIC(res1 == Ok(1));
}


TEST_CASE("Result::eq for inval with err must panic", "[result]")
{
    auto res1 = TestResult();

    REQUIRE_PANIC(res1 == Err(Error(Error::FAIL)));
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

    SECTION("must give false") {
        REQUIRE_FALSE(is);
    }

    SECTION("must not invalidate lhs") {
        auto is_inval = res1.is_inval();
        REQUIRE_FALSE(is_inval);
    }

    SECTION("must not change lhs value") {
        auto v = res1.unwrap();
        REQUIRE(v == 2);
    }
}


TEST_CASE("Result::neq for ok with diff ok must give true", "[result]")
{
    auto res1 = TestResult(Ok(2));

    auto is = res1 != Ok(3);

    SECTION("must give true") {
        REQUIRE(is);
    }

    SECTION("must not invalidate lhs") {
        auto is_inval = res1.is_inval();
        REQUIRE_FALSE(is_inval);
    }

    SECTION("must not change lhs value") {
        auto v = res1.unwrap();
        REQUIRE(v == 2);
    }
}


TEST_CASE("Result::neq for ok with err must give true", "[result]")
{
    auto res1 = TestResult(Ok(2));

    auto is = res1 != Err(Error(Error::FAIL));

    SECTION("must give true") {
        REQUIRE(is);
    }

    SECTION("must not invalidate lhs") {
        auto is_inval = res1.is_inval();
        REQUIRE_FALSE(is_inval);
    }

    SECTION("must not change lhs value") {
        auto v = res1.unwrap();
        REQUIRE(v == 2);
    }
}


TEST_CASE("Result::neq for ok with inval must panic", "[result]")
{
    auto res1 = TestResult(Ok(2));

    auto res2 = TestResult();

    REQUIRE_PANIC(res1 != res2);

    // although lhs is required to not change, panicking is a non-recoverable
    // error so program execution stops, and so res1 changing or not is moot.
}


TEST_CASE("Result::neq for err with ok must give true", "[result]")
{
    auto res1 = TestResult(Err(Error(Error::FAIL)));

    auto is = res1 != Ok(1);

    SECTION("must give true") {
        REQUIRE(is);
    }

    SECTION("must not invalidate lhs") {
        auto is_inval = res1.is_inval();
        REQUIRE_FALSE(is_inval);
    }

    SECTION("must not change lhs value") {
        auto v = res1.unwrap_err();
        REQUIRE(v == Error(Error::FAIL));
    }
}


TEST_CASE("Result::neq for err with err must give false", "[result]")
{
    auto res1 = TestResult(Err(Error(Error::FAIL)));

    auto is = res1 != Err(Error(Error::FAIL));

    SECTION("must give false") {
        REQUIRE_FALSE(is);
    }

    SECTION("must not invalidate lhs") {
        auto is_inval = res1.is_inval();
        REQUIRE_FALSE(is_inval);
    }

    SECTION("must not change lhs value") {
        auto v = res1.unwrap_err();
        REQUIRE(v == Error(Error::FAIL));
    }
}


TEST_CASE("Result::neq for err with diff err must give true", "[result]")
{
    auto res1 = TestResult(Err(Error(Error::NOENT)));

    auto is = res1 != Err(Error(Error::FAIL));

    SECTION("must give true") {
        REQUIRE(is);
    }

    SECTION("must not invalidate lhs") {
        auto is_inval = res1.is_inval();
        REQUIRE_FALSE(is_inval);
    }

    SECTION("must not change lhs value") {
        auto v = res1.unwrap_err();
        REQUIRE(v == Error(Error::NOENT));
    }
}


TEST_CASE("Result::neq for err with inval must panic", "[result]")
{
    auto res1 = TestResult(Err(Error(Error::FAIL)));

    auto res2 = TestResult();

    REQUIRE_PANIC(res1 != res2);

    // although lhs is required to not change, panicking is a non-recoverable
    // error so program execution stops, and so res1 changing or not is moot.
}

TEST_CASE("Result::neq for inval with ok must panic", "[result]")
{
    auto res1 = TestResult();

    REQUIRE_PANIC(res1 != Ok(1));
}


TEST_CASE("Result::neq for inval with err must panic", "[result]")
{
    auto res1 = TestResult();

    REQUIRE_PANIC(res1 != Err(Error(Error::FAIL)));
}


TEST_CASE("Result::neq for inval with inval must panic", "[result]")
{
    auto res1 = TestResult();

    auto res2 = TestResult();

    REQUIRE_PANIC(res1 != res2);
}

}; // namespace result
}; // namespace test
}; // namespace nel
