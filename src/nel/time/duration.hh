// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(NEL_TIME_DURATION_HH)
#    define NEL_TIME_DURATION_HH

namespace nel
{
namespace time
{

struct Duration;
struct Frequency;

} // namespace time
} // namespace nel

#    include <inttypes.h> // uint64_t

/**
 * Model time as a secs + fractional where the target platform does not have a
 * native type large enough to handle any time value withot wwrapping around
 * over the lifetime of the product.
 *
 * negative values may be an issue.
 * TODO: handle -ve values.
 */
struct Timespec
{
        uint64_t secs;
        uint64_t usecs;

    public:
        /**
         * keep fractionals under their limit.
         */
        constexpr Timespec &normalise()
        {
            uint64_t const micros = 1000000UL;
            secs += usecs / micros;
            usecs %= micros;
            return *this;
        }

        constexpr Timespec &operator+=(Timespec const &o)
        {
            secs += o.secs;
            usecs += o.usecs;
            return normalise();
        }

        constexpr Timespec operator+(Timespec const &o) const
        {
            return Timespec(*this) += o;
        }

        constexpr Timespec &operator-=(Timespec const &o)
        {
            // TODO: problem if going negative.
            secs -= o.secs;
            usecs -= o.usecs;
            return normalise();
        }

        constexpr Timespec operator-(Timespec const &o) const
        {
            return Timespec(*this) -= o;
        }

        constexpr Timespec operator*=(uint64_t s)
        {
            secs *= s;
            usecs *= s;
            return normalise();
        }

        constexpr Timespec operator*=(float s)
        {
            secs *= s;
            usecs *= s;
            return normalise();
        }

        constexpr Timespec operator*=(double s)
        {
            secs *= s;
            usecs *= s;
            return normalise();
        }

        constexpr Timespec operator*(uint64_t s) const
        {
            return Timespec(*this) *= s;
        }

        constexpr Timespec operator*(float s) const
        {
            return Timespec(*this) *= s;
        }

        constexpr Timespec operator*(double s) const
        {
            return Timespec(*this) *= s;
        }

        constexpr Timespec operator/=(uint64_t s)
        {
            secs /= s;
            usecs /= s;
            return normalise();
        }

        constexpr Timespec operator/=(float s)
        {
            secs /= s;
            usecs /= s;
            return normalise();
        }

        constexpr Timespec operator/=(double s)
        {
            secs /= s;
            usecs /= s;
            return normalise();
        }

        constexpr Timespec operator/(uint64_t s) const
        {
            return Timespec(*this) /= s;
        }

        constexpr Timespec operator/(float s) const
        {
            return Timespec(*this) /= s;
        }

        constexpr Timespec operator/(double s) const
        {
            return Timespec(*this) /= s;
        }

    public:
        constexpr bool operator==(Timespec const &o) const
        {
            return secs == o.secs && usecs == o.usecs;
        }

        constexpr bool operator!=(Timespec const &o) const
        {
            return secs != o.secs || usecs != o.usecs;
        }

        constexpr bool operator<(Timespec const &o) const
        {
            return secs < o.secs || usecs < o.usecs;
        }

        constexpr bool operator>(Timespec const &o) const
        {
            return secs > o.secs || usecs > o.usecs;
        }

        constexpr bool operator<=(Timespec const &o) const
        {
            return secs <= o.secs || usecs <= o.usecs;
        }

        constexpr bool operator>=(Timespec const &o) const
        {
            return secs >= o.secs || usecs >= o.usecs;
        }
};

namespace nel
{
namespace time
{

/**
 * Duration
 * A period of time between two instances of time.
 */
struct Duration
{
    private:
        uint64_t usecs_;

    public:
        constexpr Duration(void)
            : usecs_(0)
        {
        }

    private:
        constexpr explicit Duration(uint64_t usecs)
            : usecs_(usecs)
        {
        }

    public:
        constexpr Duration &operator+=(Duration const &b)
        {
            usecs_ += b.usecs_;
            return *this;
        }

        constexpr Duration &operator-=(Duration const &b)
        {
            usecs_ -= b.usecs_;
            return *this;
        }

        constexpr Duration operator+(Duration const &b) const
        {
            return Duration(usecs_) += b;
        }

        constexpr Duration operator-(Duration const &b) const
        {
            return Duration(usecs_) -= b;
        }

        constexpr Duration &operator*=(uint64_t s)
        {
            usecs_ *= s;
            return *this;
        }

        constexpr Duration &operator*=(float s)
        {
            usecs_ *= s;
            return *this;
        }

        constexpr Duration &operator*=(double s)
        {
            usecs_ *= s;
            return *this;
        }

        constexpr Duration operator*(uint64_t s) const
        {
            return Duration(*this) *= s;
        }

        constexpr Duration operator*(float s) const
        {
            return Duration(*this) *= s;
        }

        constexpr Duration operator*(double s) const
        {
            return Duration(*this) *= s;
        }

        constexpr Duration &operator/=(uint64_t s)
        {
            usecs_ /= s;
            return *this;
        }

        constexpr Duration &operator/=(float s)
        {
            usecs_ /= s;
            return *this;
        }

        constexpr Duration &operator/=(double s)
        {
            usecs_ /= s;
            return *this;
        }

        constexpr Duration operator/(uint64_t s) const
        {
            return Duration(*this) /= s;
        }

        constexpr Duration operator/(float s) const
        {
            return Duration(*this) /= s;
        }

        constexpr Duration operator/(double s) const
        {
            return Duration(*this) /= s;
        }

    public:
        constexpr bool operator==(Duration const &b) const
        {
            return usecs_ == b.usecs_;
        }

        constexpr bool operator!=(Duration const &b) const
        {
            return usecs_ != b.usecs_;
        }

        constexpr bool operator>(Duration const &b) const
        {
            return usecs_ > b.usecs_;
        }

        constexpr bool operator<(Duration const &b) const
        {
            return usecs_ < b.usecs_;
        }

        constexpr bool operator>=(Duration const &b) const
        {
            return usecs_ >= b.usecs_;
        }

        constexpr bool operator<=(Duration const &b) const
        {
            return usecs_ <= b.usecs_;
        }

    public:
        constexpr static Duration from_micros(uint64_t usecs)
        {
            return Duration(usecs);
        }

        constexpr static Duration from_millis(uint64_t msecs)
        {
            return Duration::from_micros(1000 * msecs);
        }

        constexpr static Duration from_secs(uint64_t secs)
        {
            return Duration::from_millis(1000 * secs);
        }

        constexpr static Duration from(Timespec const &spec)
        {
            return Duration::from(spec.secs, spec.usecs);
        }

        constexpr static Duration from(uint64_t secs, uint64_t usecs)
        {
            return Duration(1000000 * secs + usecs);
        }

        constexpr static Duration from(float secs)
        {
            return Duration::from_micros(1000000.0f * secs);
        }

        constexpr static Duration from(double secs)
        {
            return Duration::from_micros(1000000.0 * secs);
        }

        constexpr static Duration from(Frequency f);
        constexpr Frequency as_frequency() const;
        constexpr operator Frequency(void) const;

    public:
        constexpr uint64_t as_micros(void) const
        {
            return usecs_;
        }

        constexpr uint64_t as_millis(void) const
        {
            return as_micros() / 1000;
        }

        constexpr uint64_t as_secs(void) const
        {
            return as_micros() / 1000000;
        }

        constexpr float as_float(void) const
        {
            return as_micros() / 1000000.0f;
        }

        constexpr double as_double(void) const
        {
            return as_micros() / 1000000.0;
        }

        constexpr Timespec as_timespec(void) const
        {
            return Timespec {usecs_ / 1000000, usecs_ % 1000000};
        }
};

/**
 * Frequency
 * A unit expressing counts per second.
 */
struct Frequency
{
    private:
        // should it be fixed point?
        // or floating?
        // or bignum?
        uint64_t freq_;

    public:
        constexpr Frequency(void)
            : freq_(0)
        {
        }

    public:
        constexpr explicit Frequency(uint64_t units_per_sec)
            : freq_(units_per_sec)
        {
        }

        constexpr explicit Frequency(float ps)
            : freq_(ps)
        {
        }

        constexpr explicit Frequency(double ps)
            : freq_(ps)
        {
        }

    public:
        // TODO: handle prefix/scalers better..
        constexpr static Frequency from_unit(uint64_t ps)
        {
            return Frequency(ps);
        }

        constexpr static Frequency from_kilo(uint64_t ps)
        {
            return Frequency(ps * 1000UL);
        }

        constexpr static Frequency from_mega(uint64_t ps)
        {
            return Frequency(ps * 1000000UL);
        }

        constexpr static Frequency from_tera(uint64_t ps)
        {
            return Frequency(ps * 1000000000UL);
        }

        constexpr static Frequency from(uint64_t ps)
        {
            return Frequency(ps);
        }

        constexpr static Frequency from(float ps)
        {
            return Frequency(ps);
        }

        constexpr static Frequency from(double ps)
        {
            return Frequency(ps);
        }

    public:
        // TODO: handle prefix/scalers better..
        constexpr uint64_t as_unit() const
        {
            return freq_;
        }

        constexpr uint64_t as_kilo() const
        {
            return freq_ / 1000UL;
        }

        constexpr uint64_t as_mega() const
        {
            return freq_ / 1000000UL;
        }

        constexpr uint64_t as_tera() const
        {
            return freq_ / 1000000000UL;
        }

        constexpr double as_double(void) const
        {
            return freq_;
        }

        constexpr float as_float(void) const
        {
            return freq_;
        }

        // private:
        //     constexpr operator double(void) const {
        //         return freq_;
        //     }
        //     constexpr operator float(void) const {
        //         return freq_;
        //     }

    public:
        // operator Duration(void) const {
        //     return Duration(1.0 / static_cast<double>(*this));
        // }
        constexpr operator Duration(void) const;

        constexpr Duration as_duration(void) const;

    public:
        constexpr static Frequency from(Duration d);

    public:
        constexpr Frequency &operator+=(Frequency const &o)
        {
            freq_ += o.freq_;
            return *this;
        }

        constexpr Frequency operator+(Frequency const &o) const
        {
            return Frequency(*this) += o;
        }

        constexpr Frequency &operator-=(Frequency const &o)
        {
            freq_ -= o.freq_;
            return *this;
        }

        constexpr Frequency operator-(Frequency const &o) const
        {
            return Frequency(*this) -= o;
        }

        constexpr Frequency &operator*=(uint64_t s)
        {
            freq_ *= s;
            return *this;
        }

        constexpr Frequency &operator*=(float s)
        {
            freq_ *= s;
            return *this;
        }

        constexpr Frequency &operator*=(double s)
        {
            freq_ *= s;
            return *this;
        }

        constexpr Frequency operator*(uint64_t s) const
        {
            return Frequency(*this) *= s;
        }

        constexpr Frequency operator*(float s) const
        {
            return Frequency(*this) *= s;
        }

        constexpr Frequency operator*(double s) const
        {
            return Frequency(*this) *= s;
        }

        constexpr Frequency &operator/=(uint64_t s)
        {
            freq_ /= s;
            return *this;
        }

        constexpr Frequency &operator/=(float s)
        {
            freq_ /= s;
            return *this;
        }

        constexpr Frequency &operator/=(double s)
        {
            freq_ /= s;
            return *this;
        }

        constexpr Frequency operator/(uint64_t s) const
        {
            return Frequency(*this) /= s;
        }

        constexpr Frequency operator/(float s) const
        {
            return Frequency(*this) /= s;
        }

        constexpr Frequency operator/(double s) const
        {
            return Frequency(*this) /= s;
        }

    public:
        constexpr bool operator==(Frequency const &b) const
        {
            return freq_ == b.freq_;
        }

        constexpr bool operator!=(Frequency const &b) const
        {
            return freq_ != b.freq_;
        }

        constexpr bool operator>(Frequency const &b) const
        {
            return freq_ > b.freq_;
        }

        constexpr bool operator<(Frequency const &b) const
        {
            return freq_ < b.freq_;
        }

        constexpr bool operator>=(Frequency const &b) const
        {
            return freq_ >= b.freq_;
        }

        constexpr bool operator<=(Frequency const &b) const
        {
            return freq_ <= b.freq_;
        }
};

constexpr Frequency Frequency::from(Duration d)
{
    return Frequency(1.0 / d.as_double());
}

constexpr Frequency::operator Duration(void) const
{
    return Duration::from(1.0 / as_double());
}

constexpr Duration Frequency::as_duration(void) const
{
    return Duration::from(1.0 / as_double());
}

constexpr Duration Duration::from(Frequency f)
{
    return f.as_duration();
}

constexpr Duration::operator Frequency(void) const
{
    return as_frequency();
}

constexpr Frequency Duration::as_frequency() const
{
    return Frequency::from(*this);
}

} // namespace time
} // namespace nel

#endif // defined(DURATION_HH)
