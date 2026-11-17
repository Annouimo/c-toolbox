typedef struct { const StringList *list; int (*compare)(const void *,const void *); } StringSortContext;
static int tb_string_index_compare(const StringSortContext *ctx,size_t a,size_t b) {
    int order=ctx->compare(&ctx->list->data[a],&ctx->list->data[b]);
    return order?order:(a<b?-1:a>b?1:0);
}
static void tb_string_index_merge_sort(size_t *data,size_t *scratch,size_t n,const StringSortContext *ctx) {
    if(n<2)return;
    size_t mid=n/2;
    tb_string_index_merge_sort(data,scratch,mid,ctx);tb_string_index_merge_sort(data+mid,scratch+mid,n-mid,ctx);
    size_t i=0,j=mid,k=0;
    while(i<mid && j<n)scratch[k++]=tb_string_index_compare(ctx,data[i],data[j])<=0?data[i++]:data[j++];
    while(i<mid)scratch[k++]=data[i++];
    while(j<n)scratch[k++]=data[j++];
    memcpy(data,scratch,n*sizeof *data);
}
static IndexVector string_list_argsort_by(const StringList *list,int (*compare)(const void *,const void *)) {
    if(!compare)tb_fail("null comparator");
    IndexVector indices=IndexVector_new();IndexVector_reserve(&indices,list->len);
    for(size_t i=0;i<list->len;++i)IndexVector_push(&indices,i);
    if(indices.len>1) { size_t *scratch=tb_resize(NULL,indices.len,sizeof *scratch);StringSortContext ctx={list,compare};
        tb_string_index_merge_sort(indices.data,scratch,indices.len,&ctx);free(scratch); }
    return indices;
}
static IndexVector string_list_argsort(const StringList *list) { return string_list_argsort_by(list,tb_string_compare); }
