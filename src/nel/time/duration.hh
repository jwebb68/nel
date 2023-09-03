// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(NEL_TIME_DURATION_HH)
#    define NEL_TIME_DURATION_HH

namespace nel
{
namespace time
{

class Duration;

};
}; // namespace nel

#    include <inttypes.h> // uint64_t

struct Timespec
{
        uint64_t secs;
        uint64_t usecs;
};

namespace nel
{
namespace time
{

class Duration
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

        // Duration(Duration const &);
        // Duration operator=(Duration const &);

    public:
        Duration &operator+=(Duration const &b)
        {
            usecs_ += b.usecs_;
            return *this;
        }

        Duration &operator-=(Duration const &b)
        {
            usecs_ -= b.usecs_;
            return *this;
        }

        constexpr Duration operator+(Duration const &b) const
        {
            return Duration(usecs_ + b.usecs_);
        }

        constexpr Duration operator-(Duration const &b) const
        {
            return Duration(usecs_ - b.usecs_);
        }

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
            return Duration::from_micros(1000000.0 * secs);
        }

        constexpr static Duration from(double secs)
        {
            return Duration::from_micros(1000000.0 * secs);
        }

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
            return as_micros() / 1000000.0;
        }

        constexpr double as_double(void) const
        {
            return as_micros() / 1000000.0;
        }

        constexpr Timespec as_spec(void) const
        {
            return Timespec {usecs_ / 1000000, usecs_ % 1000000};
        }

    public:
};

}; // namespace time
}; // namespace nel

#endif // defined(DURATION_HH)
