#ifndef CTB_MATRIX_CUSTOM_H
#define CTB_MATRIX_CUSTOM_H
#include "detail/border.h"
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
static void CTB_JOIN(Name,_swap)(Name *a,Name *b) { Name x=*a;*a=*b;*b=x; } \
static void CTB_JOIN(Name,_fill)(Name *m, Type value) { \
for (size_t i = 0; i < m->rows * m->cols; ++i) m->data[i] = value; \
} \
static Name CTB_JOIN(Name,_filled)(size_t rows, size_t cols, Type value) { \
Name m = CTB_JOIN(Name,_new)(rows, cols); \
CTB_JOIN(Name,_fill)(&m, value); \
return m; \
} \
static bool CTB_JOIN(Name,_empty)(const Name *m) { \
return m->rows == 0 || m->cols == 0; \
} \
static bool CTB_JOIN(Name,_in_bounds)(const Name *m, intmax_t row, intmax_t col) { \
return row >= 0 && col >= 0 && (uintmax_t)row < m->rows && (uintmax_t)col < m->cols; \
} \
typedef struct { bool ok; Type value; } CTB_JOIN(Name,Result); \
static CTB_JOIN(Name,Result) CTB_JOIN(Name,_try_get)(const Name *m, intmax_t row, intmax_t col) { \
if (!CTB_JOIN(Name,_in_bounds)(m, row, col)) return (CTB_JOIN(Name,Result)){0}; \
return (CTB_JOIN(Name,Result)){true, CTB_JOIN(Name,_get)(m, (size_t)row, (size_t)col)}; \
} \
static void CTB_JOIN(Name,_check_rect)(const Name *m, size_t row, size_t col, \
size_t height, size_t width) { \
if (row > m->rows || col > m->cols || height > m->rows - row || width > m->cols - col) \
tb_fail("matrix rectangle out of range"); \
} \
static void CTB_JOIN(Name,_fill_rect)(Name *m, size_t row, size_t col, \
size_t height, size_t width, Type value) { \
CTB_JOIN(Name,_check_rect)(m, row, col, height, width); \
if (!width) return; \
for (size_t r = 0; r < height; ++r) \
for (size_t c = 0; c < width; ++c) \
m->data[(row + r) * m->cols + col + c] = value; \
} \
static void CTB_JOIN(Name,_fill_row)(Name *m, size_t row, Type value) { \
CTB_JOIN(Name,_fill_rect)(m, row, 0, 1, m->cols, value); \
} \
static void CTB_JOIN(Name,_fill_col)(Name *m, size_t col, Type value) { \
CTB_JOIN(Name,_fill_rect)(m, 0, col, m->rows, 1, value); \
} \
static void CTB_JOIN(Name,_draw_rect)(Name *m, size_t row, size_t col, \
size_t height, size_t width, Type value) { \
CTB_JOIN(Name,_check_rect)(m, row, col, height, width); \
if (!height || !width) return; \
for (size_t r = 0; r < height; ++r) { \
size_t step = tb_matrix_border_step(r, height, width); \
for (size_t c = 0; c < width; c += step) { \
m->data[(row + r) * m->cols + col + c] = value; \
if (c == width - 1) break; \
} \
} \
} \
static Name CTB_JOIN(Name,_slice)(const Name *m, size_t row, size_t col, \
size_t height, size_t width) { \
CTB_JOIN(Name,_check_rect)(m, row, col, height, width); \
Name out = CTB_JOIN(Name,_new)(height, width); \
if (width) \
for (size_t r = 0; r < height; ++r) \
memcpy(out.data + r * width, m->data + (row + r) * m->cols + col, \
width * sizeof *out.data); \
return out; \
} \
static bool CTB_JOIN(Name,_try_set)(Name *m, intmax_t row, intmax_t col, Type value) { \
if (!CTB_JOIN(Name,_in_bounds)(m, row, col)) return false; \
CTB_JOIN(Name,_set)(m, (size_t)row, (size_t)col, value); \
return true; \
} \
static size_t CTB_JOIN(Name,_rows)(const Name *m) { return m->rows; } \
static size_t CTB_JOIN(Name,_cols)(const Name *m) { return m->cols; } \
static size_t CTB_JOIN(Name,_size)(const Name *m) { return m->rows * m->cols; } \
static void CTB_JOIN(Name,_resize)(Name *m, size_t rows, size_t cols, Type value) { \
if (rows == m->rows && cols == m->cols) return; \
Name out = CTB_JOIN(Name,_filled)(rows, cols, value); \
size_t common_rows = rows < m->rows ? rows : m->rows; \
size_t common_cols = cols < m->cols ? cols : m->cols; \
if (common_cols) \
for (size_t r = 0; r < common_rows; ++r) \
memcpy(out.data + r * cols, m->data + r * m->cols, \
common_cols * sizeof *out.data); \
CTB_JOIN(Name,_free)(m); \
*m = out; \
}

#endif
