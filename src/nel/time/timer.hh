// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(NEL_TIME_TIMER_HH)
#    define NEL_TIME_TIMER_HH

namespace nel
{
namespace time
{

struct Timer;

}
} // namespace nel

#    include <nel/time/duration.hh>
#    include <nel/time/instant.hh>

namespace nel
{
namespace time
{

/**
 * Timer
 * Something that expires when a dealine has been reached.
 * That cannot be reset.
 * but can be overwritten?
 */
#    define TIMEREP_INFINITE

#    ifdef TIMEREP_INFINITE
struct Timer
{
    public:
    private:
        // If Instants are infinite and do not wrap in their representation
        // then can track using the expiry time.
        Instant expiry_;

    public:
        constexpr explicit Timer(Duration duration, Instant now)
            : expiry_(now + duration)
        {
        }

    public:
        /**
         * Determine if the timer has expired when time is now.
         *
         * A timer is defined to expire when the now time is equal or later than it's expiry.
         *
         * @param now The timepoint to check.
         *
         * @return true if expired.
         * @return false if not expired.
         */
        constexpr bool has_expired(Instant now) const
        {
            return now >= expiry_;
        }
};

#    else

struct Timer
{
    public:
    private:
        // If Instants are not infinite and do wrap in their representation
        // then need to track using a differential method.
        Instant initial_;
        Duration duration_;

    public:
        constexpr explicit Timer(Duration duration, Instant now)
            : initial_(now)
            , duration_(duration)
        {
        }

    public:
        /**
         * Determine if the timer has expired when time is now.
         *
         * A timer is defined to expire when the now time is equal or later than it's expiry.
         *
         * @param now The timepoint to check.
         *
         * @return true if expired.
         * @return false if not expired.
         */
        constexpr bool has_expired(Instant now) const
        {
            return now.elapsed_since(initial_) >= duration_;
        }
};
#    endif

}; // namespace time
}; // namespace nel

#endif //! defined(NEL_TIME_TIMER_HH)
