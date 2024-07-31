//
// Created by 郭修屹 on 2024/7/31.
//

#ifndef THEOREM_SUFFICIENT_BV_H
#define THEOREM_SUFFICIENT_BV_H

#include "bit_vector.h"
#include "../util/util.h"
#include "compressed_array.h"
#include "multibranch_tree.h"

class sufficient_bv {
  private:

    const bit_vector bv_;

    size_t size_, rank_;

    int log_n_, log2_n_, sqrt_log_n_, half_log_n_;

    compressed_array rank_1_;

    compressed_array *rank_2_;

    compressed_array *rank_table_;

    compressed_array *select_table_;

    compressed_array select_;

    multibranch_tree **select_tree_;

    void build_rank_() {
        for (int i = 0; i < size_ / log2_n_ + 1; i++) {
            new(rank_2_ + i) compressed_array((log2_n_ + half_log_n_ - 1) / half_log_n_ + 1,
                                              bit_len(log2_n_));
        }
        for (int i = 0; i < size_; i++) {
            int lb_id = i / log2_n_;
            int sb_id = (i - i / log2_n_ * log2_n_) / half_log_n_;
            rank_1_[lb_id + 1] += bv_[i];
            rank_2_[lb_id][sb_id + 1] += bv_[i];
        }
        for (int i = 0; i < size_ / log2_n_ + 1; i++) {
            if (i > 0) {
                rank_1_[i] += rank_1_[i - 1];
            }
            for (int j = 1; j < (log2_n_ + half_log_n_ - 1) / half_log_n_; j++) {

                rank_2_[i][j] += rank_2_[i][j - 1];
            }
        }
        int item_size = bit_len(half_log_n_ + 1);
        for (int i = 0; i < 1 << half_log_n_; i++) {
            new(rank_table_ + i) compressed_array(half_log_n_, item_size);
            for (int j = 0; j < half_log_n_; j++) {
                rank_table_[i][j] = __builtin_popcount(i & ((1 << (j + 1)) - 1));
            }
        }
    }

    void build_select_() {
        typedef multibranch_tree::node node;
        int block_num = 0, rank = 0, cnt = 0;
        std::queue<node *> q;
        for (int i = 0; i < size_; i++) {
            cnt++;
            if (bv_[i] == 1) {
                rank++;
            }
            if (cnt == half_log_n_ || rank == log2_n_ || i == size_ - 1) {
                int value = bv_.interval_to_int(i - cnt + 1, i + 1);
                q.push(new node(rank_table_[value][half_log_n_ - 1], (int) q.size()));
                cnt = 0;
            }
            if (rank == log2_n_ || i == size_ - 1) {
                select_[block_num] = i;
                select_tree_[block_num++] = new multibranch_tree(sqrt_log_n_, q);
                while (!q.empty()) {
                    q.pop();
                }
                rank = 0;
            }
        }
        int item_size = bit_len(half_log_n_);
        for (int i = 0; i < 1 << half_log_n_; i++) {
            new(select_table_ + i) compressed_array(half_log_n_, item_size);
            int rk = 0;
            for (int j = 0; j <= half_log_n_; j++) {
                if (i & (1 << j)) {
                    select_table_[i][rk++] = j;
                }
            }
        }
    }

  public:
    sufficient_bv() = delete;

    explicit sufficient_bv(const std::string &s) : sufficient_bv(bit_vector(s)) {}

    explicit sufficient_bv(const bit_vector &bv) : size_(bv.size()), bv_(bv), log_n_(bit_len(bv.size())),
                                                   sqrt_log_n_(std::ceil(std::sqrt(log_n_))),
                                                   log2_n_(log_n_ * log_n_),
                                                   half_log_n_((log_n_ + 1) / 2),
                                                   rank_(bv.rank()),
                                                   rank_1_(compressed_array(size_ / log2_n_ + 1,
                                                                            log_n_)),
                                                   rank_2_(std::allocator<compressed_array>().allocate(
                                                           size_ / log2_n_ + 1)),
                                                   rank_table_(std::allocator<compressed_array>().allocate(
                                                           1 << half_log_n_)),
                                                   select_(compressed_array((rank_ + log2_n_) / log2_n_,
                                                                            log_n_)),
                                                   select_table_(std::allocator<compressed_array>().allocate(
                                                           1 << half_log_n_)),
                                                   select_tree_(std::allocator<multibranch_tree *>().allocate(
                                                           (rank_ + log2_n_) / log2_n_)) {
        build_rank_();
        build_select_();
    }

    ~sufficient_bv() {
        delete rank_2_;
        delete rank_table_;
        for (int i = 0; i < (rank_ + log2_n_) / log2_n_; i++) {
            delete select_tree_[i];
        }
        delete select_tree_;
    }

    [[nodiscard]] size_t size() const {
        return size_;
    }

    [[nodiscard]] int operator[](int i) const {
        return bv_[i];
    }


    [[nodiscard]] int rank(int i) const {

        int lb_id = i / log2_n_;
        int sb_id = (i - i / log2_n_ * log2_n_) / half_log_n_;
        return (int) rank_1_[lb_id] + rank_2_[lb_id][sb_id]
               + rank_table_[bv_.interval_to_int(lb_id * log2_n_ + sb_id * half_log_n_, i + 1)]
               [i - lb_id * log2_n_ - sb_id * half_log_n_];
    }

    [[nodiscard]] size_t rank() const {
        return rank_;
    }

    [[nodiscard]] int select(int i) const {
        if (i < 1) {
            return -1;
        }
        int block = (i - 1) / log2_n_;
        int offset = (i - 1) % log2_n_ + 1;
        auto p = select_tree_[block]->select(offset);
        if (p.first < 0) {
            return -1;
        }
        int pre_index = (block == 0 ? 0 : select_[block - 1] + 1) + p.first * half_log_n_;
        return pre_index + select_table_[bv_.interval_to_int(pre_index, pre_index + half_log_n_)][p.second - 1];
    }

    friend std::ostream &operator<<(std::ostream &os, const sufficient_bv &bv) {
        return os << bv.bv_;
    }
};


#endif //THEOREM_SUFFICIENT_BV_H
