#ifndef CTB_MATRIX_LONG_LONG_H
#define CTB_MATRIX_LONG_LONG_H
#include "../io.h"
#include "detail/border.h"
#include "detail/integer.h"
/* SPDX-License-Identifier: MIT-0 */
typedef struct { long long *data; size_t rows, cols; } LongLongMatrix;
static LongLongMatrix LongLongMatrix_new(size_t rows,size_t cols) {
    if(cols && rows>SIZE_MAX/cols)tb_fail("matrix size overflow");
    size_t n=rows*cols;LongLongMatrix m={n?tb_resize(NULL,n,sizeof *m.data):NULL,rows,cols};
    for(size_t i=0;i<n;++i)m.data[i]=(long long){0};
    return m;
}
static size_t LongLongMatrix_index(const LongLongMatrix *m,size_t row,size_t col) {
    if(row>=m->rows || col>=m->cols)tb_fail("matrix index out of range");
    return row*m->cols+col;
}
static long long LongLongMatrix_get(const LongLongMatrix *m,size_t row,size_t col) { return m->data[LongLongMatrix_index(m,row,col)]; }
static void LongLongMatrix_set(LongLongMatrix *m,size_t row,size_t col,long long value) { m->data[LongLongMatrix_index(m,row,col)]=value; }
static LongLongMatrix LongLongMatrix_clone(const LongLongMatrix *m) {
    LongLongMatrix copy=LongLongMatrix_new(m->rows,m->cols);size_t n=m->rows*m->cols;
    if(n)memcpy(copy.data,m->data,n*sizeof *m->data);
    return copy;
}
static LongLongMatrix LongLongMatrix_transpose(const LongLongMatrix *m) {
    LongLongMatrix result=LongLongMatrix_new(m->cols,m->rows);
    for(size_t i=0;i<m->rows;++i)for(size_t j=0;j<m->cols;++j)result.data[j*result.cols+i]=m->data[i*m->cols+j];
    return result;
}
static void LongLongMatrix_free(LongLongMatrix *m) { free(m->data);*m=(LongLongMatrix){0}; }
static void LongLongMatrix_swap(LongLongMatrix *a,LongLongMatrix *b) { LongLongMatrix x=*a;*a=*b;*b=x; }

static void LongLongMatrix_fill(LongLongMatrix *m, long long value) {
    for (size_t i = 0; i < m->rows * m->cols; ++i) m->data[i] = value;
}
static LongLongMatrix LongLongMatrix_filled(size_t rows, size_t cols, long long value) {
    LongLongMatrix m = LongLongMatrix_new(rows, cols);
    LongLongMatrix_fill(&m, value);
    return m;
}
static bool LongLongMatrix_empty(const LongLongMatrix *m) {
    return m->rows == 0 || m->cols == 0;
}
static bool LongLongMatrix_in_bounds(const LongLongMatrix *m, intmax_t row, intmax_t col) {
    return row >= 0 && col >= 0 && (uintmax_t)row < m->rows && (uintmax_t)col < m->cols;
}
typedef struct { bool ok; long long value; } LongLongMatrixResult;
static LongLongMatrixResult LongLongMatrix_try_get(const LongLongMatrix *m, intmax_t row, intmax_t col) {
    if (!LongLongMatrix_in_bounds(m, row, col)) return (LongLongMatrixResult){0};
    return (LongLongMatrixResult){true, LongLongMatrix_get(m, (size_t)row, (size_t)col)};
}
static void LongLongMatrix_check_rect(const LongLongMatrix *m, size_t row, size_t col,
                              size_t height, size_t width) {
    if (row > m->rows || col > m->cols || height > m->rows - row || width > m->cols - col)
        tb_fail("matrix rectangle out of range");
}
static void LongLongMatrix_fill_rect(LongLongMatrix *m, size_t row, size_t col,
                             size_t height, size_t width, long long value) {
    LongLongMatrix_check_rect(m, row, col, height, width);
    if (!width) return;
    for (size_t r = 0; r < height; ++r)
        for (size_t c = 0; c < width; ++c)
            m->data[(row + r) * m->cols + col + c] = value;
}
static void LongLongMatrix_fill_row(LongLongMatrix *m, size_t row, long long value) {
    LongLongMatrix_fill_rect(m, row, 0, 1, m->cols, value);
}
static void LongLongMatrix_fill_col(LongLongMatrix *m, size_t col, long long value) {
    LongLongMatrix_fill_rect(m, 0, col, m->rows, 1, value);
}
static void LongLongMatrix_draw_rect(LongLongMatrix *m, size_t row, size_t col,
                             size_t height, size_t width, long long value) {
    LongLongMatrix_check_rect(m, row, col, height, width);
    if (!height || !width) return;
    for (size_t r = 0; r < height; ++r) {
        size_t step = tb_matrix_border_step(r, height, width);
        for (size_t c = 0; c < width; c += step) {
            m->data[(row + r) * m->cols + col + c] = value;
            if (c == width - 1) break;
        }
    }
}
static LongLongMatrix LongLongMatrix_slice(const LongLongMatrix *m, size_t row, size_t col,
                           size_t height, size_t width) {
    LongLongMatrix_check_rect(m, row, col, height, width);
    LongLongMatrix out = LongLongMatrix_new(height, width);
    if (width)
        for (size_t r = 0; r < height; ++r)
            memcpy(out.data + r * width, m->data + (row + r) * m->cols + col,
                   width * sizeof *out.data);
    return out;
}

static bool LongLongMatrix_try_set(LongLongMatrix *m, intmax_t row, intmax_t col, long long value) {
    if (!LongLongMatrix_in_bounds(m, row, col)) return false;
    LongLongMatrix_set(m, (size_t)row, (size_t)col, value);
    return true;
}
static size_t LongLongMatrix_rows(const LongLongMatrix *m) { return m->rows; }
static size_t LongLongMatrix_cols(const LongLongMatrix *m) { return m->cols; }
static size_t LongLongMatrix_size(const LongLongMatrix *m) { return m->rows * m->cols; }
static void LongLongMatrix_resize(LongLongMatrix *m, size_t rows, size_t cols, long long value) {
    if (rows == m->rows && cols == m->cols) return;
    LongLongMatrix out = LongLongMatrix_filled(rows, cols, value);
    size_t common_rows = rows < m->rows ? rows : m->rows;
    size_t common_cols = cols < m->cols ? cols : m->cols;
    if (common_cols)
        for (size_t r = 0; r < common_rows; ++r)
            memcpy(out.data + r * cols, m->data + r * m->cols,
                   common_cols * sizeof *out.data);
    LongLongMatrix_free(m);
    *m = out;
}
/* SPDX-License-Identifier: MIT-0 */
static LongLongMatrix LongLongMatrix_multiply(const LongLongMatrix *a,const LongLongMatrix *b) {
    if(a->cols!=b->rows)tb_fail("matrix dimension mismatch");
    LongLongMatrix result=LongLongMatrix_new(a->rows,b->cols);
    for(size_t i=0;i<a->rows;++i)for(size_t j=0;j<b->cols;++j) {
        long long total=0;
        for(size_t k=0;k<a->cols;++k) {
            long long x = a->data[i*a->cols+k];
            long long y = b->data[k*b->cols+j];
            total = tb_matrix_add_product(total, x, y);
        }

        result.data[i*result.cols+j]=(long long)total;
    }
    return result;
}
static LongLongMatrix read_ll_matrix(size_t rows,size_t cols) {
    LongLongMatrix m=LongLongMatrix_new(rows,cols);size_t n=rows*cols;
    for(size_t i=0;i<n;++i)m.data[i]=read_ll();
    return m;
}
static long long LongLongMatrix_sum_region(const LongLongMatrix *m, size_t row, size_t col,
                                      size_t height, size_t width, bool border) {
    LongLongMatrix_check_rect(m, row, col, height, width);
    long long total = 0;
    if (!width) return total;
    for (size_t r = 0; r < height; ++r) {
        size_t step = border ? tb_matrix_border_step(r, height, width) : 1;
        for (size_t c = 0; c < width; c += step) {
            long long value = m->data[(row + r) * m->cols + col + c];
            total = tb_matrix_checked_add(total, value, "matrix sum overflow");
            if (c == width - 1) break;
        }
    }
    return total;
}
static long long LongLongMatrix_sum_rect(const LongLongMatrix *m, size_t row, size_t col,
                                    size_t height, size_t width) {
    return LongLongMatrix_sum_region(m, row, col, height, width, false);
}
static long long LongLongMatrix_sum_border(const LongLongMatrix *m, size_t row, size_t col,
                                      size_t height, size_t width) {
    return LongLongMatrix_sum_region(m, row, col, height, width, true);
}
/* SPDX-License-Identifier: MIT-0 */
static void print_ll_matrix(const LongLongMatrix *m,
                                  const char *elem_sep, const char *row_sep, const char *end) {
    for (size_t r = 0; r < m->rows; ++r) {
        if (r) print_text(row_sep, "");
        for (size_t c = 0; c < m->cols; ++c) {
            if (c) print_text(elem_sep, "");
            long long value = m->data[r * m->cols + c];
            print_ll(value, "");
        }
    }
    print_text("", end);
}

static void LongLongMatrix_add_at(LongLongMatrix *v, size_t row, size_t col, long long delta) {
    size_t index = LongLongMatrix_index(v, row, col);
    long long value = v->data[index];
    if ((delta > 0 && value > LLONG_MAX - delta) ||
        (delta < 0 && value < LLONG_MIN - delta))
        tb_fail("element addition overflow");
    long long result = value + delta;
    v->data[index] = result;
}

#endif
