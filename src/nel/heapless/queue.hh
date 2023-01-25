// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
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

#include <nel/iterator.hh>
#include <nel/slice.hh>
#include <nel/optional.hh>
#include <nel/result.hh>
#include <nel/log.hh>
#include <nel/panic.hh>

namespace nel
{
namespace heapless
{
/**
 * A constrained queue of values (a FIFO queue)
 *
 * Values stored inline in object (no heap usage)
 * Queue owns values , on destroy of queue, owned values destroyed as well.
 * The FIFO ordering is preserved.
 */
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

    private:
        Queue(void)
            : len_(0)
            , wp_(0)
            , rp_(0)
        {
        }

    public:
        // no implicit copying allowed
        constexpr Queue(Queue const &o) = delete;
        constexpr Queue &operator=(Queue const &o) = delete;

        // moving allowed though.
        constexpr Queue(Queue &&o)
            : len_(o.len_)
            , wp_(o.wp_)
            , rp_(o.rp_)
        {
            // TODO: replace with slice().move_from(o.slice())
            T *d = &store_[rp_];
            T *s = &o.store_[rp_];
            for (Index l = len_; l != 0; --l) {
                new (d) T(move(*s));
                s += 1;
                if (s == &o.store_[N]) { s = &o.store_[0]; }
                d += 1;
                if (d == &store_[N]) { d = &store_[0]; }
            }
            o.len_ = 0;
            o.wp_ = o.rp_ = 0;
        }

        constexpr Queue &operator=(Queue &&o)
        {
            if (this != &o) {
                this->~Queue();
                new (this) Queue(move(o));
            }
            return *this;
        }

    public:
        /**
         * Create a queue with no values in it.
         *
         * @returns the queue created.
         */
        constexpr static Queue empty(void)
        {
            return Queue();
        }

    public:
        /**
         * Determine if the queue is empty.
         *
         * @returns true if queue is empty, false otherwise.
         */
        bool is_empty(void) const
        {
            return len_ == 0;
        }

        /**
         * Determine if the queue is full.
         *
         * @returns true if queue is full, false otherwise.
         */
        bool is_full(void) const
        {
            return len_ == N;
        }

        /**
         * Return the number of items in the queue.
         *
         * @returns number of items in the queue.
         */
        Length len(void) const
        {
            return len_;
        }

        /**
         * Put a value onto the queue.
         *
         * if successfull, val is moved into the queue
         * if unsuccessful, val is still moved, just into the result.
         *
         * @param val The value to move into the queue.
         * @returns if successful, Result<void, T>::Ok()
         * @returns if unsuccessful, Result<void, T>::Err() holding val
         */
        Result<void, T> put(T &&val)
        {
            if (is_full()) { return Result<void, T>::Err(val); }
            len_ += 1;
            new (&store_[wp_]) T(val);
            wp_ += 1;
            if (wp_ == N) { wp_ = 0; }
            return Result<void, T>::Ok();
        }

        /**
         * Get next value from the queue.
         *
         * @param val The value to move into the queue.
         * @returns if successful, Optional<T>::Some(val)
         * @returns if unsuccessful, Optional<T>::None
         */
        Optional<T> get(void)
        {
            if (is_empty()) { return None; }
            len_ -= 1;
            auto r = Some(move(store_[rp_]));
            rp_ += 1;
            if (rp_ == N) { rp_ = 0; }
            return r;
        }

    public:
        typedef ChainIterator<SliceIterator<T>> QueueIteratorMut;

        QueueIteratorMut iter(void)
        {
            if (len() == 0) {
                auto s1 = Slice<T>::empty();
                auto s2 = s1;
                return QueueIteratorMut(s1.iter(), s2.iter());
            } else if (rp_ < wp_) {
                auto s1 = Slice<T>::from(&store_[rp_], &store_[wp_]);
                auto s2 = Slice<T>::empty();
                return QueueIteratorMut(s1.iter(), s2.iter());
            } else {
                auto s1 = Slice<T>::from(&store_[rp_], &store_[N]);
                auto s2 = Slice<T>::from(&store_[0], &store_[wp_]);
                return QueueIteratorMut(s1.iter(), s2.iter());
            }
        }

        typedef ChainIterator<SliceIterator<T const>> QueueIterator;

        QueueIterator iter(void) const
        {
            if (len() == 0) {
                auto s1 = Slice<T const>::empty();
                auto s2 = s1;
                return QueueIterator(s1.iter(), s2.iter());
            } else if (rp_ < wp_) {
                auto s1 = Slice<T const>::from(&store_[rp_], &store_[wp_]);
                auto s2 = Slice<T const>::empty();
                return QueueIterator(s1.iter(), s2.iter());
            } else {
                auto s1 = Slice<T const>::from(&store_[rp_], &store_[N]);
                auto s2 = Slice<T const>::from(&store_[0], &store_[wp_]);
                return QueueIterator(s1.iter(), s2.iter());
            }
        }

    public:
        /**
         * Format/emit a representation of this object as a charstring
         * for debugging purposes.
         *
         * @param val the value to format
         * @param outs the stream to dump the representation into.
         */
        // TODO: replace <<(Log ) with dbgfmt, so separate out from
        // any other form of conversion to charstring.
        // TODO: insert into formatter and not final dest type.
        friend Log &operator<<(Log &outs, Queue const &v)
        {
            outs << "Queue<" << N << ">(" << v.len() << "){";
            outs << v.iter();
            outs << '}';
            return outs;
        }
};

} // namespace heapless
} // namespace nel

#endif // NEL_HEAPLESS_QUEUE_HH
