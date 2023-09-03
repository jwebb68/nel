// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(NEL_TIME_INSTANT_HH)
#    define NEL_TIME_INSTANT_HH

namespace nel
{
namespace time
{

struct Instant;

}
} // namespace nel

#    include <nel/time/duration.hh>

namespace nel
{
namespace time
{

struct Instant
{
    private:
        // an instant is nothing more than a duration from a fixed point.
        // usually time since boot.
        Duration timepoint_;

    public:
        constexpr Instant()
            : timepoint_()
        {
        }

    private:
        constexpr explicit Instant(Duration d)
            : timepoint_(d)
        {
        }

    public:
        constexpr bool operator==(Instant const &o) const
        {
            return timepoint_ == o.timepoint_;
        }

        constexpr bool operator!=(Instant const &o) const
        {
            return timepoint_ != o.timepoint_;
        }

        constexpr bool operator<(Instant const &o) const
        {
            return timepoint_ < o.timepoint_;
        }

        constexpr bool operator<=(Instant const &o) const
        {
            return timepoint_ <= o.timepoint_;
        }

        constexpr bool operator>(Instant const &o) const
        {
            return timepoint_ > o.timepoint_;
        }

        constexpr bool operator>=(Instant const &o) const
        {
            return timepoint_ >= o.timepoint_;
        }

    public:
        constexpr Instant &operator+=(Duration const &d)
        {
            timepoint_ += d;
            return *this;
        }

        constexpr Instant operator+(Duration const &d)
        {
            return Instant(timepoint_ + d);
        }

        constexpr Instant &operator-=(Duration const &d)
        {
            timepoint_ -= d;
            return *this;
        }

        constexpr Instant operator-(Duration const &d)
        {
            return Instant(timepoint_ - d);
        }

    public:
        constexpr Duration elapsed_since(Instant const &o) const
        {
            return timepoint_ - o.timepoint_;
        }
};

} // namespace time
} // namespace nel

#endif // defined(NEL_TIME_INSTANT_HH)
