#ifndef CTB_VEC_UINT_H
#define CTB_VEC_UINT_H
#include "io.h"
/* SPDX-License-Identifier: MIT-0 */
/* Typed, contiguous storage. Elements are shallow copied; no element destructor. */
typedef struct { unsigned int *data; size_t len, cap; } UIntVector;
static UIntVector UIntVector_new(void) { return (UIntVector){0}; }
static void UIntVector_reserve(UIntVector *v, size_t needed) {
    if (needed <= v->cap) return;
    size_t cap = tb_capacity(v->cap, needed);
    v->data = tb_resize(v->data, cap, sizeof *v->data);
    v->cap = cap;
}
static void UIntVector_push(UIntVector *v, unsigned int value) {
    UIntVector_reserve(v, tb_add_size(v->len, 1));
    v->data[v->len++] = value;
}
static bool UIntVector_pop_into(UIntVector *v, unsigned int *out) {
    if (!v->len) return false;
    --v->len;
    if (out) *out = v->data[v->len];
    return true;
}
static bool UIntVector_insert(UIntVector *v, size_t pos, unsigned int value) {
    if (pos > v->len) return false;
    UIntVector_reserve(v, tb_add_size(v->len, 1));
    memmove(v->data + pos + 1, v->data + pos, (v->len - pos) * sizeof *v->data);
    v->data[pos] = value;
    ++v->len;
    return true;
}
static bool UIntVector_erase(UIntVector *v, size_t pos, size_t count) {
    if (pos > v->len || count > v->len - pos) return false;
    if (count) memmove(v->data + pos, v->data + pos + count,
                       (v->len - pos - count) * sizeof *v->data);
    v->len -= count;
    return true;
}
static UIntVector UIntVector_clone(const UIntVector *v) {
    UIntVector copy = {0};
    if (v->len) {
        UIntVector_reserve(&copy, v->len);
        memcpy(copy.data, v->data, v->len * sizeof *v->data);
    }
    copy.len = v->len;
    return copy;
}
static void UIntVector_clear(UIntVector *v) { v->len = 0; }
static void UIntVector_sort(UIntVector *v, int (*compare)(const void *, const void *)) {
    if (v->len > 1) qsort(v->data, v->len, sizeof *v->data, compare);
}
static void UIntVector_free(UIntVector *v) { free(v->data); *v = (UIntVector){0}; }
static void UIntVector_swap(UIntVector *a,UIntVector *b) { UIntVector value=*a;*a=*b;*b=value; }

/* SPDX-License-Identifier: MIT-0 */
static bool UIntVector_get_into(const UIntVector *v, size_t pos, unsigned int *out) {
    if (pos >= v->len) return false;
    if (out) *out = v->data[pos];
    return true;
}
static bool UIntVector_set(UIntVector *v, size_t pos, unsigned int value) {
    if (pos >= v->len) return false;
    v->data[pos] = value;
    return true;
}
static bool UIntVector_front_into(const UIntVector *v, unsigned int *out) {
    return UIntVector_get_into(v, 0, out);
}
static bool UIntVector_back_into(const UIntVector *v, unsigned int *out) {
    return v->len && UIntVector_get_into(v, v->len - 1, out);
}
/* Snapshot supports extending from the vector's own storage. */
static void UIntVector_extend(UIntVector *v, unsigned int const *items, size_t count) {
    if (!count) return;
    size_t length = tb_add_size(v->len, count);
    unsigned int *copy = tb_resize(NULL, count, sizeof *copy);
    memcpy(copy, items, count * sizeof *copy);
    UIntVector_reserve(v, length);
    memcpy(v->data + v->len, copy, count * sizeof *copy);
    v->len = length;
    free(copy);
}
static void UIntVector_resize(UIntVector *v, size_t length, unsigned int fill) {
    UIntVector_reserve(v, length);
    for (size_t i = v->len; i < length; ++i) v->data[i] = fill;
    v->len = length;
}
static void UIntVector_reverse(UIntVector *v) {
    for (size_t i = 0; i < v->len / 2; ++i) {
        unsigned int value = v->data[i];
        v->data[i] = v->data[v->len - 1 - i];
        v->data[v->len - 1 - i] = value;
    }
}
static size_t UIntVector_size(const UIntVector *v) { return v->len; }
static bool UIntVector_empty(const UIntVector *v) { return v->len == 0; }
static unsigned int UIntVector_at(const UIntVector *v, size_t pos) {
    if (pos >= v->len) tb_fail("vector index out of range");
    return v->data[pos];
}
static void UIntVector_fill(UIntVector *v, unsigned int value) {
    for (size_t i = 0; i < v->len; ++i) v->data[i] = value;
}
static UIntVector UIntVector_slice(const UIntVector *v, size_t pos, size_t count) {
    if (pos > v->len || count > v->len - pos) tb_fail("invalid vector slice");
    UIntVector result = UIntVector_new();
    if (count) UIntVector_extend(&result, v->data + pos, count);
    return result;
}
/* Forward half-open slice [first,last) with a positive stride. */
static UIntVector UIntVector_slice_step(const UIntVector *v,size_t first,size_t last,size_t step) {
    if(!step || first>last || last>v->len)tb_fail("invalid vector slice");
    UIntVector result=UIntVector_new();
    size_t count=first==last?0:1+(last-first-1)/step;
    UIntVector_reserve(&result,count);
    for(size_t i=first;i<last;) { UIntVector_push(&result,v->data[i]);if(step>last-1-i)break;i+=step; }
    return result;
}
static void UIntVector_extend_vector(UIntVector *v, const UIntVector *other) {
    UIntVector_extend(v, other->data, other->len);
}

/* SPDX-License-Identifier: MIT-0 */
/* Eager, same-type operations. Element copies follow the vector's shallow policy. */
static UIntVector UIntVector_from(unsigned int const *items, size_t count) {
    UIntVector result = UIntVector_new();
    UIntVector_extend(&result, items, count);
    return result;
}
/* At least one value; use _new() for empty. Arguments are evaluated once. */
#define UIntVector_values(...) UIntVector_from((unsigned int const[]){__VA_ARGS__}, \
    sizeof((unsigned int const[]){__VA_ARGS__}) / sizeof(unsigned int))
static UIntVector UIntVector_filter(const UIntVector *v, bool (*predicate)(unsigned int)) {
    if (!predicate) tb_fail("missing predicate");
    UIntVector result = UIntVector_new();
    for (size_t i = 0; i < v->len; ++i)
        if (predicate(v->data[i])) UIntVector_push(&result, v->data[i]);
    return result;
}
static UIntVector UIntVector_filter_ctx(const UIntVector *v, bool (*predicate)(unsigned int,void *),void *context) {
    if (!predicate) tb_fail("missing predicate");
    UIntVector result=UIntVector_new();
    for(size_t i=0;i<v->len;++i)if(predicate(v->data[i],context))UIntVector_push(&result,v->data[i]);
    return result;
}
static UIntVector UIntVector_map(const UIntVector *v, unsigned int (*transform)(unsigned int)) {
    if (!transform) tb_fail("missing transform");
    UIntVector result = UIntVector_new();
    UIntVector_reserve(&result, v->len);
    for (size_t i = 0; i < v->len; ++i) UIntVector_push(&result, transform(v->data[i]));
    return result;
}
static UIntVector UIntVector_map_ctx(const UIntVector *v,unsigned int (*transform)(unsigned int,void *),void *context) {
    if(!transform)tb_fail("missing transform");
    UIntVector result=UIntVector_new();UIntVector_reserve(&result,v->len);
    for(size_t i=0;i<v->len;++i)UIntVector_push(&result,transform(v->data[i],context));
    return result;
}
static void UIntVector_map_inplace(UIntVector *v, unsigned int (*transform)(unsigned int)) {
    if (!transform) tb_fail("missing transform");
    for (size_t i = 0; i < v->len; ++i) v->data[i] = transform(v->data[i]);
}
static size_t UIntVector_find_if(const UIntVector *v, bool (*predicate)(unsigned int)) {
    if (!predicate) tb_fail("missing predicate");
    for (size_t i = 0; i < v->len; ++i) if (predicate(v->data[i])) return i;
    return SIZE_MAX;
}
static size_t UIntVector_find_if_ctx(const UIntVector *v,bool (*predicate)(unsigned int,void *),void *context) {
    if(!predicate)tb_fail("missing predicate");
    for(size_t i=0;i<v->len;++i)if(predicate(v->data[i],context))return i;
    return SIZE_MAX;
}
static size_t UIntVector_count_if(const UIntVector *v, bool (*predicate)(unsigned int)) {
    if (!predicate) tb_fail("missing predicate");
    size_t count = 0;
    for (size_t i = 0; i < v->len; ++i) if (predicate(v->data[i])) ++count;
    return count;
}
static bool UIntVector_any(const UIntVector *v, bool (*predicate)(unsigned int)) {
    return UIntVector_find_if(v, predicate) != SIZE_MAX;
}
static bool UIntVector_all(const UIntVector *v, bool (*predicate)(unsigned int)) {
    if (!predicate) tb_fail("missing predicate");
    for (size_t i = 0; i < v->len; ++i) if (!predicate(v->data[i])) return false;
    return true;
}
static unsigned int UIntVector_reduce(const UIntVector *v, unsigned int initial,
                           unsigned int (*combine)(unsigned int, unsigned int)) {
    if (!combine) tb_fail("missing reducer");
    unsigned int result = initial;
    for (size_t i = 0; i < v->len; ++i) result = combine(result, v->data[i]);
    return result;
}
static unsigned int UIntVector_reduce_ctx(const UIntVector *v,unsigned int initial,unsigned int (*combine)(unsigned int,unsigned int,void *),void *context) {
    if(!combine)tb_fail("missing reducer");
    for(size_t i=0;i<v->len;++i)initial=combine(initial,v->data[i],context);
    return initial;
}

/* SPDX-License-Identifier: MIT-0 */
#include <math.h>
/* Numeric helpers for int, long long and double. Floating operands must be finite. */
static int UIntVector_compare_asc(const void *a, const void *b) {
    unsigned int x = *(const unsigned int *)a, y = *(const unsigned int *)b;
    return (x > y) - (x < y);
}
static int UIntVector_compare_desc(const void *a, const void *b) {
    return UIntVector_compare_asc(b, a);
}
static void UIntVector_sort_asc(UIntVector *v) { UIntVector_sort(v, UIntVector_compare_asc); }
static void UIntVector_sort_desc(UIntVector *v) { UIntVector_sort(v, UIntVector_compare_desc); }
static size_t UIntVector_find(const UIntVector *v, unsigned int value) {
    for (size_t i = 0; i < v->len; ++i) if (v->data[i] == value) return i;
    return SIZE_MAX;
}
static bool UIntVector_contains(const UIntVector *v, unsigned int value) {
    return UIntVector_find(v, value) != SIZE_MAX;
}
static size_t UIntVector_count(const UIntVector *v, unsigned int value) {
    size_t count = 0;
    for (size_t i = 0; i < v->len; ++i) if (v->data[i] == value) ++count;
    return count;
}
static size_t UIntVector_lower_bound(const UIntVector *v, unsigned int key) {
    size_t left = 0, right = v->len;
    while (left < right) {
        size_t mid = left + (right - left) / 2;
        if (v->data[mid] < key) left = mid + 1;
        else right = mid;
    }
    return left;
}
static size_t UIntVector_upper_bound(const UIntVector *v, unsigned int key) {
    size_t left = 0, right = v->len;
    while (left < right) {
        size_t mid = left + (right - left) / 2;
        if (v->data[mid] <= key) left = mid + 1;
        else right = mid;
    }
    return left;
}
static void UIntVector_unique_sorted(UIntVector *v) {
    size_t written = 0;
    for (size_t i = 0; i < v->len; ++i)
        if (!written || v->data[i] != v->data[written - 1]) v->data[written++] = v->data[i];
    v->len = written;
}
static size_t UIntVector_argmin(const UIntVector *v) {
    if (!v->len) tb_fail("argmin of empty vector");
    size_t best = 0;
    for (size_t i = 1; i < v->len; ++i) if (v->data[i] < v->data[best]) best = i;
    return best;
}
static size_t UIntVector_argmax(const UIntVector *v) {
    if (!v->len) tb_fail("argmax of empty vector");
    size_t best = 0;
    for (size_t i = 1; i < v->len; ++i) if (v->data[i] > v->data[best]) best = i;
    return best;
}
static unsigned int UIntVector_min(const UIntVector *v) { return v->data[UIntVector_argmin(v)]; }
static unsigned int UIntVector_max(const UIntVector *v) { return v->data[UIntVector_argmax(v)]; }
static double UIntVector_mean(const UIntVector *v) {
    if (!v->len) tb_fail("mean of empty vector");
    long double result = 0;
    for (size_t i = 0; i < v->len; ++i)
        result += ((long double)v->data[i] - result) / (long double)(i + 1);
    if (!isfinite((double)result)) tb_fail("nonfinite statistic");
    return (double)result;
}
/* Population variance, dividing by n. Original data is unchanged. */
static double UIntVector_variance(const UIntVector *v) {
    if (!v->len) tb_fail("variance of empty vector");
    long double mean = 0, total = 0;
    for (size_t i = 0; i < v->len; ++i) {
        long double value = v->data[i], delta = value - mean;
        mean += delta / (long double)(i + 1);
        total += delta * (value - mean);
    }
    double result = (double)(total / (long double)v->len);
    if (!isfinite(result)) tb_fail("variance outside double range");
    return result;
}
static double UIntVector_stddev(const UIntVector *v) { return sqrt(UIntVector_variance(v)); }
static double UIntVector_median(const UIntVector *v) {
    if (!v->len) tb_fail("median of empty vector");
    UIntVector copy = UIntVector_clone(v);
    UIntVector_sort_asc(&copy);
    size_t mid = copy.len / 2;
    long double result = copy.data[mid];
    if (!(copy.len % 2)) result = ((long double)copy.data[mid - 1] + result) / 2;
    UIntVector_free(&copy);
    if (!isfinite((double)result)) tb_fail("nonfinite median");
    return (double)result;
}

/* Exact binary lookup on ascending input. */
static size_t UIntVector_binary_search(const UIntVector *v,unsigned int key){size_t i=UIntVector_lower_bound(v,key);return i<v->len && v->data[i]==key?i:SIZE_MAX;}

/* Exact unsigned accumulation; fails before wraparound. */
static unsigned long long UIntVector_sum(const UIntVector *v) {
    unsigned long long result=0;
    for(size_t i=0;i<v->len;++i) {
        unsigned long long value=(unsigned long long)v->data[i];
        if(result>ULLONG_MAX-value)tb_fail("unsigned vector sum overflow");
        result+=value;
    }
    return result;
}

/* SPDX-License-Identifier: MIT-0 */
typedef struct { bool ok; unsigned int value; } UIntVectorResult;
static UIntVectorResult UIntVector_try_get(const UIntVector *v,size_t pos){UIntVectorResult r={0};r.ok=UIntVector_get_into(v,pos,&r.value);return r;}
static unsigned int UIntVector_get(const UIntVector *v,size_t pos){UIntVectorResult r=UIntVector_try_get(v,pos);if(!r.ok)tb_fail("vector index out of range");return r.value;}
static UIntVectorResult UIntVector_try_pop(UIntVector *v){UIntVectorResult r={0};r.ok=UIntVector_pop_into(v,&r.value);return r;}
static unsigned int UIntVector_pop(UIntVector *v){UIntVectorResult r=UIntVector_try_pop(v);if(!r.ok)tb_fail("pop of empty vector");return r.value;}
static unsigned int UIntVector_front(const UIntVector *v){return UIntVector_get(v,0);}
static unsigned int UIntVector_back(const UIntVector *v){if(!v->len)tb_fail("back of empty vector");return UIntVector_get(v,v->len-1);}
static void UIntVector_merge_sort(unsigned int *data,unsigned int *scratch,size_t n,int (*compare)(const unsigned int *,const unsigned int *)){
    if(n<2)return;
    size_t mid=n/2;
    UIntVector_merge_sort(data,scratch,mid,compare);UIntVector_merge_sort(data+mid,scratch+mid,n-mid,compare);
    size_t i=0,j=mid,k=0;
    while(i<mid && j<n){if(compare(&data[i],&data[j])<=0)scratch[k++]=data[i++];else scratch[k++]=data[j++];}
    while(i<mid)scratch[k++]=data[i++];
    while(j<n)scratch[k++]=data[j++];
    memcpy(data,scratch,n*sizeof *data);
}
/* Stable, typed comparison; no void-pointer casts required from callers. */
static void UIntVector_sort_by(UIntVector *v,int (*compare)(const unsigned int *,const unsigned int *)){
    if(!compare)tb_fail("null comparator");
    if(v->len<2)return;
    unsigned int *scratch=tb_resize(NULL,v->len,sizeof *scratch);UIntVector_merge_sort(v->data,scratch,v->len,compare);free(scratch);
}

static UIntVector read_uints(size_t count) { UIntVector v=UIntVector_new();UIntVector_reserve(&v,count);for(size_t i=0;i<count;++i) UIntVector_push(&v,read_uint());return v;}

static void print_uints(const UIntVector *v,const char *sep,const char *end) {for(size_t i=0;i<v->len;++i){if(i)print_text(sep,"");print_uint(v->data[i],"");}print_text("",end);}

#endif
