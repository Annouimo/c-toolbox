#ifndef CTB_MATRIX_DOUBLE_H
#define CTB_MATRIX_DOUBLE_H
#include "../io.h"
#include "detail/border.h"
/* SPDX-License-Identifier: MIT-0 */
typedef struct { double *data; size_t rows, cols; } DoubleMatrix;
static DoubleMatrix DoubleMatrix_new(size_t rows,size_t cols) {
    if(cols && rows>SIZE_MAX/cols)tb_fail("matrix size overflow");
    size_t n=rows*cols;DoubleMatrix m={n?tb_resize(NULL,n,sizeof *m.data):NULL,rows,cols};
    for(size_t i=0;i<n;++i)m.data[i]=(double){0};
    return m;
}
static size_t DoubleMatrix_index(const DoubleMatrix *m,size_t row,size_t col) {
    if(row>=m->rows || col>=m->cols)tb_fail("matrix index out of range");
    return row*m->cols+col;
}
static double DoubleMatrix_get(const DoubleMatrix *m,size_t row,size_t col) { return m->data[DoubleMatrix_index(m,row,col)]; }
static void DoubleMatrix_set(DoubleMatrix *m,size_t row,size_t col,double value) { m->data[DoubleMatrix_index(m,row,col)]=value; }
static DoubleMatrix DoubleMatrix_clone(const DoubleMatrix *m) {
    DoubleMatrix copy=DoubleMatrix_new(m->rows,m->cols);size_t n=m->rows*m->cols;
    if(n)memcpy(copy.data,m->data,n*sizeof *m->data);
    return copy;
}
static DoubleMatrix DoubleMatrix_transpose(const DoubleMatrix *m) {
    DoubleMatrix result=DoubleMatrix_new(m->cols,m->rows);
    for(size_t i=0;i<m->rows;++i)for(size_t j=0;j<m->cols;++j)result.data[j*result.cols+i]=m->data[i*m->cols+j];
    return result;
}
static void DoubleMatrix_free(DoubleMatrix *m) { free(m->data);*m=(DoubleMatrix){0}; }
static void DoubleMatrix_swap(DoubleMatrix *a,DoubleMatrix *b) { DoubleMatrix x=*a;*a=*b;*b=x; }

static void DoubleMatrix_fill(DoubleMatrix *m, double value) {
    for (size_t i = 0; i < m->rows * m->cols; ++i) m->data[i] = value;
}
static DoubleMatrix DoubleMatrix_filled(size_t rows, size_t cols, double value) {
    DoubleMatrix m = DoubleMatrix_new(rows, cols);
    DoubleMatrix_fill(&m, value);
    return m;
}
static bool DoubleMatrix_empty(const DoubleMatrix *m) {
    return m->rows == 0 || m->cols == 0;
}
static bool DoubleMatrix_in_bounds(const DoubleMatrix *m, intmax_t row, intmax_t col) {
    return row >= 0 && col >= 0 && (uintmax_t)row < m->rows && (uintmax_t)col < m->cols;
}
typedef struct { bool ok; double value; } DoubleMatrixResult;
static DoubleMatrixResult DoubleMatrix_try_get(const DoubleMatrix *m, intmax_t row, intmax_t col) {
    if (!DoubleMatrix_in_bounds(m, row, col)) return (DoubleMatrixResult){0};
    return (DoubleMatrixResult){true, DoubleMatrix_get(m, (size_t)row, (size_t)col)};
}
static void DoubleMatrix_check_rect(const DoubleMatrix *m, size_t row, size_t col,
                              size_t height, size_t width) {
    if (row > m->rows || col > m->cols || height > m->rows - row || width > m->cols - col)
        tb_fail("matrix rectangle out of range");
}
static void DoubleMatrix_fill_rect(DoubleMatrix *m, size_t row, size_t col,
                             size_t height, size_t width, double value) {
    DoubleMatrix_check_rect(m, row, col, height, width);
    if (!width) return;
    for (size_t r = 0; r < height; ++r)
        for (size_t c = 0; c < width; ++c)
            m->data[(row + r) * m->cols + col + c] = value;
}
static void DoubleMatrix_fill_row(DoubleMatrix *m, size_t row, double value) {
    DoubleMatrix_fill_rect(m, row, 0, 1, m->cols, value);
}
static void DoubleMatrix_fill_col(DoubleMatrix *m, size_t col, double value) {
    DoubleMatrix_fill_rect(m, 0, col, m->rows, 1, value);
}
static void DoubleMatrix_draw_rect(DoubleMatrix *m, size_t row, size_t col,
                             size_t height, size_t width, double value) {
    DoubleMatrix_check_rect(m, row, col, height, width);
    if (!height || !width) return;
    for (size_t r = 0; r < height; ++r) {
        size_t step = tb_matrix_border_step(r, height, width);
        for (size_t c = 0; c < width; c += step) {
            m->data[(row + r) * m->cols + col + c] = value;
            if (c == width - 1) break;
        }
    }
}
static DoubleMatrix DoubleMatrix_slice(const DoubleMatrix *m, size_t row, size_t col,
                           size_t height, size_t width) {
    DoubleMatrix_check_rect(m, row, col, height, width);
    DoubleMatrix out = DoubleMatrix_new(height, width);
    if (width)
        for (size_t r = 0; r < height; ++r)
            memcpy(out.data + r * width, m->data + (row + r) * m->cols + col,
                   width * sizeof *out.data);
    return out;
}
/* SPDX-License-Identifier: MIT-0 */
static DoubleMatrix DoubleMatrix_multiply(const DoubleMatrix *a,const DoubleMatrix *b) {
    if(a->cols!=b->rows)tb_fail("matrix dimension mismatch");
    DoubleMatrix result=DoubleMatrix_new(a->rows,b->cols);
    for(size_t i=0;i<a->rows;++i)for(size_t j=0;j<b->cols;++j) {
        double total=0;
        for(size_t k=0;k<a->cols;++k) {
            double x = a->data[i*a->cols+k];
            double y = b->data[k*b->cols+j];
            total += x * y;
        }

        result.data[i*result.cols+j]=(double)total;
    }
    return result;
}
static DoubleMatrix read_double_matrix(size_t rows,size_t cols) {
    DoubleMatrix m=DoubleMatrix_new(rows,cols);size_t n=rows*cols;
    for(size_t i=0;i<n;++i)m.data[i]=read_double();
    return m;
}
static void print_double_matrix(const DoubleMatrix *m, int digits,const char *separator,const char *end) {
    for(size_t i=0;i<m->rows;++i) {
        for(size_t j=0;j<m->cols;++j) { if(j)print_text(separator,"");double value = m->data[i*m->cols+j]; print_fixed(value, digits, ""); }
        print_char('\n',"");
    }
    print_text("",end);
}

static double DoubleMatrix_sum_region(const DoubleMatrix *m, size_t row, size_t col,
                                      size_t height, size_t width, bool border) {
    DoubleMatrix_check_rect(m, row, col, height, width);
    double total = 0;
    if (!width) return total;
    for (size_t r = 0; r < height; ++r) {
        size_t step = border ? tb_matrix_border_step(r, height, width) : 1;
        for (size_t c = 0; c < width; c += step) {
            double value = m->data[(row + r) * m->cols + col + c];
            total += value;
            if (c == width - 1) break;
        }
    }
    return total;
}
static double DoubleMatrix_sum_rect(const DoubleMatrix *m, size_t row, size_t col,
                                    size_t height, size_t width) {
    return DoubleMatrix_sum_region(m, row, col, height, width, false);
}
static double DoubleMatrix_sum_border(const DoubleMatrix *m, size_t row, size_t col,
                                      size_t height, size_t width) {
    return DoubleMatrix_sum_region(m, row, col, height, width, true);
}

#endif
