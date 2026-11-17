#ifndef CTB_MATRIX_LONG_LONG_H
#define CTB_MATRIX_LONG_LONG_H
#include "../io.h"
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
static void print_ll_matrix(const LongLongMatrix *m,const char *separator,const char *end) {
    for(size_t i=0;i<m->rows;++i) {
        for(size_t j=0;j<m->cols;++j) { if(j)print_text(separator,"");long long value = m->data[i*m->cols+j]; print_ll(value, ""); }
        print_char('\n',"");
    }
    print_text("",end);
}

#endif
