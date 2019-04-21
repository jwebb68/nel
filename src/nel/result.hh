#ifndef NEL_RESULT_HH
#define NEL_RESULT_HH

namespace nel
{

template<typename T>
class Ok;

template<typename E>
class Err;

template<typename T, typename E>
class Result;

}

#include "optional.hh" //nel::OptionalT

#include <exception> //std::terminate


namespace nel
{
template<typename T>
class Ok
{
    private:
        T value_;

    public:
        Ok() noexcept
        {}

        explicit Ok(const T &ok) noexcept:
            value_(ok)
        {}

        Ok(T &&ok) noexcept:
            value_(std::move(ok))
        {}

    public:
        const T &unwrap() const noexcept
        {
            return this->value_;
        }

        T unwrap() noexcept
        {
            return this->value_;
        }
};


template<typename E>
class Err
{
    private:
        E value_;

    public:
        Err() noexcept
        {}

        explicit Err(const E &err) noexcept:
            value_(err)
        {}

        Err(E &&err) noexcept:
            value_(std::move(err))
        {}

    public:
        // TODO: unwrap or deref operator for value access?
        const E &unwrap() const noexcept
        {
            return this->value_;
        }

        E unwrap() noexcept
        {
            return this->value_;
        }
};


template<typename T, typename E>
class Result
{
    private:
        enum
        {
            OK,
            ERR
        } tag_;

        union
        {
            Ok<T> ok_;
            Err<E> err_;
        };

    public:
        ~Result() noexcept
        {
            switch (this->tag_)
            {
                case OK:
                    this->ok_.Ok<T>::~Ok();
                    break;

                case ERR:
                    this->err_.Err<E>::~Err();
                    break;

                default:
                    std::terminate();
                    break;
            }
        }

        Result(const Result<T, E> &other) noexcept:
            tag_(other.tag_)
        {
            switch (other.tag_)
            {
                case OK:
                    this->ok_ = other.ok_;
                    break;

                case ERR:
                    this->err_ = other.err_;
                    break;

                default:
                    std::terminate();
                    break;
            }
        }

        Result(Result<T, E> &&other) noexcept:
            tag_(std::move(other.tag_))
        {
            switch (this->tag_)
            {
                case OK:
                    this->ok_ = std::move(other.ok_);
                    break;

                case ERR:
                    this->err_ = std::move(other.err_);
                    break;

                default:
                    std::terminate();
                    break;
            }
        }

        Result(const Ok<T> &ok) noexcept:
            tag_(OK),
            ok_(ok)
        {}

        Result(Ok<T> &&ok) noexcept:
            tag_(OK),
            ok_(std::move(ok))
        {}

        Result(const Err<E> &err) noexcept:
            tag_(ERR),
            err_(err)
        {}

        Result(Err<E> &&err) noexcept:
            tag_(ERR),
            err_(std::move(err))
        {}

    public:
        bool operator==(const Ok<T> &other) const
        {
            if (this->tag_ != OK) return false;
            return this->ok_ == other;
        }

        bool operator!=(const Ok<T> &other) const
        {
            return !((*this) == other);
        }

        bool operator==(const Err<E> &other) const
        {
            if (this->tag_ != ERR) return false;
            return this->err_ == other;
        }

        bool operator!=(const Err<T> &other) const
        {
            return !((*this) == other);
        }

    public:
        bool is_ok() const noexcept
        {
            return this->tag_ == OK;
        }

        bool is_err() const noexcept
        {
            return this->tag_ == ERR;
        }

        Optional<T> ok() const noexcept
        {
            if (this->tag_ == OK) return this->ok_.unwrap();
            return None();
        }

        Optional<E> err() const noexcept
        {
            if (this->tag_ == ERR) return this->err_.unwrap();
            return None();
        }

        const T &unwrap() const noexcept
        {
            if (this->tag_ == OK) return this->ok_.unwrap();
            std::terminate();
        }

        T unwrap() noexcept
        {
            if (this->tag_ == OK) return this->ok_.unwrap();
            std::terminate();
        }

        const T &unwrap_err() const noexcept
        {
            if (this->tag_ == ERR) return this->err_.unwrap();
            std::terminate();
        }

        T unwrap_err() noexcept
        {
            if (this->tag_ == ERR) return this->err_.unwrap();
            std::terminate();
        }

        const T &unwrap_or(const T &other) const noexcept
        {
            if (this->tag_ == OK) return this->ok_.unwrap();
            return other;
        }

        T unwrap_or(const T &other) noexcept
        {
            if (this->tag_ == OK) return this->ok_.unwrap();
            return other;
        }
};

#define nel_result_try(r) {if (r.is_err()) return r;}while(0);

}


#endif//NEL_RESULT_HH
