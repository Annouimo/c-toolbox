#ifndef CTB_MATRIX_CHAR_H
#define CTB_MATRIX_CHAR_H
#include "detail/border.h"
#include "../io.h"
#include "../string.h"
/* SPDX-License-Identifier: MIT-0 */
typedef struct { char *data; size_t rows, cols; } CharMatrix;
static CharMatrix CharMatrix_new(size_t rows,size_t cols) {
    if(cols && rows>SIZE_MAX/cols)tb_fail("matrix size overflow");
    size_t n=rows*cols;CharMatrix m={n?tb_resize(NULL,n,sizeof *m.data):NULL,rows,cols};
    for(size_t i=0;i<n;++i)m.data[i]=(char){0};
    return m;
}
static size_t CharMatrix_index(const CharMatrix *m,size_t row,size_t col) {
    if(row>=m->rows || col>=m->cols)tb_fail("matrix index out of range");
    return row*m->cols+col;
}
static char CharMatrix_get(const CharMatrix *m,size_t row,size_t col) { return m->data[CharMatrix_index(m,row,col)]; }
static void CharMatrix_set(CharMatrix *m,size_t row,size_t col,char value) { m->data[CharMatrix_index(m,row,col)]=value; }
static CharMatrix CharMatrix_clone(const CharMatrix *m) {
    CharMatrix copy=CharMatrix_new(m->rows,m->cols);size_t n=m->rows*m->cols;
    if(n)memcpy(copy.data,m->data,n*sizeof *m->data);
    return copy;
}
static CharMatrix CharMatrix_transpose(const CharMatrix *m) {
    CharMatrix result=CharMatrix_new(m->cols,m->rows);
    for(size_t i=0;i<m->rows;++i)for(size_t j=0;j<m->cols;++j)result.data[j*result.cols+i]=m->data[i*m->cols+j];
    return result;
}
static void CharMatrix_free(CharMatrix *m) { free(m->data);*m=(CharMatrix){0}; }
static void CharMatrix_swap(CharMatrix *a,CharMatrix *b) { CharMatrix x=*a;*a=*b;*b=x; }

static void CharMatrix_fill(CharMatrix *m, char value) {
    for (size_t i = 0; i < m->rows * m->cols; ++i) m->data[i] = value;
}
static CharMatrix CharMatrix_filled(size_t rows, size_t cols, char value) {
    CharMatrix m = CharMatrix_new(rows, cols);
    CharMatrix_fill(&m, value);
    return m;
}
static bool CharMatrix_empty(const CharMatrix *m) {
    return m->rows == 0 || m->cols == 0;
}
static bool CharMatrix_in_bounds(const CharMatrix *m, intmax_t row, intmax_t col) {
    return row >= 0 && col >= 0 && (uintmax_t)row < m->rows && (uintmax_t)col < m->cols;
}
typedef struct { bool ok; char value; } CharMatrixResult;
static CharMatrixResult CharMatrix_try_get(const CharMatrix *m, intmax_t row, intmax_t col) {
    if (!CharMatrix_in_bounds(m, row, col)) return (CharMatrixResult){0};
    return (CharMatrixResult){true, CharMatrix_get(m, (size_t)row, (size_t)col)};
}
static void CharMatrix_check_rect(const CharMatrix *m, size_t row, size_t col,
                              size_t height, size_t width) {
    if (row > m->rows || col > m->cols || height > m->rows - row || width > m->cols - col)
        tb_fail("matrix rectangle out of range");
}
static void CharMatrix_fill_rect(CharMatrix *m, size_t row, size_t col,
                             size_t height, size_t width, char value) {
    CharMatrix_check_rect(m, row, col, height, width);
    if (!width) return;
    for (size_t r = 0; r < height; ++r)
        for (size_t c = 0; c < width; ++c)
            m->data[(row + r) * m->cols + col + c] = value;
}
static void CharMatrix_fill_row(CharMatrix *m, size_t row, char value) {
    CharMatrix_fill_rect(m, row, 0, 1, m->cols, value);
}
static void CharMatrix_fill_col(CharMatrix *m, size_t col, char value) {
    CharMatrix_fill_rect(m, 0, col, m->rows, 1, value);
}
static void CharMatrix_draw_rect(CharMatrix *m, size_t row, size_t col,
                             size_t height, size_t width, char value) {
    CharMatrix_check_rect(m, row, col, height, width);
    if (!height || !width) return;
    for (size_t r = 0; r < height; ++r) {
        size_t step = tb_matrix_border_step(r, height, width);
        for (size_t c = 0; c < width; c += step) {
            m->data[(row + r) * m->cols + col + c] = value;
            if (c == width - 1) break;
        }
    }
}
static CharMatrix CharMatrix_slice(const CharMatrix *m, size_t row, size_t col,
                           size_t height, size_t width) {
    CharMatrix_check_rect(m, row, col, height, width);
    CharMatrix out = CharMatrix_new(height, width);
    if (width)
        for (size_t r = 0; r < height; ++r)
            memcpy(out.data + r * width, m->data + (row + r) * m->cols + col,
                   width * sizeof *out.data);
    return out;
}

static bool CharMatrix_try_set(CharMatrix *m, intmax_t row, intmax_t col, char value) {
    if (!CharMatrix_in_bounds(m, row, col)) return false;
    CharMatrix_set(m, (size_t)row, (size_t)col, value);
    return true;
}
static size_t CharMatrix_rows(const CharMatrix *m) { return m->rows; }
static size_t CharMatrix_cols(const CharMatrix *m) { return m->cols; }
static size_t CharMatrix_size(const CharMatrix *m) { return m->rows * m->cols; }
static void CharMatrix_resize(CharMatrix *m, size_t rows, size_t cols, char value) {
    if (rows == m->rows && cols == m->cols) return;
    CharMatrix out = CharMatrix_filled(rows, cols, value);
    size_t common_rows = rows < m->rows ? rows : m->rows;
    size_t common_cols = cols < m->cols ? cols : m->cols;
    if (common_cols)
        for (size_t r = 0; r < common_rows; ++r)
            memcpy(out.data + r * cols, m->data + r * m->cols,
                   common_cols * sizeof *out.data);
    CharMatrix_free(m);
    *m = out;
}
/* SPDX-License-Identifier: MIT-0 */
static void print_char_matrix(const CharMatrix *m,
                                  const char *elem_sep, const char *row_sep, const char *end) {
    for (size_t r = 0; r < m->rows; ++r) {
        if (r) print_text(row_sep, "");
        for (size_t c = 0; c < m->cols; ++c) {
            if (c) print_text(elem_sep, "");
            char value = m->data[r * m->cols + c];
            print_char(value, "");
        }
    }
    print_text("", end);
}
/* SPDX-License-Identifier: MIT-0 */
static CharMatrix read_char_matrix(size_t rows, size_t cols) {
    CharMatrix m = CharMatrix_new(rows, cols);
    for (size_t r = 0; r < rows; ++r) {
        String line = r == 0 ? read_next_line_raw() : read_line_raw();
        if (line.len != cols) {
            string_free(&line);
            CharMatrix_free(&m);
            tb_fail("character matrix row width mismatch");
        }
        if (cols) memcpy(m.data + r * cols, string_cstr(&line), cols);
        string_free(&line);
    }
    return m;
}

#endif
