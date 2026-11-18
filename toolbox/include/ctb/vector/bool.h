#ifndef CTB_VECTOR_BOOL_H
#define CTB_VECTOR_BOOL_H
#include "../io.h"
/* SPDX-License-Identifier: MIT-0 */
/* Typed, contiguous storage. Elements are shallow copied; no element destructor. */
typedef struct { bool *data; size_t len, cap; } BoolVector;
static BoolVector BoolVector_new(void) { return (BoolVector){0}; }
static void BoolVector_reserve(BoolVector *v, size_t needed) {
    if (needed <= v->cap) return;
    size_t cap = tb_capacity(v->cap, needed);
    v->data = tb_resize(v->data, cap, sizeof *v->data);
    v->cap = cap;
}
static void BoolVector_push(BoolVector *v, bool value) {
    BoolVector_reserve(v, tb_add_size(v->len, 1));
    v->data[v->len++] = value;
}
static bool BoolVector_pop_into(BoolVector *v, bool *out) {
    if (!v->len) return false;
    --v->len;
    if (out) *out = v->data[v->len];
    return true;
}
static bool BoolVector_insert(BoolVector *v, size_t pos, bool value) {
    if (pos > v->len) return false;
    BoolVector_reserve(v, tb_add_size(v->len, 1));
    memmove(v->data + pos + 1, v->data + pos, (v->len - pos) * sizeof *v->data);
    v->data[pos] = value;
    ++v->len;
    return true;
}
static bool BoolVector_erase(BoolVector *v, size_t pos, size_t count) {
    if (pos > v->len || count > v->len - pos) return false;
    if (count) memmove(v->data + pos, v->data + pos + count,
                       (v->len - pos - count) * sizeof *v->data);
    v->len -= count;
    return true;
}
static BoolVector BoolVector_clone(const BoolVector *v) {
    BoolVector copy = {0};
    if (v->len) {
        BoolVector_reserve(&copy, v->len);
        memcpy(copy.data, v->data, v->len * sizeof *v->data);
    }
    copy.len = v->len;
    return copy;
}
static void BoolVector_clear(BoolVector *v) { v->len = 0; }
static void BoolVector_sort(BoolVector *v, int (*compare)(const void *, const void *)) {
    if (v->len > 1) qsort(v->data, v->len, sizeof *v->data, compare);
}
static void BoolVector_free(BoolVector *v) { free(v->data); *v = (BoolVector){0}; }
static void BoolVector_swap(BoolVector *a,BoolVector *b) { BoolVector value=*a;*a=*b;*b=value; }

/* SPDX-License-Identifier: MIT-0 */
static bool BoolVector_get_into(const BoolVector *v, size_t pos, bool *out) {
    if (pos >= v->len) return false;
    if (out) *out = v->data[pos];
    return true;
}
static bool BoolVector_set(BoolVector *v, size_t pos, bool value) {
    if (pos >= v->len) return false;
    v->data[pos] = value;
    return true;
}
static bool BoolVector_front_into(const BoolVector *v, bool *out) {
    return BoolVector_get_into(v, 0, out);
}
static bool BoolVector_back_into(const BoolVector *v, bool *out) {
    return v->len && BoolVector_get_into(v, v->len - 1, out);
}
/* Snapshot supports extending from the vector's own storage. */
static void BoolVector_extend(BoolVector *v, bool const *items, size_t count) {
    if (!count) return;
    size_t length = tb_add_size(v->len, count);
    bool *copy = tb_resize(NULL, count, sizeof *copy);
    memcpy(copy, items, count * sizeof *copy);
    BoolVector_reserve(v, length);
    memcpy(v->data + v->len, copy, count * sizeof *copy);
    v->len = length;
    free(copy);
}
static void BoolVector_resize(BoolVector *v, size_t length, bool fill) {
    BoolVector_reserve(v, length);
    for (size_t i = v->len; i < length; ++i) v->data[i] = fill;
    v->len = length;
}
static void BoolVector_reverse(BoolVector *v) {
    for (size_t i = 0; i < v->len / 2; ++i) {
        bool value = v->data[i];
        v->data[i] = v->data[v->len - 1 - i];
        v->data[v->len - 1 - i] = value;
    }
}
static size_t BoolVector_size(const BoolVector *v) { return v->len; }
static bool BoolVector_empty(const BoolVector *v) { return v->len == 0; }
static bool BoolVector_at(const BoolVector *v, size_t pos) {
    if (pos >= v->len) tb_fail("vector index out of range");
    return v->data[pos];
}
static void BoolVector_fill(BoolVector *v, bool value) {
    for (size_t i = 0; i < v->len; ++i) v->data[i] = value;
}
static BoolVector BoolVector_slice(const BoolVector *v, size_t pos, size_t count) {
    if (pos > v->len || count > v->len - pos) tb_fail("invalid vector slice");
    BoolVector result = BoolVector_new();
    if (count) BoolVector_extend(&result, v->data + pos, count);
    return result;
}
/* Forward half-open slice [first,last) with a positive stride. */
static BoolVector BoolVector_slice_step(const BoolVector *v,size_t first,size_t last,size_t step) {
    if(!step || first>last || last>v->len)tb_fail("invalid vector slice");
    BoolVector result=BoolVector_new();
    size_t count=first==last?0:1+(last-first-1)/step;
    BoolVector_reserve(&result,count);
    for(size_t i=first;i<last;) { BoolVector_push(&result,v->data[i]);if(step>last-1-i)break;i+=step; }
    return result;
}
static void BoolVector_extend_vector(BoolVector *v, const BoolVector *other) {
    BoolVector_extend(v, other->data, other->len);
}

/* SPDX-License-Identifier: MIT-0 */
/* Eager, same-type operations. Element copies follow the vector's shallow policy. */
static BoolVector BoolVector_from(bool const *items, size_t count) {
    BoolVector result = BoolVector_new();
    BoolVector_extend(&result, items, count);
    return result;
}
/* At least one value; use _new() for empty. Arguments are evaluated once. */
#define BoolVector_values(...) BoolVector_from((bool const[]){__VA_ARGS__}, \
    sizeof((bool const[]){__VA_ARGS__}) / sizeof(bool))
static BoolVector BoolVector_filter(const BoolVector *v, bool (*predicate)(bool)) {
    if (!predicate) tb_fail("missing predicate");
    BoolVector result = BoolVector_new();
    for (size_t i = 0; i < v->len; ++i)
        if (predicate(v->data[i])) BoolVector_push(&result, v->data[i]);
    return result;
}
static BoolVector BoolVector_filter_ctx(const BoolVector *v, bool (*predicate)(bool,void *),void *context) {
    if (!predicate) tb_fail("missing predicate");
    BoolVector result=BoolVector_new();
    for(size_t i=0;i<v->len;++i)if(predicate(v->data[i],context))BoolVector_push(&result,v->data[i]);
    return result;
}
static BoolVector BoolVector_map(const BoolVector *v, bool (*transform)(bool)) {
    if (!transform) tb_fail("missing transform");
    BoolVector result = BoolVector_new();
    BoolVector_reserve(&result, v->len);
    for (size_t i = 0; i < v->len; ++i) BoolVector_push(&result, transform(v->data[i]));
    return result;
}
static BoolVector BoolVector_map_ctx(const BoolVector *v,bool (*transform)(bool,void *),void *context) {
    if(!transform)tb_fail("missing transform");
    BoolVector result=BoolVector_new();BoolVector_reserve(&result,v->len);
    for(size_t i=0;i<v->len;++i)BoolVector_push(&result,transform(v->data[i],context));
    return result;
}
static void BoolVector_map_inplace(BoolVector *v, bool (*transform)(bool)) {
    if (!transform) tb_fail("missing transform");
    for (size_t i = 0; i < v->len; ++i) v->data[i] = transform(v->data[i]);
}
static size_t BoolVector_find_if(const BoolVector *v, bool (*predicate)(bool)) {
    if (!predicate) tb_fail("missing predicate");
    for (size_t i = 0; i < v->len; ++i) if (predicate(v->data[i])) return i;
    return SIZE_MAX;
}
static size_t BoolVector_find_if_ctx(const BoolVector *v,bool (*predicate)(bool,void *),void *context) {
    if(!predicate)tb_fail("missing predicate");
    for(size_t i=0;i<v->len;++i)if(predicate(v->data[i],context))return i;
    return SIZE_MAX;
}
static size_t BoolVector_count_if(const BoolVector *v, bool (*predicate)(bool)) {
    if (!predicate) tb_fail("missing predicate");
    size_t count = 0;
    for (size_t i = 0; i < v->len; ++i) if (predicate(v->data[i])) ++count;
    return count;
}
static bool BoolVector_any(const BoolVector *v, bool (*predicate)(bool)) {
    return BoolVector_find_if(v, predicate) != SIZE_MAX;
}
static bool BoolVector_all(const BoolVector *v, bool (*predicate)(bool)) {
    if (!predicate) tb_fail("missing predicate");
    for (size_t i = 0; i < v->len; ++i) if (!predicate(v->data[i])) return false;
    return true;
}
static bool BoolVector_reduce(const BoolVector *v, bool initial,
                           bool (*combine)(bool, bool)) {
    if (!combine) tb_fail("missing reducer");
    bool result = initial;
    for (size_t i = 0; i < v->len; ++i) result = combine(result, v->data[i]);
    return result;
}
static bool BoolVector_reduce_ctx(const BoolVector *v,bool initial,bool (*combine)(bool,bool,void *),void *context) {
    if(!combine)tb_fail("missing reducer");
    for(size_t i=0;i<v->len;++i)initial=combine(initial,v->data[i],context);
    return initial;
}

/* SPDX-License-Identifier: MIT-0 */
typedef struct { bool ok; bool value; } BoolVectorResult;
static BoolVectorResult BoolVector_try_get(const BoolVector *v,size_t pos){BoolVectorResult r={0};r.ok=BoolVector_get_into(v,pos,&r.value);return r;}
static bool BoolVector_get(const BoolVector *v,size_t pos){BoolVectorResult r=BoolVector_try_get(v,pos);if(!r.ok)tb_fail("vector index out of range");return r.value;}
static BoolVectorResult BoolVector_try_pop(BoolVector *v){BoolVectorResult r={0};r.ok=BoolVector_pop_into(v,&r.value);return r;}
static bool BoolVector_pop(BoolVector *v){BoolVectorResult r=BoolVector_try_pop(v);if(!r.ok)tb_fail("pop of empty vector");return r.value;}
static bool BoolVector_front(const BoolVector *v){return BoolVector_get(v,0);}
static bool BoolVector_back(const BoolVector *v){if(!v->len)tb_fail("back of empty vector");return BoolVector_get(v,v->len-1);}
static void BoolVector_merge_sort(bool *data,bool *scratch,size_t n,int (*compare)(const bool *,const bool *)){
    if(n<2)return;
    size_t mid=n/2;
    BoolVector_merge_sort(data,scratch,mid,compare);BoolVector_merge_sort(data+mid,scratch+mid,n-mid,compare);
    size_t i=0,j=mid,k=0;
    while(i<mid && j<n){if(compare(&data[i],&data[j])<=0)scratch[k++]=data[i++];else scratch[k++]=data[j++];}
    while(i<mid)scratch[k++]=data[i++];
    while(j<n)scratch[k++]=data[j++];
    memcpy(data,scratch,n*sizeof *data);
}
/* Stable, typed comparison; no void-pointer casts required from callers. */
static void BoolVector_sort_by(BoolVector *v,int (*compare)(const bool *,const bool *)){
    if(!compare)tb_fail("null comparator");
    if(v->len<2)return;
    bool *scratch=tb_resize(NULL,v->len,sizeof *scratch);BoolVector_merge_sort(v->data,scratch,v->len,compare);free(scratch);
}

#endif
