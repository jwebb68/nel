#include "optional.hh"

#include <catch2/catch.hpp>
#include <iostream>

#define UNUSED(v) ((void)(v))


TEST_CASE("optional::Some for some must produce a some", "[optional]")
{
    auto opt1a = nel::Optional<int>::Some(1);

    auto val = opt1a.unwrap();

    REQUIRE(val == 1);
}

TEST_CASE("optional::None for none must produce a none", "[optional]")
{
    auto opt1b = nel::Optional<int>::None();

    auto val = opt1b.unwrap_or(2);

    REQUIRE(val == 2);
}

TEST_CASE("std::move(optional)-ctor for some must move val to dest",
          "[optional]")
{
    auto opt1c = nel::Optional<int>::Some(1);
    auto opt2 = std::move(opt1c);

    auto val = opt2.unwrap();

    REQUIRE(val == 1);
}

TEST_CASE("std::move(optional)-ctor for none must move val to dest",
          "[optional]")
{
    auto opt1d = nel::Optional<int>::None();

    auto val = opt1d.unwrap_or(2);

    REQUIRE(val == 2);
}

TEST_CASE("std::move(optional)-ctor for inval must move val to dest",
          "[optional]")
{
    auto opt1e = nel::Optional<int>::None();
    auto opt2 = std::move(opt1e);
    auto opt3 = std::move(opt1e);

    REQUIRE(!opt3.is_some());
    REQUIRE(!opt3.is_none());
}


TEST_CASE("std::move(optional)-ctor for some must invalidate src", "[optional]")
{
    auto opt1f = nel::Optional<int>::Some(1);
    auto opt2 = std::move(opt1f);

    REQUIRE(!opt1f.is_some());
    REQUIRE(!opt1f.is_none());
}


TEST_CASE("std::move(optional)-ctor for none must invalidate src", "[optional]")
{
    auto opt1g = nel::Optional<int>::None();
    auto opt2 = std::move(opt1g);

    REQUIRE(!opt1g.is_some());
    REQUIRE(!opt1g.is_none());
}

TEST_CASE("std::move(optional)-ctor for inval must invalidate src",
          "[optional]")
{
    auto opt1 = nel::Optional<int>::None();
    auto opt2 = std::move(opt1);
    auto opt3 = std::move(opt1);

    REQUIRE(!opt1.is_some());
    REQUIRE(!opt1.is_none());
}


TEST_CASE("std::move(optional)-ass for some must move val to dest",
          "[optional]")
{
    auto opt1 = nel::Optional<int>::Some(1);
    auto opt2 = nel::Optional<int>::Some(2);
    opt2 = std::move(opt1);

    auto val = opt2.unwrap();

    REQUIRE(val == 1);
}

TEST_CASE("std::move(optional)-ass for none must move val to dest",
          "[optional]")
{
    auto opt1 = nel::Optional<int>::None();
    auto opt2 = nel::Optional<int>::Some(1);
    opt2 = std::move(opt1);

    auto val = opt2.unwrap_or(2);

    REQUIRE(val == 2);
}

TEST_CASE("std::move(optional)-ass for inval must move val to dest",
          "[optional]")
{
    auto opt1 = nel::Optional<int>::None();
    auto opt2 = std::move(opt1);

    auto opt3 = nel::Optional<int>::Some(1);
    opt3 = std::move(opt1);

    REQUIRE(!opt3.is_some());
    REQUIRE(!opt3.is_none());
}

TEST_CASE("std::move(optional)-ass for some must invalidate src", "[optional]")
{
    auto opt1 = nel::Optional<int>::Some(1);

    auto opt2 = nel::Optional<int>::Some(2);
    opt2 = std::move(opt1);

    REQUIRE(!opt1.is_some());
    REQUIRE(!opt1.is_none());
}

TEST_CASE("std::move(optional)-ass for none must invalidate src", "[optional]")
{
    auto opt1 = nel::Optional<int>::None();

    auto opt2 = nel::Optional<int>::Some(2);
    opt2 = std::move(opt1);

    REQUIRE(!opt1.is_some());
    REQUIRE(!opt1.is_none());
}

TEST_CASE("std::move(optional)-ass for inval must invalidate src", "[optional]")
{
    auto opt1 = nel::Optional<int>::None();
    auto opt2 = std::move(opt1);

    auto opt3 = nel::Optional<int>::Some(2);
    opt3 = std::move(opt1);

    REQUIRE(!opt1.is_some());
    REQUIRE(!opt1.is_none());
}



TEST_CASE("Optional.is_some for some must give true", "[optional]")
{
    auto opt1 = nel::Optional<int>::Some(1);

    REQUIRE(opt1.is_some());
}

TEST_CASE("Optional.is_some for none must give false", "[optional]")
{
    auto opt1 = nel::Optional<int>::None();

    REQUIRE(!opt1.is_some());
}

TEST_CASE("Optional.is_some for inval must give false", "[optional]")
{
    auto opt1 = nel::Optional<int>::None();
    auto opt2 = std::move(opt1);

    REQUIRE(!opt1.is_some());
}


TEST_CASE("Optional.is_some for some must not alter src", "[optional]")
{
    auto opt1 = nel::Optional<int>::Some(1);

    auto is = opt1.is_some();
    UNUSED(is);

    auto val = opt1.unwrap();
    REQUIRE(val == 1);
}

TEST_CASE("Optional.is_some for none must not alter src", "[optional]")
{
    auto opt1 = nel::Optional<int>::None();

    auto is = opt1.is_some();
    UNUSED(is);

    REQUIRE(opt1.is_none());
}

TEST_CASE("Optional.is_some for inval must not alter src", "[optional]")
{
    auto opt1 = nel::Optional<int>::None();
    auto opt2 = std::move(opt1);

    auto is = opt1.is_some();
    UNUSED(is);

    REQUIRE(!opt1.is_some());
    REQUIRE(!opt1.is_none());
}


TEST_CASE("Optional.is_none for some must give false", "[optional]")
{
    auto opt1 = nel::Optional<int>::Some(1);

    REQUIRE(!opt1.is_none());
}

TEST_CASE("Optional.is_none for none must give true", "[optional]")
{
    auto opt1 = nel::Optional<int>::None();

    REQUIRE(opt1.is_none());
}

TEST_CASE("Optional.is_none for inval must give false", "[optional]")
{
    auto opt1 = nel::Optional<int>::None();
    auto opt2 = std::move(opt1);

    REQUIRE(!opt1.is_none());
}

TEST_CASE("Optional.is_none for some must not alter src", "[optional]")
{
    auto opt1 = nel::Optional<int>::Some(1);

    auto is = opt1.is_none();
    UNUSED(is);

    auto val = opt1.unwrap();
    REQUIRE(val == 1);
}

TEST_CASE("Optional.is_none for none must not alter src", "[optional]")
{
    auto opt1 = nel::Optional<int>::None();

    auto is = opt1.is_none();
    UNUSED(is);

    REQUIRE(opt1.is_none());
}

TEST_CASE("Optional.is_none for inval must not alter src", "[optional]")
{
    auto opt1 = nel::Optional<int>::None();
    auto opt2 = std::move(opt1);

    auto is = opt1.is_none();
    UNUSED(is);

    REQUIRE(!opt1.is_none());
    REQUIRE(!opt1.is_some());
}


TEST_CASE("Optional.unwrap for Some must give some value", "[optional]")
{
    auto opt1 = nel::Optional<int>::Some(1);

    auto val = opt1.unwrap();

    REQUIRE(val == 1);
}

// TEST_CASE("Optional.unwrap for None aborts", "[optional]")
// {
// how do I do this?
// }

// TEST_CASE("Optional.unwrap for Inval aborts", "[optional]")
// {
// how do I do this?
// }

TEST_CASE("Optional.unwrap for Some must invalidate src", "[optional]")
{
    auto opt1 = nel::Optional<int>::Some(1);

    auto val = opt1.unwrap();
    UNUSED(val);

    REQUIRE(!opt1.is_some());
    REQUIRE(!opt1.is_none());
}



TEST_CASE("Optional.unwrap_or for Some must give some value", "[optional]")
{
    auto opt1a = nel::Optional<int>::Some(1);

    auto val = opt1a.unwrap_or(2);

    REQUIRE(val == 1);
}

TEST_CASE("Optional.unwrap_or for None must give or value", "[optional]")
{
    auto opt1b = nel::Optional<int>::None();

    auto val = opt1b.unwrap_or(2);

    REQUIRE(val == 2);
}

TEST_CASE("Optional.unwrap_or for Inval must give or value", "[optional]")
{
    auto opt1c = nel::Optional<int>::None();
    auto opt2 = std::move(opt1c);

    auto val = opt1c.unwrap_or(2);

    REQUIRE(val == 2);
}

TEST_CASE("Optional.unwrap_or for Some must invalidate src", "[optional]")
{
    auto opt1d = nel::Optional<int>::Some(1);

    auto val = opt1d.unwrap_or(2);
    UNUSED(val);

    REQUIRE(!opt1d.is_some());
    REQUIRE(!opt1d.is_none());
}

TEST_CASE("Optional.unwrap_or for None must invalidate src", "[optional]")
{
    auto opt1e = nel::Optional<int>::None();

    auto val = opt1e.unwrap_or(2);
    UNUSED(val);

    REQUIRE(!opt1e.is_some());
    REQUIRE(!opt1e.is_none());
}

TEST_CASE("Optional.unwrap_or for Inval must invalidate src", "[optional]")
{
    auto opt1f = nel::Optional<int>::None();
    auto opt2 = std::move(opt1f);

    auto val = opt1f.unwrap_or(2);
    UNUSED(val);

    REQUIRE(!opt1f.is_some());
    REQUIRE(!opt1f.is_none());
}


TEST_CASE("Optional.eq for some with some must give true", "[optional]")
{
    auto opt1 = nel::Optional<int>::Some(1);

    REQUIRE(opt1 == nel::Optional<int>::Some(1));
}

TEST_CASE("Optional.eq for some with diff some must give false", "[optional]")
{
    auto opt1 = nel::Optional<int>::Some(1);

    REQUIRE(!(opt1 == nel::Optional<int>::Some(2)));
}

TEST_CASE("Optional.eq for some with none must give false", "[optional]")
{
    auto opt1 = nel::Optional<int>::Some(1);

    REQUIRE(!(opt1 == nel::Optional<int>::None()));
}

TEST_CASE("Optional.eq for some with inval must give false", "[optional]")
{
    auto opt1 = nel::Optional<int>::Some(1);

    auto opt2 = nel::Optional<int>::Some(1);
    auto opt3 = std::move(opt2);

    REQUIRE(!(opt1 == opt2));
}

TEST_CASE("Optional.eq for some with some must not change src", "[optional]")
{
    auto opt1 = nel::Optional<int>::Some(1);

    auto is_eq = opt1 == nel::Optional<int>::Some(1);
    UNUSED(is_eq);

    REQUIRE(opt1.unwrap() == 1);
}

TEST_CASE("Optional.eq for some with diff some must not change src",
          "[optional]")
{
    auto opt1 = nel::Optional<int>::Some(1);

    auto is_eq = opt1 == nel::Optional<int>::Some(2);
    UNUSED(is_eq);

    REQUIRE(opt1.unwrap() == 1);
}

TEST_CASE("Optional.eq for some with none must not change src", "[optional]")
{
    auto opt1 = nel::Optional<int>::Some(1);

    auto is_eq = opt1 == nel::Optional<int>::None();
    UNUSED(is_eq);

    REQUIRE(opt1.unwrap() == 1);
}

TEST_CASE("Optional.eq for some with inval must not change src", "[optional]")
{
    auto opt1 = nel::Optional<int>::Some(1);

    auto opt2 = nel::Optional<int>::Some(1);
    auto opt3 = std::move(opt2);

    auto is_eq = opt1 == opt2;
    UNUSED(is_eq);

    REQUIRE(opt1.unwrap() == 1);
}



TEST_CASE("Optional.eq for none with some must give false", "[optional]")
{
    auto opt1 = nel::Optional<int>::None();

    REQUIRE(!(opt1 == nel::Optional<int>::Some(1)));
}

TEST_CASE("Optional.eq for none with none must give true", "[optional]")
{
    auto opt1 = nel::Optional<int>::None();

    REQUIRE(opt1 == nel::Optional<int>::None());
}

TEST_CASE("Optional.eq for none with inval must give false", "[optional]")
{
    auto opt1 = nel::Optional<int>::None();

    auto opt2 = nel::Optional<int>::Some(1);
    auto opt3 = std::move(opt2);

    REQUIRE(!(opt1 == opt2));
}


TEST_CASE("Optional.eq for none with some must not change src", "[optional]")
{
    auto opt1 = nel::Optional<int>::None();

    auto is_eq = opt1 == nel::Optional<int>::Some(1);
    UNUSED(is_eq);

    REQUIRE(opt1.is_none());
}

TEST_CASE("Optional.eq for none with none must not change src", "[optional]")
{
    auto opt1 = nel::Optional<int>::None();

    auto is_eq = opt1 == nel::Optional<int>::None();
    UNUSED(is_eq);

    REQUIRE(opt1.is_none());
}

TEST_CASE("Optional.eq for none with inval must not change src", "[optional]")
{
    auto opt1 = nel::Optional<int>::None();

    auto opt2 = nel::Optional<int>::Some(1);
    auto opt3 = std::move(opt2);

    auto is_eq = opt1 == opt2;
    UNUSED(is_eq);

    REQUIRE(opt1.is_none());
}


TEST_CASE("Optional.eq for inval with some must give false", "[optional]")
{
    auto opt1 = nel::Optional<int>::None();
    auto opt2 = std::move(opt1);

    REQUIRE(!(opt1 == nel::Optional<int>::Some(1)));
}


TEST_CASE("Optional.eq for inval with none must give false", "[optional]")
{
    auto opt1 = nel::Optional<int>::None();
    auto opt2 = std::move(opt1);

    REQUIRE(!(opt1 == nel::Optional<int>::None()));
}

TEST_CASE("Optional.eq for inval with inval must give true", "[optional]")
{
    auto opt1 = nel::Optional<int>::None();
    auto opt2 = std::move(opt1);

    auto opt3 = nel::Optional<int>::Some(1);
    auto opt4 = std::move(opt3);

    REQUIRE(opt1 == opt3);
}


TEST_CASE("Optional.eq for inval with some must not change src", "[optional]")
{
    auto opt1 = nel::Optional<int>::None();
    auto opt2 = std::move(opt1);

    auto is_eq = opt1 == nel::Optional<int>::Some(1);
    UNUSED(is_eq);

    REQUIRE(!opt1.is_some());
    REQUIRE(!opt1.is_none());
}


TEST_CASE("Optional.eq for inval with none must not change src", "[optional]")
{
    auto opt1 = nel::Optional<int>::None();
    auto opt2 = std::move(opt1);

    auto is_eq = opt1 == nel::Optional<int>::None();
    UNUSED(is_eq);

    REQUIRE(!opt1.is_some());
    REQUIRE(!opt1.is_none());
}

TEST_CASE("Optional.eq for inval with inval must not change src", "[optional]")
{
    auto opt1 = nel::Optional<int>::None();
    auto opt2 = std::move(opt1);

    auto opt3 = nel::Optional<int>::Some(1);
    auto opt4 = std::move(opt3);

    auto is_eq = opt1 == opt3;
    UNUSED(is_eq);

    REQUIRE(!opt1.is_some());
    REQUIRE(!opt1.is_none());
}


TEST_CASE("Optional.neq for some with some must give false", "[optional]")
{
    auto opt1 = nel::Optional<int>::Some(1);

    REQUIRE(!(opt1 != nel::Optional<int>::Some(1)));
}

TEST_CASE("Optional.neq for some with diff some must give true", "[optional]")
{
    auto opt1 = nel::Optional<int>::Some(1);

    REQUIRE(opt1 != nel::Optional<int>::Some(2));
}

TEST_CASE("Optional.neq for some with none must give true", "[optional]")
{
    auto opt1 = nel::Optional<int>::Some(1);

    REQUIRE(opt1 != nel::Optional<int>::None());
}

TEST_CASE("Optional.neq for some with inval must give true", "[optional]")
{
    auto opt1 = nel::Optional<int>::Some(1);

    auto opt2 = nel::Optional<int>::Some(1);
    auto opt3 = std::move(opt2);

    REQUIRE(opt1 != opt2);
}

TEST_CASE("Optional.neq for some with some must not alter src", "[optional]")
{
    auto opt1 = nel::Optional<int>::Some(1);

    auto is_neq = opt1 != nel::Optional<int>::Some(1);
    UNUSED(is_neq);

    REQUIRE(opt1.unwrap() == 1);
}

TEST_CASE("Optional.neq for some with diff must not alter src", "[optional]")
{
    auto opt1 = nel::Optional<int>::Some(1);

    auto is_neq = opt1 != nel::Optional<int>::Some(2);
    UNUSED(is_neq);

    REQUIRE(opt1.unwrap() == 1);
}

TEST_CASE("Optional.neq for some with none must not alter src", "[optional]")
{
    auto opt1 = nel::Optional<int>::Some(1);

    auto is_neq = opt1 != nel::Optional<int>::None();
    UNUSED(is_neq);

    REQUIRE(opt1.unwrap() == 1);
}

TEST_CASE("Optional.neq for some with inval must not alter src", "[optional]")
{
    auto opt1 = nel::Optional<int>::Some(1);

    auto opt2 = nel::Optional<int>::Some(2);
    auto opt3 = std::move(opt2);

    auto is_neq = opt1 != opt2;
    UNUSED(is_neq);

    REQUIRE(opt1.unwrap() == 1);
}


TEST_CASE("Optional.neq for none with some must give true", "[optional]")
{
    auto opt1 = nel::Optional<int>::None();

    REQUIRE(opt1 != nel::Optional<int>::Some(1));
}

TEST_CASE("Optional.neq for none with none must give false", "[optional]")
{
    auto opt1 = nel::Optional<int>::None();

    REQUIRE(!(opt1 != nel::Optional<int>::None()));
}

TEST_CASE("Optional.neq for none with inval must give true", "[optional]")
{
    auto opt1 = nel::Optional<int>::None();

    auto opt2 = nel::Optional<int>::Some(1);
    auto opt3 = std::move(opt2);

    REQUIRE(opt1 != opt2);
}

TEST_CASE("Optional.neq for none with some must not alter src", "[optional]")
{
    auto opt1 = nel::Optional<int>::None();

    auto is_neq = opt1 != nel::Optional<int>::Some(1);
    UNUSED(is_neq);

    REQUIRE(opt1.is_none());
}

TEST_CASE("Optional.neq for none with none must not alter src", "[optional]")
{
    auto opt1 = nel::Optional<int>::None();

    auto is_neq = opt1 != nel::Optional<int>::None();
    UNUSED(is_neq);

    REQUIRE(opt1.is_none());
}

TEST_CASE("Optional.neq for none with inval must not alter src", "[optional]")
{
    auto opt1 = nel::Optional<int>::None();

    auto opt2 = nel::Optional<int>::Some(1);
    auto opt3 = std::move(opt2);

    auto is_neq = opt1 != opt2;
    UNUSED(is_neq);

    REQUIRE(opt1.is_none());
}



TEST_CASE("Optional.neq for inval with some must give true", "[optional]")
{
    auto opt1 = nel::Optional<int>::None();
    auto opt2 = std::move(opt1);

    REQUIRE(opt1 != nel::Optional<int>::Some(1));
}

TEST_CASE("Optional.neq for inval with none must give true", "[optional]")
{
    auto opt1 = nel::Optional<int>::None();
    auto opt2 = std::move(opt1);

    REQUIRE(opt1 != nel::Optional<int>::None());
}

TEST_CASE("Optional.neq for inval with inval must give false", "[optional]")
{
    auto opt1 = nel::Optional<int>::None();
    auto opt2 = std::move(opt1);

    auto opt3 = nel::Optional<int>::Some(1);
    auto opt4 = std::move(opt3);

    REQUIRE(!(opt1 != opt3));
}

TEST_CASE("Optional.neq for inval with some must not alter src", "[optional]")
{
    auto opt1 = nel::Optional<int>::None();
    auto opt2 = std::move(opt1);

    auto is_neq = opt1 != nel::Optional<int>::Some(1);
    UNUSED(is_neq);

    REQUIRE(!opt1.is_none());
    REQUIRE(!opt1.is_some());
}

TEST_CASE("Optional.neq for inval with none must not alter src", "[optional]")
{
    auto opt1 = nel::Optional<int>::None();
    auto opt2 = std::move(opt1);

    auto is_neq = opt1 != nel::Optional<int>::None();
    UNUSED(is_neq);

    REQUIRE(!opt1.is_none());
    REQUIRE(!opt1.is_some());
}

TEST_CASE("Optional.neq for inval with inval must not alter src", "[optional]")
{
    auto opt1 = nel::Optional<int>::None();
    auto opt2 = std::move(opt1);

    auto opt3 = nel::Optional<int>::Some(1);
    auto opt4 = std::move(opt3);

    auto is_neq = opt1 != opt3;
    UNUSED(is_neq);

    REQUIRE(!opt1.is_none());
    REQUIRE(!opt1.is_some());
}




nel::Optional<int> foo_some()
{
    return nel::Optional<int>::Some(1);
}
TEST_CASE("optional::Some auto into Optional", "[optional]")
{
    REQUIRE(foo_some().is_some());
}

nel::Optional<int> foo_none()
{
    return nel::Optional<int>::None();
}
TEST_CASE("optional::None auto into Optional", "[optional]")
{
    REQUIRE(foo_none().is_none());
}

struct Foo {
    public:
        ~Foo(void)
        {
            this->dtor_called = true;
        }
        Foo(bool &dtor_called): dtor_called(dtor_called)
        {
        }

    private:
        bool &dtor_called;
};

TEST_CASE("optional::~dtor for some, must call some dtor", "[optional]")
{
    bool dtor_called = false;
    {
        nel::Optional<Foo> opt1 = nel::Optional<Foo>::Some(Foo(dtor_called));

        dtor_called = false;
    }
    REQUIRE(dtor_called);
}
