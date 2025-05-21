#ifndef NUMBER_4_STACK_H
#define NUMBER_4_STACK_H

#include "Deque.hpp"

template <typename T>
class Stack{
private:
    my_container::Deque<T> data;
public:
    Stack() noexcept : data(my_container::Deque<T>()) {};
    Stack(const Stack<T>& other) noexcept : data(my_container::Deque<T>(other.data)) {};
    Stack(Stack<T> &&other) noexcept : data{std::move(other.data)} {};
    Stack(const std::initializer_list<T> &List) noexcept : data(my_container::Deque<T>(List)) {};

    Stack<T>& operator=(const Stack<T> &other) {
        if (*this != other) {
            data = other.data;
        }
        return *this;
    }

    Stack<T> & operator=(Stack<T> &&other) noexcept {
        if (*this != other)  {
            data = std::move(other.data);
        }
        return *this;
    }

    T& top() {return data[0];}
    const T& top() const {return data[0];}

    [[nodiscard]] bool empty() const noexcept {return data.empty();};
    [[nodiscard]] size_t size() const noexcept {return data.size();};

    void push(const T& object) noexcept {data.insert(data.begin(), object);};
    void push(T&& object) noexcept {data.insert(data.begin(), object);};

    void pop() {
        if (data.empty()) {
            throw std::out_of_range("Stack is empty");
        }
        data.pop_front();
    }

    void swap(Stack<T>& other) {
        data.swap(other.data);
    }

    bool operator== (const Stack<T>&other) const noexcept{
        return data == other.data;
    }

    bool operator!= (const Stack<T>&other) const noexcept{
        return !(data == other.data);
    }

    bool operator< (const Stack<T>&other) const noexcept{
        return data < other.data;
    }

    bool operator> (const Stack<T>&other) const noexcept{
        return data > other.data;
    }

    bool operator<= (const Stack<T>&other) const noexcept{
        return data <= other.data;
    }

    bool operator>= (const Stack<T>&other) const noexcept{
        return data >= other.data;
    }

    std::strong_ordering operator<=> (const Stack<T>&other) const noexcept{
        return data <=> other.data;
    }

};



#endif //NUMBER_4_STACK_H
