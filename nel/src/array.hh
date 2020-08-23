#ifndef NEL_ARRAY_HH
#define NEL_ARRAY_HH

namespace nel {

template<typename T, size_t const N>
struct Array;

}


namespace nel {

// fixed size array
// all items allocd when created
// all items deletd when destroyed
//
template<typename T, size_t const N>
struct Array {
    public:
    private:
        T values[N];
    public:
        ~Array(void) = default;

        // if no init list?
        // only if T has default cons
        // is this default?
        constexpr Array(void) = default;

        // initialiser list initialisation required here..

        // if type has copy, then could fill?

        // no copying
        constexpr Array(Array const &o) = delete;
        constexpr Array &operator=(Array const &o) = delete;

        // moving allowed
        constexpr Array(Array &&o)
        {
            for (size_t i = 0; i < N; ++i) {
                //values_[i] = std::move(o.values_[i]);
                new (&values_[i]) T(std::move(o.values_[i]));
            }
        }
        constexpr Array &operator=(Array const &o)
        {
            // expensive to move o to temp, then to swap o and this (using moves)
            // quicker to just move o into this.
            ~Array();
            new (this) Array(std::move(o));
        }

    public:
        // move item in (idx)
        // move item out (idx)
        // [const] ref item (idx)
        constexpr T &operator[](int idx)
        {
            // if checked, panic if idx > size
            assert(idx < N);
            return values_[idx];
        }
        constexpr T const &operator[](int idx) const
        {
            // if checked, panic if idx > size
            assert(idx < N);
            return values_[idx];
        }

};


template<typename T>
struct Array {
    public:
    private:
        typedef Node<T> ArrNode;
        // cannot use new/delete as created using malloc/realloc.
        ArrNode *item_;

    public:
        ~Array(void)
        {
            ArrNode::free(item_);
        }

        // if no init list?
        // only if T has default cons
        // is this default?
        constexpr Array(void)
            : item_(nullptr)
        {}

        // initialiser list initialisation required here..

        // if type has copy, then could fill?

        // no copying
        constexpr Array(Array const &o) = delete;
        constexpr Array &operator=(Array const &o) = delete;

        // moving allowed
        constexpr Array(Array &&o)
            : item_(std::move(o.item_))
        {
            o.item_ = nullptr;
        }

        constexpr Array &operator=(Array &&o)
        {
            Array t(std::move(o));
            swap(t);
            return *this;
        }
        void swap(Array &o)
        {
            std::swap(item_, o.item_);
        }

    public:
        // how to convert vector to array
        // into on vec, then need to pass Node into array using private ctor.. needs arr::friend vec
        // from on array, then need to detach node from vec using provate func.. needs vec:friend arr
        // rust: as_ptr(&self) -> *const T
        // rust: as_mut_ptr(&mut self) -> *mut T

        constexpr Array(ArrayNode *const n)
            : item_(n)
        {}

    public:
        constexpr size_t len(void) const
        {
            return (item != nullptr) ? item_->len() : 0;
        }

        // move item in (idx) - use mut []
        // move item out (idx) - use mut []

        constexpr T &operator[](int idx)
        {
            assert(item_ != nullptr);
            return (*item_)[idx];
        }
        constexpr T const &operator[](int idx) const
        {
            assert(item_ != nullptr);
            return (*item_)[idx];
        }

};

}



#endif//NEL_ARRAY_HH
