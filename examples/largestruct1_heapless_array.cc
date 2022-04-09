#include "largestruct1.hh"

#include <nel/heapless/array.hh>
#include <nel/panic.hh>
#include <nel/log.hh>
#include <nel/defs.hh>

#include <utility> // std::move

typedef nel::heapless::Array<U8Buf<256>, 8> Array1;

void ex1() {
    // what should this give?
    Array1 arr1 = Array1::fill(U8Buf<256>());

    nel::log << arr1 << "\n";
}

#if 0
void ex2() {
    Array1 arr1 {
        U8Buf<256>(0x26),
        U8Buf<256>(0x27),
        U8Buf<256>(0x28),
        U8Buf<256>(0x29),
        U8Buf<256>(0x2a),
        U8Buf<256>(0x2b),
        U8Buf<256>(0x2c),
        U8Buf<256>(0x2d),
     };

    nel::log << arr1 << "\n";
}
#endif

#if 0
void ex3() {
    Array1 arr1 {
        {(uint8_t)0x36},
        {(uint8_t)0x37},
        {(uint8_t)0x38},
        {(uint8_t)0x39},
        {(uint8_t)0x3a},
        {(uint8_t)0x3b},
        {(uint8_t)0x3c},
        {(uint8_t)0x3d},
     };

    nel::log << arr1 << "\n";
}
#endif

void ex4() {
    Array1 arr1 = Array1::fill(U8Buf<256>());

    arr1[0] = U8Buf<256>(0x11);
    arr1[5] = U8Buf<256>(0x22);
    arr1[6] = U8Buf<256>(0x33);

    auto it = arr1.iter();
    while (true) {
        auto e = it.next();
        // checking for none and unwrapping/ from opt
        // causes bigger loops
        // unwrap if some with other, with ind to say if value came from opt or passed in.
        // i.e. if let Some(x) = e {..}
        // up = e.unwrap_or_if(sentinel)
        // if (up[0]) { x = up[1]; ... }
        // T x;
        // if (e.some(x)) { ... }
        if (e.is_none()) {
            break;
        }
        // calls is_some() but always is_some..
        // opt does optimise it out but is there a better way?
        nel::log << e.unwrap() << "\n";
    }
}

void ex41() {
    Array1 arr1 = Array1::fill(U8Buf<256>());

    arr1[0] = U8Buf<256>(0x11);
    arr1[5] = U8Buf<256>(0x22);
    arr1[6] = U8Buf<256>(0x33);

    auto it = first_n_it(arr1.iter(), 2);
    while (true) {
        auto e = it.next();
        // checking for none and unwrapping/ from opt
        // causes bigger loops
        // unwrap if some with other, with ind to say if value came from opt or passed in.
        // i.e. if let Some(x) = e {..}
        // up = e.unwrap_or_if(sentinel)
        // if (up[0]) { x = up[1]; ... }
        // T x;
        // if (e.some(x)) { ... }
        if (e.is_none()) {
            break;
        }
        // calls is_some() but always is_some..
        // opt does optimise it out but is there a better way?
        nel::log << e.unwrap() << "\n";
    }
}


void ex5() {
    Array1 arr1 = Array1::fill(U8Buf<256>());

    arr1[0] = U8Buf<256>(0x44);
    arr1[5] = U8Buf<256>(0x55);
    arr1[6] = U8Buf<256>(0x66);

    for (auto en = arr1.enumerate(); !en.is_done(); en.inc()) {
        // user can miss out the &, so forcing a copy/move of items being enumerated.
        auto & v = en.get();
        nel::log << v << "\n";
    }

    // how to enumerators stack..?
    // enum taking values from a second?
}

void ex51() {
    Array1 arr1 = Array1::fill(U8Buf<256>());

    arr1[0] = U8Buf<256>(0x77);
    arr1[5] = U8Buf<256>(0x88);
    arr1[6] = U8Buf<256>(0x99);

    for (auto en = first_n_en(arr1.enumerate(), 9); !en.is_done(); en.inc()) {
        // user can miss out the &, so forcing a copy/move of items being enumerated.
        auto & v = en.get();
        nel::log << v << "\n";
    }

    // how to enumerators stack..?
    // enum taking values from a second?
}


void ex6() {
    Array1 arr1 = Array1::fill(U8Buf<256>());

    arr1[0] = U8Buf<256>(0xaa);
    arr1[5] = U8Buf<256>(0xbb);
    arr1[6] = U8Buf<256>(0xcc);

    for (auto en = arr1.enumerate(); en; ++en) {
        auto & v = *en;
        nel::log << v << "\n";
    }
}

int main() {
    nel::log << "ex1:b" << "\n";
    ex1();
    nel::log << "ex1:e" << "\n";
    // nel::log << "ex2:b" << "\n";
    // ex2();
    // nel::log << "ex2:e" << "\n";
    // nel::log << "ex3:b" << "\n";
    // ex3();
    // nel::log << "ex3:e" << "\n";
    nel::log << "ex4:b" << "\n";
    ex4();
    nel::log << "ex4:e" << "\n";
    nel::log << "ex41:b" << "\n";
    ex41();
    nel::log << "ex41:e" << "\n";
    nel::log << "ex5:b" << "\n";
    ex5();
    nel::log << "ex5:e" << "\n";
    nel::log << "ex6:b" << "\n";
    ex6();
    nel::log << "ex6:e" << "\n";
}
