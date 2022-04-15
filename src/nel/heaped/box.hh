#ifndef NEL_HEAPED_BOX_HH
#define NEL_HEAPED_BOX_HH

namespace nel
{
namespace heaped
{

template<typename T>
struct Box;

} // namespace heaped
} // namespace nel

#include <nel/result.hh>
#include <nel/element.hh>
#include <nel/panic.hh>

#include <utility> // std::move, std::forward
#include <memory> // std::unique_ptr

namespace nel
{
namespace heaped
{

/**
 * A T on the heap, owned (a bit like std::unique_ptr)
 */
template<typename T>
struct Box {
    public:
        // heh, blatant rust-ism
        typedef Box Self;

    private:
        typedef Element<T> ElementT;
        std::unique_ptr<ElementT> value_;

    public:
        ~Box(void) = default;

        // No default, must create a T.
        constexpr Box(void) noexcept = delete;

        // No copying..
        constexpr Box(Box const &o) noexcept = delete;
        constexpr Box &operator=(Box const &o) noexcept = delete;

        // Can move though.
        constexpr Box(Box &&) noexcept = default;
        constexpr Box &operator=(Box &&) noexcept = default;

        // create contained inplace.
        // works for moving-into as well.
        template<typename... Args>
        constexpr Box(Args &&...args) noexcept: value_(new Element<T>(std::forward<Args>(args)...))
        {
        }

    public:
        /**
         * Return a mutable reference to the boxed value.
         *
         * @returns reference to the boxed value.
         * @warning Panics if no value to return (e.g. use after move).
         */
        constexpr T &operator*(void) noexcept
        {
            return deref();
        }
        constexpr T &deref(void) noexcept
        {
            nel_panic_if_not(has_value(), "not a value");
            return value_->get();
        }

        /**
         * Return a const reference to the boxed value.
         *
         * @returns reference to the boxed value.
         * @warning Panics if no value to return (e.g. use after move).
         */
        constexpr T const &operator*(void) const noexcept
        {
            return deref();
        }
        constexpr T const &deref(void) const noexcept
        {
            nel_panic_if_not(has_value(), "not a value");
            return value_->get();
        }

        /**
         * Determines if this box has a value.
         *
         * @returns true if there is a value, false otherwise.
         */
        constexpr bool has_value(void) const noexcept
        {
            return value_ != nullptr;
        }
        constexpr operator bool(void) const noexcept
        {
            return has_value();
        }

        /**
         * Removes and returns the value contained in the box.
         *
         * The box is invalidated by this action.
         *
         * @returns the value in the box.
         * @warning Panics if no value to return (e.g. use after move).
         */
        constexpr T unwrap(void) noexcept
        {
            nel_panic_if_not(has_value(), "not a value");
            std::unique_ptr<ElementT> t = std::move(value_);
            return t->unwrap();
        }

        // No comparators by design.
        // (i.e. Keep it simple)
};

} // namespace heaped
} // namespace nel

#endif // NEL_HEAPED_BOX_HH
