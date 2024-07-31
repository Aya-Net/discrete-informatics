//
// Created by 郭修屹 on 2024/7/31.
//

#ifndef THEOREM_MULTI_BRANCH_TREE_H
#define THEOREM_MULTI_BRANCH_TREE_H

#include <queue>

#include "compressed_array.h"
#include "../util/util.h"

class multi_branch_tree {
  public:
    class node {
      private:
        compressed_array rank_;

        compressed_array branch_;

        compressed_array data_;

      public:
        node **child;

        compressed_array lookup_table;

        explicit node(int branch, int rank, node *_child[]) : rank_(1, bit_len(rank)), branch_(1, bit_len(branch)),
                                                              child(new node *[branch]),
                                                              lookup_table(rank, bit_len(branch)),
                                                              data_(0, 0) {
            rank_.set(0, rank);
            branch_.set(0, branch);
            std::copy(_child, _child + branch, child);
            for (int i = 1; i < branch; i++) {
                int pre_rank = child[i - 1]->rank() + child[i]->rank();
                child[i]->rank_ = compressed_array(1, bit_len(pre_rank));
                child[i]->rank_[0] = pre_rank;
            }
        }

        explicit node(int rank, int data) : rank_(1, bit_len(rank)), branch_(0, 0),
                                            child(nullptr), lookup_table(0, 0),
                                            data_(1, bit_len(data)) {
            rank_.set(0, rank);
            data_.set(0, data);
        }

        ~node() {
            delete[] child;
        }

        int branch() {
            return branch_[0];
        }

        int rank() {
            return rank_[0];
        }

        int data() {
            return data_[0];
        }
    };

  private:
    int max_branch_;

    node *root_;

    node *build_(std::queue<node *> &nodes) {
        if (nodes.size() == 1) {
            return nodes.front();
        }
        std::queue<node *> next;
        int branch = 0, rank = 0;
        node **child = new node *[max_branch_];
        while (!nodes.empty()) {
            child[branch++] = nodes.front();
            rank += nodes.front()->rank();
            nodes.pop();
            if (branch == max_branch_ || nodes.empty()) {
                node *root = new node(branch, rank, child);
                int now = 0, cnt = 0;
                for (int i = 1; i <= root->rank(); i++) {
                    cnt++;
                    while (now < root->branch() && cnt > root->child[now]->rank()) {
                        now++;
                    }
                    root->lookup_table[i - 1] = now;
                }
                next.push(root);
                branch = 0;
                rank = 0;
            }
        }
        delete[] child;
        return build_(next);
    }

    void delete_(node *root) {
        if (root == nullptr) {
            return;
        }
        for (int i = 0; i < root->branch(); i++) {
            delete_(root->child[i]);
        }
        delete root;
    }

  public:
    multi_branch_tree() = delete;

    explicit multi_branch_tree(int max_branch, std::queue<node *> &leaf) : max_branch_(max_branch) {
        root_ = build_(leaf);
    }

    ~multi_branch_tree() {
        delete_(root_);
    }

    node *root() {
        return root_;
    }

    int rank() {
        return root_->rank();
    }

    std::pair<int, int> select(int rank) {
        node *root = root_;
        if (rank < 1 || rank > root_->rank()) {
            return {-1, -1};
        }
        while (root->branch()) {
            int branch = root->lookup_table[rank - 1];
            if (branch > root->branch()) {
                return {-1, -1};
            }
            if (branch > 0) {
                rank -= root->child[branch - 1]->rank();
            }
            root = root->child[branch];
        }
        return {root->data(), rank};
    }
};

#endif //THEOREM_MULTI_BRANCH_TREE_H
