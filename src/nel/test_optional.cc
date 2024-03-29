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

TEST_CASE("optional(Some): unwrap for some must produce some's value", "[optional]")
{
    nel::Optional<int> opt1a = nel::Some(1);

    auto val = opt1a.unwrap();

    REQUIRE(val == 1);
}

TEST_CASE("optional(None): unwrap_or(2) must produce 2", "[optional]")
{
    nel::Optional<int> opt1b = nel::None;

    auto val = opt1b.unwrap_or(2);

    REQUIRE(val == 2);
}

TEST_CASE("move(optional)-ctor for some must move val to dest", "[optional]")
{
    nel::Optional<int> opt1c = nel::Some(1);
    auto opt2 = move(opt1c);

    auto val = opt2.unwrap();

    REQUIRE(val == 1);
}

TEST_CASE("move(optional)-ctor for none must move val to dest", "[optional]")
{
    nel::Optional<int> opt1d = nel::None;

    auto val = opt1d.unwrap_or(2);

    REQUIRE(val == 2);
}

#if 0
// disabled until panic testing is available
TEST_CASE("move(optional)-ctor for inval must move val to dest", "[optional]")
{
    nel::Optional<int> opt1e = nel::None;
    auto opt2 = move(opt1e);
    auto opt3 = move(opt1e);

    REQUIRE(!opt3.is_some());
    REQUIRE(!opt3.is_none());
}
#endif

#if 0
// disabled until panic testing is available
TEST_CASE("move(optional)-ctor for some must invalidate src", "[optional]")
{
    nel::Optional<int> opt1f = nel::Some(1);
    auto opt2 = move(opt1f);

    REQUIRE(!opt1f.is_some());
    REQUIRE(!opt1f.is_none());
}
#endif

#if 0
// disabled until panic testing is available
TEST_CASE("move(optional)-ctor for none must invalidate src", "[optional]")
{
    nel::Optional<int> opt1g = nel::None;
    auto opt2 = move(opt1g);

    REQUIRE(!opt1g.is_some());
    REQUIRE(!opt1g.is_none());
}
#endif

#if 0
// disabled until panic testing is available
TEST_CASE("move(optional)-ctor for inval must invalidate src", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;
    auto opt2 = move(opt1);
    auto opt3 = move(opt1);

    REQUIRE(!opt1.is_some());
    REQUIRE(!opt1.is_none());
}
#endif

TEST_CASE("move(optional)-ass for some must move val to dest", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);
    nel::Optional<int> opt2 = nel::Some(2);
    opt2 = move(opt1);

    auto val = opt2.unwrap();

    REQUIRE(val == 1);
}

TEST_CASE("move(optional)-ass for none must move val to dest", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;
    nel::Optional<int> opt2 = nel::Some(1);
    opt2 = move(opt1);

    auto val = opt2.unwrap_or(2);

    REQUIRE(val == 2);
}

#if 0
// disabled until panic testing is available
TEST_CASE("move(optional)-ass for inval must move val to dest", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;
    auto opt2 = move(opt1);

    nel::Optional<int> opt3 = nel::Some(1);
    opt3 = move(opt1);

    REQUIRE(!opt3.is_some());
    REQUIRE(!opt3.is_none());
}
#endif

#if 0
// disabled until panic testing is available
TEST_CASE("move(optional)-ass for some must invalidate src", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    nel::Optional<int> opt2 = nel::Some(2);
    opt2 = move(opt1);

    REQUIRE(!opt1.is_some());
    REQUIRE(!opt1.is_none());
}
#endif

#if 0
// disabled until panic testing is available
TEST_CASE("move(optional)-ass for none must invalidate src", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;

    nel::Optional<int> opt2 = nel::Some(2);
    opt2 = move(opt1);

    REQUIRE(!opt1.is_some());
    REQUIRE(!opt1.is_none());
}
#endif

#if 0
// disabled until panic testing is available
TEST_CASE("move(optional)-ass for inval must invalidate src", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;
    auto opt2 = move(opt1);

    nel::Optional<int> opt3 = nel::Some(2);
    opt3 = move(opt1);

    REQUIRE(!opt1.is_some());
    REQUIRE(!opt1.is_none());
}
#endif

TEST_CASE("Optional.is_some for some must give true", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    REQUIRE(opt1.is_some());
}

TEST_CASE("Optional.is_some for none must give false", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;

    REQUIRE(!opt1.is_some());
}

#if 0
// disabled until panic testing is available
TEST_CASE("Optional.is_some for inval must panic", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;
    auto opt2 = move(opt1);

    REQUIRE(!opt1.is_some());
}
#endif

TEST_CASE("Optional.is_some for some must not alter src", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    auto is = opt1.is_some();
    NEL_UNUSED(is);

    auto val = opt1.unwrap();
    REQUIRE(val == 1);
}

TEST_CASE("Optional.is_some for none must not alter src", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;

    auto is = opt1.is_some();
    NEL_UNUSED(is);

    REQUIRE(opt1.is_none());
}

#if 0
// disabled until panic testing is available
TEST_CASE("Optional.is_some for inval must not alter src", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;
    auto opt2 = move(opt1);

    auto is = opt1.is_some();
    NEL_UNUSED(is);

    REQUIRE(!opt1.is_some());
    REQUIRE(!opt1.is_none());
}
#endif

TEST_CASE("Optional.is_none for some must give false", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    REQUIRE(!opt1.is_none());
}

TEST_CASE("Optional.is_none for none must give true", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;

    REQUIRE(opt1.is_none());
}

#if 0
// disabled until panic testing is available
TEST_CASE("Optional.is_none for inval must panic", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;
    auto opt2 = move(opt1);

    REQUIRE(!opt1.is_none());
}
#endif

TEST_CASE("Optional.is_none for some must not alter src", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    auto is = opt1.is_none();
    NEL_UNUSED(is);

    auto val = opt1.unwrap();
    REQUIRE(val == 1);
}

TEST_CASE("Optional.is_none for none must not alter src", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;

    auto is = opt1.is_none();
    NEL_UNUSED(is);

    REQUIRE(opt1.is_none());
}

#if 0
// disabled until panic testing is available
TEST_CASE("Optional.is_none for inval must not alter src", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;
    auto opt2 = move(opt1);

    auto is = opt1.is_none();
    NEL_UNUSED(is);

    REQUIRE(!opt1.is_none());
    REQUIRE(!opt1.is_some());
}
#endif

TEST_CASE("Optional.unwrap for Some must give some value", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    auto val = opt1.unwrap();

    REQUIRE(val == 1);
}

#if 0
TEST_CASE("Optional.unwrap for None aborts", "[optional]")
{
// how do I do this?
}

TEST_CASE("Optional.unwrap for Inval aborts", "[optional]")
{
// how do I do this?
}
#endif

#if 0
// disabled until panic testing is available
TEST_CASE("Optional.unwrap for Some must invalidate src", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    auto val = opt1.unwrap();
    NEL_UNUSED(val);

    REQUIRE(!opt1.is_some());
    REQUIRE(!opt1.is_none());
}
#endif

TEST_CASE("Optional.unwrap with default for Some must give some value", "[optional]")
{
    nel::Optional<int> opt1a = nel::Some(1);

    auto val = opt1a.unwrap_or(2);

    REQUIRE(val == 1);
}

TEST_CASE("Optional.unwrap with default for None must give or value", "[optional]")
{
    nel::Optional<int> opt1b = nel::None;

    auto val = opt1b.unwrap_or(2);

    REQUIRE(val == 2);
}

#if 0
// disabled until panic testing is available
TEST_CASE("Optional.unwrap with default for Inval must give or value", "[optional]")
{
    nel::Optional<int> opt1c = nel::None;
    auto opt2 = move(opt1c);

    auto val = opt1c.unwrap_or(2);

    REQUIRE(val == 2);
}
#endif

#if 0
// disabled until panic testing is available
TEST_CASE("Optional.unwrap with default for Some must invalidate src", "[optional]")
{
    nel::Optional<int> opt1d = nel::Some(1);

    auto val = opt1d.unwrap_or(2);
    NEL_UNUSED(val);

    REQUIRE(!opt1d.is_some());
    REQUIRE(!opt1d.is_none());
}
#endif

#if 0
// disabled until panic testing is available
TEST_CASE("Optional.unwrap with default for None must invalidate src", "[optional]")
{
    nel::Optional<int> opt1e = nel::None;

    auto val = opt1e.unwrap_or(2);
    NEL_UNUSED(val);

    REQUIRE(!opt1e.is_some());
    REQUIRE(!opt1e.is_none());
}
#endif

#if 0
// disabled until panic testing is available
TEST_CASE("Optional.unwrap with default for Inval must invalidate src", "[optional]")
{
    nel::Optional<int> opt1f = nel::None;
    auto opt2 = move(opt1f);

    auto val = opt1f.unwrap_or(2);
    NEL_UNUSED(val);

    REQUIRE(!opt1f.is_some());
    REQUIRE(!opt1f.is_none());
}
#endif

#if 0
TEST_CASE("Optional.eq for some with some must give true", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    REQUIRE(opt1 == nel::Some(1));
}


TEST_CASE("Optional.eq for some with diff some must give false", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    REQUIRE(!(opt1 == nel::Some(2)));
}


TEST_CASE("Optional.eq for some with none must give false", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    REQUIRE(!(opt1 == None));
}


TEST_CASE("Optional.eq for some with inval must give false", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    auto opt2 = nel::Some(1);
    auto opt3 = move(opt2);

    REQUIRE(!(opt1 == opt2));
}


TEST_CASE("Optional.eq for some with some must not change src", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    auto is_eq = opt1 == nel::Some(1);
    NEL_UNUSED(is_eq);

    REQUIRE(opt1.unwrap() == 1);
}


TEST_CASE("Optional.eq for some with diff some must not change src",
          "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    auto is_eq = opt1 == nel::Some(2);
    NEL_UNUSED(is_eq);

    REQUIRE(opt1.unwrap() == 1);
}


TEST_CASE("Optional.eq for some with none must not change src", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    nel::Optional<int> is_eq = opt1 == nel::None;
    NEL_UNUSED(is_eq);

    REQUIRE(opt1.unwrap() == 1);
}


TEST_CASE("Optional.eq for some with inval must not change src", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    auto opt2 = nel::Some(1);
    auto opt3 = move(opt2);

    auto is_eq = opt1 == opt2;
    NEL_UNUSED(is_eq);

    REQUIRE(opt1.unwrap() == 1);
}


TEST_CASE("Optional.eq for none with some must give false", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;

    REQUIRE(!(opt1 == nel::Some(1)));
}


TEST_CASE("Optional.eq for none with none must give true", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;

    REQUIRE(opt1 == None);
}


TEST_CASE("Optional.eq for none with inval must give false", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;

    auto opt2 = nel::Some(1);
    auto opt3 = move(opt2);

    REQUIRE(!(opt1 == opt2));
}



TEST_CASE("Optional.eq for none with some must not change src", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;

    nel::Optional<int> is_eq = opt1 == nel::Some(1);
    NEL_UNUSED(is_eq);

    REQUIRE(opt1.is_none());
}


TEST_CASE("Optional.eq for none with none must not change src", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;

    nel::Optional<int> is_eq = opt1 == nel::None;
    NEL_UNUSED(is_eq);

    REQUIRE(opt1.is_none());
}


TEST_CASE("Optional.eq for none with inval must not change src", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;

    nel::Optional<int> opt2 = nel::Some(1);
    auto opt3 = move(opt2);

    auto is_eq = opt1 == opt2;
    NEL_UNUSED(is_eq);

    REQUIRE(opt1.is_none());
}



TEST_CASE("Optional.eq for inval with some must give false", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;
    auto opt2 = move(opt1);

    REQUIRE(!(opt1 == nel::Some(1)));
}



TEST_CASE("Optional.eq for inval with none must give false", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;
    auto opt2 = move(opt1);

    REQUIRE(!(opt1 == None));
}


TEST_CASE("Optional.eq for inval with inval must give true", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;
    auto opt2 = move(opt1);

    nel::Optional<int> opt3 = nel::Some(1);
    auto opt4 = move(opt3);

    REQUIRE(opt1 == opt3);
}



TEST_CASE("Optional.eq for inval with some must not change src", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;
    auto opt2 = move(opt1);

    nel::Optional<int> is_eq = opt1 == nel::Some(1);
    NEL_UNUSED(is_eq);

    REQUIRE(!opt1.is_some());
    REQUIRE(!opt1.is_none());
}



TEST_CASE("Optional.eq for inval with none must not change src", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;
    auto opt2 = move(opt1);

    nel::Optional<int> is_eq = opt1 == nel::None;
    NEL_UNUSED(is_eq);

    REQUIRE(!opt1.is_some());
    REQUIRE(!opt1.is_none());
}


TEST_CASE("Optional.eq for inval with inval must not change src", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;
    auto opt2 = move(opt1);

    nel::Optional<int> opt3 = nel::Some(1);
    auto opt4 = move(opt3);

    auto is_eq = opt1 == opt3;
    NEL_UNUSED(is_eq);

    REQUIRE(!opt1.is_some());
    REQUIRE(!opt1.is_none());
}



TEST_CASE("Optional.neq for some with some must give false", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    REQUIRE(!(opt1 != nel::Some(1)));
}


TEST_CASE("Optional.neq for some with diff some must give true", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    REQUIRE(opt1 != nel::Some(2));
}


TEST_CASE("Optional.neq for some with none must give true", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    REQUIRE(opt1 != None);
}


TEST_CASE("Optional.neq for some with inval must give true", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    nel::Optional<int> opt2 = nel::Some(1);
    auto opt3 = move(opt2);

    REQUIRE(opt1 != opt2);
}


TEST_CASE("Optional.neq for some with some must not alter src", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    auto is_neq = opt1 != nel::Some(1);
    NEL_UNUSED(is_neq);

    REQUIRE(opt1.unwrap() == 1);
}


TEST_CASE("Optional.neq for some with diff must not alter src", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    auto is_neq = opt1 != nel::Some(2);
    NEL_UNUSED(is_neq);

    REQUIRE(opt1.unwrap() == 1);
}


TEST_CASE("Optional.neq for some with none must not alter src", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    auto is_neq = opt1 != nel::None;
    NEL_UNUSED(is_neq);

    REQUIRE(opt1.unwrap() == 1);
}


TEST_CASE("Optional.neq for some with inval must not alter src", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    nel::Optional<int> opt2 = nel::Some(2);
    auto opt3 = move(opt2);

    auto is_neq = opt1 != opt2;
    NEL_UNUSED(is_neq);

    REQUIRE(opt1.unwrap() == 1);
}



TEST_CASE("Optional.neq for none with some must give true", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;

    REQUIRE(opt1 != nel::Some(1));
}


TEST_CASE("Optional.neq for none with none must give false", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;

    REQUIRE(!(opt1 != None));
}


TEST_CASE("Optional.neq for none with inval must give true", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;

    nel::Optional<int> opt2 = nel::Some(1);
    auto opt3 = move(opt2);

    REQUIRE(opt1 != opt2);
}


TEST_CASE("Optional.neq for none with some must not alter src", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;

    auto is_neq = opt1 != nel::Some(1);
    NEL_UNUSED(is_neq);

    REQUIRE(opt1.is_none());
}


TEST_CASE("Optional.neq for none with none must not alter src", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;

    auto is_neq = opt1 != nel::None;
    NEL_UNUSED(is_neq);

    REQUIRE(opt1.is_none());
}


TEST_CASE("Optional.neq for none with inval must not alter src", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;

    nel::Optional<int> opt2 = nel::Some(1);
    auto opt3 = move(opt2);

    auto is_neq = opt1 != opt2;
    NEL_UNUSED(is_neq);

    REQUIRE(opt1.is_none());
}


TEST_CASE("Optional.neq for inval with some must give true", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;
    auto opt2 = move(opt1);

    REQUIRE(opt1 != nel::Some(1));
}


TEST_CASE("Optional.neq for inval with none must give true", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;
    auto opt2 = move(opt1);

    REQUIRE(opt1 != None);
}


TEST_CASE("Optional.neq for inval with inval must give false", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;
    auto opt2 = move(opt1);

    nel::Optional<int> opt3 = nel::Some(1);
    auto opt4 = move(opt3);

    REQUIRE(!(opt1 != opt3));
}


TEST_CASE("Optional.neq for inval with some must not alter src", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;
    auto opt2 = move(opt1);

    auto is_neq = opt1 != nel::Some(1);
    NEL_UNUSED(is_neq);

    REQUIRE(!opt1.is_none());
    REQUIRE(!opt1.is_some());
}


TEST_CASE("Optional.neq for inval with none must not alter src", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;
    auto opt2 = move(opt1);

    nel::Optional<int> is_neq = opt1 != nel::None;
    NEL_UNUSED(is_neq);

    REQUIRE(!opt1.is_none());
    REQUIRE(!opt1.is_some());
}


TEST_CASE("Optional.neq for inval with inval must not alter src", "[optional]")
{
    nel::Optional<int> opt1 = nel::None;
    auto opt2 = move(opt1);

    nel::Optional<int> opt3 = nel::Some(1);
    auto opt4 = move(opt3);

    auto is_neq = opt1 != opt3;
    NEL_UNUSED(is_neq);

    REQUIRE(!opt1.is_none());
    REQUIRE(!opt1.is_some());
}
#endif

nel::Optional<int> foo_some()
{
    return nel::Some(1);
    // return nel::nel::Some(1);
}

TEST_CASE("optional::Some auto into Optional", "[optional]")
{
    REQUIRE(foo_some().is_some());
}

nel::Optional<int> foo_none()
{
    return nel::None;
}

TEST_CASE("optional::None auto into Optional", "[optional]")
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

TEST_CASE("optional::~dtor for some, must call some dtor", "[optional]")
{
    bool dtor_called = false;
    {
        nel::Optional<Foo> opt1 = nel::Some(Foo(dtor_called));

        dtor_called = false;
    }
    REQUIRE(dtor_called);
}

TEST_CASE("optional::map for some must produce a some", "[optional]")
{
    nel::Optional<int> opt1a = nel::Some(1);
    auto opt2a = opt1a.map<char const *>([](auto &&) -> char const * { return "haha"; });
    auto val = opt2a.unwrap();

    REQUIRE(strcmp(val, "haha") == 0);
}

TEST_CASE("optional::map for none must produce a none", "[optional]")
{
    nel::Optional<int> opt1a = nel::None;
    auto opt2a = opt1a.map<char const *>([](auto &&) -> char const * { return "haha"; });
    REQUIRE(opt2a.is_none());
}

}; // namespace optional
}; // namespace test
}; // namespace nel
