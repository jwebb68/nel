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

    auto fmt = nel::Formatter(nel::log);
    queue1.dbgfmt(fmt) << '\n';
}

void ex2()
{
    auto queue1 = Queue1::empty();

    U8Buf<256> v = U8Buf<256>((uint8_t)0x66);
    queue1.push(nel::move(v)).is_ok();

    auto fmt = nel::Formatter(nel::log);
    queue1.dbgfmt(fmt) << '\n';
}

void ex3()
{
    auto queue1 = Queue1::empty();

    queue1.push((uint8_t)0x67).is_ok();

    auto fmt = nel::Formatter(nel::log);
    queue1.dbgfmt(fmt) << '\n';

    auto e = queue1.pop();
    e.dbgfmt(fmt) << '\n';
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
}
