// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(NEL_ELEMENT_HH)
#    define NEL_ELEMENT_HH

namespace nel
{

template<typename T>
struct Element;

} // namespace nel

#    include <nel/formatter.hh>
#    include <nel/memory.hh> // move, forward

namespace nel
{

template<typename T>
struct Element
{
    public:
        typedef T Type;

    private:
        Type value_;

    public:
        constexpr ~Element(void) = default;

        // Element(void) = delete;
        constexpr Element(void) = default;

        constexpr Element(Element const &) = delete;
        constexpr Element &operator=(Element const &) const = delete;

        constexpr Element(Element &&o) = default;
        constexpr Element &operator=(Element &&o) = default;

        constexpr Element(Type &&val)
            : value_(forward<Type>(val))
        {
        }

        template<typename... Args>
        constexpr Element(Args &&...args)
            : value_(forward<Args>(args)...)
        {
        }

    public:
        constexpr Type const &ref(void) const
        {
            return value_;
        }

        constexpr Type &ref(void)
        {
            return value_;
        }

        constexpr Type const &operator*(void) const
        {
            return ref();
        }

        constexpr Type &operator*(void)
        {
            return ref();
        }

        constexpr bool operator==(Element const &o) const
        {
            return ref() == o.ref();
        }

        constexpr bool operator!=(Element const &o) const
        {
            return ref() != o.ref();
        }

    public:
        Formatter &dbgfmt(Formatter &fmt) const
        {
            return value_.dbgfmt(fmt);
        }
};

} // namespace nel

#endif // !defined(NEL_ELEMENT_HH)
