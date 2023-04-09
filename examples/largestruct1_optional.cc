// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include "largestruct1.hh"

#include <nel/optional.hh>
#include <nel/log.hh>
#include <nel/defs.hh>

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
    nel::log << a.is_some() << '\n';
}

void ex3()
{
    nel::Optional<char const *> a = nel::Some(T("ex3"));
    nel::log << a.unwrap() << '\n';
}

void ex4()
{
    nel::Optional<char const *> a = nel::Some(T("ex3"));
    nel::log << a.map<bool>([](auto &&) -> bool { return true; }) << '\n';
}

int main()
{
    nel::log << "ex1:b" << '\n';
    ex1();
    nel::log << "ex1:e" << '\n';
    nel::log << "ex2:b" << '\n';
    ex2();
    nel::log << "ex2:e" << '\n';
    nel::log << "ex3:b" << '\n';
    ex3();
    nel::log << "ex3:e" << '\n';
    nel::log << "ex4:b" << '\n';
    ex4();
    nel::log << "ex4:e" << '\n';
}
