// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(NEL_FORMATTER_HH)
#    define NEL_FORMATTER_HH

namespace nel
{

}

#    include <nel/charwrite.hh>

namespace nel
{

struct Formatter: public CharWrite
{
    private:
        CharWrite &w_;

        // TODO:
        // flags;
        // fill;
        // align;
        // width;
        // precis;

    public:
        constexpr Formatter(CharWrite &w)
            : w_(w)
        {
        }

    public:
        int32_t NEL_WARN_UNUSED_RESULT write(Char c) override
        {
            return w_.write(c);
        }

        int32_t NEL_WARN_UNUSED_RESULT write(Char c, uint32_t n) override
        {
            return w_.write(c, n);
        }

        int32_t NEL_WARN_UNUSED_RESULT write(Char const a[], uint32_t n) override
        {
            return w_.write(a, n);
        }

        int32_t NEL_WARN_UNUSED_RESULT write(Char const *const s) override
        {
            return w_.write(s);
        }

    public:
        friend Formatter &operator<<(Formatter &fmt, bool b);
        friend Formatter &operator<<(Formatter &fmt, Formatter::Char c);
        friend Formatter &operator<<(Formatter &fmt, Formatter::Char const *s);

        friend Formatter &operator<<(Formatter &fmt, uint8_t v);
        friend Formatter &operator<<(Formatter &fmt, uint16_t v);
        friend Formatter &operator<<(Formatter &fmt, uint32_t v);

        friend Formatter &operator<<(Formatter &fmt, int8_t v);
        friend Formatter &operator<<(Formatter &fmt, int16_t v);
        friend Formatter &operator<<(Formatter &fmt, int32_t v);

        // template<typename T>
        // constexpr Formatter &dbgfmt(T const &v) {
        //     return v.dbgfmt(*this);
        // }

        // template<typename T>
        // constexpr Formatter &fmt(T const &v) {
        //     return v.fmt(*this);
        // }
};
} // namespace nel

#    include <concepts>

namespace nel
{
namespace concepts
{
namespace fmt
{

template<typename T>
concept DisplayFmt = requires(T t, Formatter &fmt) {
                         {
                             t.fmt(fmt)
                             } -> std::same_as<Formatter &>;
                     };

template<typename T>
concept DebugFmt = requires(T t, Formatter &fmt) {
                       {
                           t.dbgfmt(fmt)
                           } -> std::same_as<Formatter &>;
                   };

} // namespace fmt
} // namespace concepts
} // namespace nel

#endif //! defined(NEL_FORMATTER_HH)
