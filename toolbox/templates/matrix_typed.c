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
static void print_@SUFFIX@_matrix(const @NAME@ *m@PRECISION@,const char *separator,const char *end) {
    for(size_t i=0;i<m->rows;++i) {
        for(size_t j=0;j<m->cols;++j) { if(j)print_text(separator,"");@TYPE@ value = m->data[i*m->cols+j]; @PRINT@; }
        print_char('\n',"");
    }
    print_text("",end);
}
