#ifndef CTB_MATRIX_DOUBLE_H
#define CTB_MATRIX_DOUBLE_H
#include "../io.h"
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

#endif
