#ifndef NEL_VECTOR_HH
#define NEL_VECTOR_HH

namespace nel {

template<typename T>
struct Vector;

}

#include "log.hh"
#include "node.hh"

namespace nel {

template<typename T>
struct Vector {
    public:
    private:
        // may be shared between Vector<T> and Array<T> and Slice<T>?
        // vector: dynamic
        // array: static
        // array from vector: move node from vector to array.
        // vector from array: move node from vector into array,  arguably nope.


        // don't use unieq_ptr as didn't use new to alloc it.
        //std::unique_ptr<Node> item_;
        typedef Node<T> VecNode;
        VecNode *item_;

    public:
        // own an existing array
        // hmm, fraught with issues that
        // Array(T *const v, size_t len)
        //     : item_(v)
        // {}
        ~Vector(void)
        {
            VecNode::free(item_);
        }

        //constexpr Vector(void) = default;
        constexpr Vector(void)
            : item_(nullptr)
        {}

        // no copying..
        constexpr Vector(Vector const &o) = delete;
        constexpr Vector &operator=(Vector const &o) = delete;

        constexpr Vector(Vector &&o)
            : item_(std::move(o.item_))
        {
            o.item_ = nullptr;
        }
        constexpr Vector &operator=(Vector &&o)
        {
            if (this != &o) {
                Vector t(std::move(o));
                swap(t);
            }
            return *this;
        }
        void swap(Vector &o)
        {
            std::swap(item_, o.item_);
        }

    public:
        constexpr VecNode *detach(void)
        {
            return item_.release();
        }

    public:
        Vector(size_t initial)
            : item_(VecNode::malloc(initial))
        {}

        constexpr size_t capacity(void) const
        {
            return (item_ == nullptr) ? 0 : item_->capacity();
        }
        constexpr size_t len(void) const
        {
            return (item_ == nullptr) ? 0 : item_->len();
        }
        constexpr bool is_empty(void) const
        {
            return item_ == nullptr || item_->is_empty();
        }
        void empty(void)
        {
            if (item_ != nullptr) {
                item_->empty();
            }
        }

        void reserve(size_t new_capacity)
        {
            // TODO: could over alloc to some quanta, e.g. 16, 8, etc..
            // TODO: handle alignment issues here.
            if (item_ == nullptr) {
                // no current alloc..
                item_ = VecNode::malloc(new_capacity);
            } else if (new_capacity > item_->capacity()) {
                // grow.. + realloc
                // no copying of contained, so bitwise move is ok
                item_ = VecNode::realloc(item_, new_capacity);
            } else if (new_capacity < item_->capacity()) {
                // shrink (release off excess..) + realloc
                // can shrink below current inuse ?
                if (new_capacity > item_->len()) {
                    // no copying of contained, so bitwise move is ok
                    // can realloc handle shrink?
                    item_ = VecNode::realloc(item_, new_capacity);
                } else {
                    // not shrinking if same or below current inuse
                }
            } else {
                // new_capacity == item_->capacity()
                // no action
            }
        }

        // bool push_back(T &&v)
        // {
        //     reserve(capacity()+1);
        //     return item_->push_back(std::move(v));
        // }
        template<typename ...Args>
        void push_back(Args &&...args)
        {
            if (len() >= capacity()) {
                reserve(capacity() + 10);
            }
            item_->push_back_unchecked(std::forward<Args>(args)...);
        }
        Optional<T> pop_back(void)
        {
            return (item_ == nullptr)
                   ? Optional<T>::None()
                   : item_->pop_back();
        }

        constexpr T &operator[](size_t idx)
        {
            nel_panic_if(item_ == nullptr, "invalid vector");
            return *item_[idx];
        }
        constexpr T const &operator[](size_t idx) const
        {
            nel_panic_if(item_ == nullptr, "invalid vector");
            return *item_[idx];
        }

        Iterator<T, T const &> iter(void) const
        {
            return (item_ == nullptr)
                ? Iterator<T, T const &>(*item_, 0)
                : Iterator<T, T const &>(*item_);
        }
        Enumerator<T> enumer(void) const
        {
            return (item_ == nullptr)
                ? Enumerator<T>(*item_, 0)
                : Enumerator<T>(*item_);
        }

    public:
        friend Log &operator<<(Log &outs, Vector const &v) {
            NEL_UNUSED(v);
            outs << "Vector()";
            return outs;
        }
};

}



#endif//NEL_VECTOR_HH
