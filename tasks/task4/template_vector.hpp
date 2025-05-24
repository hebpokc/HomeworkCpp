#pragma once
#include "template_iterator.hpp"

template <class T>
class Vector
{
public: 
    static const size_t reserve_miltiply_ = 2;
    static const size_t empty_vector_size_ = 8;
public:
    using iterator = Iterator<T>;
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using size_type = size_t;

    Vector()
        : data_(reinterpret_cast<pointer>(new char[empty_vector_size_ * sizeof(T)])),
          size_(0),
          capacity_(empty_vector_size_)
    {
        for (size_type i = 0; i < capacity_; ++i) {
            new(data_ + i) T();
        }
    }

    ~Vector()
    {
        clear();

        delete[] reinterpret_cast<char*>(data_);
    }

    reference operator[] (size_type n) {
        if (n >= size_) {
            throw std::out_of_range("Out of range");
        }
        return data_[n];
    }

    const reference operator[] (size_type n) const {
        if (n >= size_) {
            throw std::out_of_range("Out of range");
        }
        return data_[n];
    }

    void push_back(const T & value) {
        if (size_ >= capacity_) {
            reserve(capacity_ == 0 ? empty_vector_size_ : capacity_ * reserve_miltiply_);
        }
        data_[size_] = value;
        ++size_;
    }

    void push_back(T && value) {
        if (size_ >= capacity_) {
            reserve(capacity_ == 0 ? empty_vector_size_ : capacity_ * reserve_miltiply_);
        }
        data_[size_] = std::move(value);
        ++size_;
    }

    void pop_back() {
        if (empty()) {
            throw std::out_of_range("Empty vector");
        }
        data_[--size_].~T();
        new(data_ + size_) T();
    }

    void clear() {
        while (size_ > 0) {
            pop_back();
        }
    }

    void reserve(size_type new_size) {
        if (new_size <= capacity_) {
            return;
        }

        pointer new_data = reinterpret_cast<pointer>(new char[new_size * sizeof(T)]);
        for (size_type i = 0; i < new_size; ++i) {
            new(new_data + i) T();
        }

        for (size_type i = 0; i < size_; ++i) {
            new_data[i] = std::move(data_[i]);
        }

        for (size_type i = 0; i < capacity_; ++i) {
            data_[i].~T();
        }
        delete[] reinterpret_cast<char*>(data_);

        data_ = new_data;
        capacity_ = new_size;
    }

    void resize(size_type new_size) {
        if (new_size < size_) {
            while (size_ > new_size) {
                pop_back();
            }
        } else if (new_size > size_) {
            reserve(new_size);
            size_ = new_size;
        }
    }

    iterator begin() const noexcept {
        return iterator(data_);
    }

    iterator end() const noexcept {
        return iterator(data_ + size_);
    }

    size_type size() const noexcept {
        return size_;
    }

    size_type capacity() const noexcept {
        return capacity_;
    }

    bool empty() const noexcept {
        return size_ == 0;
    }

private:
    pointer data_;
    size_type size_;
    size_type capacity_;
};
