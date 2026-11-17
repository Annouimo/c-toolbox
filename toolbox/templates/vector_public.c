/* SPDX-License-Identifier: MIT-0 */
typedef struct { bool ok; @TYPE@ value; } @NAME@Result;
static @NAME@Result @NAME@_try_get(const @NAME@ *v,size_t pos){@NAME@Result r={false,(@TYPE@){0}};r.ok=@NAME@_get_into(v,pos,&r.value);return r;}
static @TYPE@ @NAME@_get(const @NAME@ *v,size_t pos){@NAME@Result r=@NAME@_try_get(v,pos);if(!r.ok)tb_fail("vector index out of range");return r.value;}
static @NAME@Result @NAME@_try_pop(@NAME@ *v){@NAME@Result r={false,(@TYPE@){0}};r.ok=@NAME@_pop_into(v,&r.value);return r;}
static @TYPE@ @NAME@_pop(@NAME@ *v){@NAME@Result r=@NAME@_try_pop(v);if(!r.ok)tb_fail("pop of empty vector");return r.value;}
static @TYPE@ @NAME@_front(const @NAME@ *v){return @NAME@_get(v,0);}
static @TYPE@ @NAME@_back(const @NAME@ *v){if(!v->len)tb_fail("back of empty vector");return @NAME@_get(v,v->len-1);}
static void @NAME@_merge_sort(@TYPE@ *data,@TYPE@ *scratch,size_t n,int (*compare)(const @TYPE@ *,const @TYPE@ *)){
    if(n<2)return;
    size_t mid=n/2;
    @NAME@_merge_sort(data,scratch,mid,compare);@NAME@_merge_sort(data+mid,scratch+mid,n-mid,compare);
    size_t i=0,j=mid,k=0;
    while(i<mid && j<n){if(compare(&data[i],&data[j])<=0)scratch[k++]=data[i++];else scratch[k++]=data[j++];}
    while(i<mid)scratch[k++]=data[i++];
    while(j<n)scratch[k++]=data[j++];
    memcpy(data,scratch,n*sizeof *data);
}
/* Stable, typed comparison; no void-pointer casts required from callers. */
static void @NAME@_sort_by(@NAME@ *v,int (*compare)(const @TYPE@ *,const @TYPE@ *)){
    if(!compare)tb_fail("null comparator");
    if(v->len<2)return;
    @TYPE@ *scratch=tb_resize(NULL,v->len,sizeof *scratch);@NAME@_merge_sort(v->data,scratch,v->len,compare);free(scratch);
}
