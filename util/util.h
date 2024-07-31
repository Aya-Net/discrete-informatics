//
// Created by 郭修屹 on 2024/7/31.
//

#ifndef THEOREM_UTIL_H
#define THEOREM_UTIL_H

#include "constant.h"

static inline int bit_len(unsigned int num) {
    return num == 0 ? 1 : UINT_SIZE - __builtin_clz(num);
}

#endif //THEOREM_UTIL_H
