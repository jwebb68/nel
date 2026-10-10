// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include "optional.hh"

#include <nel/defs.hh> // unused

#include <catch2/catch.hpp>
#include <catch2/catch2_panic.hh>

#include <cstring> //strcmp

namespace nel
{
namespace test
{
namespace optional
{

struct UDT_0 {
    constexpr UDT_0() {}
};

struct UDT_1 {
    int v_;
    constexpr UDT_1(int v): v_(v) {}
};

TEST_CASE("Some<int>() creates a Optional(some)", "[optional]")
{
    auto opt1 = Some(1);
    // checks that does not panic.
    // and can be created this way.
    REQUIRE(opt1.is_some());
}


TEST_CASE("Optional<int>::Some()", "[optional]")
{
    auto opt1 = Optional<int>(Some(1));
    // checks that does not panic.
    // and can be created this way.
    REQUIRE(opt1.is_some());
}

TEST_CASE("Optional<UDT_0>::Some()", "[optional]")
{
    auto opt1 = Optional<UDT_0>(Some(UDT_0()));
    // checks that does not panic.
    // and can be created this way.
    REQUIRE(opt1.is_some());
}

TEST_CASE("Optional<UDT_1>::Some()", "[optional]")
{
    auto opt1 = Optional<UDT_1>(Some(UDT_1(23)));
    // checks that does not panic.
    // and can be created this way.
    REQUIRE(opt1.is_some());
}

TEST_CASE("Optional<Void>::Some()", "[optional]")
{
    auto opt1 = Optional<Void>(Some());
    // checks that does not panic.
    // and can be created this way.
    REQUIRE(opt1.is_some());
}


TEST_CASE("Optional<int> = None", "[optional]")
{
    auto opt1 = Optional<int>(None());
    // checks that does not panic.
    // and can be created this way.
    REQUIRE(opt1.is_none());
}

TEST_CASE("Optional<UDT_0> = None", "[optional]")
{
    auto opt1 = Optional<UDT_0>(None());
    // checks that does not panic.
    // and can be created this way.
    REQUIRE(opt1.is_none());
}

TEST_CASE("Optional<UDT_1> = None", "[optional]")
{
    auto opt1 = Optional<UDT_1>(None());
    // checks that does not panic.
    // and can be created this way.
    REQUIRE(opt1.is_none());
}

TEST_CASE("Optional<Void>::None()", "[optional]")
{
    auto opt1 = Optional<Void>(None());
    // checks that does not panic.
    // and can be created this way.
    REQUIRE(opt1.is_none());
}

TEST_CASE("Optional<int>::Optional()", "[optional]")
{
    // can declare optional.
    auto opt1  = Optional<int>();

    // checks that does not panic.
    // and can be created this way.
    REQUIRE(opt1.is_inval());
}

TEST_CASE("Optional<UDT_0>::Optional()", "[optional]")
{
    // can declare optional.
    auto opt1  = Optional<UDT_0>();

    // checks that does not panic.
    // and can be created this way.
    REQUIRE(opt1.is_inval());
}

TEST_CASE("Optional<UDT_1>::Optional()", "[optional]")
{
    // can declare optional.
    auto opt1  = Optional<UDT_1>();

    // checks that does not panic.
    // and can be created this way.
    REQUIRE(opt1.is_inval());
}

TEST_CASE("Optional<Void>::Optional()", "[optional]")
{
    // can declare optional.
    auto opt1  = Optional<Void>();

    // checks that does not panic.
    // and can be created this way.
    REQUIRE(opt1.is_inval());
}

TEST_CASE("Optional<>(some).move-ctor", "[optional]")
{
    auto opt1 = Some(1);

    auto opt2 = move(opt1);

    SECTION("must move val to dest")
    {
        REQUIRE(opt2.is_some());
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

TEST_CASE("Optional<Void>(some).move-ctor", "[optional]")
{
    auto opt1 = Some();

    auto opt2 = move(opt1);

    SECTION("must move val to dest")
    {
        REQUIRE(opt2.is_some());
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
    auto opt1 = Optional<int>(None());

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

TEST_CASE("Optional<Void>(none).move-ctor: must move val to dest", "[optional]")
{
    auto opt1 = Optional<Void>(None());

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
    auto opt1 = nel::Optional<int>();

    auto opt2 = move(opt1);

    SECTION("must move val to dest")
    {
        REQUIRE(opt2.is_inval());
    }
    SECTION("must invalidate src")
    {
        REQUIRE(opt1.is_inval());
    }
}

TEST_CASE("Optional<Void>(inval).move-ctor: must move val to dest", "[optional]")
{
    // moves value to dest (dest is now inval)
    // doesn't panic.
    auto opt1 = Optional<Void>();

    auto opt2 = move(opt1);

    SECTION("must move val to dest")
    {
        REQUIRE(opt2.is_inval());
    }
    SECTION("must invalidate src")
    {
        REQUIRE(opt1.is_inval());
    }
}

TEST_CASE("Optional<>(some).move-ass", "[optional]")
{
    auto opt1 = Optional<int>(Some(1));
    auto opt2 = Optional<int>(Some(2));

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

TEST_CASE("Optional<Void>(some).move-ass", "[optional]")
{
    auto opt1 = Optional<Void>(Some());
    auto opt2 = Optional<Void>(Some());

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
    auto opt1 = Optional<int>(Some(10));
    auto opt2 = Optional<int>(Some(2));
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

TEST_CASE("Optional<Void>(none).move-ass", "[optional]")
{
    auto opt1 = Optional<Void>(Some());
    auto opt2 = Optional<Void>(None());
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
    auto opt1 = nel::Optional<int>();

    auto opt2 = nel::Optional<int>();

    opt2 = move(opt1);

    SECTION("must move val to dest")
    {
        REQUIRE(opt2.is_inval());
    }
    SECTION("must invalidate src")
    {
        REQUIRE(opt1.is_inval());
    }
}

TEST_CASE("Optional<>(some).is_some()", "[optional]")
{
    auto opt1 = Optional<int>(Some(1));

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

TEST_CASE("Optional<Void>(some).is_some()", "[optional]")
{
    auto opt1 = Optional<Void>(Some());

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
    auto opt1 = Optional<int>(None());

    SECTION("must return false")
    {
        REQUIRE_FALSE(opt1.is_some());
    }
    SECTION("must not alter src")
    {
        // check value has not been invalidated
        REQUIRE(opt1.is_none());
    }
}

TEST_CASE("Optional<Void>(none).is_some()", "[optional]")
{
    auto opt1 = Optional<Void>(None());

    SECTION("must return false")
    {
        REQUIRE_FALSE(opt1.is_some());
    }
    SECTION("must not alter src")
    {
        // check value has not been invalidated
        REQUIRE(opt1.is_none());
    }
}

TEST_CASE("Optional<>(inval).is_some()", "[optional]")
{
    auto opt1 = Optional<int>(None());
    auto opt2 = move(opt1);

    SECTION("must panic")
    {
        REQUIRE_PANIC(opt1.is_some());
    }
}

TEST_CASE("Optional<Void>(inval).is_some()", "[optional]")
{
    auto opt1 = Optional<Void>(None());
    auto opt2 = move(opt1);

    SECTION("must panic")
    {
        REQUIRE_PANIC(opt1.is_some());
    }
}

TEST_CASE("Optional<>(some).is_none()", "[optional]")
{
    auto opt1 = Optional<int>(Some(1));

    SECTION("must return false")
    {
        REQUIRE_FALSE(opt1.is_none());
    }
    SECTION("must not alter src")
    {
        // check value has not been invalidated.
        REQUIRE(opt1.is_some());
        // check value has not been changed.
        REQUIRE(opt1.unwrap() == 1);
    }
}

TEST_CASE("Optional<Void>(some).is_none()", "[optional]")
{
    auto opt1 = Optional<Void>(Some());

    SECTION("must return false")
    {
        REQUIRE_FALSE(opt1.is_none());
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
    auto opt1 = Optional<int>(None());

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

TEST_CASE("Optional<Void>(none).is_none()", "[optional]")
{
    auto opt1 = Optional<Void>(None());

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
    auto opt1 = Optional<int>();

    SECTION("must panic")
    {
        REQUIRE_PANIC(opt1.is_none());
    }
}

TEST_CASE("Optional<Void>(inval).is_none()", "[optional]")
{
    auto opt1 = Optional<Void>();

    SECTION("must panic")
    {
        REQUIRE_PANIC(opt1.is_none());
    }
}

TEST_CASE("Optional<>(some).unwrap()", "[optional]")
{
    auto opt1 = Optional<int>(Some(1));

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

TEST_CASE("Optional<Void>(some).unwrap()", "[optional]")
{
    auto opt1 = Optional<Void>(Some());

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
    auto opt1 = Optional<int>(None());

    SECTION("panics")
    {
        REQUIRE_PANIC(opt1.unwrap());
    }
}

TEST_CASE("Optional<Void>(none).unwrap()", "[optional]")
{
    auto opt1 = Optional<Void>(None());

    SECTION("panics")
    {
        REQUIRE_PANIC(opt1.unwrap());
    }
}

TEST_CASE("Optional<>(inval).unwrap()", "[optional]")
{
    auto opt1 = Optional<int>();

    SECTION("panics")
    {
        REQUIRE_PANIC(opt1.unwrap());
    }
}

TEST_CASE("Optional<Void>(inval).unwrap()", "[optional]")
{
    auto opt1 = Optional<Void>();

    SECTION("panics")
    {
        REQUIRE_PANIC(opt1.unwrap());
    }
}

TEST_CASE("Optional<>(some).unwrap_or(other)", "[optional]")
{
    auto opt1 = Optional<int>(Some(1));

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

TEST_CASE("Optional<Void>(some).unwrap_or(other)", "[optional]")
{
    auto opt1 = Optional<Void>(Some());

    SECTION("returns original value")
    {
        // check that it doesn't panic..
        opt1.unwrap_or(Void());
    }
    SECTION("invalidates src")
    {
        opt1.unwrap_or(Void());
        REQUIRE(opt1.is_inval());
    }
}

TEST_CASE("Optional<>(none).unwrap_or(other)", "[optional]")
{
    auto opt1 = Optional<int>(None());

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

TEST_CASE("Optional<Void>(none).unwrap_or(other)", "[optional]")
{
    auto opt1 = Optional<Void>(None());

    SECTION("returns other value")
    {
        // check that it doesn't panic..
        opt1.unwrap_or(Void());
    }
    SECTION("invalidates src")
    {
        opt1.unwrap_or(Void());
        REQUIRE(opt1.is_inval());
    }
}

TEST_CASE("Optional<>(inval).unwrap_or(other)", "[optional]")
{
    auto opt1 = Optional<int>();

    SECTION("must panic")
    {
        // i.e. use after move.
        REQUIRE_PANIC(opt1.unwrap_or(2));
    }
}

TEST_CASE("Optional<>(some).eq()", "[optional]")
{
    auto opt1 = Optional<int>(Some(1));

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
        REQUIRE_FALSE(opt1 == nel::Some(2));
    }
    SECTION("must return false if compared to none")
    {
        REQUIRE_FALSE(opt1 == None());
    }
    SECTION("must panic if compared to invalid")
    {
        auto opt2 = nel::Optional<int>();
        REQUIRE_PANIC(opt1 == opt2);
    }
}

TEST_CASE("Optional<Void>(some).eq()", "[optional]")
{
    auto opt1 = Optional<Void>(Some());

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
    //     REQUIRE_FALSE(opt1 == nel::Some(2));
    // }
    SECTION("must return false if compared to none")
    {
        REQUIRE_FALSE(opt1 == None());
    }
    SECTION("must panic if compared to invalid")
    {
        auto opt2 = Optional<Void>();
        REQUIRE_PANIC(opt1 == opt2);
    }
}

TEST_CASE("Optional<>(none).eq()", "[optional]")
{
    auto opt1 = Optional<int>(None());

    SECTION("must return true for same value")
    {
        REQUIRE(opt1 == nel::Optional<int>(None()));
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
    //     REQUIRE_FALSE(opt1 == nel::Some(2));
    // }
    SECTION("must return false for Some value")
    {
        REQUIRE_FALSE(opt1 == nel::Some(2));
    }
    SECTION("must panic if compared to invalid")
    {
        auto opt2 = nel::Optional<int>();
        REQUIRE_PANIC(opt1 == opt2);
    }
}

TEST_CASE("Optional<Void>(none).eq()", "[optional]")
{
    auto opt1 = nel::Optional<Void>(None());

    SECTION("must return true for same value")
    {
        REQUIRE(opt1 == None());
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
    //     REQUIRE_FALSE(opt1 == nel::Some(2));
    // }
    SECTION("must return false for Some value")
    {
        REQUIRE_FALSE(opt1 == nel::Some());
    }
    SECTION("must panic if compared to invalid")
    {
        auto opt2 = Optional<Void>();
        REQUIRE_PANIC(opt1 == opt2);
    }
}

TEST_CASE("Optional<>(inval).eq()", "[optional]")
{
    auto opt1 = Optional<int>();

    SECTION("must panic if compared to same value")
    {
        auto opt10 = Optional<int>();
        REQUIRE_PANIC(opt1 == opt10);
    }
    SECTION("must panic if compared to same obj")
    {
        REQUIRE_PANIC(opt1 == opt1);
    }
    SECTION("must panic if compared to Some value")
    {
        REQUIRE_PANIC(opt1 == Some(1));
    }
}

TEST_CASE("Optional<Void>(inval).eq()", "[optional]")
{
    auto opt1 = nel::Optional<Void>();

    SECTION("must panic if compared to same value")
    {
        auto opt10 = Optional<Void>();
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
}

TEST_CASE("Optional<>(some).ne()", "[optional]")
{
    auto opt1 = Optional<int>(Some(1));

    SECTION("must return false if compared to same value")
    {
        REQUIRE_FALSE(opt1 != nel::Some(1));
    }
    SECTION("must return false if compared to same obj")
    {
        REQUIRE_FALSE(opt1 != opt1);
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
        REQUIRE(opt1 != None());
    }
    SECTION("must panic if compared to invalid")
    {
        auto opt2 = nel::Optional<int>();
        REQUIRE_PANIC(opt1 != opt2);
    }
}

TEST_CASE("Optional<Void>(some).ne()", "[optional]")
{
    auto opt1 = Optional<Void>(Some());

    SECTION("must return false if compared to same value")
    {
        REQUIRE_FALSE(opt1 != nel::Some());
    }
    SECTION("must return false if compared to same obj")
    {
        REQUIRE_FALSE(opt1 != opt1);
    }
    SECTION("must not change src")
    {
        /// check that does not panic
        opt1.unwrap();
    }
    // SECTION("must return false for diff value") {
    //     REQUIRE_FALSE(opt1 == nel::Some(2));
    // }
    SECTION("must return true if compared to none")
    {
        REQUIRE(opt1 != None());
    }
    SECTION("must panic if compared to invalid")
    {
        auto opt2 = nel::Optional<Void>();
        REQUIRE_PANIC(opt1 != opt2);
    }
}

TEST_CASE("Optional<>(none).ne()", "[optional]")
{
    auto opt1 = Optional<int>(None());

    SECTION("must return false for same value")
    {
        REQUIRE_FALSE(opt1 != nel::Optional<int>(None()));
    }
    SECTION("must return false if compared to same obj")
    {
        REQUIRE_FALSE(opt1 != opt1);
    }
    SECTION("must not change src")
    {
        REQUIRE(opt1.is_none());
    }
    // SECTION("must return false for diff value") {
    //     REQUIRE_FALSE(opt1 == nel::Some(2));
    // }
    SECTION("must return true for Some value")
    {
        REQUIRE_FALSE(opt1 == nel::Some(2));
    }
    SECTION("must panic if compared to invalid")
    {
        auto opt2 = nel::Optional<int>();
        REQUIRE_PANIC(opt1 != opt2);
    }
}

TEST_CASE("Optional<Void>(none).ne()", "[optional]")
{
    auto opt1 = nel::Optional<Void>(None());

    SECTION("must return false for same value")
    {
        REQUIRE_FALSE(opt1 != nel::Optional<Void>(None()));
    }
    SECTION("must return false if compared to same obj")
    {
        REQUIRE_FALSE(opt1 != opt1);
    }
    SECTION("must not change src")
    {
        REQUIRE(opt1.is_none());
    }
    // SECTION("must return false for diff value") {
    //     REQUIRE_FALSE(opt1 == nel::Some(2));
    // }
    SECTION("must return true for Some value")
    {
        REQUIRE(opt1 != nel::Some());
    }
    SECTION("must panic if compared to invalid")
    {
        auto opt2 = nel::Optional<Void>();
        REQUIRE_PANIC(opt1 != opt2);
    }
}

TEST_CASE("Optional<>(inval).ne()", "[optional]")
{
    auto opt1 = Optional<int>();

    SECTION("must panic if compared to same value")
    {
        auto opt10 = nel::Optional<int>();
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
}

TEST_CASE("Optional<Void>(inval).ne()", "[optional]")
{
    auto opt1 = nel::Optional<Void>();

    SECTION("must panic if compared to same value")
    {
        auto opt10 = nel::Optional<Void>();
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
}

nel::Optional<int> foo_some()
{
    return nel::Some(1);
}

TEST_CASE("Optional(some).?  into Optional", "[optional]")
{
    REQUIRE(foo_some().is_some());
}

nel::Optional<int> foo_none()
{
    return nel::None();
}

TEST_CASE("Optional(none()) auto into Optional", "[optional]")
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
    auto opt1 = Optional<int>(Some(1));
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

TEST_CASE("Optional<Void>(some).map()", "[optional]")
{
    auto opt1 = Optional<Void>(Some());
    auto opt2 = opt1.map<char const *>([](Void &&) -> char const * { return "haha"; });

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
    auto opt1 = Optional<int>(None());
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

TEST_CASE("Optional<Void>(none).map", "[optional]")
{
    auto opt1 = Optional<Void>(None());
    auto opt2 = opt1.map<char const *>([](Void &&) -> char const * { return "haha"; });

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
    auto opt1 = Optional<int>();

    REQUIRE_PANIC(opt1.map<char const *>([](auto &&) -> char const * { return "haha"; }));
}

TEST_CASE("Optional<Void>(inval).map: panics", "[optional]")
{
    auto opt1 = Optional<Void>();

    REQUIRE_PANIC(opt1.map<char const *>([](Void &&) -> char const * { return "haha"; }));
}

}; // namespace optional
}; // namespace test
}; // namespace nel
