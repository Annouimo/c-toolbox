#ifndef CTB_VEC_SIZE_H
#define CTB_VEC_SIZE_H
#include "io.h"
/* SPDX-License-Identifier: MIT-0 */
/* Typed, contiguous storage. Elements are shallow copied; no element destructor. */
typedef struct { size_t *data; size_t len, cap; } IndexVector;
static IndexVector IndexVector_new(void) { return (IndexVector){0}; }
static void IndexVector_reserve(IndexVector *v, size_t needed) {
    if (needed <= v->cap) return;
    size_t cap = tb_capacity(v->cap, needed);
    v->data = tb_resize(v->data, cap, sizeof *v->data);
    v->cap = cap;
}
static void IndexVector_push(IndexVector *v, size_t value) {
    IndexVector_reserve(v, tb_add_size(v->len, 1));
    v->data[v->len++] = value;
}
static bool IndexVector_pop_into(IndexVector *v, size_t *out) {
    if (!v->len) return false;
    --v->len;
    if (out) *out = v->data[v->len];
    return true;
}
static bool IndexVector_insert(IndexVector *v, size_t pos, size_t value) {
    if (pos > v->len) return false;
    IndexVector_reserve(v, tb_add_size(v->len, 1));
    memmove(v->data + pos + 1, v->data + pos, (v->len - pos) * sizeof *v->data);
    v->data[pos] = value;
    ++v->len;
    return true;
}
static bool IndexVector_erase(IndexVector *v, size_t pos, size_t count) {
    if (pos > v->len || count > v->len - pos) return false;
    if (count) memmove(v->data + pos, v->data + pos + count,
                       (v->len - pos - count) * sizeof *v->data);
    v->len -= count;
    return true;
}
static IndexVector IndexVector_clone(const IndexVector *v) {
    IndexVector copy = {0};
    if (v->len) {
        IndexVector_reserve(&copy, v->len);
        memcpy(copy.data, v->data, v->len * sizeof *v->data);
    }
    copy.len = v->len;
    return copy;
}
static void IndexVector_clear(IndexVector *v) { v->len = 0; }
static void IndexVector_sort(IndexVector *v, int (*compare)(const void *, const void *)) {
    if (v->len > 1) qsort(v->data, v->len, sizeof *v->data, compare);
}
static void IndexVector_free(IndexVector *v) { free(v->data); *v = (IndexVector){0}; }
static void IndexVector_swap(IndexVector *a,IndexVector *b) { IndexVector value=*a;*a=*b;*b=value; }

/* SPDX-License-Identifier: MIT-0 */
static bool IndexVector_get_into(const IndexVector *v, size_t pos, size_t *out) {
    if (pos >= v->len) return false;
    if (out) *out = v->data[pos];
    return true;
}
static bool IndexVector_set(IndexVector *v, size_t pos, size_t value) {
    if (pos >= v->len) return false;
    v->data[pos] = value;
    return true;
}
static bool IndexVector_front_into(const IndexVector *v, size_t *out) {
    return IndexVector_get_into(v, 0, out);
}
static bool IndexVector_back_into(const IndexVector *v, size_t *out) {
    return v->len && IndexVector_get_into(v, v->len - 1, out);
}
/* Snapshot supports extending from the vector's own storage. */
static void IndexVector_extend(IndexVector *v, size_t const *items, size_t count) {
    if (!count) return;
    size_t length = tb_add_size(v->len, count);
    size_t *copy = tb_resize(NULL, count, sizeof *copy);
    memcpy(copy, items, count * sizeof *copy);
    IndexVector_reserve(v, length);
    memcpy(v->data + v->len, copy, count * sizeof *copy);
    v->len = length;
    free(copy);
}
static void IndexVector_resize(IndexVector *v, size_t length, size_t fill) {
    IndexVector_reserve(v, length);
    for (size_t i = v->len; i < length; ++i) v->data[i] = fill;
    v->len = length;
}
static void IndexVector_reverse(IndexVector *v) {
    for (size_t i = 0; i < v->len / 2; ++i) {
        size_t value = v->data[i];
        v->data[i] = v->data[v->len - 1 - i];
        v->data[v->len - 1 - i] = value;
    }
}
static size_t IndexVector_size(const IndexVector *v) { return v->len; }
static bool IndexVector_empty(const IndexVector *v) { return v->len == 0; }
static size_t IndexVector_at(const IndexVector *v, size_t pos) {
    if (pos >= v->len) tb_fail("vector index out of range");
    return v->data[pos];
}
static void IndexVector_fill(IndexVector *v, size_t value) {
    for (size_t i = 0; i < v->len; ++i) v->data[i] = value;
}
static IndexVector IndexVector_slice(const IndexVector *v, size_t pos, size_t count) {
    if (pos > v->len || count > v->len - pos) tb_fail("invalid vector slice");
    IndexVector result = IndexVector_new();
    if (count) IndexVector_extend(&result, v->data + pos, count);
    return result;
}
/* Forward half-open slice [first,last) with a positive stride. */
static IndexVector IndexVector_slice_step(const IndexVector *v,size_t first,size_t last,size_t step) {
    if(!step || first>last || last>v->len)tb_fail("invalid vector slice");
    IndexVector result=IndexVector_new();
    size_t count=first==last?0:1+(last-first-1)/step;
    IndexVector_reserve(&result,count);
    for(size_t i=first;i<last;) { IndexVector_push(&result,v->data[i]);if(step>last-1-i)break;i+=step; }
    return result;
}
static void IndexVector_extend_vector(IndexVector *v, const IndexVector *other) {
    IndexVector_extend(v, other->data, other->len);
}

/* SPDX-License-Identifier: MIT-0 */
/* Eager, same-type operations. Element copies follow the vector's shallow policy. */
static IndexVector IndexVector_from(size_t const *items, size_t count) {
    IndexVector result = IndexVector_new();
    IndexVector_extend(&result, items, count);
    return result;
}
/* At least one value; use _new() for empty. Arguments are evaluated once. */
#define IndexVector_values(...) IndexVector_from((size_t const[]){__VA_ARGS__}, \
    sizeof((size_t const[]){__VA_ARGS__}) / sizeof(size_t))
static IndexVector IndexVector_filter(const IndexVector *v, bool (*predicate)(size_t)) {
    if (!predicate) tb_fail("missing predicate");
    IndexVector result = IndexVector_new();
    for (size_t i = 0; i < v->len; ++i)
        if (predicate(v->data[i])) IndexVector_push(&result, v->data[i]);
    return result;
}
static IndexVector IndexVector_filter_ctx(const IndexVector *v, bool (*predicate)(size_t,void *),void *context) {
    if (!predicate) tb_fail("missing predicate");
    IndexVector result=IndexVector_new();
    for(size_t i=0;i<v->len;++i)if(predicate(v->data[i],context))IndexVector_push(&result,v->data[i]);
    return result;
}
static IndexVector IndexVector_map(const IndexVector *v, size_t (*transform)(size_t)) {
    if (!transform) tb_fail("missing transform");
    IndexVector result = IndexVector_new();
    IndexVector_reserve(&result, v->len);
    for (size_t i = 0; i < v->len; ++i) IndexVector_push(&result, transform(v->data[i]));
    return result;
}
static IndexVector IndexVector_map_ctx(const IndexVector *v,size_t (*transform)(size_t,void *),void *context) {
    if(!transform)tb_fail("missing transform");
    IndexVector result=IndexVector_new();IndexVector_reserve(&result,v->len);
    for(size_t i=0;i<v->len;++i)IndexVector_push(&result,transform(v->data[i],context));
    return result;
}
static void IndexVector_map_inplace(IndexVector *v, size_t (*transform)(size_t)) {
    if (!transform) tb_fail("missing transform");
    for (size_t i = 0; i < v->len; ++i) v->data[i] = transform(v->data[i]);
}
static size_t IndexVector_find_if(const IndexVector *v, bool (*predicate)(size_t)) {
    if (!predicate) tb_fail("missing predicate");
    for (size_t i = 0; i < v->len; ++i) if (predicate(v->data[i])) return i;
    return SIZE_MAX;
}
static size_t IndexVector_find_if_ctx(const IndexVector *v,bool (*predicate)(size_t,void *),void *context) {
    if(!predicate)tb_fail("missing predicate");
    for(size_t i=0;i<v->len;++i)if(predicate(v->data[i],context))return i;
    return SIZE_MAX;
}
static size_t IndexVector_count_if(const IndexVector *v, bool (*predicate)(size_t)) {
    if (!predicate) tb_fail("missing predicate");
    size_t count = 0;
    for (size_t i = 0; i < v->len; ++i) if (predicate(v->data[i])) ++count;
    return count;
}
static bool IndexVector_any(const IndexVector *v, bool (*predicate)(size_t)) {
    return IndexVector_find_if(v, predicate) != SIZE_MAX;
}
static bool IndexVector_all(const IndexVector *v, bool (*predicate)(size_t)) {
    if (!predicate) tb_fail("missing predicate");
    for (size_t i = 0; i < v->len; ++i) if (!predicate(v->data[i])) return false;
    return true;
}
static size_t IndexVector_reduce(const IndexVector *v, size_t initial,
                           size_t (*combine)(size_t, size_t)) {
    if (!combine) tb_fail("missing reducer");
    size_t result = initial;
    for (size_t i = 0; i < v->len; ++i) result = combine(result, v->data[i]);
    return result;
}
static size_t IndexVector_reduce_ctx(const IndexVector *v,size_t initial,size_t (*combine)(size_t,size_t,void *),void *context) {
    if(!combine)tb_fail("missing reducer");
    for(size_t i=0;i<v->len;++i)initial=combine(initial,v->data[i],context);
    return initial;
}

/* SPDX-License-Identifier: MIT-0 */
typedef struct { bool ok; size_t value; } IndexVectorResult;
static IndexVectorResult IndexVector_try_get(const IndexVector *v,size_t pos){IndexVectorResult r={false,(size_t){0}};r.ok=IndexVector_get_into(v,pos,&r.value);return r;}
static size_t IndexVector_get(const IndexVector *v,size_t pos){IndexVectorResult r=IndexVector_try_get(v,pos);if(!r.ok)tb_fail("vector index out of range");return r.value;}
static IndexVectorResult IndexVector_try_pop(IndexVector *v){IndexVectorResult r={false,(size_t){0}};r.ok=IndexVector_pop_into(v,&r.value);return r;}
static size_t IndexVector_pop(IndexVector *v){IndexVectorResult r=IndexVector_try_pop(v);if(!r.ok)tb_fail("pop of empty vector");return r.value;}
static size_t IndexVector_front(const IndexVector *v){return IndexVector_get(v,0);}
static size_t IndexVector_back(const IndexVector *v){if(!v->len)tb_fail("back of empty vector");return IndexVector_get(v,v->len-1);}
static void IndexVector_merge_sort(size_t *data,size_t *scratch,size_t n,int (*compare)(const size_t *,const size_t *)){
    if(n<2)return;
    size_t mid=n/2;
    IndexVector_merge_sort(data,scratch,mid,compare);IndexVector_merge_sort(data+mid,scratch+mid,n-mid,compare);
    size_t i=0,j=mid,k=0;
    while(i<mid && j<n){if(compare(&data[i],&data[j])<=0)scratch[k++]=data[i++];else scratch[k++]=data[j++];}
    while(i<mid)scratch[k++]=data[i++];
    while(j<n)scratch[k++]=data[j++];
    memcpy(data,scratch,n*sizeof *data);
}
/* Stable, typed comparison; no void-pointer casts required from callers. */
static void IndexVector_sort_by(IndexVector *v,int (*compare)(const size_t *,const size_t *)){
    if(!compare)tb_fail("null comparator");
    if(v->len<2)return;
    size_t *scratch=tb_resize(NULL,v->len,sizeof *scratch);IndexVector_merge_sort(v->data,scratch,v->len,compare);free(scratch);
}

static IndexVector read_sizes(size_t count) { IndexVector v=IndexVector_new();IndexVector_reserve(&v,count);for(size_t i=0;i<count;++i) IndexVector_push(&v,read_ull());return v;}

static void print_sizes(const IndexVector *v,const char *sep,const char *end) {for(size_t i=0;i<v->len;++i){if(i)print_text(sep,"");print_ull(v->data[i],"");}print_text("",end);}

#endif
