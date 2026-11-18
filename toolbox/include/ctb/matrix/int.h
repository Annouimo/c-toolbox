#ifndef CTB_MATRIX_INT_H
#define CTB_MATRIX_INT_H
#include "../io.h"
#include "detail/border.h"
#include "detail/integer.h"
/* SPDX-License-Identifier: MIT-0 */
typedef struct { int *data; size_t rows, cols; } IntMatrix;
static IntMatrix IntMatrix_new(size_t rows,size_t cols) {
    if(cols && rows>SIZE_MAX/cols)tb_fail("matrix size overflow");
    size_t n=rows*cols;IntMatrix m={n?tb_resize(NULL,n,sizeof *m.data):NULL,rows,cols};
    for(size_t i=0;i<n;++i)m.data[i]=(int){0};
    return m;
}
static size_t IntMatrix_index(const IntMatrix *m,size_t row,size_t col) {
    if(row>=m->rows || col>=m->cols)tb_fail("matrix index out of range");
    return row*m->cols+col;
}
static int IntMatrix_get(const IntMatrix *m,size_t row,size_t col) { return m->data[IntMatrix_index(m,row,col)]; }
static void IntMatrix_set(IntMatrix *m,size_t row,size_t col,int value) { m->data[IntMatrix_index(m,row,col)]=value; }
static IntMatrix IntMatrix_clone(const IntMatrix *m) {
    IntMatrix copy=IntMatrix_new(m->rows,m->cols);size_t n=m->rows*m->cols;
    if(n)memcpy(copy.data,m->data,n*sizeof *m->data);
    return copy;
}
static IntMatrix IntMatrix_transpose(const IntMatrix *m) {
    IntMatrix result=IntMatrix_new(m->cols,m->rows);
    for(size_t i=0;i<m->rows;++i)for(size_t j=0;j<m->cols;++j)result.data[j*result.cols+i]=m->data[i*m->cols+j];
    return result;
}
static void IntMatrix_free(IntMatrix *m) { free(m->data);*m=(IntMatrix){0}; }
static void IntMatrix_swap(IntMatrix *a,IntMatrix *b) { IntMatrix x=*a;*a=*b;*b=x; }

static void IntMatrix_fill(IntMatrix *m, int value) {
    for (size_t i = 0; i < m->rows * m->cols; ++i) m->data[i] = value;
}
static IntMatrix IntMatrix_filled(size_t rows, size_t cols, int value) {
    IntMatrix m = IntMatrix_new(rows, cols);
    IntMatrix_fill(&m, value);
    return m;
}
static bool IntMatrix_empty(const IntMatrix *m) {
    return m->rows == 0 || m->cols == 0;
}
static bool IntMatrix_in_bounds(const IntMatrix *m, intmax_t row, intmax_t col) {
    return row >= 0 && col >= 0 && (uintmax_t)row < m->rows && (uintmax_t)col < m->cols;
}
typedef struct { bool ok; int value; } IntMatrixResult;
static IntMatrixResult IntMatrix_try_get(const IntMatrix *m, intmax_t row, intmax_t col) {
    if (!IntMatrix_in_bounds(m, row, col)) return (IntMatrixResult){0};
    return (IntMatrixResult){true, IntMatrix_get(m, (size_t)row, (size_t)col)};
}
static void IntMatrix_check_rect(const IntMatrix *m, size_t row, size_t col,
                              size_t height, size_t width) {
    if (row > m->rows || col > m->cols || height > m->rows - row || width > m->cols - col)
        tb_fail("matrix rectangle out of range");
}
static void IntMatrix_fill_rect(IntMatrix *m, size_t row, size_t col,
                             size_t height, size_t width, int value) {
    IntMatrix_check_rect(m, row, col, height, width);
    if (!width) return;
    for (size_t r = 0; r < height; ++r)
        for (size_t c = 0; c < width; ++c)
            m->data[(row + r) * m->cols + col + c] = value;
}
static void IntMatrix_fill_row(IntMatrix *m, size_t row, int value) {
    IntMatrix_fill_rect(m, row, 0, 1, m->cols, value);
}
static void IntMatrix_fill_col(IntMatrix *m, size_t col, int value) {
    IntMatrix_fill_rect(m, 0, col, m->rows, 1, value);
}
static void IntMatrix_draw_rect(IntMatrix *m, size_t row, size_t col,
                             size_t height, size_t width, int value) {
    IntMatrix_check_rect(m, row, col, height, width);
    if (!height || !width) return;
    for (size_t r = 0; r < height; ++r) {
        size_t step = tb_matrix_border_step(r, height, width);
        for (size_t c = 0; c < width; c += step) {
            m->data[(row + r) * m->cols + col + c] = value;
            if (c == width - 1) break;
        }
    }
}
static IntMatrix IntMatrix_slice(const IntMatrix *m, size_t row, size_t col,
                           size_t height, size_t width) {
    IntMatrix_check_rect(m, row, col, height, width);
    IntMatrix out = IntMatrix_new(height, width);
    if (width)
        for (size_t r = 0; r < height; ++r)
            memcpy(out.data + r * width, m->data + (row + r) * m->cols + col,
                   width * sizeof *out.data);
    return out;
}
/* SPDX-License-Identifier: MIT-0 */
static IntMatrix IntMatrix_multiply(const IntMatrix *a,const IntMatrix *b) {
    if(a->cols!=b->rows)tb_fail("matrix dimension mismatch");
    IntMatrix result=IntMatrix_new(a->rows,b->cols);
    for(size_t i=0;i<a->rows;++i)for(size_t j=0;j<b->cols;++j) {
        long long total=0;
        for(size_t k=0;k<a->cols;++k) {
            int x = a->data[i*a->cols+k];
            int y = b->data[k*b->cols+j];
            total = tb_matrix_add_product(total, x, y);
        }
        if (total < INT_MIN || total > INT_MAX) tb_fail("matrix multiplication overflow");
        result.data[i*result.cols+j]=(int)total;
    }
    return result;
}
static IntMatrix read_int_matrix(size_t rows,size_t cols) {
    IntMatrix m=IntMatrix_new(rows,cols);size_t n=rows*cols;
    for(size_t i=0;i<n;++i)m.data[i]=read_int();
    return m;
}
static void print_int_matrix(const IntMatrix *m,const char *separator,const char *end) {
    for(size_t i=0;i<m->rows;++i) {
        for(size_t j=0;j<m->cols;++j) { if(j)print_text(separator,"");int value = m->data[i*m->cols+j]; print_int(value, ""); }
        print_char('\n',"");
    }
    print_text("",end);
}

static long long IntMatrix_sum_region(const IntMatrix *m, size_t row, size_t col,
                                      size_t height, size_t width, bool border) {
    IntMatrix_check_rect(m, row, col, height, width);
    long long total = 0;
    if (!width) return total;
    for (size_t r = 0; r < height; ++r) {
        size_t step = border ? tb_matrix_border_step(r, height, width) : 1;
        for (size_t c = 0; c < width; c += step) {
            int value = m->data[(row + r) * m->cols + col + c];
            total = tb_matrix_checked_add(total, value, "matrix sum overflow");
            if (c == width - 1) break;
        }
    }
    return total;
}
static long long IntMatrix_sum_rect(const IntMatrix *m, size_t row, size_t col,
                                    size_t height, size_t width) {
    return IntMatrix_sum_region(m, row, col, height, width, false);
}
static long long IntMatrix_sum_border(const IntMatrix *m, size_t row, size_t col,
                                      size_t height, size_t width) {
    return IntMatrix_sum_region(m, row, col, height, width, true);
}

#endif
