#ifndef NEL_BOX_HH
#define NEL_BOX_HH

namespace nel {

template<typename T>
struct Box;

}
#include "element.hh"

#include <utility> // std::move, std::forward
#include <memory> // std::unique_ptr

namespace nel {

template<typename T>
struct Box {
    public:
        // heh, blatant rust-ism
        typedef Box Self;

    private:
        typedef Element<T> ElementT;
        std::unique_ptr<ElementT> value_;

    public:
        // no default, must create a T
        constexpr Box(void) = delete;

        // no copying..
        // can move though.
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
            return *value_;
        }
        constexpr T const &operator*(void) const noexcept
        {
            return *value_;
        }

        constexpr bool has_value(void) const noexcept
        {
            return value_ != nullptr;
        }

        constexpr T unwrap(void)
        {
            assert(value_ != nullptr);
            T v(value_->unwrap());
            value_ = nullptr;
            return v;
        }

        constexpr bool operator==(Box const &o) const
        {
            // same struct, must be same..
            return value_ == o.value_;
        }
        constexpr bool operator!=(Box const &o) const
        {
            // same struct, must be same..
            return value_ != o.value_;
        }
        constexpr bool operator<(Box const &o) const
        {
            return *(this) < *o;
        }
        constexpr bool operator>(Box const &o) const
        {
            return *(this) > *o;
        }
        constexpr bool operator<=(Box const &o) const
        {
            return *(this) <= *o;
        }
        constexpr bool operator>=(Box const &o) const
        {
            return *(this) >= *o;
        }
};

}


#endif//NEL_BOX_HH
