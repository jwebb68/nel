#ifndef NEL_HEAPLESS_QUEUE_HH
#define NEL_HEAPLESS_QUEUE_HH

#include <nel/defs.hh> //NEL_UNUSED, nel::Length

namespace nel
{
namespace heapless
{

template<typename T, Length const N>
struct Queue;

} // namespace heapless
} // namespace nel

#include <nel/optional.hh>
#include <nel/result.hh>
#include <nel/log.hh>
#include <nel/panic.hh>

namespace nel
{
namespace heapless
{

template<typename T, Length const N>
struct Queue {
    public:
    private:
        T store_[N];
        Length len_;
        Index wp_;
        Index rp_;

    public:
        ~Queue(void)
        {
            // delete in reverse, as allocated forward, so reducing heap fragmentation
            // although a freespace coalescing allocator would obviate that need.
            T *it = &store_[wp_];
            for (Index l = len_; l != 0; --l) {
                it -= 1;
                it->~T();
                if (it == &store_[0]) { it = &store_[N]; }
            }
        }

    protected:
        Queue(void): len_(0), wp_(0), rp_(0)
        {
            // store initially uninitialised.
        }

    public:
        static Queue empty(void) noexcept
        {
            return Queue();
        }

        // no implicit copying allowed
        constexpr Queue(Queue const &o) = delete;
        constexpr Queue &operator=(Queue const &o) = delete;

        // moving allowed though.
        constexpr Queue(Queue &&o): len_(o.len_), wp_(o.wp_), rp_(o.rp_)
        {
            T *d = &store_[rp_];
            T *s = &o.store_[rp_];
            for (Index l = len_; l != 0; --l) {
                new (d) T(std::move(*s));
                s += 1;
                if (s == &o.store_[N]) { s = &o.store_[0]; }
                d += 1;
                if (d == &store_[N]) { d = &store_[0]; }
            }
            o.len_ = 0;
            o.wp_ = o.rp_ = 0;
        }
        constexpr Queue &operator=(Queue &&o) noexcept
        {
            if (this != &o) {
                this->~Queue();
                new (this) Queue(std::move(o));
            }
            return *this;
        }

    public:
        Length len(void) const
        {
            return len_;
        }

        bool is_empty(void) const
        {
            return len_ == 0;
        }

        // if Some, then v not stored
        // if None, then v stored.
        //
        Result<void, T> put(T &&v)
        {
            if (is_full()) { return Result<void, T>::Err(v); }
            len_ += 1;
            new (&store_[wp_]) T(v);
            wp_ += 1;
            if (wp_ == N) { wp_ = 0; }
            return Result<void, T>::Ok();
        }

        Optional<T> get(void)
        {
            if (is_empty()) { return Optional<T>::None(); }
            len_ -= 1;
            auto r = Optional<T>::Some(std::move(store_[rp_]));
            rp_ += 1;
            if (rp_ == N) { rp_ = 0; }
            return r;
        }

    public:
    private:
        bool is_full(void)
        {
            return len_ == N;
        }

    public:
        friend Log &operator<<(Log &outs, Queue const &v) noexcept
        {
            outs << "Queue<" << N << ">(" << v.len() << "){"
                 << "\n";
            Index rp = v.rp_;
            for (Index i = 0; i < v.len(); ++i) {
                outs << "[" << i << "]:" << v.store_[rp] << "\n";
                rp += 1;
                if (rp >= v.len()) { rp = 0; }
            }
            outs << "}";
            return outs;
        }
};

} // namespace heapless
} // namespace nel

#endif // NEL_HEAPLESS_QUEUE_HH
