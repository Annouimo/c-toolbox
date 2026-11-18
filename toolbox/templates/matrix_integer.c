/* SPDX-License-Identifier: MIT-0 */
static long long tb_matrix_checked_add(long long total, long long value, const char *error) {
    if ((value > 0 && total > LLONG_MAX - value) ||
        (value < 0 && total < LLONG_MIN - value))
        tb_fail(error);
    return total + value;
}
static long long tb_matrix_add_product(long long total, long long x, long long y) {
    if (!tb_multiply_fits(x, y)) tb_fail("matrix multiplication overflow");
    return tb_matrix_checked_add(total, x * y, "matrix multiplication overflow");
}
