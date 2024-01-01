// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include "optional.hh"
#include "defs.hh"

#include <catch2/catch.hpp>
#include <cstring> //strcmp

namespace nel
{
namespace test
{
namespace optional
{

TEST_CASE("Optional<>::Some()", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);
    // checks that does not panic.
    // and can be created this way.
    nel::unused(opt1);
}

TEST_CASE("Optional<void>::Some()", "[optional]")
{
    nel::Optional<void> opt1 = nel::Some();
    // checks that does not panic.
    // and can be created this way.
    nel::unused(opt1);
}

TEST_CASE("Optional<>::None()", "[optional]")
{
    nel::Optional<int> opt1 = None;
    // checks that does not panic.
    // and can be created this way.
    nel::unused(opt1);
}

TEST_CASE("Optional<void>::None()", "[optional]")
{
    nel::Optional<void> opt1 = None;
    // checks that does not panic.
    // and can be created this way.
    nel::unused(opt1);
}

TEST_CASE("Optional<>(some).move-ctor", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    auto opt2 = move(opt1);

    SECTION("must move val to dest")
    {
        auto val = opt2.unwrap();
        REQUIRE(val == 1);
    }
    SECTION("must invalidate src")
    {
        auto val = opt2.unwrap();
        nel::unused(val);
        REQUIRE(opt1.is_inval());
    }
}

TEST_CASE("Optional<void>(some).move-ctor", "[optional]")
{
    nel::Optional<void> opt1 = nel::Some();

    auto opt2 = move(opt1);

    SECTION("must move val to dest")
    {
        // check that it does not panic.
        opt2.unwrap();
    }
    SECTION("must invalidate src")
    {
        REQUIRE(opt1.is_inval());
    }
}

TEST_CASE("Optional<>(none).move-ctor", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;
    auto opt2 = move(opt1);

    SECTION("must move val to dest")
    {
        REQUIRE(opt2.is_none());
    }
    SECTION("must invalidate src")
    {
        REQUIRE(opt1.is_inval());
    }
}

TEST_CASE("Optional<void>(none).move-ctor: must move val to dest", "[optional]")
{
    nel::Optional<void> opt1 = nel::None;
    auto opt2 = move(opt1);

    SECTION("must move val to dest")
    {
        REQUIRE(opt2.is_none());
    }
    SECTION("must invalidate src")
    {
        REQUIRE(opt1.is_inval());
    }
}

TEST_CASE("Optional<>(inval).move-ctor: must move val to dest", "[optional]")
{
    // moves value to dest (dest is now inval)
    // doesn't panic.
    nel::Optional<int> opt1 = nel::None;
    auto opt2 = move(opt1);
    nel::unused(opt2);

    auto opt3 = move(opt1);

    SECTION("must move val to dest")
    {
        REQUIRE(opt3.is_inval());
    }
    SECTION("must invalidate src")
    {
        REQUIRE(opt1.is_inval());
    }
}

TEST_CASE("Optional<void>(inval).move-ctor: must move val to dest", "[optional]")
{
    // moves value to dest (dest is now inval)
    // doesn't panic.
    nel::Optional<void> opt1 = nel::None;
    auto opt2 = move(opt1);
    nel::unused(opt2);

    auto opt3 = move(opt1);

    SECTION("must move val to dest")
    {
        REQUIRE(opt3.is_inval());
    }
    SECTION("must invalidate src")
    {
        REQUIRE(opt1.is_inval());
    }
}

TEST_CASE("Optional<>(some).move-ass", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);
    nel::Optional<int> opt2 = nel::Some(2);

    opt2 = move(opt1);

    SECTION("must move val to dest")
    {
        auto val = opt2.unwrap();
        REQUIRE(val == 1);
    }
    SECTION("must invalidate src")
    {
        REQUIRE(opt1.is_inval());
    }
}

TEST_CASE("Optional<void>(some).move-ass", "[optional]")
{
    nel::Optional<void> opt1 = nel::Some();
    nel::Optional<void> opt2 = nel::Some();
    opt2 = move(opt1);

    SECTION("must move val to dest")
    {
        REQUIRE(opt2.is_some());
    }
    SECTION("must invalidate src")
    {
        REQUIRE(opt1.is_inval());
    }
}

TEST_CASE("Optional<>(none).move-ass", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(10);
    nel::Optional<int> opt2 = nel::Some(2);
    opt2 = move(opt1);

    SECTION("must move val to dest")
    {
        REQUIRE(opt2.unwrap() == 10);
    }
    SECTION("must invalidate src")
    {
        REQUIRE(opt1.is_inval());
    }
}

TEST_CASE("Optional<void>(none).move-ass", "[optional]")
{
    nel::Optional<void> opt1 = nel::Some();
    nel::Optional<void> opt2 = nel::None;
    opt2 = move(opt1);

    SECTION("must move val to dest")
    {
        REQUIRE(opt2.is_some());
    }
    SECTION("must invalidate src")
    {
        REQUIRE(opt1.is_inval());
    }
}

TEST_CASE("Optional<>(inval).move-ass: must move val to dest", "[optional]")
{
    // inval state moed to dest.
    // does not panic.
    nel::Optional<int> opt1 = nel::Some(12);
    auto opt2 = move(opt1);
    nel::unused(opt2);

    nel::Optional<int> opt3 = nel::Some(1);
    opt3 = move(opt1);

    SECTION("must move val to dest")
    {
        REQUIRE(opt3.is_inval());
    }
    SECTION("must invalidate src")
    {
        REQUIRE(opt1.is_inval());
    }
}

TEST_CASE("Optional<>(some).is_some()", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    SECTION("must return true")
    {
        REQUIRE(opt1.is_some());
    }
    SECTION("must not alter src")
    {
        // check has not been invalidated.
        // check has not changed value.
        REQUIRE(opt1.unwrap() == 1);
    }
}

TEST_CASE("Optional<void>(some).is_some()", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    SECTION("must return true")
    {
        REQUIRE(opt1.is_some());
    }
    SECTION("must not alter src")
    {
        // check value has not been invalidated
        REQUIRE(opt1.is_some());
    }
}

TEST_CASE("Optional<>(none).is_some()", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;

    SECTION("must return false")
    {
        REQUIRE(!opt1.is_some());
    }
    SECTION("must not alter src")
    {
        // check value has not been invalidated
        REQUIRE(opt1.is_none());
    }
}

TEST_CASE("Optional<void>(none).is_some()", "[optional]")
{
    nel::Optional<void> opt1 = nel::None;

    SECTION("must return false")
    {
        REQUIRE(!opt1.is_some());
    }
    SECTION("must not alter src")
    {
        // check value has not been invalidated
        REQUIRE(opt1.is_none());
    }
}

TEST_CASE("Optional<>(inval).is_some()", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;
    auto opt2 = move(opt1);

    SECTION("must panic")
    {
        REQUIRE_PANIC(opt1.is_some());
    }
}

TEST_CASE("Optional<void>(inval).is_some()", "[optional]")
{
    nel::Optional<void> opt1 = nel::None;
    auto opt2 = move(opt1);

    SECTION("must panic")
    {
        REQUIRE_PANIC(opt1.is_some());
    }
}

TEST_CASE("Optional<>(some).is_none()", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    SECTION("must return false")
    {
        REQUIRE(!opt1.is_none());
    }
    SECTION("must not alter src")
    {
        // check value has not been invalidated.
        // check value has not been changed.
        REQUIRE(opt1.unwrap() == 1);
    }
}

TEST_CASE("Optional<void>(some).is_none()", "[optional]")
{
    nel::Optional<void> opt1 = nel::Some();

    SECTION("must return false")
    {
        REQUIRE(!opt1.is_none());
    }
    SECTION("must not alter src")
    {
        // check value has not been invalidated.
        // check value has not been changed.
        REQUIRE(opt1.is_some());
    }
}

TEST_CASE("Optional<>(none).is_none()", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;

    SECTION("must return true")
    {
        REQUIRE(opt1.is_none());
    }
    SECTION("must not alter src")
    {
        // check value has not been invalidated.
        // check value has not been changed.
        REQUIRE(opt1.is_none());
    }
}

TEST_CASE("Optional<void>(none).is_none()", "[optional]")
{
    nel::Optional<void> opt1 = nel::None;

    SECTION("must return true")
    {
        REQUIRE(opt1.is_none());
    }
    SECTION("must not alter src")
    {
        // check value has not been invalidated.
        // check value has not been changed.
        REQUIRE(opt1.is_none());
    }
}

TEST_CASE("Optional<>(inval).is_none()", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;
    auto opt2 = move(opt1);
    nel::unused(opt2);

    SECTION("must panic")
    {
        REQUIRE_PANIC(opt1.is_none());
    }
}

TEST_CASE("Optional<void>(inval).is_none()", "[optional]")
{
    nel::Optional<void> opt1 = nel::None;
    auto opt2 = move(opt1);
    nel::unused(opt2);

    SECTION("must panic")
    {
        REQUIRE_PANIC(opt1.is_none());
    }
}

TEST_CASE("Optional<>(some).unwrap()", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    SECTION("must return wrapped value")
    {
        auto val = opt1.unwrap();
        REQUIRE(val == 1);
    }
    SECTION("must invalidate src")
    {
        auto val = opt1.unwrap();
        nel::unused(val);
        REQUIRE(opt1.is_inval());
    }
}

TEST_CASE("Optional<void>(some).unwrap()", "[optional]")
{
    nel::Optional<void> opt1 = nel::Some();

    SECTION("must return wrapped value")
    {
        // check that unwrap does not panic
        opt1.unwrap();
    }
    SECTION("must invalidate src")
    {
        opt1.unwrap();
        REQUIRE(opt1.is_inval());
    }
}

TEST_CASE("Optional<>(none).unwrap()", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;

    SECTION("panics")
    {
        REQUIRE_PANIC(opt1.unwrap());
    }
}

TEST_CASE("Optional<void>(none).unwrap()", "[optional]")
{
    nel::Optional<void> opt1 = nel::None;

    SECTION("panics")
    {
        REQUIRE_PANIC(opt1.unwrap());
    }
}

TEST_CASE("Optional<>(inval).unwrap()", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;
    auto opt2 = move(opt1);
    nel::unused(opt2);

    SECTION("panics")
    {
        REQUIRE_PANIC(opt1.unwrap());
    }
}

TEST_CASE("Optional<void>(inval).unwrap()", "[optional]")
{
    nel::Optional<void> opt1 = nel::None;
    auto opt2 = move(opt1);
    nel::unused(opt2);

    SECTION("panics")
    {
        REQUIRE_PANIC(opt1.unwrap());
    }
}

TEST_CASE("Optional<>(some).unwrap_or(other)", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    SECTION("returns original value")
    {
        auto val = opt1.unwrap_or(2);
        REQUIRE(val == 1);
    }
    SECTION("invalidates src")
    {
        opt1.unwrap_or(2);
        REQUIRE(opt1.is_inval());
    }
}

TEST_CASE("Optional<void>(some).unwrap_or(other)", "[optional]")
{
    nel::Optional<void> opt1 = nel::Some();

    SECTION("returns original value")
    {
        // check that it doesn't panic..
        opt1.unwrap_or();
    }
    SECTION("invalidates src")
    {
        opt1.unwrap_or();
        REQUIRE(opt1.is_inval());
    }
}

TEST_CASE("Optional<>(none).unwrap_or(other)", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;

    SECTION("returns other value")
    {
        auto val = opt1.unwrap_or(2);
        REQUIRE(val == 2);
    }
    SECTION("invalidates src")
    {
        opt1.unwrap_or(2);
        REQUIRE(opt1.is_inval());
    }
}

TEST_CASE("Optional<void>(none).unwrap_or(other)", "[optional]")
{
    nel::Optional<void> opt1 = nel::None;

    SECTION("returns other value")
    {
        // check that it doesn't panic..
        opt1.unwrap_or();
    }
    SECTION("invalidates src")
    {
        opt1.unwrap_or();
        REQUIRE(opt1.is_inval());
    }
}

TEST_CASE("Optional<>(inval).unwrap_or(other)", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;
    auto opt2 = move(opt1);
    nel::unused(opt2);

    SECTION("must panic")
    {
        // i.e. use after move.
        REQUIRE_PANIC(opt1.unwrap_or(2));
    }
}

TEST_CASE("Optional<>(some).eq()", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    SECTION("must return true if compared to same value")
    {
        REQUIRE(opt1 == nel::Some(1));
    }
    SECTION("must return true if compared to same obj")
    {
        REQUIRE(opt1 == opt1);
    }
    SECTION("must not change src")
    {
        REQUIRE(opt1.unwrap() == 1);
    }
    SECTION("must return false if compared to diff value")
    {
        REQUIRE(!(opt1 == nel::Some(2)));
    }
    SECTION("must return false if compared to none")
    {
        REQUIRE(!(opt1 == None));
    }
    SECTION("must panic if compared to invalid")
    {
        nel::Optional<int> opt2 = nel::Some(1);
        auto opt3 = move(opt2);
        REQUIRE_PANIC(opt1 == opt2);
    }
}

TEST_CASE("Optional<void>(some).eq()", "[optional]")
{
    nel::Optional<void> opt1 = nel::Some();

    SECTION("must return true if compared to same value")
    {
        REQUIRE(opt1 == nel::Some());
    }
    SECTION("must return true if compared to same obj")
    {
        REQUIRE(opt1 == opt1);
    }
    SECTION("must not change src")
    {
        /// check that does not panic
        opt1.unwrap();
    }
    // SECTION("must return false for diff value") {
    //     REQUIRE(!(opt1 == nel::Some(2)));
    // }
    SECTION("must return false if compared to none")
    {
        REQUIRE(!(opt1 == None));
    }
    SECTION("must panic if compared to invalid")
    {
        nel::Optional<void> opt2 = nel::Some();
        auto opt3 = move(opt2);
        REQUIRE_PANIC(opt1 == opt2);
    }
}

TEST_CASE("Optional<>(none).eq()", "[optional]")
{
    nel::Optional<int> opt1 = None;

    SECTION("must return true for same value")
    {
        REQUIRE(opt1 == nel::Optional<int>(None));
    }
    SECTION("must return true if compared to same obj")
    {
        REQUIRE(opt1 == opt1);
    }
    SECTION("must not change src")
    {
        REQUIRE(opt1.is_none());
    }
    // SECTION("must return false for diff value") {
    //     REQUIRE(!(opt1 == nel::Some(2)));
    // }
    SECTION("must return false for Some value")
    {
        REQUIRE(!(opt1 == nel::Some(2)));
    }
    SECTION("must panic if compared to invalid")
    {
        nel::Optional<int> opt2 = nel::Some(1);
        auto opt3 = move(opt2);
        REQUIRE_PANIC(opt1 == opt2);
    }
}

TEST_CASE("Optional<void>(none).eq()", "[optional]")
{
    nel::Optional<void> opt1 = None;

    SECTION("must return true for same value")
    {
        REQUIRE(opt1 == nel::Optional<void>(None));
    }
    SECTION("must return true if compared to same obj")
    {
        REQUIRE(opt1 == opt1);
    }
    SECTION("must not change src")
    {
        REQUIRE(opt1.is_none());
    }
    // SECTION("must return false for diff value") {
    //     REQUIRE(!(opt1 == nel::Some(2)));
    // }
    SECTION("must return false for Some value")
    {
        REQUIRE(!(opt1 == nel::Some()));
    }
    SECTION("must panic if compared to invalid")
    {
        nel::Optional<void> opt2 = nel::Some();
        auto opt3 = move(opt2);
        REQUIRE_PANIC(opt1 == opt2);
    }
}

TEST_CASE("Optional<>(inval).eq()", "[optional]")
{
    nel::Optional<int> opt1 = None;
    auto opt2 = move(opt1);

    SECTION("must panic if compared to same value")
    {
        nel::Optional<int> opt10 = None;
        auto opt20 = move(opt10);
        REQUIRE_PANIC(opt1 == opt10);
    }
    SECTION("must panic if compared to same obj")
    {
        REQUIRE_PANIC(opt1 == opt1);
    }
    SECTION("must panic if compared to Some value")
    {
        REQUIRE_PANIC(opt1 == nel::Some(1));
    }
    SECTION("must panic if compared to invalid")
    {
        nel::Optional<int> opt10 = nel::Some(2);
        auto opt30 = move(opt10);
        nel::unused(opt30);
        REQUIRE_PANIC(opt1 == opt10);
    }
}

TEST_CASE("Optional<void>(inval).eq()", "[optional]")
{
    nel::Optional<void> opt1 = None;
    auto opt2 = move(opt1);

    SECTION("must panic if compared to same value")
    {
        nel::Optional<void> opt10 = None;
        auto opt20 = move(opt10);
        REQUIRE_PANIC(opt1 == opt10);
    }
    SECTION("must panic if compared to same obj")
    {
        REQUIRE_PANIC(opt1 == opt1);
    }
    SECTION("must panic if compared to Some value")
    {
        REQUIRE_PANIC(opt1 == nel::Some());
    }
    SECTION("must panic if compared to invalid")
    {
        nel::Optional<void> opt10 = nel::Some();
        auto opt30 = move(opt10);
        nel::unused(opt30);
        REQUIRE_PANIC(opt1 == opt10);
    }
}

TEST_CASE("Optional<>(some).ne()", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    SECTION("must return false if compared to same value")
    {
        REQUIRE(!(opt1 != nel::Some(1)));
    }
    SECTION("must return false if compared to same obj")
    {
        REQUIRE(!(opt1 != opt1));
    }
    SECTION("must not change src")
    {
        REQUIRE(opt1.unwrap() == 1);
    }
    SECTION("must return true if compared to diff value")
    {
        REQUIRE(opt1 != nel::Some(2));
    }
    SECTION("must return true if compared to none")
    {
        REQUIRE(opt1 != None);
    }
    SECTION("must panic if compared to invalid")
    {
        nel::Optional<int> opt2 = nel::Some(1);
        auto opt3 = move(opt2);
        REQUIRE_PANIC(opt1 != opt2);
    }
}

TEST_CASE("Optional<void>(some).ne()", "[optional]")
{
    nel::Optional<void> opt1 = nel::Some();

    SECTION("must return false if compared to same value")
    {
        REQUIRE(!(opt1 != nel::Some()));
    }
    SECTION("must return false if compared to same obj")
    {
        REQUIRE(!(opt1 != opt1));
    }
    SECTION("must not change src")
    {
        /// check that does not panic
        opt1.unwrap();
    }
    // SECTION("must return false for diff value") {
    //     REQUIRE(!(opt1 == nel::Some(2)));
    // }
    SECTION("must return true if compared to none")
    {
        REQUIRE(opt1 != None);
    }
    SECTION("must panic if compared to invalid")
    {
        nel::Optional<void> opt2 = nel::Some();
        auto opt3 = move(opt2);
        REQUIRE_PANIC(opt1 != opt2);
    }
}

TEST_CASE("Optional<>(none).ne()", "[optional]")
{
    nel::Optional<int> opt1 = None;

    SECTION("must return false for same value")
    {
        REQUIRE(!(opt1 != nel::Optional<int>(None)));
    }
    SECTION("must return false if compared to same obj")
    {
        REQUIRE(!(opt1 != opt1));
    }
    SECTION("must not change src")
    {
        REQUIRE(opt1.is_none());
    }
    // SECTION("must return false for diff value") {
    //     REQUIRE(!(opt1 == nel::Some(2)));
    // }
    SECTION("must return true for Some value")
    {
        REQUIRE(!(opt1 == nel::Some(2)));
    }
    SECTION("must panic if compared to invalid")
    {
        nel::Optional<int> opt2 = nel::Some(1);
        auto opt3 = move(opt2);
        REQUIRE_PANIC(opt1 != opt2);
    }
}

TEST_CASE("Optional<void>(none).ne()", "[optional]")
{
    nel::Optional<void> opt1 = None;

    SECTION("must return false for same value")
    {
        REQUIRE(!(opt1 != nel::Optional<void>(None)));
    }
    SECTION("must return false if compared to same obj")
    {
        REQUIRE(!(opt1 != opt1));
    }
    SECTION("must not change src")
    {
        REQUIRE(opt1.is_none());
    }
    // SECTION("must return false for diff value") {
    //     REQUIRE(!(opt1 == nel::Some(2)));
    // }
    SECTION("must return true for Some value")
    {
        REQUIRE(opt1 != nel::Some());
    }
    SECTION("must panic if compared to invalid")
    {
        nel::Optional<void> opt2 = nel::Some();
        auto opt3 = move(opt2);
        REQUIRE_PANIC(opt1 != opt2);
    }
}

TEST_CASE("Optional<>(inval).ne()", "[optional]")
{
    nel::Optional<int> opt1 = None;
    auto opt2 = move(opt1);

    SECTION("must panic if compared to same value")
    {
        nel::Optional<int> opt10 = None;
        auto opt20 = move(opt10);
        REQUIRE_PANIC(opt1 != opt10);
    }
    SECTION("must panic if compared to same obj")
    {
        REQUIRE_PANIC(opt1 != opt1);
    }
    SECTION("must panic if compared to Some value")
    {
        REQUIRE_PANIC(opt1 != nel::Some(1));
    }
    SECTION("must panic if compared to invalid")
    {
        nel::Optional<int> opt10 = nel::Some(2);
        auto opt30 = move(opt10);
        nel::unused(opt30);
        REQUIRE_PANIC(opt1 != opt10);
    }
}

TEST_CASE("Optional<void>(inval).ne()", "[optional]")
{
    nel::Optional<void> opt1 = None;
    auto opt2 = move(opt1);

    SECTION("must panic if compared to same value")
    {
        nel::Optional<void> opt10 = None;
        auto opt20 = move(opt10);
        REQUIRE_PANIC(opt1 != opt10);
    }
    SECTION("must panic if compared to same obj")
    {
        REQUIRE_PANIC(opt1 != opt1);
    }
    SECTION("must panic if compared to Some value")
    {
        REQUIRE_PANIC(opt1 != nel::Some());
    }
    SECTION("must panic if compared to invalid")
    {
        nel::Optional<void> opt10 = nel::Some();
        auto opt30 = move(opt10);
        nel::unused(opt30);
        REQUIRE_PANIC(opt1 != opt10);
    }
}

nel::Optional<int> foo_some()
{
    return nel::Some(1);
    // return nel::nel::Some(1);
}

TEST_CASE("Optional(some).?  into Optional", "[optional]")
{
    REQUIRE(foo_some().is_some());
}

nel::Optional<int> foo_none()
{
    return nel::None;
}

TEST_CASE("Optional(none) auto into Optional", "[optional]")
{
    REQUIRE(foo_none().is_none());
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

TEST_CASE("Optional(some)::~dtor, must call some dtor", "[optional]")
{
    bool dtor_called = false;
    {
        nel::Optional<Foo> opt1 = nel::Some(Foo(dtor_called));

        dtor_called = false;
    }
    REQUIRE(dtor_called);
}

TEST_CASE("Optional<>(some).map()", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);
    auto opt2 = opt1.map<char const *>([](auto &&) -> char const * { return "haha"; });

    SECTION("must produce mapped value")
    {
        auto val = opt2.unwrap();
        REQUIRE(strcmp(val, "haha") == 0);
    }
    SECTION("must invalidate src")
    {
        REQUIRE(opt1.is_inval());
    }
}

TEST_CASE("Optional<void>(some).map()", "[optional]")
{
    nel::Optional<void> opt1 = nel::Some();
    auto opt2 = opt1.map<char const *>([]() -> char const * { return "haha"; });

    SECTION("must produce mapped value")
    {
        auto val = opt2.unwrap();
        REQUIRE(strcmp(val, "haha") == 0);
    }
    SECTION("must invalidate src")
    {
        REQUIRE(opt1.is_inval());
    }
}

TEST_CASE("Optional<>(none).map", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;
    auto opt2 = opt1.map<char const *>([](auto &&) -> char const * { return "haha"; });

    SECTION("must not change value")
    {
        REQUIRE(opt2.is_none());
    }
    SECTION("must invalidate src")
    {
        REQUIRE(opt1.is_inval());
    }
}

TEST_CASE("Optional<void>(none).map", "[optional]")
{
    nel::Optional<void> opt1 = nel::None;
    auto opt2 = opt1.map<char const *>([]() -> char const * { return "haha"; });

    SECTION("must not change value")
    {
        REQUIRE(opt2.is_none());
    }
    SECTION("must invalidate src")
    {
        REQUIRE(opt1.is_inval());
    }
}

TEST_CASE("Optional<>(inval).map: panics", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;
    auto opt3 = move(opt1);
    nel::unused(opt3);

    REQUIRE_PANIC(opt1.map<char const *>([](auto &&) -> char const * { return "haha"; }));
}

TEST_CASE("Optional<void>(none).map: panics", "[optional]")
{
    nel::Optional<void> opt1 = nel::None;
    auto opt3 = move(opt1);
    nel::unused(opt3);

    REQUIRE_PANIC(opt1.map<char const *>([]() -> char const * { return "haha"; }));
}

}; // namespace optional
}; // namespace test
}; // namespace nel
