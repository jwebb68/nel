#ifndef NEL_ELEMENT_HH
#define NEL_ELEMENT_HH

namespace nel {
template<typename T>
struct Element;
}

#include <utility> // std::move, std::forward


namespace nel {
template<typename T>
struct Element {
    private:
        T value_;
    public:
        ~Element(void) = default;

        // what if T has a default cons?
        // Element(void) = delete;

        Element(Element const &) = delete;
        Element &operator=(Element const &) const = delete;

        Element(Element &&o) = default;
        Element &operator=(Element &&o) = default;
    public:
        Element(T &&v) noexcept
            : value_(std::forward<T>(v))
        {}

        template<typename ...Args> Element(Args &&...args) noexcept
            : value_(std::forward<Args>(args)...)
        {}
    public:
        T unwrap(void) noexcept
        {
            return std::forward<T>(value_);
        }
        constexpr bool operator==(Element const &o) const noexcept
        {
            return this == &o || value_ == o.value_;
        }
        constexpr bool operator!=(Element const &o) const noexcept
        {
            return this != &o && value_ != o.value_;
        }

        T const &get(void) const noexcept
        {
            return value_;
        }

        T const &operator*(void) const noexcept
        {
            return value_;
        }
};

}


#endif//NEL_ELEMENT_HH
