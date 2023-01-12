// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#ifndef NEL_ELEMENT_HH
#define NEL_ELEMENT_HH

namespace nel
{
template<typename T>
struct Element;
} // namespace nel

#include <utility> // std::move, std::forward

namespace nel
{
template<typename T>
struct Element {
    private:
        T value_;

    public:
        ~Element(void) = default;

        // Element(void) = delete;
        Element(void) = default;

        Element(Element const &) = delete;
        Element &operator=(Element const &) const = delete;

        constexpr Element(Element &&o) noexcept = default;
        constexpr Element &operator=(Element &&o) noexcept = default;

        Element(T &&val) noexcept
            : value_(std::forward<T>(val))
        {
        }

        template<typename... Args>
        Element(Args &&...args) noexcept
            : value_(std::forward<Args>(args)...)
        {
        }

    public:
        T unwrap(void) noexcept
        {
            return std::forward<T>(value_);
        }

        T const &get(void) const noexcept
        {
            return value_;
        }

        T &get(void) noexcept
        {
            return value_;
        }

        T const &operator*(void) const noexcept
        {
            return value_;
        }

        T &operator*(void) noexcept
        {
            return value_;
        }
};

} // namespace nel

#endif // NEL_ELEMENT_HH
