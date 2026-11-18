#ifndef CTB_MATRIX_BOOL_H
#define CTB_MATRIX_BOOL_H
#include "detail/border.h"
#include "../io.h"
/* SPDX-License-Identifier: MIT-0 */
typedef struct { bool *data; size_t rows, cols; } BoolMatrix;
static BoolMatrix BoolMatrix_new(size_t rows,size_t cols) {
    if(cols && rows>SIZE_MAX/cols)tb_fail("matrix size overflow");
    size_t n=rows*cols;BoolMatrix m={n?tb_resize(NULL,n,sizeof *m.data):NULL,rows,cols};
    for(size_t i=0;i<n;++i)m.data[i]=(bool){0};
    return m;
}
static size_t BoolMatrix_index(const BoolMatrix *m,size_t row,size_t col) {
    if(row>=m->rows || col>=m->cols)tb_fail("matrix index out of range");
    return row*m->cols+col;
}
static bool BoolMatrix_get(const BoolMatrix *m,size_t row,size_t col) { return m->data[BoolMatrix_index(m,row,col)]; }
static void BoolMatrix_set(BoolMatrix *m,size_t row,size_t col,bool value) { m->data[BoolMatrix_index(m,row,col)]=value; }
static BoolMatrix BoolMatrix_clone(const BoolMatrix *m) {
    BoolMatrix copy=BoolMatrix_new(m->rows,m->cols);size_t n=m->rows*m->cols;
    if(n)memcpy(copy.data,m->data,n*sizeof *m->data);
    return copy;
}
static BoolMatrix BoolMatrix_transpose(const BoolMatrix *m) {
    BoolMatrix result=BoolMatrix_new(m->cols,m->rows);
    for(size_t i=0;i<m->rows;++i)for(size_t j=0;j<m->cols;++j)result.data[j*result.cols+i]=m->data[i*m->cols+j];
    return result;
}
static void BoolMatrix_free(BoolMatrix *m) { free(m->data);*m=(BoolMatrix){0}; }
static void BoolMatrix_swap(BoolMatrix *a,BoolMatrix *b) { BoolMatrix x=*a;*a=*b;*b=x; }

static void BoolMatrix_fill(BoolMatrix *m, bool value) {
    for (size_t i = 0; i < m->rows * m->cols; ++i) m->data[i] = value;
}
static BoolMatrix BoolMatrix_filled(size_t rows, size_t cols, bool value) {
    BoolMatrix m = BoolMatrix_new(rows, cols);
    BoolMatrix_fill(&m, value);
    return m;
}
static bool BoolMatrix_empty(const BoolMatrix *m) {
    return m->rows == 0 || m->cols == 0;
}
static bool BoolMatrix_in_bounds(const BoolMatrix *m, intmax_t row, intmax_t col) {
    return row >= 0 && col >= 0 && (uintmax_t)row < m->rows && (uintmax_t)col < m->cols;
}
typedef struct { bool ok; bool value; } BoolMatrixResult;
static BoolMatrixResult BoolMatrix_try_get(const BoolMatrix *m, intmax_t row, intmax_t col) {
    if (!BoolMatrix_in_bounds(m, row, col)) return (BoolMatrixResult){0};
    return (BoolMatrixResult){true, BoolMatrix_get(m, (size_t)row, (size_t)col)};
}
static void BoolMatrix_check_rect(const BoolMatrix *m, size_t row, size_t col,
                              size_t height, size_t width) {
    if (row > m->rows || col > m->cols || height > m->rows - row || width > m->cols - col)
        tb_fail("matrix rectangle out of range");
}
static void BoolMatrix_fill_rect(BoolMatrix *m, size_t row, size_t col,
                             size_t height, size_t width, bool value) {
    BoolMatrix_check_rect(m, row, col, height, width);
    if (!width) return;
    for (size_t r = 0; r < height; ++r)
        for (size_t c = 0; c < width; ++c)
            m->data[(row + r) * m->cols + col + c] = value;
}
static void BoolMatrix_fill_row(BoolMatrix *m, size_t row, bool value) {
    BoolMatrix_fill_rect(m, row, 0, 1, m->cols, value);
}
static void BoolMatrix_fill_col(BoolMatrix *m, size_t col, bool value) {
    BoolMatrix_fill_rect(m, 0, col, m->rows, 1, value);
}
static void BoolMatrix_draw_rect(BoolMatrix *m, size_t row, size_t col,
                             size_t height, size_t width, bool value) {
    BoolMatrix_check_rect(m, row, col, height, width);
    if (!height || !width) return;
    for (size_t r = 0; r < height; ++r) {
        size_t step = tb_matrix_border_step(r, height, width);
        for (size_t c = 0; c < width; c += step) {
            m->data[(row + r) * m->cols + col + c] = value;
            if (c == width - 1) break;
        }
    }
}
static BoolMatrix BoolMatrix_slice(const BoolMatrix *m, size_t row, size_t col,
                           size_t height, size_t width) {
    BoolMatrix_check_rect(m, row, col, height, width);
    BoolMatrix out = BoolMatrix_new(height, width);
    if (width)
        for (size_t r = 0; r < height; ++r)
            memcpy(out.data + r * width, m->data + (row + r) * m->cols + col,
                   width * sizeof *out.data);
    return out;
}

static bool BoolMatrix_try_set(BoolMatrix *m, intmax_t row, intmax_t col, bool value) {
    if (!BoolMatrix_in_bounds(m, row, col)) return false;
    BoolMatrix_set(m, (size_t)row, (size_t)col, value);
    return true;
}
static size_t BoolMatrix_rows(const BoolMatrix *m) { return m->rows; }
static size_t BoolMatrix_cols(const BoolMatrix *m) { return m->cols; }
static size_t BoolMatrix_size(const BoolMatrix *m) { return m->rows * m->cols; }
static void BoolMatrix_resize(BoolMatrix *m, size_t rows, size_t cols, bool value) {
    if (rows == m->rows && cols == m->cols) return;
    BoolMatrix out = BoolMatrix_filled(rows, cols, value);
    size_t common_rows = rows < m->rows ? rows : m->rows;
    size_t common_cols = cols < m->cols ? cols : m->cols;
    if (common_cols)
        for (size_t r = 0; r < common_rows; ++r)
            memcpy(out.data + r * cols, m->data + r * m->cols,
                   common_cols * sizeof *out.data);
    BoolMatrix_free(m);
    *m = out;
}
/* SPDX-License-Identifier: MIT-0 */
static void print_bool_matrix(const BoolMatrix *m,
                                  const char *elem_sep, const char *row_sep, const char *end) {
    for (size_t r = 0; r < m->rows; ++r) {
        if (r) print_text(row_sep, "");
        for (size_t c = 0; c < m->cols; ++c) {
            if (c) print_text(elem_sep, "");
            bool value = m->data[r * m->cols + c];
            print_int(value, "");
        }
    }
    print_text("", end);
}

#endif
