/* SPDX-License-Identifier: MIT-0 */
#ifndef CTB_MATRIX_H
#define CTB_MATRIX_H
#include "io.h"
/* Row-major, double elements. Owns data; copy through matrix_clone(). */
typedef struct { double *data;size_t rows,cols; } Matrix;
static Matrix matrix_new(size_t rows,size_t cols){if(cols && rows>SIZE_MAX/cols)tb_fail("matrix size overflow");size_t n=rows*cols;Matrix m={n?tb_resize(NULL,n,sizeof(double)):NULL,rows,cols};for(size_t i=0;i<n;++i)m.data[i]=0;return m;}
static size_t tb_matrix_index(const Matrix *m,size_t row,size_t col){if(row>=m->rows || col>=m->cols)tb_fail("matrix index out of range");return row*m->cols+col;}
static double matrix_get(const Matrix *m,size_t row,size_t col){return m->data[tb_matrix_index(m,row,col)];}
static void matrix_set(Matrix *m,size_t row,size_t col,double value){m->data[tb_matrix_index(m,row,col)]=value;}
static Matrix matrix_clone(const Matrix *m){Matrix r=matrix_new(m->rows,m->cols);if(m->rows && m->cols)memcpy(r.data,m->data,m->rows*m->cols*sizeof(double));return r;}
static Matrix matrix_transpose(const Matrix *m){Matrix r=matrix_new(m->cols,m->rows);for(size_t i=0;i<m->rows;++i)for(size_t j=0;j<m->cols;++j)r.data[j*r.cols+i]=m->data[i*m->cols+j];return r;}
static Matrix matrix_multiply(const Matrix *a,const Matrix *b){if(a->cols!=b->rows)tb_fail("matrix dimension mismatch");Matrix r=matrix_new(a->rows,b->cols);for(size_t i=0;i<a->rows;++i)for(size_t k=0;k<a->cols;++k)for(size_t j=0;j<b->cols;++j)r.data[i*r.cols+j]+=a->data[i*a->cols+k]*b->data[k*b->cols+j];return r;}
static Matrix read_matrix(size_t rows,size_t cols){Matrix m=matrix_new(rows,cols);for(size_t i=0;i<rows*cols;++i)m.data[i]=read_double();return m;}
static void print_matrix(const Matrix *m,int digits,const char *separator,const char *end){for(size_t i=0;i<m->rows;++i){for(size_t j=0;j<m->cols;++j){if(j)print_text(separator,"");print_fixed(m->data[i*m->cols+j],digits,"");}print_char('\n',"");}print_text("",end);}
static void matrix_free(Matrix *m){free(m->data);*m=(Matrix){0};}
static void matrix_swap(Matrix *a,Matrix *b){Matrix x=*a;*a=*b;*b=x;}
#endif
