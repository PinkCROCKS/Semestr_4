#ifndef NUMBER_5_VECTOR_H
#define NUMBER_5_VECTOR_H

#include <algorithm>
#include <compare>
#include "container.hpp"
#include "../../../../../Ярослав/Desktop/logg.hpp"

namespace my_container {
    template <typename T>
    class Vector : public Container<T>{
    private:
        T* array = nullptr;
        size_t size_;
        size_t capacity_;
    public:
        using iterator = typename Container<T>::iterator;
        using const_iterator = typename Container<T>::const_iterator;

        explicit Vector(size_t capacity_ = 2) : capacity_{capacity_} {
            array = new T[capacity_];
            size_ = 0;
        }
        Vector(const Vector<T>& other) noexcept : Vector(other.capacity_) {
            if (this != &other) {
                new T[other.capacity_];
                capacity_ = other.capacity_;
                size_ = other.size_;
                std::copy_n(other.array, other.size(), array);
            }
        }

        Vector(std::initializer_list<T> init) : array(new T[init.size()]), size_(init.size()),
            capacity_(init.size()) {
            std::copy(init.begin(), init.end(), array);
        }

        Vector(const Vector<T>&& list) noexcept: array(list.array){
            size_ = list.size_;
            capacity_ = list.capacity_;
            list.size_ = 0;
            list.capacity_ = 0;
            list.array = nullptr;
        }

        Vector& operator=(const Vector<T>& other) noexcept{
            if (this != &other){
                T temp;
                resize(other.capacity_, temp);
                std::copy_n(other.array, other.size_, array);
                size_ = other.size_;
            }
            return *this;
        }

        Vector& operator=(Vector<T>&& other) noexcept{
            if (this != &other){
                reserve(other.capacity_);
                std::swap(array, other.array);
                std::swap(capacity_, other.capacity_);
                std::swap(size_, other.size_);
                other.clear;
            }
            return *this;
        }

        Vector& operator=(std::initializer_list<T>& other) {
            reserve(other.size());
            std::copy(other.begin(), other.end(), array);
            size_ = other.size();
            return *this;
        };

        Vector& operator=(std::initializer_list<T>&& other) {
            reserve(other.size());
            std::copy(other.begin(), other.end(), array);
            size_ = other.size();
            return *this;
        };

        ~Vector(){
            delete[] array;
            size_ = 0;
            capacity_ = 0;
        }

        [[nodiscard]] bool empty() const noexcept override{
            return size_ == 0;
        }
        
        [[nodiscard]] size_t size() const noexcept override{
            return size_;
        }
        
        [[nodiscard]] size_t capacity() const noexcept{
            return capacity_;
        }

        void reserve(const size_t new_capacity) noexcept {
            if (capacity_ < new_capacity){
                T* temp = new T[new_capacity];
                std::copy_n(array, size_, temp);
                delete[] array;
                array = temp;
                capacity_ = new_capacity;
            }
        }

        void shrink_to_fit() noexcept{
            if (size_ < capacity_){
                T* temp = new T[size_];
                std::copy_n(array, size_, temp);
                delete[] array;
                array = temp;
                capacity_ = size_;
            }
        }

        void clear() noexcept{
            size_ = 0;
        }

        void push_back(const T& element) noexcept {
            if (size_ < capacity_) {
                array[size_++] = element;
            } else {
                reserve(capacity_ * 2);
                array[size_++] = element;
            }
        }

        void pop_back() noexcept {
            if (size_ > 0){
                size_--;
            }
        }

        void insert(const size_t index,const T& element){
            if (index == size_ - 1) {
                push_back(element);
                return;
            } else if (index < size_) {
                if (size_ == capacity_) {
                    reserve(capacity_ * 2);
                }
                for (size_t i = size_ - 1; i >= index; i--) {
                    array[i + 1] = array[i];
                }
                array[index] = element;
            } else {
                if (index >= capacity_) {
                    reserve(index * 2);
                }
                array[index] = element;
            }
            size_++;
        }

        void erase(size_t index) {
            if (index >= size_) {
                throw std::range_error("Incorrect index");
            }

            for (size_t i = index + 1; i < size_; ++i) {
                array[i - 1] = array[i];
            }
            size_--;
        }

        void resize(const size_t size, T& elem) {
            if (size > size_) {
                reserve(size);
                for (size_t i = size_; i < size; ++i) {
                    array[i] = elem;
                }
                size_ = capacity_;
            } else if (size < size_) {
                size_ = size;
            }
        }

        void swap(Vector& other) {
            using std::swap;
            swap(array, other.array);
            swap(size_, other.size_);
            swap(capacity_, other.capacity_);
        }

        T& operator[](const size_t& index) const{
            if (index >= size_) {
                throw std::out_of_range("Limits!!!!!!!!");
            }
            return array[index];
        };

        T& at(size_t index){
            if (index >= size_) {
                throw std::out_of_range("Limits!!!!!!!!");
            }
            return array[index];
        }

        const T& at(size_t index) const{
            if (index >= size_) {
                throw std::out_of_range("Limits!!!!!!!!");
            }
            resize_v();
            return array[index];
        }

        const T& front() const {
            if (size_ == 0){
                throw std::out_of_range("Vector is empty");
            }
            return array[0];
        }

        T& front() {
            if (size_ == 0){
                throw std::out_of_range("Vector is empty");
            }
            return array[0];
        }


        T& back() {
            if (size_ == 0){
                throw std::out_of_range("Vector is empty");
            }
            return array[size_ - 1];
        }

        const T& back() const{
            if (size_ == 0){
                throw std::out_of_range("Vector is empty");
            }
            return array[size_ - 1];
        }

        T* data() noexcept{
            return array;
        }

        const T* data() const noexcept {
            return array;
        }

        [[nodiscard]] size_t max_size() const noexcept override{
            return capacity_;
        }

        std::strong_ordering operator<=> (const Vector<T>& other) const noexcept{
            if (auto cmp = size_ <=> other.size_; cmp != 0) {
                return cmp;
            }
            resize_v();
            return std::lexicographical_compare_three_way(
                    array,
                    array + size_,
                    other.data(),
                    other.data() + other.size_
            );
        }

//        friend bool operator==(const Vector<T>& a, const Vector<T>& b) noexcept {
//            return (a <=> b) == 0;
//        }

        bool operator==(const Vector<T>& other) const noexcept{
            return ((*this) <=> other) == 0;
        }

        bool operator!=(const Vector<T>& other) const noexcept{
            return !((*this) == other);
        }

        bool operator<(const Vector<T>& other) const noexcept{
            return ((*this) <=> other) == std::strong_ordering::less;
        }

        bool operator>(const Vector<T>& other) const noexcept{
            return (*this <=> other) == std::strong_ordering::greater;
        }

        bool operator<=(const Vector<T>& other) const noexcept{
            return !((*this) > other);
        }

        bool operator>=(const Vector<T>& other) const noexcept{
            return !((*this) < other);
        }

//        void resize(const size_t i) {
//            if (this != &other){
//                resize(other.capacity_);
//                std::copy_n(other.array, other.size_, array);
//                size_ = other.size_;
//            }
//            return *this;
//        }
    };
}

#endif