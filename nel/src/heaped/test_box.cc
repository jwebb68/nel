#include "heaped/box.hh"

#include <catch2/catch.hpp>
//#include <iostream>


TEST_CASE("Box", "[heaped][box]")
{
    // must be able to create an empty array..
    auto a1 = nel::heaped::Box<int>(1);

    // must create not empty.
    REQUIRE(a1.has_value());

}


TEST_CASE("Box::move", "[heaped][box]")
{
    // must be able to move box contents from one box to another.
    auto a1 = nel::heaped::Box<int>(1);
    auto a2 = std::move(a1);

    // moved from is no empty
    REQUIRE(!a1.has_value());

    // moved to is not empty
    REQUIRE(a2.has_value());

    // moved to has old a1 value
    REQUIRE(*a2 == 1);

    // A const boxed cannot be moved?
    // what is const, the box or the value?
}

// TODO: test Box copy must fail compile.

TEST_CASE("Box::deref", "[heaped][box]")
{
    // deref must return value value put into the box.
    // mutable version
    auto a1 = nel::heaped::Box<int>(1);
    REQUIRE(*a1 == 1);

    // and value is not consumed by the deref
    REQUIRE(*a1 == 1);

    // and can mutate the boxed value
    *a1 = 3;
    REQUIRE(*a1 == 3);


    // const version
    auto const c1 = nel::heaped::Box<int>(2);
    REQUIRE(*c1 == 2);
    // and value is not consumed by the deref
    REQUIRE(*c1 == 2);

    // and cannot mutate the boxed value
    // *c1 = 3; // compile fail
    // REQUIRE(*a1 == 2);

}


TEST_CASE("Box::has_value", "[heaped][box]")
{
    // a newly created boxed always has a value.
    auto a1 = nel::heaped::Box<int>(1);
    REQUIRE(a1.has_value());

    // moving the boxed value to another causes the variable to lose it's value.
    auto a2 = std::move(a1);
    REQUIRE(!a1.has_value());
    REQUIRE(a2.has_value());
}


TEST_CASE("Box::unwrap", "[heaped][box]")
{
    // unwrap of empty produces panic..
    // can box ever be created empty?
    // TODO: how to test this?
    // auto a1 = nel::heaped::Box<int>();
    // REQUIRE(!a1.has_value());
    // REQUIRE(a1.unwrap());

    // unwrap of value returns that value.
    // and invalidates the box.
    auto a2 = nel::heaped::Box<int>(1);
    REQUIRE(a2.unwrap() == 1);
    REQUIRE(!a2.has_value());

    // cannot unwrap const box?
    // unwraping invalidates box so cannot be const.. (?)
    // TODO: how to test fails compile?
    // auto const c1 = nel::heaped::Box<int>(2);
    // REQUIRE(c2.unwrap() == 2);

    // unwrap of a box containing a const must work.
    auto a3 = nel::heaped::Box<int const>(2);
    REQUIRE(a3.unwrap() == 2);
    REQUIRE(!a3.has_value());
}

#if 0
TEST_CASE("Box::eq", "[heaped][box]")
{
    auto a0 = nel::heaped::Box<int>(0);
    auto a1 = nel::heaped::Box<int>(1);
    auto a2 = nel::heaped::Box<int>(2);
    auto i = nel::heaped::Box<int>(1);
    REQUIRE(i == a1);
    REQUIRE(i != a2);
    REQUIRE(i > a0);
    REQUIRE(i < a2);
}
#endif
