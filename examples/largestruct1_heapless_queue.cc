// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include "largestruct1.hh"

#include <nel/heapless/queue.hh>
#include <nel/log.hh>
#include <nel/memory.hh>
#include <nel/defs.hh>
#include "libnosys_stubs.cx"

typedef nel::heapless::Queue<U8Buf<256>, 5> Queue1;

void ex1()
{
    auto queue1 = Queue1::empty();

    nel::log << queue1 << '\n';
}

void ex2()
{
    auto queue1 = Queue1::empty();

    U8Buf<256> v = U8Buf<256>((uint8_t)0x66);
    queue1.push(nel::move(v)).is_ok();

    nel::log << queue1 << '\n';
}

void ex3()
{
    auto queue1 = Queue1::empty();

    queue1.push((uint8_t)0x67).is_ok();

    nel::log << queue1 << '\n';

    auto e = queue1.pop();
    nel::log << e << '\n';
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
}
