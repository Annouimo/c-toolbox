#ifndef CTB_MATRIX_DETAIL_INTEGER_H
#define CTB_MATRIX_DETAIL_INTEGER_H
#include "../../math/arithmetic.h"
/* SPDX-License-Identifier: MIT-0 */
static long long tb_matrix_add_product(long long total, long long x, long long y) {
    if (!tb_multiply_fits(x, y)) tb_fail("matrix multiplication overflow");
    long long product = x * y;
    if ((product > 0 && total > LLONG_MAX - product) ||
        (product < 0 && total < LLONG_MIN - product))
        tb_fail("matrix multiplication overflow");
    return total + product;
}

#endif
