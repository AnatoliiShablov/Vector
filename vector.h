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
    struct data_with_info {
        size_t size_;
        size_t capacity_;
        size_t masters_;
        T data_[];
    };

    data_with_info *data_;

 public:
    typedef T value_type;

    typedef T *iterator;

    typedef T const *const_iterator;

    typedef std::reverse_iterator<iterator> reverse_iterator;

    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    base_vector() noexcept : data_(nullptr) {}

    base_vector(base_vector const &rhs) noexcept : data_(rhs.data_) {
        if (data_) {
            ++data_->masters_;
        }
    }

    template<typename InputIterator>
    base_vector(InputIterator first, InputIterator last) {
        size_t sz = std::distance(first, last);
        if (sz == 0) {
            data_(nullptr);
        } else {
            data_ = static_cast<data_with_info *>(operator new(sizeof(data_with_info) + sizeof(T) * sz));
            data_->size_ = sz;
            data_->capacity_ = sz;
            data_->masters_ = 1;
            std::uninitialized_copy(first, last, data_->data_);
        }
    }

    base_vector &operator=(base_vector const &rhs) noexcept {
        if (data_ == rhs.data_) {
            return *this;
        }
        if (data_) {
            --data_->masters_;
            if (!data_->masters_) {
                for (size_t i = 0; i < data_->size_; ++i) {
                    data_->data_[i].~T();
                }
                operator delete(data_);
            }
        }
        data_ = rhs.data_;
        if (data_) {
            ++data_->masters_;
        }
        return *this;
    }

    template<typename InputIterator>
    void assign(InputIterator first, InputIterator last) {
        size_t sz = std::distance(first, last);
        if (data_ && data_->masters_ == 1 && data_->capacity_ >= sz) {
            for (size_t i = 0; i < data_->size_; ++i) {
                data_->data_[i].~T();
            }
            std::uninitialized_copy(first, last, data_->data_);
            data_->size_ = sz;
            return;
        }
        data_with_info *new_data = nullptr;
        if (sz) {
            new_data = static_cast<data_with_info *>(operator new(sizeof(data_with_info) + sz * sizeof(T)));
            std::uninitialized_copy(first, last, new_data->data_);
            new_data->size_ = sz;
            new_data->capacity_ = sz;
            new_data->masters_ = 1;
        }
        auto old_data = data_;
        data_ = new_data;
        if (old_data) {
            --old_data->masters_;
            if (!old_data->masters_) {
                for (size_t i = 0; i < old_data->size_; ++i) {
                    old_data->data_[i].~T();
                }
                operator delete(old_data);
            }
        }
    }

    T const &operator[](size_t i) const {
        return data_->data_[i];
    }

    T &operator[](size_t i) {
        if (data_->masters_ != 1) {
            assign(cbegin(), cend());
        }
        return data_->data_[i];
    }

    T const &front() const {
        return operator[](0);
    }

    T const &back() const {
        return operator[](data_->size_ - 1);
    }

    T &front() {
        return operator[](0);
    }

    T &back() {
        return operator[](data_->size_ - 1);
    }

    void push_back(T const &item) {

        if (!data_) {
            data_ = static_cast<data_with_info *>(operator new(sizeof(data_with_info) + 8 * sizeof(T)));
            data_->size_ = 0;
            data_->capacity_ = 8;
            data_->masters_ = 1;
        }
        if (data_->size_ == data_->capacity_) {
            auto new_data = static_cast<data_with_info *>(operator new(sizeof(data_with_info) + capacity() * 2 * sizeof(T)));
            std::uninitialized_copy(begin(), end(), new_data->data_);
            new_data->size_ = data_->size_;
            new_data->capacity_ = capacity() * 2;
            new_data->masters_ = 1;
            auto old_data = data_;
            data_ = new_data;

            new(&data_->data_[data_->size_++]) T(item);

            --old_data->masters_;
            if (!old_data->masters_) {
                for (size_t i = 0; i < old_data->size_; ++i) {
                    old_data->data_[i].~T();
                }
                operator delete(old_data);
            }
            return;
        }
        if (data_->masters_ != 1) {
            assign(cbegin(), cend());
        }
        new(&data_->data_[data_->size_++]) T(item);
    }

    void pop_back() {
        if (data_->masters_ != 1) {
            assign(cbegin(), cend() - 1);
        } else {
            data_->data_[--data_->size_].~T();
        }
    }

    T const *data() const noexcept {
        if (!data_) {
            return nullptr;
        }
        return data_->data_;
    }

    T *data() {
        if (!data_) {
            return nullptr;
        }
        if (data_->masters_ != 1) {
            assign(cbegin(), cend());
        }
        return data_->data_;
    }

    iterator begin() {
        if (!data_) {
            return nullptr;
        }
        if (data_->masters_ != 1) {
            assign(cbegin(), cend());
        }
        return data_->data_;
    }

    iterator end() {
        if (!data_) {
            return nullptr;
        }
        if (data_->masters_ != 1) {
            assign(cbegin(), cend());
        }
        return data_->data_ + data_->size_;
    }

    const_iterator begin() const noexcept {
        return data_ ? data_->data_ : nullptr;
    }

    const_iterator end() const noexcept {
        return data_ ? (data_->data_ + data_->size_) : nullptr;
    }

    const_iterator cbegin() const noexcept {
        return data_ ? data_->data_ : nullptr;
    }

    const_iterator cend() const noexcept {
        return data_ ? (data_->data_ + data_->size_) : nullptr;
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

    bool empty() const noexcept {
        return data_ ? (data_->size_ == 0) : true;
    }

    size_t size() const noexcept {
        return data_ ? data_->size_ : 0;
    }

    size_t capacity() const noexcept {
        return data_ ? data_->capacity_ : 0;
    }

    void resize(size_t new_size) {
        if (size() == new_size) {
            return;
        }
        if (!data_) {
            if (new_size) {
                data_ = operator new(sizeof(data_with_info) + new_size * (sizeof(T)));
                for (size_t i = 0; i < new_size; i++) {
                    data_->data_[i] = T();
                }
            }
            return;
        }

        if (data_->capacity_ < new_size) {
            reserve(new_size);
        }
        if (data_->masters_ != 1) {
            if (new_size < size()) {
                assign(cbegin(), cbegin() + new_size);
            } else {
                assign(cbegin(), cend());
            }
        }
        while (size() > new_size) {
            pop_back();
        }
        while (size() < new_size) {
            push_back(T());
        }
    }

    void resize(size_t new_size, T const &item) {
        if (size() == new_size) {
            return;
        }

        if (data_->capacity_ < new_size) {
            reserve(new_size);
        }
        if (data_->masters_ != 1) {
            if (new_size < size()) {
                assign(cbegin(), cbegin() + new_size);
            } else {
                assign(cbegin(), cend());
            }
        }
        while (size() > new_size) {
            pop_back();
        }
        while (size() < new_size) {
            push_back(item);
        }
    }

    void reserve(size_t new_capacity) {
        if (new_capacity <= capacity()) {
            return;
        }
        if (!data_) {
            data_ = static_cast<data_with_info *>(operator new(sizeof(data_with_info) + new_capacity * (sizeof(T))));
            data_->size_ = 0;
            data_->capacity_ = new_capacity;
            data_->masters_ = 1;
            return;
        }
        auto new_data = static_cast<data_with_info *>(operator new(sizeof(data_with_info) + new_capacity * sizeof(T)));
        std::uninitialized_copy(begin(), end(), new_data->data_);
        new_data->size_ = data_->size_;
        new_data->capacity_ = new_capacity;
        new_data->masters_ = 1;
        auto old_data = data_;
        data_ = new_data;

        --old_data->masters_;
        if (!old_data->masters_) {
            for (size_t i = 0; i < old_data->size_; ++i) {
                old_data->data_[i].~T();
            }
            operator delete(old_data);
        }
    }

    void shrink_to_fit() {
        if (capacity() == size()) {
            return;
        }
        if (data_->masters_ != 1) {
            assign(cbegin(), cend());
            return;
        }
        data_with_info *new_data = nullptr;

        if (size()) {
            new_data = static_cast<data_with_info *>(operator new(sizeof(data_with_info) + size() * sizeof(T)));
            std::uninitialized_copy(begin(), end(), new_data->data_);
            new_data->size_ = size();
            new_data->capacity_ = size();
            new_data->masters_ = 1;
        }
        auto old_data = data_;
        data_ = new_data;
        for (size_t i = 0; i < data_->size_; ++i) {
            old_data->data_[i].~T();
        }
        operator delete(old_data);
    }

    void clear() {
        resize(0);
    }

    iterator insert(const_iterator pos, T const &val) {
        size_t index = std::distance(const_cast<const_iterator>(begin()), pos);
        if (index == size()) {
            push_back(val);
        } else {
            push_back(back());
            for (auto i = size() - 2; i != index; --i) {
                data_->data_[i] = data_->data_[i - 1];
            }
            data_->data_[index] = val;
        }
        return begin() + index;
    }

    iterator erase(const_iterator pos) {
        return erase(pos, pos + 1);
    }

    iterator erase(const_iterator first, const_iterator last) {
        if (data_->masters_ != 1) {
            assign(cbegin(), cend());
        }
        size_t indexl = std::distance(const_cast<const_iterator>(begin()), first);
        size_t indexr = std::distance(const_cast<const_iterator>(begin()), last);
        size_t dist = indexr - indexl;
        for (size_t i = indexl; i < size() - dist; i++) {
            data_->data_[i] = data_->data_[i + dist];
        }
        for (size_t i = 0; i < dist; i++) {
            pop_back();
        }
        return begin() + indexl;
    }

    friend bool operator==(base_vector const &lhs, base_vector const &rhs) {
        if (lhs.data_ == rhs.data_) {
            return true;
        }
        if (lhs.size() != rhs.size()) {
            return false;
        }
        auto ld = lhs.data();
        auto rd = rhs.data();
        for (size_t i = 0; i < lhs.size(); i++) {
            if (ld[i] != rd[i]) {
                return false;
            }
        }
        return true;
    }

    ~base_vector() {
        if (data_) {
            --data_->masters_;
            if (!data_->masters_) {
                for (size_t i = 0; i < size(); i++) {
                    data_->data_[i].~T();
                }
                operator delete(data_);
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

    template<typename InputIterator>
    vector(InputIterator first, InputIterator last) {
        if (last - first == 1) {
            data_ = T(*first);
        } else if (last - first > 1) {
            data_ = base_vector(first, last);
        }
    }

    template<typename InputIterator>
    void assign(InputIterator first, InputIterator last) {
        if (first == last) {
            if (data_.index() == 2) {
                std::get<2>(data_).clear();
            } else {
                data_ = std::monostate();
            }
        } else if (first - last == 1) {
            if (data_.index() == 2) {
                std::get<2>(data_).assign(first, last);
            } else {
                data_ = T(*first);
            }
        } else {
            if (data_.index() == 2) {
                std::get<2>(data_).assign(first, last);
            } else {
                data_ = base_vector(first, last);
            }
        }
    }

    vector() = default;

    vector(vector const &rhs) = default;

    T const &operator[](size_t i) const {
        if (data_.index() == 1) { return std::get<1>(data_); }
        return std::get<2>(data_)[i];
    }

    T &operator[](size_t i) {
        if (data_.index() == 1) { return std::get<1>(data_); }
        return std::get<2>(data_)[i];
    }

    T const &front() const {
        if (data_.index() == 1) { return std::get<1>(data_); }
        return std::get<2>(data_).front();
    }

    T const &back() const {
        if (data_.index() == 1) { return std::get<1>(data_); }
        return std::get<2>(data_).back();
    }

    T &front() {
        if (data_.index() == 1) { return std::get<1>(data_); }
        return std::get<2>(data_).front();
    }

    T &back() {
        if (data_.index() == 1) { return std::get<1>(data_); }
        return std::get<2>(data_).back();
    }

    void push_back(T const &item) {
        if (data_.index() == 0) {
            data_ = T(item);
        } else if (data_.index() == 1) {
            T tmp = std::get<1>(data_);
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

    T const *data() const noexcept {
        if (data_.index() == 0) {
            return nullptr;
        } else if (data_.index() == 1) {
            return &std::get<1>(data_);
        }
        return std::get<2>(data_).data();
    }

    T *data() {
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

    void resize(size_t new_size) {
        if (data_.index() == 0) {
            if (new_size == 0) {
                return;
            }
            if (new_size == 1) {
                data_ = T();
                return;
            }
            data_ = base_vector<T>();
        } else if (data_.index() == 1) {
            if (new_size == 0) {
                data_ = std::monostate();
                return;
            }
            if (new_size == 1) {
                return;
            }
            base_vector<T> tmp_vec;
            tmp_vec.push_back(std::get<1>(data_));
            data_ = tmp_vec;
        }
        std::get<2>(data_).resize(new_size);
    }

    void resize(size_t new_size, T const &item) {
        if (data_.index() == 0) {
            if (new_size == 0) {
                return;
            }
            if (new_size == 1) {
                data_ = T(item);
                return;
            }
            data_ = base_vector<T>();
        } else if (data_.index() == 1) {
            if (new_size == 0) {
                data_ = std::monostate();
                return;
            }
            if (new_size == 1) {
                return;
            }
            base_vector<T> tmp_vec;
            tmp_vec.push_back(std::get<1>(data_));
            data_ = tmp_vec;
        }
        std::get<2>(data_).resize(new_size, item);
    }

    void reserve(size_t new_capacity) {
        if (data_.index() == 0) {
            if (new_capacity == 0) {
                return;
            }
            data_ = base_vector<T>();
        } else if (data_.index() == 1) {
            if (new_capacity > 1) {
                base_vector<T> tmp_vec;
                tmp_vec.push_back(std::get<1>(data_));
                data_ = tmp_vec;
            } else {
                return;
            }
        }
        std::get<2>(data_).reserve(new_capacity);
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

    iterator insert(const_iterator pos, T const &val) {
        size_t index = pos - begin();
        if (data_.index() == 0) {
            data_ = T(val);
            return begin();
        } else if (data_.index() == 1) {
            if (&val == &std::get<1>(data_)) {
                T tmp_val = val;
                data_ = base_vector<T>();
                std::get<2>(data_).push_back(tmp_val);
                return std::get<2>(data_).insert(begin() + index, tmp_val);
            }
            T tmp_val = std::get<1>(data_);
            data_ = base_vector<T>();
            std::get<2>(data_).push_back(tmp_val);
            return std::get<2>(data_).insert(begin() + index, val);
        }
        if (&val >= pos && &val < end()) {
            T tmp_val = val;
            return std::get<2>(data_).insert(begin() + index, tmp_val);
        }
        return std::get<2>(data_).insert(begin() + index, val);
    }

    iterator erase(const_iterator pos) {
        return erase(pos, pos + 1);
    }

    iterator erase(const_iterator first, const_iterator last) {
        if (last == first) {
            return begin() + std::distance(const_cast<const_iterator>(begin()), first);
        }
        if (data_.index() == 0) {
            return begin();
        } else if (data_.index() == 1) {
            data_ = std::monostate();
            return begin();
        }
        return std::get<2>(data_).erase(first, last);
    }

    friend bool operator==(vector const &lhs, vector const &rhs) {
        if (lhs.data_.index() == 2 && rhs.data_.index() == 2) {
            return std::get<2>(lhs.data_) == std::get<2>(rhs.data_);
        }
        if (lhs.size() != rhs.size()) {
            return false;
        }
        auto ld = lhs.data();
        auto rd = rhs.data();
        for (size_t i = 0; i < lhs.size(); i++) {
            if (ld[i] != rd[i]) {
                return false;
            }
        }
        return true;
    }

    friend bool operator!=(vector const &lhs, vector const &rhs) {
        return !(lhs == rhs);
    }

    friend bool operator<(vector const &lhs, vector const &rhs) {
        return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
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
