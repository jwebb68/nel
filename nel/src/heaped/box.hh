#ifndef NEL_HEAPED_BOX_HH
#define NEL_HEAPED_BOX_HH

namespace nel {
namespace heaped {

template<typename T>
struct Box;

} // namespace heaped
} // namespace nel

#include "element.hh"

#include <utility> // std::move, std::forward
#include <memory> // std::unique_ptr

namespace nel {
namespace heaped {

template<typename T>
struct Box {
    public:
        // heh, blatant rust-ism
        typedef Box Self;

    private:
        typedef Element<T> ElementT;
        std::unique_ptr<ElementT> value_;

    public:
        // No default, must create a T.
        constexpr Box(void) = delete;

        // No copying..
        // Can move though.
        constexpr Box(const Box &o) = delete;
        constexpr Box &operator=(const Box &o) const = delete;

    public:
        // constexpr Box(T &&v) noexcept : value_(new T(std::move(v))) {}

        template<typename ...Args>
        constexpr Box(Args &&... args) noexcept
            : value_(new ElementT(std::forward<Args>(args)...))
        {}

    public:
        constexpr T &operator*(void) noexcept
        {
            nel_panic_if_not(has_value(), "not a value");
            return *value_;
        }
        constexpr T const &operator*(void) const noexcept
        {
            nel_panic_if_not(has_value(), "not a value");
            return *value_;
        }

        constexpr bool has_value(void) const noexcept
        {
            return value_ != nullptr;
        }

        constexpr T unwrap(void) noexcept
        {
            nel_panic_if_not(has_value(), "not a value");
            auto t = value_;
            value_ = nullptr;
            return t->unwrap();
        }

        constexpr bool operator==(Box const &o) const noexcept
        {
            // if (!has_value() && !o.has_value()) {
            //     return true;
            // } else if (has_value() && o.!has_value()) {
            //     return false;
            // } else if (!has_value() && o.has_value()) {
            //     return false;
            // } else if (has_value() && o.has_value()) {
            //     return *value_ == o.value_;
            // }
            // return (!has_value() && !o.has_value())
            //         || (has_value() && o.has_value() && *value_ == *o.value_);
            // does unique-ptr do value equality?
            return value_ == o.value_;
        }
        constexpr bool operator!=(Box const &o) const noexcept
        {
            // if (!has_value() && !o.has_value()) {
            //     return false;
            // } else if (has_value() && !o.has_value()) {
            //     return true;
            // } else if (!has_value() && o.has_value()) {
            //     return true;
            // } else if (has_value() && o.has_value()) {
            //     return *value_ != o.value_;
            // }
            // does unique-ptr do value equality?
            return value_ != o.value_;
        }
        constexpr bool operator<(Box const &o) const noexcept
        {
            return *(this) < *o;
        }
        constexpr bool operator>(Box const &o) const noexcept
        {
            return *(this) > *o;
        }
        constexpr bool operator<=(Box const &o) const noexcept
        {
            return *(this) <= *o;
        }
        constexpr bool operator>=(Box const &o) const noexcept
        {
            return *(this) >= *o;
        }
};

} // namespace heaped
} // namespace nel


#endif // NEL_HEAPED_BOX_HH
