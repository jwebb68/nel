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
        Element(void) = default;

        Element(Element const &) = delete;
        Element &operator=(Element const &) const = delete;

        constexpr Element(Element &&o) = default;
        constexpr Element &operator=(Element &&o) = default;

        Element(T &&val)
            : value_(forward<T>(val))
        {
        }

        template<typename... Args>
        Element(Args &&...args)
            : value_(forward<Args>(args)...)
        {
        }

    public:
        T &&unwrap(void)
        {
            return forward<T>(value_);
        }

        T const &get(void) const
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

        T &operator*(void)
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
