// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include <nel/time/instant.hh>

#if defined(ARDUINO_TEENSY41)
#    include <Arduino.h> // micros()
#endif

namespace nel
{
namespace time
{

// Instant, a 64bit number, wraps in:
// 18446744073709551615 micros
// 1.844674407370955e+16 millis
// 18446744073709.55 secs
// 307445734561.82587 mins
// 5124095576.030431 hrs
// 213503982.33460128 days
// 584942.417355072 years..
static Instant last;
static uint32_t last_usecs;

bool Instant::init(uint32_t now_usecs)
{
    last = Instant::boot();
    last_usecs = now_usecs;
    return true;
}

#if defined(ARDUINO_TEENSY41)
bool Instant::init()
{
    return Instant::init(micros());
}
#endif

void Instant::tick(uint32_t now_usecs)
{
    // use the same type as system so it wraps the same.
    auto dur_usecs = now_usecs - last_usecs;
    last_usecs = now_usecs;
    last += Duration::from_micros(dur_usecs);
}

#if defined(ARDUINO_TEENSY41)
void Instant::tick()
{
    Instant::tick(micros());
}
#endif

Instant Instant::now(void)
{
    return last;
}

}; // namespace time
}; // namespace nel
