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
