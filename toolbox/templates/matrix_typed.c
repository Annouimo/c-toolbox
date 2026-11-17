/* Generated typed integer matrix; values are owned and row-major. */
typedef struct { @TYPE@ *data; size_t rows, cols; } @NAME@;
static @NAME@ @NAME@_new(size_t rows,size_t cols) {
    if(cols && rows>SIZE_MAX/cols)tb_fail("matrix size overflow");
    size_t n=rows*cols;@NAME@ m={n?tb_resize(NULL,n,sizeof *m.data):NULL,rows,cols};
    for(size_t i=0;i<n;++i)m.data[i]=0;
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
static @NAME@ @NAME@_multiply(const @NAME@ *a,const @NAME@ *b) {
    if(a->cols!=b->rows)tb_fail("matrix dimension mismatch");
    @NAME@ result=@NAME@_new(a->rows,b->cols);
    for(size_t i=0;i<a->rows;++i)for(size_t j=0;j<b->cols;++j) {
        long long total=0;
        for(size_t k=0;k<a->cols;++k) {
            long long product;
            @MUL@
            if((product>0 && total>LLONG_MAX-product)||(product<0 && total<LLONG_MIN-product))tb_fail("matrix multiplication overflow");
            total+=product;
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
static void print_@SUFFIX@_matrix(const @NAME@ *m,const char *separator,const char *end) {
    for(size_t i=0;i<m->rows;++i) {
        for(size_t j=0;j<m->cols;++j) { if(j)print_text(separator,"");@PRINT@(m->data[i*m->cols+j],""); }
        print_char('\n',"");
    }
    print_text("",end);
}
static void @NAME@_free(@NAME@ *m) { free(m->data);*m=(@NAME@){0}; }
static void @NAME@_swap(@NAME@ *a,@NAME@ *b) { @NAME@ x=*a;*a=*b;*b=x; }
