#ifndef NEL_OPTIONAL_HH
#define NEL_OPTIONAL_HH

namespace nel
{

template<typename T>
class SomeT;

template<typename T>
class OptionalT;

}


#include "none.hh"

#include <cassert> // assert()
#include <exception> //std::terminate

namespace nel
{

template<typename T>
class SomeT
{
    private:
        T value;
    public:
        explicit SomeT(const T &other) noexcept:
            value(other)
        {}
    public:
        const T &getvalue() const noexcept
        {
            return this->value;
        }
        T getvalue() noexcept
        {
            return this->value;
        }
};
template<typename T>
SomeT<T> Some(const T &other) noexcept
{
    return SomeT<T>(other);
}


template<typename T>
class OptionalT
{
    private:
        enum
        {
            NONE = 0,
            SOME
        } tag;
        union
        {
            SomeT<T> some;
            NoneT none;
        };

    public:
        ~OptionalT() noexcept
        {
            switch (this->tag)
            {
                case SOME:
                    this->some.SomeT<T>::~SomeT();
                    break;

                case NONE:
                    this->none.NoneT::~NoneT();
                    break;

                default:
                    assert(false);
                    break;
            }
        }

        OptionalT(const OptionalT<T> &other) noexcept:
            tag(other.tag)
        {
            switch (other.tag)
            {
                case SOME:
                    this->some = other.some;
                    break;

                case NONE:
                    this->none = other.none;
                    break;

                default:
                    assert(false);
                    break;

            }
        }

        OptionalT() noexcept:
            tag(NONE),
            none()
        {}

        OptionalT(const NoneT &) noexcept:
            tag(NONE),
            none()
        {}

        OptionalT(const SomeT<T> &v) noexcept:
            tag(SOME),
            some(v)
        {}

    public:
        bool is_some() const noexcept
        {
            return this->tag == SOME;
        }

        bool is_none() const noexcept
        {
            return this->tag == NONE;

        }

        const T &unwrap() const noexcept
        {
            if (this->tag == SOME) return this->some.getvalue();
            std::terminate();
        }
        T unwrap()  noexcept
        {
            if (this->tag == SOME) return this->some.getvalue();
            std::terminate();
        }

        const T &unwrap_or(const T &other) const noexcept
        {
            if (this->tag == SOME) return this->some.getvalue();
            return other;
        }
        T unwrap_or(const T &other) noexcept
        {
            if (this->tag == SOME) return this->some.getvalue();
            return other;
        }

};
template<typename T>
OptionalT<T> Optional(const T &other) noexcept
{
    return OptionalT<T>(other);
}

}


#endif//NEL_OPTIONAL_HH
