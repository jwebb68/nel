#include "optional.hh"

#include <catch2/catch.hpp>

TEST_CASE("optional::Some works", "[optional]") {
    auto opt1 = nel::Optional<int>(nel::Optional<int>::Some(1));

    REQUIRE(opt1.is_some());
    REQUIRE(!opt1.is_none());
    REQUIRE(opt1.unwrap() == 1);
    REQUIRE(opt1.unwrap_or(2) == 1);
}

TEST_CASE("optional::None works", "[optional]") {
    auto opt1 = nel::Optional<int>(nel::Optional<int>::None());

    REQUIRE(!opt1.is_some());
    REQUIRE(opt1.is_none());
    REQUIRE(opt1.unwrap_or(2) == 2);
}

TEST_CASE("optional::Some into Optional", "[optional]") {
    nel::Optional<int> opt1 = nel::Optional<int>::Some(1);

    REQUIRE(opt1.unwrap() == 1);
}

TEST_CASE("optional::None into Optional", "[optional]") {
    nel::Optional<int> opt1 = nel::Optional<int>::None();

    REQUIRE(opt1.unwrap_or(2) == 2);
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

TEST_CASE("optional::Some dtor called", "[optional]") {
    bool bar = false;
    {
        nel::Optional<Foo> opt1 = nel::Optional<Foo>::Some(Foo(&bar));

        // REQUIRE(!bar);
        bar == false;
    }
    REQUIRE(bar);
}
