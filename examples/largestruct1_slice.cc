// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include "largestruct1.hh"

#include <nel/heaped/vector.hh>
#include <nel/log.hh>
#include <nel/defs.hh>

typedef nel::heaped::Vector<U8Buf<256>> Vec1;

void ex1(void)
{
    Vec1 vec1 = Vec1::with_capacity(5);

    vec1.push((uint8_t)0x67).is_ok();
    vec1.push((uint8_t)0x68).is_ok();
    vec1.push((uint8_t)0x69).is_ok();

    auto s1 = vec1.slice();
    nel::log << s1.len() << '\n' << s1;
}

int main()
{
    nel::log << "ex1:b" << '\n';
    ex1();
    nel::log << "ex1:e" << '\n';
}
