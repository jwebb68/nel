#include "largestruct1.hh"

#include <nel/heaped/vector.hh>
#include <nel/log.hh>
#include <nel/defs.hh>

#include <utility> // std::move

typedef nel::heaped::Vector<U8Buf<256>> Vec1;

void ex1() {
    Vec1 vec1 = Vec1::with_capacity(5);

    nel::log << vec1 << "\n";
}

void ex2() {
    Vec1 vec1 = Vec1::with_capacity(5);

    U8Buf<256> v = U8Buf<256>((uint8_t)0x66);
    vec1.push_back(std::move(v));

    nel::log << vec1 << "\n";
}

void ex3() {
    Vec1 vec1 = Vec1::with_capacity(5);

    vec1.push_back((uint8_t)0x67);

    nel::log << vec1 << "\n";
}

void ex4() {
    Vec1 vec1 = {
        ((uint8_t)0x67),
        ((uint8_t)0x68),
        ((uint8_t)0x69),
    };

    auto it = vec1.iter();
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
    Vec1 vec1 = Vec1::with_capacity(5);

    vec1.push_back((uint8_t)0x67);
    vec1.push_back((uint8_t)0x68);
    vec1.push_back((uint8_t)0x69);

    for (auto en = vec1.enumerate(); !en.is_done(); en.inc()) {
        // user can miss out the &, so forcing a copy/move of items being enumerated.
        auto & v = en.get();
        nel::log << v << "\n";
    }

    // how to enumerators stack..?
    // enum taking values from a second?
}

void ex6() {
    Vec1 vec1 = Vec1::with_capacity(5);

    vec1.push_back((uint8_t)0x67);
    vec1.push_back((uint8_t)0x68);
    vec1.push_back((uint8_t)0x69);

    for (auto en = vec1.enumerate(); en; ++en) {
        auto & v = *en;
        nel::log << v << "\n";
    }
}

int main() {
    nel::log << "ex1:b" << "\n";
    ex1();
    nel::log << "ex1:e" << "\n";
    nel::log << "ex2:b" << "\n";
    ex2();
    nel::log << "ex2:e" << "\n";
    nel::log << "ex3:b" << "\n";
    ex3();
    nel::log << "ex3:e" << "\n";
    nel::log << "ex4:b" << "\n";
    ex4();
    nel::log << "ex4:e" << "\n";
    nel::log << "ex5:b" << "\n";
    ex5();
    nel::log << "ex5:e" << "\n";
    nel::log << "ex6:b" << "\n";
    ex6();
    nel::log << "ex6:e" << "\n";
}
