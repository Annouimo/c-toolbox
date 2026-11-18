/* SPDX-License-Identifier: MIT-0 */
static @NAME@ @NAME@_multiply(const @NAME@ *a,const @NAME@ *b) {
    if(a->cols!=b->rows)tb_fail("matrix dimension mismatch");
    @NAME@ result=@NAME@_new(a->rows,b->cols);
    for(size_t i=0;i<a->rows;++i)for(size_t j=0;j<b->cols;++j) {
        @ACCUMULATOR@ total=0;
        for(size_t k=0;k<a->cols;++k) {
            @TYPE@ x = a->data[i*a->cols+k];
            @TYPE@ y = b->data[k*b->cols+j];
            @ACCUMULATE@
        }
        @CHECK@
        result.data[i*result.cols+j]=(@TYPE@)total;
    }
    return result;
}
static @NAME@ read_@SUFFIX@_matrix(size_t rows,size_t cols) {
    @NAME@ m=@NAME@_new(rows,cols);size_t n=rows*cols;
    for(size_t i=0;i<n;++i)m.data[i]=@READ@();
    return m;
}
static @ACCUMULATOR@ @NAME@_sum_region(const @NAME@ *m, size_t row, size_t col,
                                      size_t height, size_t width, bool border) {
    @NAME@_check_rect(m, row, col, height, width);
    @ACCUMULATOR@ total = 0;
    if (!width) return total;
    for (size_t r = 0; r < height; ++r) {
        size_t step = border ? tb_matrix_border_step(r, height, width) : 1;
        for (size_t c = 0; c < width; c += step) {
            @TYPE@ value = m->data[(row + r) * m->cols + col + c];
            @SUM@
            if (c == width - 1) break;
        }
    }
    return total;
}
static @ACCUMULATOR@ @NAME@_sum_rect(const @NAME@ *m, size_t row, size_t col,
                                    size_t height, size_t width) {
    return @NAME@_sum_region(m, row, col, height, width, false);
}
static @ACCUMULATOR@ @NAME@_sum_border(const @NAME@ *m, size_t row, size_t col,
                                      size_t height, size_t width) {
    return @NAME@_sum_region(m, row, col, height, width, true);
}
