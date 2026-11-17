#ifndef CTB_MATRIX_INT_H
#define CTB_MATRIX_INT_H
#include "io.h"
/* Generated typed integer matrix; values are owned and row-major. */
typedef struct { int *data; size_t rows, cols; } IntMatrix;
static IntMatrix IntMatrix_new(size_t rows,size_t cols) {
    if(cols && rows>SIZE_MAX/cols)tb_fail("matrix size overflow");
    size_t n=rows*cols;IntMatrix m={n?tb_resize(NULL,n,sizeof *m.data):NULL,rows,cols};
    for(size_t i=0;i<n;++i)m.data[i]=0;
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
static IntMatrix IntMatrix_multiply(const IntMatrix *a,const IntMatrix *b) {
    if(a->cols!=b->rows)tb_fail("matrix dimension mismatch");
    IntMatrix result=IntMatrix_new(a->rows,b->cols);
    for(size_t i=0;i<a->rows;++i)for(size_t j=0;j<b->cols;++j) {
        long long total=0;
        for(size_t k=0;k<a->cols;++k) {
            long long product;
            product=(long long)a->data[i*a->cols+k]*(long long)b->data[k*b->cols+j];
            if((product>0 && total>LLONG_MAX-product)||(product<0 && total<LLONG_MIN-product))tb_fail("matrix multiplication overflow");
            total+=product;
        }
        if(total<INT_MIN || total>INT_MAX)tb_fail("matrix multiplication overflow");
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
        for(size_t j=0;j<m->cols;++j) { if(j)print_text(separator,"");print_int(m->data[i*m->cols+j],""); }
        print_char('\n',"");
    }
    print_text("",end);
}
static void IntMatrix_free(IntMatrix *m) { free(m->data);*m=(IntMatrix){0}; }
static void IntMatrix_swap(IntMatrix *a,IntMatrix *b) { IntMatrix x=*a;*a=*b;*b=x; }

#endif
