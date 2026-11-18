/* SPDX-License-Identifier: MIT-0 */
typedef struct { @TYPE@ *data; size_t rows, cols; } @NAME@;
static @NAME@ @NAME@_new(size_t rows,size_t cols) {
    if(cols && rows>SIZE_MAX/cols)tb_fail("matrix size overflow");
    size_t n=rows*cols;@NAME@ m={n?tb_resize(NULL,n,sizeof *m.data):NULL,rows,cols};
    for(size_t i=0;i<n;++i)m.data[i]=(@TYPE@){0};
    return m;
}
static size_t @NAME@_index(const @NAME@ *m,size_t row,size_t col) {
    if(row>=m->rows || col>=m->cols)tb_fail("matrix index out of range");
    return row*m->cols+col;
}
static @TYPE@ @NAME@_get(const @NAME@ *m,size_t row,size_t col) { return m->data[@NAME@_index(m,row,col)]; }
static void @NAME@_set(@NAME@ *m,size_t row,size_t col,@TYPE@ value) { m->data[@NAME@_index(m,row,col)]=value; }
static @NAME@ @NAME@_clone(const @NAME@ *m) {
    @NAME@ copy=@NAME@_new(m->rows,m->cols);size_t n=m->rows*m->cols;
    if(n)memcpy(copy.data,m->data,n*sizeof *m->data);
    return copy;
}
static @NAME@ @NAME@_transpose(const @NAME@ *m) {
    @NAME@ result=@NAME@_new(m->cols,m->rows);
    for(size_t i=0;i<m->rows;++i)for(size_t j=0;j<m->cols;++j)result.data[j*result.cols+i]=m->data[i*m->cols+j];
    return result;
}
static void @NAME@_free(@NAME@ *m) { free(m->data);*m=(@NAME@){0}; }
static void @NAME@_swap(@NAME@ *a,@NAME@ *b) { @NAME@ x=*a;*a=*b;*b=x; }

static void @NAME@_fill(@NAME@ *m, @TYPE@ value) {
    for (size_t i = 0; i < m->rows * m->cols; ++i) m->data[i] = value;
}
static @NAME@ @NAME@_filled(size_t rows, size_t cols, @TYPE@ value) {
    @NAME@ m = @NAME@_new(rows, cols);
    @NAME@_fill(&m, value);
    return m;
}
static bool @NAME@_empty(const @NAME@ *m) {
    return m->rows == 0 || m->cols == 0;
}
static bool @NAME@_in_bounds(const @NAME@ *m, intmax_t row, intmax_t col) {
    return row >= 0 && col >= 0 && (uintmax_t)row < m->rows && (uintmax_t)col < m->cols;
}
typedef struct { bool ok; @TYPE@ value; } @NAME@Result;
static @NAME@Result @NAME@_try_get(const @NAME@ *m, intmax_t row, intmax_t col) {
    if (!@NAME@_in_bounds(m, row, col)) return (@NAME@Result){0};
    return (@NAME@Result){true, @NAME@_get(m, (size_t)row, (size_t)col)};
}
static void @NAME@_check_rect(const @NAME@ *m, size_t row, size_t col,
                              size_t height, size_t width) {
    if (row > m->rows || col > m->cols || height > m->rows - row || width > m->cols - col)
        tb_fail("matrix rectangle out of range");
}
static void @NAME@_fill_rect(@NAME@ *m, size_t row, size_t col,
                             size_t height, size_t width, @TYPE@ value) {
    @NAME@_check_rect(m, row, col, height, width);
    if (!width) return;
    for (size_t r = 0; r < height; ++r)
        for (size_t c = 0; c < width; ++c)
            m->data[(row + r) * m->cols + col + c] = value;
}
static void @NAME@_fill_row(@NAME@ *m, size_t row, @TYPE@ value) {
    @NAME@_fill_rect(m, row, 0, 1, m->cols, value);
}
static void @NAME@_fill_col(@NAME@ *m, size_t col, @TYPE@ value) {
    @NAME@_fill_rect(m, 0, col, m->rows, 1, value);
}
static void @NAME@_draw_rect(@NAME@ *m, size_t row, size_t col,
                             size_t height, size_t width, @TYPE@ value) {
    @NAME@_check_rect(m, row, col, height, width);
    if (!height || !width) return;
    for (size_t r = 0; r < height; ++r) {
        size_t step = tb_matrix_border_step(r, height, width);
        for (size_t c = 0; c < width; c += step) {
            m->data[(row + r) * m->cols + col + c] = value;
            if (c == width - 1) break;
        }
    }
}
static @NAME@ @NAME@_slice(const @NAME@ *m, size_t row, size_t col,
                           size_t height, size_t width) {
    @NAME@_check_rect(m, row, col, height, width);
    @NAME@ out = @NAME@_new(height, width);
    if (width)
        for (size_t r = 0; r < height; ++r)
            memcpy(out.data + r * width, m->data + (row + r) * m->cols + col,
                   width * sizeof *out.data);
    return out;
}
