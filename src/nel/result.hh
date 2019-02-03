#ifndef NEL_RESULT_HH
#define NEL_RESULT_HH

namespace nel
{

template<typename T>
class OkT;

template<typename E>
class ErrT;

template<typename T, typename E>
class ResultT;

}

#include "optional.hh" //nel::OptionalT

#include <cassert> // assert()
#include <exception> //std::terminate
#include <utility> //std::move

namespace nel
{
template<typename T>
class OkT
{
    private:
        T value;
    public:
        constexpr OkT() noexcept
        {}
        constexpr explicit OkT(const T &other) noexcept:
            value(other)
        {}
    public:
        // TODO: get_value or deref operator for value access?
        const T &get_value() const noexcept
        {
            return this->value;
        }
        T get_value() noexcept
        {
            return this->value;
        }
};
template<typename T>
constexpr OkT<T> Ok(const T &v) noexcept
{
    return OkT<T>(v);
}


template<typename E>
class ErrT
{
    private:
        E value;
    public:
        constexpr ErrT() noexcept
        {}
        constexpr explicit ErrT(const E &other) noexcept:
            value(other)
        {}
    public:
        // TODO: get_value or deref operator for value access?
        const E &get_value() const noexcept
        {
            return this->value;
        }
        E get_value() noexcept
        {
            return this->value;
        }
};
template<typename E>
constexpr ErrT<E> Err(const E &v) noexcept
{
    return ErrT<E>(v);
}


template<typename T, typename E>
class ResultT
{
    private:
        enum
        {
            OK = 0,
            ERR
        } tag;
        union
        {
            OkT<T> ok_;
            ErrT<E> err_;
        };

    public:
        ~ResultT() noexcept
        {
            switch (this->tag)
            {
                case OK:
                    this->ok_.OkT<T>::~OkT();
                    break;

                case ERR:
                    this->err_.ErrT<E>::~ErrT();
                    break;

                default:
                    assert(false);
                    break;
            }
        }

        constexpr ResultT(const ResultT<T, E> &other) noexcept:
            tag(other.tag)
        {
            switch (this->tag)
            {
                case OK:
                    this->ok_ = other.ok_;
                    break;

                case ERR:
                    this->err_ = other.err_;
                    break;

                default:
                    assert(false);
                    break;
            }
        }

        constexpr ResultT(ResultT<T, E> &&other) noexcept:
            tag(std::move(other.tag))
        {
            switch (this->tag)
            {
                case OK:
                    this->ok_ = std::move(other.ok_);
                    break;

                case ERR:
                    this->err_ = std::move(other.err_);
                    break;

                default:
                    assert(false);
                    break;
            }
        }

        constexpr ResultT(const OkT<T> &ok) noexcept:
            tag(OK),
            ok_(ok)
        {}

        constexpr ResultT(const ErrT<E> &err) noexcept:
            tag(ERR),
            err_(err)
        {}


    public:
        constexpr bool operator==(const OkT<T> &other) const
        {
            if (this->tag != OK) return false;
            return this->ok_ == other;
        }
        constexpr bool operator!=(const OkT<T> &other) const
        {
            return !((*this) == other);
        }

        constexpr bool operator==(const ErrT<E> &other) const
        {
            if (this->tag != ERR) return false;
            return this->err_ == other;
        }
        constexpr bool operator!=(const ErrT<T> &other) const
        {
            return !((*this) == other);
        }

    public:
        constexpr bool is_ok() const noexcept
        {
            return this->tag == OK;
        }
        constexpr bool is_err() const noexcept
        {
            return this->tag == ERR;
        }

        constexpr OptionalT<T> ok() const noexcept
        {
            if (this->tag == OK) return this->ok_.get_value();
            return None();
        }
        constexpr OptionalT<E> err() const noexcept
        {
            if (this->tag == ERR) return this->err_.get_value();
            return None();
        }

        const T &unwrap() const noexcept
        {
            if (this->tag == OK) return this->ok_.get_value();
            std::terminate();
        }
        T unwrap() noexcept
        {
            if (this->tag == OK) return this->ok_.get_value();
            std::terminate();
        }

        const T &unwrap_err() const noexcept
        {
            if (this->tag == ERR) return this->err_.get_value();
            std::terminate();
        }
        T unwrap_err() noexcept
        {
            if (this->tag == ERR) return this->err_.get_value();
            std::terminate();
        }

        const T &unwrap_or(const T &other) const noexcept
        {
            if (this->tag == OK) return this->ok_.get_value();
            return other;
        }

        T unwrap_or(const T &other) noexcept
        {
            if (this->tag == OK) return this->ok_.get_value();
            return other;
        }

};

//#define nel_result_try(r) {if (r.is_err()) return r;}while(0);
#define nel_result_try(r) ((r).is_ok()?(r).unwrap(): return (r))
}


#endif//NEL_RESULT_HH
