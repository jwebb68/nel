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
        Element(T &&v)
            : value_(std::move(v))
        {}

        template<typename ...Args> Element(Args &&...args)
            : value_(std::forward<Args>(args)...)
        {}
    public:
        T unwrap(void)
        {
            return std::move(value_);
        }
        constexpr bool operator==(Element const &o) const
        {
            return value_ == o.value_;
        }
        constexpr bool operator!=(Element const &o) const
        {
            return value_ != o.value_;
        }
        // T &get(void)
        // {
        //     return value_;
        // }
        T const &get(void) const
        {
            return value_;
        }
        // T &operator*(void)
        // {
        //     return value_;
        // }
        T const &operator*(void) const
        {
            return value_;
        }
};

}


#endif//NEL_ELEMENT_HH
