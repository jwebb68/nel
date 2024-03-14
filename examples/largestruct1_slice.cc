// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include "largestruct1.hh"

#include <nel/heaped/vector.hh>
#include <nel/log.hh>
#include <nel/defs.hh>
#include "libnosys_stubs.cx"

typedef nel::heaped::Vector<U8Buf<256>> Vec1;

void ex1(void)
{
    Vec1 vec1 = Vec1::with_capacity(5);
    auto fmt = nel::Formatter(nel::log);

    vec1.push((uint8_t)0x67).is_ok();
    vec1.push((uint8_t)0x68).is_ok();
    vec1.push((uint8_t)0x69).is_ok();

    auto s1 = vec1.slice();
    fmt << s1.len() << '\n';
    s1.dbgfmt(fmt);
}

int main()
{
    auto fmt = nel::Formatter(nel::log);

    fmt << "ex1:b" << '\n';
    ex1();
    fmt << "ex1:e" << '\n';
}
