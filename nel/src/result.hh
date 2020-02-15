#ifndef NEL_RESULT_HH
#define NEL_RESULT_HH

namespace nel {

template<typename T, typename E>
class Result;

}

#include "assert.h"
#include "optional.hh" //nel::OptionalT

#include <exception> //std::terminate
#include <functional> //std::function

namespace nel {

template<typename T, typename E>
class Result {
  public:
    class Ok {
      private:
        T value_;

      public:
        Ok() noexcept {}

        explicit Ok(const T &ok) noexcept: value_(ok) {}

      public:
        const T &unwrap() const noexcept {
            return this->value_;
        }

        T unwrap() noexcept {
            return this->value_;
        }

        bool operator==(const T &other) const {
            return this->value_ == other;
        }
        bool operator!=(const T &other) const {
            return !(this->value_ == other);
        }
    };

    class Err {
      private:
        E value_;

      public:
        Err() noexcept {}

        explicit Err(const E &err) noexcept: value_(err) {}

      public:
        // TODO: unwrap or deref operator for value access?
        const E &unwrap() const noexcept {
            return this->value_;
        }

        E unwrap() noexcept {
            return this->value_;
        }
    };

  private:
    enum { OK = 0, ERR } tag_;

    union {
        Ok ok_;
        Err err_;
    };

  public:
    ~Result() noexcept {
        switch (this->tag_) {
            case OK:
                this->ok_.Ok::~Ok();
                break;

            case ERR:
                this->err_.Err::~Err();
                break;

            default:
                std::terminate();
                break;
        }
    }

    // Result(const Result &other) noexcept:
    //     tag_(other.tag_)
    // {
    //     switch (other.tag_)
    //     {
    //         case OK:
    //             this->ok_ = other.ok_;
    //             break;

    //         case ERR:
    //             this->err_ = other.err_;
    //             break;

    //         default:
    //             std::terminate();
    //             break;
    //     }
    // }

    Result(const Ok &ok) noexcept: tag_(OK), ok_(ok) {}

    Result(const Err &err) noexcept: tag_(ERR), err_(err) {}

  public:
    bool operator==(const Result<T, E> &other) const {
        if (this->is_ok() && other.is_ok()) {
            return this->unwrap() == other.unwrap();
        }
        if (this->is_err() && other.is_err()) {
            return this->unwrap_err() == other.unwrap_err();
        }
        return false;
    }

    bool operator!=(const Result<T, E> &other) const {
        return !(*this == other);
    }

    bool operator==(const Ok &other) const {
        if (!this->is_ok()) return false;
        return this->ok_.unwrap() == other.unwrap();
    }

    bool operator!=(const Ok &other) const {
        return !(*this == other);
    }

    bool operator==(const Err &other) const {
        if (!this->is_err()) return false;
        return this->err_.unwrap() == other.unwrap();
    }

    bool operator!=(const Err &other) const {
        return !(*this == other);
    }

  public:
    bool is_ok() const noexcept {
        return this->tag_ == OK;
    }

    bool is_err() const noexcept {
        return this->tag_ == ERR;
    }

    operator bool() const {
        return this->is_ok();
    }

    Optional<T> ok() const noexcept {
        return (this->is_ok()) ?
                   Optional<T>(typename Optional<T>::Some(this->ok_.unwrap())) :
                   Optional<T>(typename Optional<T>::None());
    }

    Optional<E> err() const noexcept {
        return (this->is_err()) ? Optional<E>(typename Optional<E>::Some(
                                      this->err_.unwrap())) :
                                  Optional<E>(typename Optional<E>::None());
    }

    const T &unwrap() const noexcept {
        if (!this->is_ok()) std::terminate();
        return this->ok_.unwrap();
    }

    T unwrap() noexcept {
        if (!this->is_ok()) std::terminate();
        return this->ok_.unwrap();
    }

    const E &unwrap_err() const noexcept {
        if (!this->is_err()) std::terminate();
        return this->err_.unwrap();
    }

    E unwrap_err() noexcept {
        if (!this->is_err()) std::terminate();
        return this->err_.unwrap();
    }

    const T &unwrap_or(const T &other) const noexcept {
        return (this->is_ok()) ? this->ok_.unwrap() : other;
    }

    T unwrap_or(const T &other) noexcept {
        return (this->is_ok()) ? this->ok_.unwrap() : other;
    }

    template<class U>
    Result<U, E> map(std::function<U(const T &)> fn) const noexcept {
        return (this->is_ok()) ?
                   Result<U, E>(
                       typename Result<U, E>::Ok(fn(this->ok_.unwrap()))) :
                   Result<U, E>(
                       typename Result<U, E>::Err(this->err_.unwrap()));
    }

    template<class F>
    Result<T, F> map_err(std::function<F(const E &)> fn) const noexcept {
        return (this->is_err()) ?
                   Result<T, F>(
                       typename Result<T, F>::Err(fn(this->err_.unwrap()))) :
                   Result<T, F>(typename Result<T, F>::Ok(this->ok_.unwrap()));
    }
};

#define NEL_RESULT_TRY(f) \
    ({ \
        auto v = std::move(f); \
        if (v.is_err()) return std::move(v); \
        v.unwrap(); \
    })

} // namespace nel

#endif // NEL_RESULT_HH
