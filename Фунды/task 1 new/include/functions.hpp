#ifndef TASK_1_NEW_FUNCTIONS_HPP
#define TASK_1_NEW_FUNCTIONS_HPP

#include <cstddef>
#include <stdexcept>
#include <algorithm>
#include <iterator>
#include <compare>
#include <iostream>
#include "functions.hpp"

namespace my_container {

    template <typename type>
    class Container {
    public:
        virtual ~Container() = 0;
        virtual size_t size() const noexcept = 0;
        virtual size_t max_size() const noexcept = 0;
        virtual bool empty() const noexcept = 0;

        virtual bool operator==(const Container&) const = 0;
        virtual bool operator!=(const Container&) const = 0;
    };

    template <typename type>
    Container<type>::~Container() = default;

    template <typename type, size_t N>
    class Array final : public Container<type> {
        type data_[N]{};

    public:
        Array() = default;

        Array(std::initializer_list<type> init) {
            std::copy_n(init.begin(), std::min(N, init.size()), data_);
        }
        Array(const Array&) = default;
        Array& operator=(const Array&) = default;

        Array(Array&&) noexcept = default;
        Array& operator=(Array&&) noexcept = default;

        ~Array() override = default;

        type& at(size_t pos) {
            if (pos >= N) throw std::out_of_range("Array index out of range");
            return data_[pos];
        }

        const type& at(size_t pos) const {
            if (pos >= N) throw std::out_of_range("Array index out of range");
            return data_[pos];
        }

        type& operator[](size_t pos) noexcept { return data_[pos]; }
        const type& operator[](size_t pos) const noexcept { return data_[pos]; }

        type& front() noexcept { return data_[0]; }
        const type& front() const noexcept { return data_[0]; }

        type& back() noexcept { return data_[N-1]; }
        const type& back() const noexcept { return data_[N-1]; }

        type* data() noexcept { return data_; }
        const type* data() const noexcept { return data_; }

        auto begin() noexcept { return data_; }
        auto begin() const noexcept { return data_; }
        auto cbegin() const noexcept { return data_; }

        auto end() noexcept { return data_ + N; }
        auto end() const noexcept { return data_ + N; }
        auto cend() const noexcept { return data_ + N; }

        auto rbegin() noexcept { return std::reverse_iterator(end()); }
        auto rbegin() const noexcept { return std::reverse_iterator(end()); }
        auto crbegin() const noexcept { return std::reverse_iterator(cend()); }

        auto rend() noexcept { return std::reverse_iterator(begin()); }
        auto rend() const noexcept { return std::reverse_iterator(begin()); }
        auto crend() const noexcept { return std::reverse_iterator(cbegin()); }

        void fill(const type& value) {
            std::fill_n(data_, N, value);
        }

        void swap(Array& other) noexcept {
            std::swap_ranges(data_, data_ + N, other.data_);
        }

        bool operator==(const Container<type>& other) const override {
            const Array* p = dynamic_cast<const Array*>(&other);
            return p && std::equal(begin(), end(), p->begin());
        }

        bool operator!=(const Container<type>& other) const override {
            return !(*this == other);
        }

        bool operator<(const Container<type>& other) const {
            if (this->size() == other.size()){
                for (int i = 0; i < other.size();i++){
                    if (this[i] != other[i]) {
                        return this[i] < other[i];
                    }
                }
                return false;
            }
            return this->size() < other.size();
        }

        bool operator>=(const Container<type>& other) const {
            return !(this < other);
        }

        bool operator<=(const Container<type>& other) const {
            if (this->size() == other.size()){
                for (int i = 0; i < other.size();i++){
                    if (this[i] != other[i]) {
                        return this[i] < other[i];
                    }
                }
                return true;
            }
            return this->size() < other.size();
        }

        bool operator>(const Container<type>& other) const{
            return !(this <= other);
        }

        std::strong_ordering operator<=>(const Array<type, N>& rhs) const {
            return std::lexicographical_compare_three_way(cbegin(), cend(), rhs.cbegin(), rhs.cend());
        }

        size_t size() const noexcept override { return N; }
        size_t max_size() const noexcept override { return N; }
        bool empty() const noexcept override { return N == 0; }
    };

    template<typename type, size_t n>
    std::ostream &operator<<(std::ostream &out, my_container::Array<type, n> el) {
        for (int i = 0; i < el.size(); i++) {
            out << el[i] << " ";
        }
        return out;
    }

}

int add(int, int);
#endif
