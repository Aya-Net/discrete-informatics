cmake_minimum_required(VERSION 3.28)
project(theorem)

set(CMAKE_CXX_STANDARD 17)

add_executable(theorem main.cpp
        model/bit_vector.h
        util/type.h
        model/compressed_array.h
        util/util.h
        util/constant.h
        model/sufficient_bv.h
        model/multi_branch_tree.h)
