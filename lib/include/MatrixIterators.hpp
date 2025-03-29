#ifndef MATRIX_ITERATORS_HPP
#define MATRIX_ITERATORS_HPP

#include <cstddef>
#include <memory>

template <class T, bool is_const>
struct MatrixIterator {
    friend MatrixIterator<T, !is_const>;
    using pointer = std::conditional_t<is_const, const T, T>*;
    using value_type = T;
    using reference = std::conditional_t<is_const, const T, T>&;
    using difference_type = ptrdiff_t;
    using iterator_category = std::random_access_iterator_tag;
    MatrixIterator() noexcept : ptr_(nullptr) {}
    MatrixIterator(pointer ptr) : ptr_(ptr) {}
    template <bool other_const>
    MatrixIterator(MatrixIterator<T, other_const>& other) requires (is_const >= other_const) : ptr_(other.ptr_) {}
    template <bool other_const> 
    MatrixIterator& operator=(const MatrixIterator<T, other_const>& other) noexcept requires (is_const >= other_const) {
        ptr_ = other.ptr_;
        return *this;
    }
    MatrixIterator& operator++() noexcept { ptr_++; return *this; }
    MatrixIterator operator++(int) { MatrixIterator i = *this; ptr_++; return i; }
    MatrixIterator& operator--() noexcept { ptr_--; return *this; }
    MatrixIterator operator--(int) noexcept { MatrixIterator i = *this; ptr_--; return i; }
    reference operator*() const noexcept { return *ptr_; }
    pointer operator->() const noexcept { return ptr_; }   
    template <bool other_const> 
    bool operator==(const MatrixIterator<T, other_const>& other) const noexcept { return ptr_ == other.ptr_; }
    template <bool other_const>
    bool operator>(const MatrixIterator<T, other_const>& other) const noexcept { return ptr_ > other.ptr_; }
    template <bool other_const>
    bool operator>=(const MatrixIterator<T, other_const>& other) const noexcept { return ptr_ >= other.ptr_; }
    template <bool other_const>
    bool operator<(const MatrixIterator<T, other_const>& other) const noexcept { return ptr_ < other.ptr_; }
    template <bool other_const>
    bool operator<=(const MatrixIterator<T, other_const>& other) const noexcept { return ptr_ <= other.ptr_; }
    MatrixIterator& operator+=(const difference_type diff) noexcept { ptr_ += diff; return *this; }
    MatrixIterator& operator-=(const difference_type diff) noexcept { ptr_ -= diff; return *this; }
    template <bool other_const>
    friend MatrixIterator operator+(const MatrixIterator<T, other_const>& it, const difference_type diff) noexcept { return MatrixIterator(diff + it.ptr_); }
    template <bool other_const>
    friend MatrixIterator operator+(const difference_type diff, const MatrixIterator<T, other_const>& it) noexcept { return MatrixIterator(diff + it.ptr_); }
    template <bool other_const>
    friend MatrixIterator operator-(const difference_type diff, const MatrixIterator<T, other_const>& it) noexcept { return MatrixIterator(diff - it.ptr_); }
    template <bool other_const>
    friend MatrixIterator operator-(const MatrixIterator<T, other_const>& it, const difference_type diff) noexcept { return MatrixIterator(it.ptr_ - diff); }
    template <bool other_const>
    difference_type operator-(const MatrixIterator<T, other_const>& it) const noexcept {return ptr_ - it.ptr_; }
    reference operator[](const difference_type index) const noexcept { return ptr_[index]; } 
    private:
        pointer ptr_;
};

template <class T, bool is_const> 
struct MatrixColumnIterator {
    friend MatrixColumnIterator<T, !is_const>;
    using pointer = std::conditional_t<is_const, const T, T>*;
    using value_type = T;
    using reference = T&;
    using difference_type = ptrdiff_t;
    using iterator_category = std::random_access_iterator_tag;
    MatrixColumnIterator() noexcept : ptr_(nullptr), width_(0)  {}
    MatrixColumnIterator(pointer ptr, size_t width) : ptr_(ptr), width_(width) {}
    template <bool other_const>
    MatrixColumnIterator(MatrixColumnIterator<T, other_const>& other) requires (is_const >= other_const) : ptr_(other.ptr_), width_(other.width) {}
    template <bool other_const> 
    MatrixColumnIterator& operator=(const MatrixColumnIterator<T, other_const>& other) noexcept requires (is_const >= other_const) {
        ptr_ = other.ptr_;
        width_ = other.width_;
        return *this;
    }
    MatrixColumnIterator& operator++() noexcept { ptr_ += width_; return *this; }
    MatrixColumnIterator operator++(int) { MatrixColumnIterator i = *this; ptr_ += width_; return i; }
    MatrixColumnIterator& operator--() noexcept { ptr_ -= width_; return *this; }
    MatrixColumnIterator operator--(int) noexcept { MatrixColumnIterator i = *this; ptr_ -= width_; return i; }
    reference operator*() const noexcept { return *ptr_; }
    pointer operator->() const noexcept { return ptr_; }   
    template <bool other_const> 
    bool operator==(const MatrixColumnIterator<T, other_const>& other) const noexcept { return ptr_ == other.ptr_; }
    template <bool other_const>
    bool operator>(const MatrixColumnIterator<T, other_const>& other) const noexcept { return ptr_ > other.ptr_; }
    template <bool other_const>
    bool operator>=(const MatrixColumnIterator<T, other_const>& other) const noexcept { return ptr_ >= other.ptr_; }
    template <bool other_const>
    bool operator<(const MatrixColumnIterator<T, other_const>& other) const noexcept { return ptr_ < other.ptr_; }
    template <bool other_const>
    bool operator<=(const MatrixColumnIterator<T, other_const>& other) const noexcept { return ptr_ <= other.ptr_; }
    MatrixColumnIterator& operator+=(const difference_type diff) noexcept { ptr_ += diff * width_; return *this; }
    MatrixColumnIterator& operator-=(const difference_type diff) noexcept { ptr_ -= diff * width_; return *this; }
    template <bool other_const>
    friend MatrixColumnIterator operator+(const MatrixColumnIterator<T, other_const>& it, const difference_type diff) noexcept { return MatrixColumnIterator(diff * it.width_ + it.ptr_, it.width_); }
    template <bool other_const>
    friend MatrixColumnIterator operator+(const difference_type diff, const MatrixColumnIterator<T, other_const>& it) noexcept { return MatrixColumnIterator(diff * it.width_ + it.ptr_, it.width_); }
    template <bool other_const>
    friend MatrixColumnIterator operator-(const difference_type diff, const MatrixColumnIterator<T, other_const>& it) noexcept { return MatrixColumnIterator(diff * it.width_ - it.ptr_, it.width_); }
    template <bool other_const>
    friend MatrixColumnIterator operator-(const MatrixColumnIterator<T, other_const>& it, const difference_type diff) noexcept { return MatrixColumnIterator(it.ptr_ - diff * it.width_, it.width_); }
    template <bool other_const>
    difference_type operator-(const MatrixColumnIterator<T, other_const>& it) const noexcept {return (ptr_ - it.ptr_) / width_; }
    reference operator[](const difference_type index) const noexcept { return ptr_[index * width_]; }
    private:
        size_t width_;
        pointer ptr_;
};

#endif
