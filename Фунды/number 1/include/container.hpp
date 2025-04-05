#ifndef NUMBER_1_CONTAINER_H
#define NUMBER_1_CONTAINER_H

#include <algorithm>
#include <iterator>
#include <compare>
#include <iostream>

namespace my_container {

    template<typename type>
    class Container {
    public:
        virtual ~Container() = 0;

        virtual size_t size() const noexcept = 0;

        [[nodiscard]] virtual size_t max_size() const noexcept = 0;

        virtual bool empty() const noexcept = 0;

        virtual bool operator==(const Container &) const = 0;

        virtual bool operator!=(const Container &) const = 0;
    };

    template<typename type>
    Container<type>::~Container() = default;
}
#endif //NUMBER_1_CONTAINER_H
