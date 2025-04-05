#ifndef NUMBER_4_DEQUE_HPP
#define NUMBER_4_DEQUE_HPP

#include "List.hpp"

namespace my_container {

    template <typename T>
    class Deque : public List<T> {
    public:
        Deque();
        explicit Deque(const std::initializer_list<T> &init);
        Deque(const Deque<T> &other);
        Deque(Deque<T> &&other) noexcept;

        Deque<T> &operator=(const Deque<T> &other);
        Deque<T> &operator=(Deque<T> &&other) noexcept;
        T &at(size_t pos);
        T &operator[](size_t pos);
    };

    template <typename T>
    Deque<T> &Deque<T>::operator=(Deque<T> &&other) noexcept {
        if (this != &other) {
            List<T>::operator=(std::move(other));
        }
        return *this;
    }

    template <typename T>
    Deque<T> &Deque<T>::operator=(const Deque<T> &other) {
        if (this != &other) {
            List<T>::operator=(other);
        }
        return *this;
    }

    template <typename T>
    Deque<T>::Deque(Deque<T> &&other) noexcept : List<T>(std::move(other)) {}

    template <typename T>
    Deque<T>::Deque(const Deque<T> &other) : List<T>(other) {}

    template <typename T>
    Deque<T>::Deque(const std::initializer_list<T> &init) : List<T>(init) {}

    template <typename T>
    Deque<T>::Deque() : List<T>() {}

    template <typename T>
    T &Deque<T>::operator[](size_t pos) {
        auto iter = this->begin();
        for (size_t i = 0; i < pos; ++i) {
            ++iter;
        }
        return *iter;
    }

    template <typename T>
    T &Deque<T>::at(size_t pos) {
        if (pos >= this->size()) {
            throw std::out_of_range("At");
        }
        return (*this)[pos];
    }

}

#endif //NUMBER_4_DEQUE_HPP
