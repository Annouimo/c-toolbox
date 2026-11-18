#ifndef CTB_MATRIX_DETAIL_BORDER_H
#define CTB_MATRIX_DETAIL_BORDER_H
#include "../../detail/core.h"
/* SPDX-License-Identifier: MIT-0 */
/* Interior rows visit just their two endpoints (one if width is one). */
static size_t tb_matrix_border_step(size_t row, size_t height, size_t width) {
    return row == 0 || row == height - 1 || width < 2 ? 1 : width - 1;
}

#endif
