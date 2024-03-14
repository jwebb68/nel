// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include "largestruct1.hh"

#include <nel/optional.hh>
#include <nel/log.hh>
#include <nel/defs.hh>
#include "libnosys_stubs.cx"

char const *T(char const *v)
{
    return v;
}

void ex1()
{
    nel::Optional<char const *> a = nel::Some(T("ex1"));
}

void ex2()
{
    nel::Optional<char const *> a = nel::Some(T("ex2"));
    auto fmt = nel::Formatter(nel::log);
    fmt << a.is_some() << '\n';
}

void ex3()
{
    nel::Optional<char const *> a = nel::Some(T("ex3"));
    auto fmt = nel::Formatter(nel::log);
    fmt << a.unwrap() << '\n';
}

struct Bool
{
    private:
        bool v_;

    public:
        constexpr Bool(bool v)
            : v_(v)
        {
        }

        constexpr operator bool() const
        {
            return v_;
        }

    public:
        nel::Formatter &dbgfmt(nel::Formatter &fmt) const
        {
            return fmt << v_;
        }
};

void ex4()
{
    nel::Optional<char const *> a = nel::Some(T("ex3"));
    auto fmt = nel::Formatter(nel::log);
    a.map<Bool>([](auto &&) -> Bool { return Bool(true); }).dbgfmt(fmt) << '\n';
}

int main()
{
    auto fmt = nel::Formatter(nel::log);

    fmt << "ex1:b" << '\n';
    ex1();
    fmt << "ex1:e" << '\n';
    fmt << "ex2:b" << '\n';
    ex2();
    fmt << "ex2:e" << '\n';
    fmt << "ex3:b" << '\n';
    ex3();
    fmt << "ex3:e" << '\n';
    fmt << "ex4:b" << '\n';
    ex4();
    fmt << "ex4:e" << '\n';
}
