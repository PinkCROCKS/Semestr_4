#ifndef NUMBER_5_CONTAINER_H
#define NUMBER_5_CONTAINER_H

#include <iostream>

template <typename T>
class Container {
public:
    using iterator = T *;
    using const_iterator = const T *;
    Container() = default;
    Container(const Container &container) = default;
    Container &operator=(const Container &container) = default;
    virtual ~Container() = default;

    bool operator==(const Container &other) const = default;
    bool operator!=(const Container &other) const = default;

    [[nodiscard]] virtual size_t size() const = 0;
    [[nodiscard]] virtual size_t max_size() const = 0;
    [[nodiscard]] virtual bool empty() const = 0;
};

#endif