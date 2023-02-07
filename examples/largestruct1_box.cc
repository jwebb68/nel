// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include "largestruct1.hh"

#include <nel/heaped/box.hh>
#include <nel/log.hh>
#include <nel/defs.hh>

typedef nel::heaped::Box<U8Buf<256>> Box1;

nel::Log &operator<<(nel::Log &outs, Box1 const &v)
{
    NEL_UNUSED(v);
    outs << "Box1()";
    return outs;
}

void box1()
{
    U8Buf<256> v = U8Buf<256>((uint8_t)0x66);
    auto r = Box1::try_from(nel::move(v));
    Box1 b = r.unwrap();

    nel::log << b << '\n';
}

void box2()
{
    auto r = Box1::try_from((uint8_t)0x67);
    Box1 b = r.unwrap();

    nel::log << b << '\n';
}

int main() {}
