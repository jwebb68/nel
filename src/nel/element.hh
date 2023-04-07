// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(NEL_ELEMENT_HH)
#    define NEL_ELEMENT_HH

namespace nel
{
template<typename T>
struct Element;
} // namespace nel

#    include <nel/memory.hh> // move, forward

namespace nel
{
template<typename T>
struct Element
{
    private:
        T value_;

    public:
        ~Element(void) = default;

        // Element(void) = delete;
        constexpr Element(void) = default;

        constexpr Element(Element const &) = delete;
        constexpr Element &operator=(Element const &) const = delete;

        constexpr Element(Element &&o) = default;
        constexpr Element &operator=(Element &&o) = default;

        constexpr Element(T &&val)
            : value_(forward<T>(val))
        {
        }

        template<typename... Args>
        constexpr Element(Args &&...args)
            : value_(forward<Args>(args)...)
        {
        }

    public:
        T &&unwrap(void)
        {
            return forward<T>(value_);
        }

        constexpr T const &operator*(void) const
        {
            return value_;
        }

        T &get(void)
        {
            return value_;
        }

        T const &operator*(void) const
        {
            return value_;
        }

        constexpr T &operator*(void)
        {
            return value_;
        }

        constexpr bool operator==(Element const &o) const
        {
            return value_ == o.value_;
        }

        constexpr bool operator!=(Element const &o) const
        {
            return value_ != o.value_;
        }
};

} // namespace nel

#endif // !defined(NEL_ELEMENT_HH)
