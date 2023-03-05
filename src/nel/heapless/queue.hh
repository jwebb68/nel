// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(NEL_HEAPLESS_QUEUE_HH)
#    define NEL_HEAPLESS_QUEUE_HH

#    include <nel/defs.hh> //NEL_UNUSED, Length

namespace nel
{
namespace heapless
{

template<typename T, Length const N>
struct Queue;

} // namespace heapless
} // namespace nel

#    include <nel/iterator.hh>
#    include <nel/slice.hh>
#    include <nel/optional.hh>
#    include <nel/result.hh>
#    include <nel/memory.hh>
#    include <nel/log.hh>

#    include <new> // placement new

namespace nel
{
namespace heapless
{

/**
 * Queue
 *
 * A constrained queue of values (a FIFO queue)
 * A dynamic container of T.
 * Manages a block of ram within itself (i.e. heapless)
 * Queue owns values , on destroy of queue, owned values destroyed as well.
 * Elements moved in when pushed, moved out when popped.
 * Once full, push will drop oldest value and overwrite.
 * Once empty, pop will fail.
 * All remaining elements are destroyed when queue is destroyed.
 * Queue cannot be resized.
 * Present Elements can be iterated over in push order.
 * Queue can be moved, calling the move operator on each elem.
 * Queue cannot be copied implicitly.
 * The FIFO ordering is preserved.
 */
template<typename T, Length const N>
struct Queue {
    public:
    private:
        // cannot use Array<T,N> as Array wants all elems initialised.
        // and this class does not want that.
        T store_[N];
        Length len_;
        Index wp_;
        Index rp_;

    public:
        ~Queue(void)
        {
            // TODO: delete in reverse as allocated forward..?
            iter().for_each([&](T &v) -> void { v.~T(); });
        }

    public:
        Queue(void)
            : len_(0)
            , wp_(0)
            , rp_(0)
        {
        }

    private:
        // no implicit copying allowed
        constexpr Queue(Queue const &o) = delete;
        constexpr Queue &operator=(Queue const &o) = delete;

    public:
        // moving allowed though.
        constexpr Queue(Queue &&o)
            : len_(move(o.len_))
            , wp_(move(o.wp_))
            , rp_(move(o.rp_))
        {
            o.len_ = 0;
            o.wp_ = o.rp_ = 0;

            // moved contents to this, o is now empty/invalid.
            // but cannot destroy o.store_ as this now owns them.
            if (len_ == 0) {
            } else if (rp_ < wp_) {
                T *s = &o.store_[rp_];
                for (T *d = &store_[rp_]; d != &store_[wp_]; ++d) {
                    new (d) T(move(*s));
                    ++s;
                }
            } else {
                T *s = &o.store_[rp_];
                for (T *d = &store_[rp_]; d != &store_[N]; ++d) {
                    new (d) T(move(*s));
                    ++s;
                }
                s = &o.store_[0];
                for (T *d = &store_[0]; d != &store_[wp_]; ++d) {
                    new (d) T(move(*s));
                    ++s;
                }
            }
        }

        Queue &operator=(Queue &&o)
        {
            if (this != &o) {
                rp_ = move(o.rp_);
                wp_ = move(o.wp_);
                len_ = move(o.len_);
                o.rp_ = o.wp_ = o.len_ = 0;
                if (len_ == 0) {
                } else if (rp_ < wp_) {
                    T *s = &o.store_[rp_];
                    for (T *d = &store_[rp_]; d != &store_[wp_]; ++d) {
                        *d = move(*s);
                        ++s;
                    }
                } else {
                    T *s = &o.store_[rp_];
                    for (T *d = &store_[rp_]; d != &store_[N]; ++d) {
                        *d = move(*s);
                        ++s;
                    }
                    s = &o.store_[0];
                    for (T *d = &store_[0]; d != &store_[wp_]; ++d) {
                        *d = move(*s);
                        ++s;
                    }
                }
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
        constexpr bool is_empty(void) const
        {
            return len_ == 0;
        }

        /**
         * Determine if the queue is full.
         *
         * @returns true if queue is full, false otherwise.
         */
        constexpr bool is_full(void) const
        {
            return len_ == N;
        }

        /**
         * Return the number of items in the queue.
         *
         * @returns number of items in the queue.
         */
        constexpr Length len(void) const
        {
            return len_;
        }

        /**
         * Return the capacity of the queue.
         *
         * @returns number of items.
         */
        constexpr Length capacity(void) const
        {
            return N;
        }

    public:
        void clear(void)
        {
            iter().for_each([&](T &v) -> void { v.~T(); });
            rp_ = wp_ = len_ = 0;
        }

        /**
         * Push a value onto the queue.
         *
         * if successful, val is moved into the queue
         * if unsuccessful, val is still moved, just into the result.
         *
         * @param val The value to move into the queue.
         * @returns if successful, Result<void, T>::Ok()
         * @returns if unsuccessful, Result<void, T>::Err() holding val
         */
        Result<void, T> NEL_WARN_UNUSED_RESULT push(T &&val)
        {
#    if 0
// TODO: separate out behaviour into sep classes.. (or one with traits)
// one for failing on full
// one for dropping oldest on full
            if (is_full()) { return Result<void, T>::Err(move(val)); }
            len_ += 1;
            new (&store_[wp_]) T(move(val));
            wp_ += 1;
            if (wp_ == N) { wp_ = 0; }
            return Result<void, T>::Ok();
#    else
            if (is_full()) {
                // here , rp_ == wp_.
                rp_ += 1;
                if (rp_ >= capacity()) { rp_ = 0; }
                store_[wp_] = move(val);
            } else {
                len_ += 1;
                new (&store_[wp_]) T(val);
            }
            wp_ += 1;
            if (wp_ >= capacity()) { wp_ = 0; }
            return Result<void, T>::Ok();
#    endif
        }

        /**
         * Get next value from the queue.
         *
         * @param val The value to move into the queue.
         * @returns if successful, Optional<T>::Some(val)
         * @returns if unsuccessful, Optional<T>::None
         */
        Optional<T> pop(void)
        {
            if (is_empty()) { return None; }
            len_ -= 1;
            auto rp = rp_;
            rp_ += 1;
            if (rp_ == N) { rp_ = 0; }
            return Some(move(store_[rp]));
        }

        void drain(Length n)
        {
            if (len() == 0) {
            } else if (n >= len()) {
                clear();
            } else if ((rp_ + n) < N) {
                auto s1 = Slice<T>::from(&store_[rp_], &store_[rp_ + n]);
                auto s2 = Slice<T>::empty();
                auto it = QueueIteratorMut(s1.iter(), s2.iter());
                it.for_each([&](T &v) -> void { v.~T(); });
                rp_ += n;
                len_ -= n;
            } else {
                auto s1 = Slice<T>::from(&store_[rp_], &store_[N]);
                auto s2 = Slice<T>::from(&store_[0], &store_[n - N]);
                auto it = QueueIteratorMut(s1.iter(), s2.iter());
                it.for_each([&](T &v) -> void { v.~T(); });
                rp_ = n - N;
                len_ -= n;
            }
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

}; // namespace heapless
}; // namespace nel

#endif // defined(NEL_HEAPLESS_QUEUE_HH)
