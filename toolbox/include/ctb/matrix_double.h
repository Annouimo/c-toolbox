/* SPDX-License-Identifier: MIT-0 */
#ifndef CTB_MATRIX_DOUBLE_H
#define CTB_MATRIX_DOUBLE_H
#include "matrix.h"
typedef Matrix DoubleMatrix;
static DoubleMatrix DoubleMatrix_new(size_t rows,size_t cols) { return matrix_new(rows,cols); }
static double DoubleMatrix_get(const DoubleMatrix *m,size_t row,size_t col) { return matrix_get(m,row,col); }
static void DoubleMatrix_set(DoubleMatrix *m,size_t row,size_t col,double value) { matrix_set(m,row,col,value); }
static DoubleMatrix DoubleMatrix_clone(const DoubleMatrix *m) { return matrix_clone(m); }
static DoubleMatrix DoubleMatrix_transpose(const DoubleMatrix *m) { return matrix_transpose(m); }
static DoubleMatrix DoubleMatrix_multiply(const DoubleMatrix *a,const DoubleMatrix *b) { return matrix_multiply(a,b); }
static DoubleMatrix read_double_matrix(size_t rows,size_t cols) { return read_matrix(rows,cols); }
static void print_double_matrix(const DoubleMatrix *m,int digits,const char *separator,const char *end) { print_matrix(m,digits,separator,end); }
static void DoubleMatrix_free(DoubleMatrix *m) { matrix_free(m); }
static void DoubleMatrix_swap(DoubleMatrix *a,DoubleMatrix *b) { matrix_swap(a,b); }
#endif
