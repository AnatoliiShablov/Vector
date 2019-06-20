//
// Created by anatolii on 6/15/19.
//

#ifndef VECTOR_VECTOR_H
#define VECTOR_VECTOR_H

#include <cstdio>
#include <variant>
#include <memory>

template<typename T>
class base_vector {
    struct data_storage {
        size_t size_;
        size_t capacity_;
        size_t number_of_masters_;
        T data_[];
    };

    data_storage *storage_;

    inline static data_storage *create_storage(size_t capacity) {
        return static_cast<data_storage *>(operator new(sizeof(data_storage) + capacity * sizeof(T)));
    }

    void broot_copy() {
        if (!storage_ || storage_->number_of_masters_ == 1) {
            return;
        }
        auto *new_storage_ = create_storage(storage_->capacity_);
        try {
            std::uninitialized_copy(cbegin(), cend(), new_storage_->data_);
        } catch (...) {
            operator delete(new_storage_);
            throw;
        }
        new_storage_->size_ = storage_->size_;
        new_storage_->capacity_ = storage_->capacity_;
        new_storage_->number_of_masters_ = 1;
        --storage_->number_of_masters_;
        storage_ = new_storage_;
    }

public:
    typedef T value_type;

    typedef T *iterator;

    typedef T const *const_iterator;

    typedef std::reverse_iterator<iterator> reverse_iterator;

    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    typedef T &reference;

    typedef T const &const_reference;

    typedef T *pointer;

    typedef T const *const_pointer;

    base_vector() noexcept : storage_(nullptr) {}

    base_vector(base_vector const &rhs) noexcept : storage_(rhs.storage_) {
        if (storage_) {
            ++storage_->number_of_masters_;
        }
    }

    template<typename InputIterator>
    base_vector(InputIterator first, InputIterator last) {
        auto count = std::distance(first, last);
        if (count < 1) {
            storage_ = nullptr;
        } else {
            storage_ = create_storage(count);
            try {
                std::uninitialized_copy(first, last, storage_->data_);
            } catch (...) {
                operator delete(storage_);
                storage_ = nullptr;
                throw;
            }
            storage_->size_ = count;
            storage_->capacity_ = count;
            storage_->number_of_masters_ = 1;
        }
    }

    base_vector(size_t count, const_reference item) {
        if (count < 1) {
            storage_ = nullptr;
        } else {
            storage_ = create_storage(count);
            try {
                std::uninitialized_fill_n(storage_->data_, count, item);
            } catch (...) {
                operator delete(storage_);
                storage_ = nullptr;
                throw;
            }
            storage_->size_ = count;
            storage_->capacity_ = count;
            storage_->number_of_masters_ = 1;
        }
    }

    base_vector &operator=(base_vector const &rhs) noexcept {
        if (rhs.storage_ == storage_) {
            return *this;
        }
        if (storage_) {
            if (storage_->number_of_masters_ == 1) {
                std::destroy(begin(), end());
                operator delete(storage_);
            } else {
                --storage_->number_of_masters_;
            }
        }
        storage_ = rhs.storage_;
        if (storage_) {
            ++storage_->number_of_masters_;
        }
        return *this;
    }

    template<typename InputIterator>
    void assign(InputIterator first, InputIterator last) {
        *this = base_vector(first, last);
    }

    pointer data() {
        broot_copy();
        return storage_ ? storage_->data_ : nullptr;
    }

    const_pointer data() const noexcept {
        return storage_ ? storage_->data_ : nullptr;
    }

    iterator begin() {
        broot_copy();
        return storage_ ? storage_->data_ : nullptr;
    }

    iterator end() {
        broot_copy();
        return storage_ ? storage_->data_ + storage_->size_ : nullptr;
    }

    const_iterator begin() const noexcept {
        return storage_ ? storage_->data_ : nullptr;
    }

    const_iterator end() const noexcept {
        return storage_ ? storage_->data_ + storage_->size_ : nullptr;
    }

    const_iterator cbegin() const noexcept {
        return storage_ ? storage_->data_ : nullptr;
    }

    const_iterator cend() const noexcept {
        return storage_ ? storage_->data_ + storage_->size_ : nullptr;
    }

    reverse_iterator rbegin() {
        return std::make_reverse_iterator(end());
    }

    reverse_iterator rend() {
        return std::make_reverse_iterator(begin());
    }

    const_reverse_iterator rbegin() const noexcept {
        return std::make_reverse_iterator(cend());
    }

    const_reverse_iterator rend() const noexcept {
        return std::make_reverse_iterator(cbegin());
    }

    const_reverse_iterator crbegin() const noexcept {
        return std::make_reverse_iterator(cend());
    }

    const_reverse_iterator crend() const noexcept {
        return std::make_reverse_iterator(cbegin());
    }

    reference operator[](size_t pos) {
        broot_copy();
        return storage_->data_[pos];
    }

    const_reference operator[](size_t pos) const {
        return storage_->data_[pos];
    }

    reference front() {
        return operator[](0);
    }

    const_reference front() const {
        return operator[](0);
    }

    reference back() {
        return operator[](storage_->size_ - 1);
    }

    const_reference back() const {
        return operator[](storage_->size_ - 1);
    }

    bool empty() const noexcept {
        return storage_ ? storage_->size_ == 0 : true;
    }

    size_t size() const noexcept {
        return storage_ ? storage_->size_ : 0;
    }

    size_t capacity() const noexcept {
        return storage_ ? storage_->capacity_ : 0;
    }

    void reserve(size_t new_capacity) {
        if (new_capacity <= capacity()) {
            return;
        }
        auto *new_storage_ = create_storage(new_capacity);
        try {
            std::uninitialized_copy(cbegin(), cend(), new_storage_->data_);
        } catch (...) {
            operator delete(new_storage_);
            throw;
        }
        if (!storage_) {
            new_storage_->size_ = 0;
            new_storage_->capacity_ = new_capacity;
            new_storage_->number_of_masters_ = 1;
            storage_ = new_storage_;
            return;
        }
        new_storage_->size_ = storage_->size_;
        new_storage_->capacity_ = new_capacity;
        new_storage_->number_of_masters_ = 1;

        if (storage_->number_of_masters_ == 1) {
            std::destroy(begin(), end());
            operator delete(storage_);
        } else {
            --storage_->number_of_masters_;
        }
        storage_ = new_storage_;
    }

    void resize(size_t new_size) {
        if (new_size == size()) {
            return;
        }
        if (new_size < size()) {
            if (storage_->number_of_masters_ > 1) {
                auto *new_storage_ = create_storage(storage_->capacity_);
                try {
                    std::uninitialized_copy(cbegin(), cbegin() + new_size, new_storage_->data_);
                } catch (...) {
                    operator delete(new_storage_);
                    throw;
                }
                new_storage_->size_ = new_size;
                new_storage_->capacity_ = storage_->capacity_;
                new_storage_->number_of_masters_ = 1;
                --storage_->number_of_masters_;
                storage_ = new_storage_;
            } else {
                std::destroy(begin() + new_size, end());
                storage_->size_ = new_size;
            }
        } else {
            size_t new_capacity = std::max(new_size, capacity());
            if (!storage_ || storage_->number_of_masters_ > 1 || new_capacity > capacity()) {
                auto *new_storage_ = create_storage(new_capacity);
                try {
                    std::uninitialized_copy(cbegin(), cend(), new_storage_->data_);
                } catch (...) {
                    operator delete(new_storage_);
                    throw;
                }
                try {
                    std::fill_n(new_storage_->data_ + size(), new_size - size(), T());
                } catch (...) {
                    std::destroy(new_storage_->data_, new_storage_->data_ + size());
                    operator delete(new_storage_);
                    throw;
                }
                new_storage_->size_ = new_size;
                new_storage_->capacity_ = new_capacity;
                new_storage_->number_of_masters_ = 1;
                if (storage_) {
                    if (storage_->number_of_masters_ == 1) {
                        std::destroy(begin(), end());
                        operator delete(storage_);
                    } else {
                        --storage_->number_of_masters_;
                    }
                }
                storage_ = new_storage_;
            } else {
                std::fill_n(begin() + size(), new_size - size(), T());
                storage_->size_ = new_size;
            }
        }
    }

    void resize(size_t new_size, const_reference item) {
        if (new_size == size()) {
            return;
        }
        if (new_size < size()) {
            if (storage_->number_of_masters_ > 1) {
                auto *new_storage_ = create_storage(storage_->capacity_);
                try {
                    std::uninitialized_copy(cbegin(), cbegin() + new_size, new_storage_->data_);
                } catch (...) {
                    operator delete(new_storage_);
                    throw;
                }
                new_storage_->size_ = new_size;
                new_storage_->capacity_ = storage_->capacity_;
                new_storage_->number_of_masters_ = 1;
                --storage_->number_of_masters_;
                storage_ = new_storage_;
            } else {
                std::destroy(begin() + new_size, end());
                storage_->size_ = new_size;
            }
        } else {
            size_t new_capacity = std::max(new_size, capacity());
            if (!storage_ || storage_->number_of_masters_ > 1 || new_capacity > capacity()) {
                auto *new_storage_ = create_storage(new_capacity);
                try {
                    std::uninitialized_copy(cbegin(), cend(), new_storage_->data_);
                } catch (...) {
                    operator delete(new_storage_);
                    throw;
                }
                try {
                    std::fill_n(new_storage_->data_ + size(), new_size - size(), item);
                } catch (...) {
                    std::destroy(new_storage_->data_, new_storage_->data_ + size());
                    operator delete(new_storage_);
                    throw;
                }
                new_storage_->size_ = new_size;
                new_storage_->capacity_ = new_capacity;
                new_storage_->number_of_masters_ = 1;
                if (storage_) {
                    if (storage_->number_of_masters_ == 1) {
                        std::destroy(begin(), end());
                        operator delete(storage_);
                    } else {
                        --storage_->number_of_masters_;
                    }
                }
                storage_ = new_storage_;
            } else {
                std::fill_n(begin() + size(), new_size - size(), item);
                storage_->size_ = new_size;
            }
        }
    }

    void shrink_to_fit() {
        if (size() == capacity()) {
            return;
        }
        if (size() == 0) {
            if (storage_->number_of_masters_ == 1) {
                std::destroy(begin(), end());
                operator delete(storage_);
            } else {
                --storage_->number_of_masters_;
                storage_ = nullptr;
            }
            return;
        }
        auto *new_storage_ = create_storage(storage_->size_);
        try {
            std::uninitialized_copy(cbegin(), cend(), new_storage_->data_);
        } catch (...) {
            operator delete(new_storage_);
            throw;
        }
        new_storage_->size_ = storage_->size_;
        new_storage_->capacity_ = storage_->size_;
        new_storage_->number_of_masters_ = 1;

        if (storage_->number_of_masters_ == 1) {
            std::destroy(begin(), end());
            operator delete(storage_);
        } else {
            --storage_->number_of_masters_;
        }
        storage_ = new_storage_;
    }

    void clear() {
        resize(0);
    }

    void push_back(const_reference item) {
        if (size() == capacity() || storage_->number_of_masters_ > 1) {
            size_t new_capacity = (size() == capacity() ? (capacity() ? capacity() * 2 : 8) : capacity());
            auto *new_storage_ = create_storage(new_capacity);
            try {
                std::uninitialized_copy(cbegin(), cend(), new_storage_->data_);
            } catch (...) {
                operator delete(new_storage_);
                throw;
            }
            try {
                new(new_storage_->data_ + size()) T(item);
            } catch (...) {
                std::destroy(new_storage_->data_, new_storage_->data_ + size());
                operator delete(new_storage_);
                throw;
            }

            if (!storage_) {
                new_storage_->size_ = 1;
                new_storage_->capacity_ = new_capacity;
                new_storage_->number_of_masters_ = 1;
                storage_ = new_storage_;
                return;
            }
            new_storage_->size_ = storage_->size_ + 1;
            new_storage_->capacity_ = new_capacity;
            new_storage_->number_of_masters_ = 1;

            if (storage_->number_of_masters_ == 1) {
                std::destroy(begin(), end());
                operator delete(storage_);
            } else {
                --storage_->number_of_masters_;
            }
            storage_ = new_storage_;
        } else {
            try {
                new(storage_->data_ + storage_->size_) T(item);
                ++storage_->size_;
            } catch (...) {
                throw;
            }
        }
    }

    void pop_back() {
        broot_copy();
        storage_->data_[--storage_->size_].~T();
    }

    iterator insert(const_iterator pos, const_reference item) {
        size_t index = pos - cbegin();
        if (pos == cend()) {
            push_back(item);
            return begin() + index;
        }
        if (size() == capacity() || storage_->number_of_masters_ > 1) {
            size_t new_capacity = (size() == capacity() ? (capacity() ? capacity() * 2 : 8) : capacity());

            auto *new_storage_ = create_storage(new_capacity);

            try {
                std::uninitialized_copy(cbegin(), pos, new_storage_->data_);
            } catch (...) {
                operator delete(new_storage_);
                throw;
            }
            try {
                new(new_storage_->data_ + index) T(item);
            } catch (...) {
                std::destroy(new_storage_->data_, new_storage_->data_ + index);
                operator delete(new_storage_);
                throw;
            }
            try {
                std::uninitialized_copy(pos, cend(), new_storage_->data_ + index + 1);
            } catch (...) {
                std::destroy(new_storage_->data_, new_storage_->data_ + index + 1);
                operator delete(new_storage_);
                throw;
            }

            new_storage_->size_ = storage_->size_ + 1;
            new_storage_->capacity_ = new_capacity;
            new_storage_->number_of_masters_ = 1;

            if (storage_->number_of_masters_ == 1) {
                std::destroy(begin(), end());
                operator delete(storage_);
            } else {
                --storage_->number_of_masters_;
            }
            storage_ = new_storage_;
            return begin() + index;
        }
        std::uninitialized_copy(&item, &item + 1, end());
        ++storage_->size_;
        std::rotate(begin() + index, end() - 1, end());
//        for (iterator r = end() - 1; r > begin() + index; r--) {
//            std::iter_swap(r, r - 1);
//        }
        return begin() + index;
    }

    iterator erase(const_iterator pos) {
        return erase(pos, pos + 1);
    }

    iterator erase(const_iterator first, const_iterator last) {
        size_t indexl = first - cbegin();
        size_t indexr = last - cbegin();
        if (first == last) {
            return begin() + indexl;
        }
        if (last == cend()) {
            broot_copy();
            std::destroy(begin() + indexl, end());
            storage_->size_ -= (indexr - indexl);
            return begin() + indexl;
        }

        if (storage_->number_of_masters_ > 1) {
            auto *new_storage_ = create_storage(storage_->capacity_);

            try {
                std::uninitialized_copy(cbegin(), first, new_storage_->data_);
            } catch (...) {
                operator delete(new_storage_);
                throw;
            }
            try {
                std::uninitialized_copy(last, cend(), new_storage_->data_ + indexl);
            } catch (...) {
                std::destroy(new_storage_->data_, new_storage_->data_ + indexl);
                operator delete(new_storage_);
                throw;
            }

            new_storage_->size_ = storage_->size_ - (indexr - indexl);
            new_storage_->capacity_ = storage_->capacity_;
            new_storage_->number_of_masters_ = 1;

            if (storage_->number_of_masters_ == 1) {
                std::destroy(begin(), end());
                operator delete(storage_);
            } else {
                --storage_->number_of_masters_;
            }
            storage_ = new_storage_;
            return begin() + indexl;
        }
        std::move(begin() + indexr, end(), begin() + indexl);
//        for (iterator l = begin() + indexl, r = begin() + indexr; r < end(); l++, r++) {
//            *l = *r;
//        }
        std::destroy(begin() + size() - (indexr - indexl), end());
        storage_->size_ -= (indexr - indexl);
        return begin() + indexl;
    }

    friend bool operator==(base_vector const &lhs, base_vector const &rhs) {
        if (lhs.storage_ == rhs.storage_) {
            return true;
        }
        return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
    }

    friend bool operator!=(base_vector const &lhs, base_vector const &rhs) {
        return !(lhs == rhs);
    }

    friend bool operator<(base_vector const &lhs, base_vector const &rhs) {
        if (lhs.storage_ == rhs.storage_) {
            return false;
        }
        return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    friend bool operator>(base_vector const &lhs, base_vector const &rhs) {
        return rhs < lhs;
    }

    friend bool operator<=(base_vector const &lhs, base_vector const &rhs) {
        return !(lhs > rhs);
    }

    friend bool operator>=(base_vector const &lhs, base_vector const &rhs) {
        return !(lhs < rhs);
    }

    friend void swap(base_vector &lhs, base_vector &rhs) noexcept {
        std::swap(lhs.storage_, rhs.storage_);
    }

    ~base_vector() {
        if (storage_) {
            if (storage_->number_of_masters_ == 1) {
                std::destroy(begin(), end());
                operator delete(storage_);
            } else {
                --storage_->number_of_masters_;
            }
        }
    }
};

template<typename T>
class vector {
    std::variant<std::monostate, T, base_vector<T>> data_;

public:
    typedef T value_type;

    typedef T *iterator;

    typedef T const *const_iterator;

    typedef std::reverse_iterator<iterator> reverse_iterator;

    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    typedef T &reference;

    typedef T const &const_reference;

    typedef T *pointer;

    typedef T const *const_pointer;

    vector() noexcept = default;

    vector(vector const &rhs) = default;

    template<typename InputIterator>
    vector(InputIterator first, InputIterator last) {
        if (last - first == 1) {
            data_ = T(*first);
        } else if (last - first > 1) {
            data_ = base_vector(first, last);
        }
    }

    vector &operator=(vector const &rhs) = default;

    template<typename InputIterator>
    void assign(InputIterator first, InputIterator last) {
        *this = vector(first, last);
    }

    pointer data() {
        if (data_.index() == 0) {
            return nullptr;
        } else if (data_.index() == 1) {
            return &std::get<1>(data_);
        }
        return std::get<2>(data_).data();
    }

    const_pointer data() const noexcept {
        if (data_.index() == 0) {
            return nullptr;
        } else if (data_.index() == 1) {
            return &std::get<1>(data_);
        }
        return std::get<2>(data_).data();
    }

    iterator begin() {
        return data();
    }

    iterator end() {
        return begin() + size();
    }

    const_iterator begin() const noexcept {
        return data();
    }

    const_iterator end() const noexcept {
        return begin() + size();
    }

    const_iterator cbegin() const noexcept {
        return data();
    }

    const_iterator cend() const noexcept {
        return begin() + size();
    }

    reverse_iterator rbegin() {
        return std::make_reverse_iterator(end());
    }

    reverse_iterator rend() {
        return std::make_reverse_iterator(begin());
    }

    const_reverse_iterator rbegin() const noexcept {
        return std::make_reverse_iterator(end());
    }

    const_reverse_iterator rend() const noexcept {
        return std::make_reverse_iterator(begin());
    }

    const_reverse_iterator crbegin() const noexcept {
        return std::make_reverse_iterator(end());
    }

    const_reverse_iterator crend() const noexcept {
        return std::make_reverse_iterator(begin());
    }

    reference operator[](size_t i) {
        if (data_.index() == 1) { return std::get<1>(data_); }
        return std::get<2>(data_)[i];
    }

    const_reference operator[](size_t i) const noexcept {
        if (data_.index() == 1) { return std::get<1>(data_); }
        return std::get<2>(data_)[i];
    }

    reference front() {
        if (data_.index() == 1) { return std::get<1>(data_); }
        return std::get<2>(data_).front();
    }

    const_reference front() const noexcept {
        if (data_.index() == 1) { return std::get<1>(data_); }
        return std::get<2>(data_).front();
    }

    reference back() {
        if (data_.index() == 1) { return std::get<1>(data_); }
        return std::get<2>(data_).back();
    }

    const_reference back() const noexcept {
        if (data_.index() == 1) { return std::get<1>(data_); }
        return std::get<2>(data_).back();
    }

    bool empty() const noexcept {
        return size() == 0;
    }

    size_t size() const noexcept {
        if (data_.index() == 0) {
            return 0;
        } else if (data_.index() == 1) {
            return 1;
        }
        return std::get<2>(data_).size();
    }

    size_t capacity() const noexcept {
        if (data_.index() == 0) {
            return 0;
        } else if (data_.index() == 1) {
            return 1;
        }
        std::get<2>(data_).capacity();
    }

    void reserve(size_t new_capacity) {
        base_vector<T> tmp_vec;
        if (data_.index() == 0) {
            if (new_capacity == 0) {
                return;
            }
        } else if (data_.index() == 1) {
            if (new_capacity < 2) {
                return;
            }
            tmp_vec.push_back(std::get<1>(data_));
        } else {
            std::get<2>(data_).reserve(new_capacity);
            return;
        }
        tmp_vec.reserve(new_capacity);
        data_ = tmp_vec;
    }

    void resize(size_t new_size) {
        base_vector<T> tmp_vec;
        if (data_.index() == 0) {
            if (new_size == 0) {
                return;
            }
            if (new_size == 1) {
                data_ = T();
                return;
            }
        } else if (data_.index() == 1) {
            if (new_size == 0) {
                data_ = std::monostate();
                return;
            }
            if (new_size == 1) {
                return;
            }
            tmp_vec.push_back(std::get<1>(data_));
        } else {
            std::get<2>(data_).resize(new_size);
            return;
        }
        tmp_vec.resize(new_size);
        data_ = tmp_vec;
    }

    void resize(size_t new_size, const_reference item) {
        base_vector<T> tmp_vec;
        if (data_.index() == 0) {
            if (new_size == 0) {
                return;
            }
            if (new_size == 1) {
                data_ = T(item);
                return;
            }
        } else if (data_.index() == 1) {
            if (new_size == 0) {
                data_ = std::monostate();
                return;
            }
            if (new_size == 1) {
                return;
            }
            tmp_vec.push_back(std::get<1>(data_));
        } else {
            std::get<2>(data_).resize(new_size, item);
            return;
        }
        tmp_vec.resize(new_size, item);
        data_ = tmp_vec;
    }

    void shrink_to_fit() {
        if (data_.index() == 2) {
            std::get<2>(data_).shrink_to_fit();
        }
    }

    void clear() {
        if (data_.index() == 2) {
            std::get<2>(data_).clear();
        } else {
            data_ = std::monostate();
        }
    }

    void push_back(const_reference item) {
        if (data_.index() == 0) {
            data_ = T(item);
        } else if (data_.index() == 1) {
            base_vector<T> tmp_vec;
            tmp_vec.push_back(std::get<1>(data_));
            tmp_vec.push_back(item);
            data_ = tmp_vec;
        } else {
            std::get<2>(data_).push_back(item);
        }
    }

    void pop_back() {
        if (data_.index() == 1) {
            data_ = std::monostate();
        } else {
            std::get<2>(data_).pop_back();
        }
    }

    iterator insert(const_iterator pos, const_reference val) {
        base_vector<T> tmp_vec;
        size_t index = pos - cbegin();
        if (data_.index() == 0) {
            data_ = T(val);
            return begin();
        } else if (data_.index() == 1) {
            tmp_vec.push_back(std::get<1>(data_));
            tmp_vec.insert(tmp_vec.cbegin() + index, val);
            data_ = tmp_vec;
            return begin() + index;
        }
        return std::get<2>(data_).insert(begin() + index, val);
    }

    iterator erase(const_iterator pos) {
        return erase(pos, pos + 1);
    }

    iterator erase(const_iterator first, const_iterator last) {
        if (last == first) {
            return begin() + (first - cbegin());
        }
        if (data_.index() == 1) {
            data_ = std::monostate();
            return begin();
        }
        return std::get<2>(data_).erase(first, last);
    }

    friend bool operator==(vector const &lhs, vector const &rhs) {
        if (lhs.data_.index() == 2 && rhs.data_.index() == 2) {
            return std::get<2>(lhs.data_) == std::get<2>(rhs.data_);
        }
        return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
    }

    friend bool operator!=(vector const &lhs, vector const &rhs) {
        return !(lhs == rhs);
    }

    friend bool operator<(vector const &lhs, vector const &rhs) {
        return std::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
    }

    friend bool operator>(vector const &lhs, vector const &rhs) {
        return rhs < lhs;
    }

    friend bool operator<=(vector const &lhs, vector const &rhs) {
        return !(lhs > rhs);
    }

    friend bool operator>=(vector const &lhs, vector const &rhs) {
        return !(lhs < rhs);
    }

    friend void swap(vector &lhs, vector &rhs) {
        std::swap(lhs.data_, rhs.data_);
    }
};

#endif //VECTOR_VECTOR_H
