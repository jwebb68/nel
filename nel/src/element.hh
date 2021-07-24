#ifndef NEL_ELEMENT_HH
#define NEL_ELEMENT_HH

namespace nel {
template<typename T>
struct Element;
} // namespace nel

#include <utility> // std::move, std::forward


namespace nel {
template<typename T>
struct Element {
    private:
        T value_;
    public:
        ~Element(void) = default;

        // What if T has a default cons?
        // Element(void) = delete;

        Element(Element const &) = delete;
        Element &operator=(Element const &) const = delete;

        constexpr Element(Element &&o) noexcept = default;
        constexpr Element &operator=(Element &&o) noexcept = default;

    public:
        template<typename ...Args>
        Element(Args &&...args) noexcept
            : value_(std::forward<Args>(args)...)
            // does this keep the inplace construct behaviour?
            // it's relying on the optimiser to remove the copy.
            // requires T::new_(args), which it may not have.
            // and then doesn't work for int/float etc.
            // : value_(std::move(T::new_(std::forward<Args>(args)...)))
        {}

        // template<typename ...Args>
        // static constexpr Element new_(Args &&...args) noexcept
        // {
        //     return Element(std::forward<Args>(args)...);
        // }

    public:
        // Element(T &&v) noexcept
        //     // needs T::move public, which it will be
        //     // needs T::move, which it should always have.
        //     : value_(std::move(v))
        // {}

        // static constexpr Element new_(T &&v) noexcept
        // {
        //     return Element(std::move(v));
        // }


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
