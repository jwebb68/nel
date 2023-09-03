// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(NEL_TIME_INSTANT_HH)
#    define NEL_TIME_INSTANT_HH

namespace nel
{
namespace time
{
class Instant;
};
}; // namespace nel

#    include <nel/time/duration.hh>
#    include <inttypes.h> // uint64_t

namespace nel
{
namespace time
{
class Instant
{
    private:
        uint64_t usecs_;

    public:
        // Instant(Instant const &);
        // Instant operator=(Instant const &);

    public:
        constexpr Instant()
            : usecs_(0)
        {
        }

    private:
        constexpr explicit Instant(uint64_t usecs)
            : usecs_(usecs)
        {
        }

    public:
        static Instant now(void);

        static constexpr Instant boot(void)
        {
            return Instant(0);
        }

    public:
        constexpr Duration elapsed_since(Instant const &tp) const
        {
            return Duration::from_micros(usecs_ - tp.usecs_);
        }

        constexpr bool operator==(Instant const &o) const
        {
            return usecs_ == o.usecs_;
        }

        constexpr bool operator!=(Instant const &o) const
        {
            return usecs_ != o.usecs_;
        }

        constexpr Instant &operator+=(Duration const &d)
        {
            usecs_ += d.as_micros();
            return *this;
        }

        constexpr Instant &operator-=(Duration const &d)
        {
            usecs_ -= d.as_micros();
            return *this;
        }

    public:
        // arduino micros/millis is a 32bit number so does not
        // have the range wanted (wraps in about 70 mins or so).
        // So, keep internal counter, and uptick based on last.
        static bool init(uint32_t now);
        static void tick(uint32_t now);

#    if defined(ARDUINO_TEENSY41)
        static bool init();
        static void tick();
#    endif
};

}; // namespace time
}; // namespace nel

#endif // defined(NEL_TIME_INSTANT_HH)
