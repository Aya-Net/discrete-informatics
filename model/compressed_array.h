//
// Created by 郭修屹 on 2024/7/31.
//

#ifndef THEOREM_COMPRESSED_ARRAY_H
#define THEOREM_COMPRESSED_ARRAY_H

#include <iostream>

#include "bit_vector.h"

class compressed_array {
  private:
    bit_vector data_;

    int width_;

    size_t size_;

  public:
    class reference {
      private:
        compressed_array &arr_;

        int i_;

      public:
        reference() = delete;

        explicit reference(compressed_array &arr, int i) : arr_(arr), i_(i) {}

        reference &operator=(int value) {
            arr_.set(i_, value);
            return *this;
        }

        reference &operator+=(int value) {
            arr_.set(i_, arr_.get(i_) + value);
            return *this;
        }

        reference &operator-=(int value) {
            arr_.set(i_, arr_.get(i_) - value);
            return *this;
        }

        operator int() {
            return arr_.get(i_);
        }
    };

    compressed_array() = delete;

    explicit compressed_array(size_t size, int width) : size_(size), width_(width), data_(size * width) {}

    void set(int i, int value) {
        data_.int_to_interval(i * width_, i * width_ + width_, value);
    }

    [[nodiscard]] int get(int i) const {
        return data_.interval_to_int(i * width_, i * width_ + width_);
    }

    [[nodiscard]] size_t k() const {
        return width_;
    }

    int operator[](int i) const {
        return get(i);
    }

    reference operator[](int i) {
        return reference(*this, i);
    }

    compressed_array &operator=(const compressed_array &arr) {
        if (this == &arr) {
            return *this;
        }
        size_ = arr.size_;
        width_ = arr.width_;
        data_ = arr.data_;
        return *this;
    }

    friend std::ostream &operator<<(std::ostream &os, const compressed_array &arr) {
        for (int i = 0; i < arr.size_; i++) {
            os << arr[i] << " ";
        }
        return os;
    }
};

#endif //THEOREM_COMPRESSED_ARRAY_H
