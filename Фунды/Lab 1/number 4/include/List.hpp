#ifndef NUMBER_4_LIST_HPP
#define NUMBER_4_LIST_HPP

#include <iostream>
#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <limits>
#include "Container.hpp"

namespace my_container {

    template <typename T>
    class List : public Container<T> {
    protected:
        struct Node {
            T data;
            Node* prev;
            Node* next;

            explicit Node(const T& d, Node* prev = nullptr, Node* next = nullptr)
                    : data(d), prev(prev), next(next) {}
        };

        size_t size_ = 0;
        Node* head = nullptr;
        Node* tail = nullptr;

        void init_sentinels() {
            head = new Node(T{});
            tail = new Node(T{});
            head->next = tail;
            tail->prev = head;
        }

    public:
        class iterator {
        public:
            using iterator_category = std::bidirectional_iterator_tag;
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using pointer = T*;
            using reference = T&;

            Node* node;

            explicit iterator(Node* node = nullptr) : node(node) {}

            reference operator*() { return node->data; }
            pointer operator->() { return &node->data; }

            iterator& operator++() {
                node = node->next;
                return *this;
            }

            iterator operator++(int) {
                iterator tmp = *this;
                ++(*this);
                return tmp;
            }

            iterator& operator--() {
                node = node->prev;
                return *this;
            }

            iterator operator--(int) {
                iterator tmp = *this;
                --(*this);
                return tmp;
            }

            bool operator==(const iterator& other) const { return node == other.node; }
            bool operator!=(const iterator& other) const { return node != other.node; }
        };

        class const_iterator {
        public:
            using iterator_category = std::bidirectional_iterator_tag;
            using value_type = const T;
            using difference_type = std::ptrdiff_t;
            using pointer = const T*;
            using reference = const T&;

            const Node* node;

            explicit const_iterator(const Node* node = nullptr) : node(node) {}

            const_iterator(const iterator& it) : node(it.node) {} // Конвертация

            reference operator*() const { return node->data; }
            pointer operator->() const { return &node->data; }

            const_iterator& operator++() {
                node = node->next;
                return *this;
            }

            const_iterator operator++(int) {
                const_iterator tmp = *this;
                ++(*this);
                return tmp;
            }

            const_iterator& operator--() {
                node = node->prev;
                return *this;
            }

            const_iterator operator--(int) {
                const_iterator tmp = *this;
                --(*this);
                return tmp;
            }

            bool operator==(const const_iterator& other) const { return node == other.node; }
            bool operator!=(const const_iterator& other) const { return node != other.node; }
        };

        List() { init_sentinels(); }

        List(const List<T>& other) : List() {
            for (const auto& elem : other) {
                push_back(elem);
            }
        }

        List(List<T>&& other) noexcept {
            init_sentinels();
            swap(other);
        }

        List(const std::initializer_list<T>& t) : List() {
            for (auto& elem : t) {
                push_back(elem);
            }
        }

        ~List() override {
            clear();
            delete head;
            delete tail;
        }

        List<T>& operator=(const List<T>& other) {
            List tmp(other);
            swap(tmp);
            return *this;
        }

        List<T>& operator=(List<T>&& other) noexcept {
            swap(other);
            return *this;
        }

        iterator begin() noexcept { return iterator(head->next); }
        const_iterator begin() const noexcept { return const_iterator(head->next); }
        const_iterator cbegin() const noexcept { return const_iterator(head->next); }

        iterator end() noexcept { return iterator(tail); }
        const_iterator end() const noexcept { return const_iterator(tail); }
        const_iterator cend() const noexcept { return const_iterator(tail); }

        std::reverse_iterator<iterator> rbegin() { return std::reverse_iterator(end()); }
        std::reverse_iterator<const_iterator> crbegin() const {
            return std::reverse_iterator(cend());
        }

        std::reverse_iterator<iterator> rend() { return std::reverse_iterator(begin()); }
        std::reverse_iterator<const_iterator> crend() const {
            return std::reverse_iterator(cbegin());
        }

        bool empty() const noexcept override { return size_ == 0; }
        size_t size() const noexcept override { return size_; }
        size_t max_size() const noexcept override {
            return 145674;
        }

        void clear() {
            while (!empty()) {
                pop_front();
            }
        }

        iterator insert(iterator pos, const T& value) {
            Node* new_node = new Node(value, pos.node->prev, pos.node);
            pos.node->prev->next = new_node;
            pos.node->prev = new_node;
            ++size_;
            return iterator(new_node);
        }

        iterator insert(iterator pos, T&& value) {
            Node* new_node = new Node(std::move(value), pos.node->prev, pos.node);
            pos.node->prev->next = new_node;
            pos.node->prev = new_node;
            ++size_;
            return iterator(new_node);
        }

        T& front() { return *begin(); }
        T& back() { return *(--end()); }

        void push_front(const T& val) { insert(begin(), val); }
        void push_front(T&& val) { insert(begin(), std::move(val)); }
        void push_back(const T& val) { insert(end(), val); }
        void push_back(T&& val) { insert(end(), std::move(val)); }

        void pop_back() {
            erase(--end());
        }
        void pop_front() {
            erase(begin());
        }

        iterator erase(iterator pos) {
            Node* target = pos.node;
            iterator next = iterator(target->next);
            target->prev->next = target->next;
            target->next->prev = target->prev;
            delete target;
            --size_;
            return next;
        }

        iterator erase(iterator first, iterator last) {
            while (first != last) {
                first = erase(first);
            }
            return last;
        }

        void resize(size_t count, const T& value = T()) {
            while (size_ > count) pop_back();
            while (size_ < count) push_back(value);
        }

        void swap(List<T>& rhs) noexcept {
            std::swap(head, rhs.head);
            std::swap(tail, rhs.tail);
            std::swap(size_, rhs.size_);
        }

        std::strong_ordering operator<=>(const List<T>& rhs) const {
            return std::lexicographical_compare_three_way(
                    begin(), end(), rhs.begin(), rhs.end()
            );
        }

        bool operator==(const List<T>& rhs) const {
            return size_ == rhs.size_ && (*this <=> rhs) == std::strong_ordering::equal;
        }

        bool operator!=(const List<T>& rhs) const { return !(*this == rhs); }
        bool operator>(const List<T>& rhs) const { return (*this <=> rhs) > 0; }
        bool operator>=(const List<T>& rhs) const { return (*this <=> rhs) >= 0; }
        bool operator<(const List<T>& rhs) const { return (*this <=> rhs) < 0; }
        bool operator<=(const List<T>& rhs) const { return (*this <=> rhs) <= 0; }
    };

    template <typename T>
    std::ostream& operator<<(std::ostream& out, const List<T>& list) {
        for (auto& el : list) {
            out << el << " ";
        }
        return out << "\n";
    }

}

#endif //NUMBER_4_LIST_HPP
