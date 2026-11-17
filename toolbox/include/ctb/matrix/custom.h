#ifndef CTB_MATRIX_CUSTOM_H
#define CTB_MATRIX_CUSTOM_H
#include "../detail/core.h"
#ifndef CTB_JOIN
#define CTB_JOIN_RAW(a,b) a##b
#define CTB_JOIN(a,b) CTB_JOIN_RAW(a,b)
#endif
#define CTB_MATRIX_DEFINE(Name,Type) \
typedef struct { Type *data; size_t rows, cols; } Name; \
static Name CTB_JOIN(Name,_new)(size_t rows,size_t cols) { \
if(cols && rows>SIZE_MAX/cols)tb_fail("matrix size overflow"); \
size_t n=rows*cols;Name m={n?tb_resize(NULL,n,sizeof *m.data):NULL,rows,cols}; \
for(size_t i=0;i<n;++i)m.data[i]=(Type){0}; \
return m; \
} \
static size_t CTB_JOIN(Name,_index)(const Name *m,size_t row,size_t col) { \
if(row>=m->rows || col>=m->cols)tb_fail("matrix index out of range"); \
return row*m->cols+col; \
} \
static Type CTB_JOIN(Name,_get)(const Name *m,size_t row,size_t col) { return m->data[CTB_JOIN(Name,_index)(m,row,col)]; } \
static void CTB_JOIN(Name,_set)(Name *m,size_t row,size_t col,Type value) { m->data[CTB_JOIN(Name,_index)(m,row,col)]=value; } \
static Name CTB_JOIN(Name,_clone)(const Name *m) { \
Name copy=CTB_JOIN(Name,_new)(m->rows,m->cols);size_t n=m->rows*m->cols; \
if(n)memcpy(copy.data,m->data,n*sizeof *m->data); \
return copy; \
} \
static Name CTB_JOIN(Name,_transpose)(const Name *m) { \
Name result=CTB_JOIN(Name,_new)(m->cols,m->rows); \
for(size_t i=0;i<m->rows;++i)for(size_t j=0;j<m->cols;++j)result.data[j*result.cols+i]=m->data[i*m->cols+j]; \
return result; \
} \
static void CTB_JOIN(Name,_free)(Name *m) { free(m->data);*m=(Name){0}; } \
static void CTB_JOIN(Name,_swap)(Name *a,Name *b) { Name x=*a;*a=*b;*b=x; }

#endif
