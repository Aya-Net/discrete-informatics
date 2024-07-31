//
// Created by 郭修屹 on 2024/7/31.
//

#ifndef THEOREM_BIT_VECTOR_H
#define THEOREM_BIT_VECTOR_H

#include <ostream>
#include <iostream>

#include "../util/type.h"
#include "../util/constant.h"

class bit_vector {
  private:
    byte *data_;

    size_t size_, block_num_, rank_;

  public:
    class reference {
      private:
        bit_vector &bv_;
        int i;
      public:
        reference() = delete;

        reference(bit_vector &bv, int i) : bv_(bv), i(i) {}

        reference &operator=(int value) {
            bv_.set(i, value);
            return *this;
        }

        operator int() {
            return bv_.get(i);
        }
    };

    bit_vector() = delete;

    explicit bit_vector(size_t size) {
        size_ = size;
        block_num_ = (size + BYTE_SIZE - 1) / BYTE_SIZE;
        rank_ = 0;
        data_ = new byte[block_num_];
    }

    explicit bit_vector(const std::string &s) {
        size_ = s.length();
        block_num_ = (s.length() + BYTE_SIZE - 1) / BYTE_SIZE;
        data_ = new byte[block_num_];
        rank_ = std::count(s.begin(), s.end(), '1');
        for (int i = 0; i < s.length(); i++) {
            set(i, s[i] - '0');
        }
    }

    bit_vector(const bit_vector &bv) {
        size_ = bv.size_;
        block_num_ = bv.block_num_;
        data_ = new byte[size_];
        rank_ = bv.rank_;
        std::copy(bv.data_, bv.data_ + block_num_, data_);
    }

    ~bit_vector() {
        delete[] data_;
    }

    void set(int i, int value) {
        if (value) {
            data_[i / BYTE_SIZE] |= 1 << (i % BYTE_SIZE);
        } else {
            data_[i / BYTE_SIZE] &= ~(1 << (i % BYTE_SIZE));
        }
    }

    [[nodiscard]] byte get_block(int i) const {
        return data_[i];
    }

    void set_block(int i, byte value) {
        data_[i] = value;
    }

    [[nodiscard]] bool get(int i) const {
        return data_[i / BYTE_SIZE] & (1 << (i % BYTE_SIZE));
    }

    [[nodiscard]] int interval_to_int(int l, int r) const {
        int pos = std::min((int) size_, r) - 1;
        int res = 0;
        while (pos >= l) {
            if ((pos & (BYTE_SIZE - 1)) == (BYTE_SIZE - 1) && pos - l >= (BYTE_SIZE - 1)) {
                res = (res << BYTE_SIZE) | data_[pos / BYTE_SIZE];
                pos -= BYTE_SIZE;
            } else {
                res = (res << 1) | get(pos);
                pos--;
            }
        }
        return res;
    }

    void int_to_interval(int l, int r, int value) {
        int pos = l;
        while (pos < r) {
            if ((pos & (BYTE_SIZE - 1)) == 0 && pos + (BYTE_SIZE - 1) < r) {
                data_[pos / BYTE_SIZE] = value & ((1 << BYTE_SIZE) - 1);
                value >>= BYTE_SIZE;
                pos += BYTE_SIZE;
            } else {
                set(pos, value & 1);
                value >>= 1;
                pos++;
            }
        }
    }

    [[nodiscard]] size_t size() const {
        return size_;
    }

    [[nodiscard]] size_t rank() const {
        return rank_;
    }

    bool operator[](int i) const {
        return get(i);
    }

    reference operator[](int i) {
        return {*this, i};
    }

    bit_vector &operator=(const bit_vector &bv) {
        if (this == &bv) {
            return *this;
        }
        size_ = bv.size_;
        block_num_ = bv.block_num_;
        delete[] data_;
        data_ = new byte[block_num_];
        std::copy(bv.data_, bv.data_ + block_num_, data_);
        return *this;
    }

    friend std::ostream &operator<<(std::ostream &os, const bit_vector &bv) {
        for (int i = 0; i < bv.size_; i++) {
            os << bv.get(i);
        }
        return os;
    }
};

#endif //THEOREM_BIT_VECTOR_H
