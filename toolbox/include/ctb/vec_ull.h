#ifndef CTB_VEC_ULL_H
#define CTB_VEC_ULL_H
#include "io.h"
/* SPDX-License-Identifier: MIT-0 */
/* Typed, contiguous storage. Elements are shallow copied; no element destructor. */
typedef struct { unsigned long long *data; size_t len, cap; } ULongLongVector;
static ULongLongVector ULongLongVector_new(void) { return (ULongLongVector){0}; }
static void ULongLongVector_reserve(ULongLongVector *v, size_t needed) {
    if (needed <= v->cap) return;
    size_t cap = tb_capacity(v->cap, needed);
    v->data = tb_resize(v->data, cap, sizeof *v->data);
    v->cap = cap;
}
static void ULongLongVector_push(ULongLongVector *v, unsigned long long value) {
    ULongLongVector_reserve(v, tb_add_size(v->len, 1));
    v->data[v->len++] = value;
}
static bool ULongLongVector_pop_into(ULongLongVector *v, unsigned long long *out) {
    if (!v->len) return false;
    --v->len;
    if (out) *out = v->data[v->len];
    return true;
}
static bool ULongLongVector_insert(ULongLongVector *v, size_t pos, unsigned long long value) {
    if (pos > v->len) return false;
    ULongLongVector_reserve(v, tb_add_size(v->len, 1));
    memmove(v->data + pos + 1, v->data + pos, (v->len - pos) * sizeof *v->data);
    v->data[pos] = value;
    ++v->len;
    return true;
}
static bool ULongLongVector_erase(ULongLongVector *v, size_t pos, size_t count) {
    if (pos > v->len || count > v->len - pos) return false;
    if (count) memmove(v->data + pos, v->data + pos + count,
                       (v->len - pos - count) * sizeof *v->data);
    v->len -= count;
    return true;
}
static ULongLongVector ULongLongVector_clone(const ULongLongVector *v) {
    ULongLongVector copy = {0};
    if (v->len) {
        ULongLongVector_reserve(&copy, v->len);
        memcpy(copy.data, v->data, v->len * sizeof *v->data);
    }
    copy.len = v->len;
    return copy;
}
static void ULongLongVector_clear(ULongLongVector *v) { v->len = 0; }
static void ULongLongVector_sort(ULongLongVector *v, int (*compare)(const void *, const void *)) {
    if (v->len > 1) qsort(v->data, v->len, sizeof *v->data, compare);
}
static void ULongLongVector_free(ULongLongVector *v) { free(v->data); *v = (ULongLongVector){0}; }
static void ULongLongVector_swap(ULongLongVector *a,ULongLongVector *b) { ULongLongVector value=*a;*a=*b;*b=value; }

/* SPDX-License-Identifier: MIT-0 */
static bool ULongLongVector_get_into(const ULongLongVector *v, size_t pos, unsigned long long *out) {
    if (pos >= v->len) return false;
    if (out) *out = v->data[pos];
    return true;
}
static bool ULongLongVector_set(ULongLongVector *v, size_t pos, unsigned long long value) {
    if (pos >= v->len) return false;
    v->data[pos] = value;
    return true;
}
static bool ULongLongVector_front_into(const ULongLongVector *v, unsigned long long *out) {
    return ULongLongVector_get_into(v, 0, out);
}
static bool ULongLongVector_back_into(const ULongLongVector *v, unsigned long long *out) {
    return v->len && ULongLongVector_get_into(v, v->len - 1, out);
}
/* Snapshot supports extending from the vector's own storage. */
static void ULongLongVector_extend(ULongLongVector *v, unsigned long long const *items, size_t count) {
    if (!count) return;
    size_t length = tb_add_size(v->len, count);
    unsigned long long *copy = tb_resize(NULL, count, sizeof *copy);
    memcpy(copy, items, count * sizeof *copy);
    ULongLongVector_reserve(v, length);
    memcpy(v->data + v->len, copy, count * sizeof *copy);
    v->len = length;
    free(copy);
}
static void ULongLongVector_resize(ULongLongVector *v, size_t length, unsigned long long fill) {
    ULongLongVector_reserve(v, length);
    for (size_t i = v->len; i < length; ++i) v->data[i] = fill;
    v->len = length;
}
static void ULongLongVector_reverse(ULongLongVector *v) {
    for (size_t i = 0; i < v->len / 2; ++i) {
        unsigned long long value = v->data[i];
        v->data[i] = v->data[v->len - 1 - i];
        v->data[v->len - 1 - i] = value;
    }
}
static size_t ULongLongVector_size(const ULongLongVector *v) { return v->len; }
static bool ULongLongVector_empty(const ULongLongVector *v) { return v->len == 0; }
static unsigned long long ULongLongVector_at(const ULongLongVector *v, size_t pos) {
    if (pos >= v->len) tb_fail("vector index out of range");
    return v->data[pos];
}
static void ULongLongVector_fill(ULongLongVector *v, unsigned long long value) {
    for (size_t i = 0; i < v->len; ++i) v->data[i] = value;
}
static ULongLongVector ULongLongVector_slice(const ULongLongVector *v, size_t pos, size_t count) {
    if (pos > v->len || count > v->len - pos) tb_fail("invalid vector slice");
    ULongLongVector result = ULongLongVector_new();
    if (count) ULongLongVector_extend(&result, v->data + pos, count);
    return result;
}
/* Forward half-open slice [first,last) with a positive stride. */
static ULongLongVector ULongLongVector_slice_step(const ULongLongVector *v,size_t first,size_t last,size_t step) {
    if(!step || first>last || last>v->len)tb_fail("invalid vector slice");
    ULongLongVector result=ULongLongVector_new();
    size_t count=first==last?0:1+(last-first-1)/step;
    ULongLongVector_reserve(&result,count);
    for(size_t i=first;i<last;) { ULongLongVector_push(&result,v->data[i]);if(step>last-1-i)break;i+=step; }
    return result;
}
static void ULongLongVector_extend_vector(ULongLongVector *v, const ULongLongVector *other) {
    ULongLongVector_extend(v, other->data, other->len);
}

/* SPDX-License-Identifier: MIT-0 */
/* Eager, same-type operations. Element copies follow the vector's shallow policy. */
static ULongLongVector ULongLongVector_from(unsigned long long const *items, size_t count) {
    ULongLongVector result = ULongLongVector_new();
    ULongLongVector_extend(&result, items, count);
    return result;
}
/* At least one value; use _new() for empty. Arguments are evaluated once. */
#define ULongLongVector_values(...) ULongLongVector_from((unsigned long long const[]){__VA_ARGS__}, \
    sizeof((unsigned long long const[]){__VA_ARGS__}) / sizeof(unsigned long long))
static ULongLongVector ULongLongVector_filter(const ULongLongVector *v, bool (*predicate)(unsigned long long)) {
    if (!predicate) tb_fail("missing predicate");
    ULongLongVector result = ULongLongVector_new();
    for (size_t i = 0; i < v->len; ++i)
        if (predicate(v->data[i])) ULongLongVector_push(&result, v->data[i]);
    return result;
}
static ULongLongVector ULongLongVector_filter_ctx(const ULongLongVector *v, bool (*predicate)(unsigned long long,void *),void *context) {
    if (!predicate) tb_fail("missing predicate");
    ULongLongVector result=ULongLongVector_new();
    for(size_t i=0;i<v->len;++i)if(predicate(v->data[i],context))ULongLongVector_push(&result,v->data[i]);
    return result;
}
static ULongLongVector ULongLongVector_map(const ULongLongVector *v, unsigned long long (*transform)(unsigned long long)) {
    if (!transform) tb_fail("missing transform");
    ULongLongVector result = ULongLongVector_new();
    ULongLongVector_reserve(&result, v->len);
    for (size_t i = 0; i < v->len; ++i) ULongLongVector_push(&result, transform(v->data[i]));
    return result;
}
static ULongLongVector ULongLongVector_map_ctx(const ULongLongVector *v,unsigned long long (*transform)(unsigned long long,void *),void *context) {
    if(!transform)tb_fail("missing transform");
    ULongLongVector result=ULongLongVector_new();ULongLongVector_reserve(&result,v->len);
    for(size_t i=0;i<v->len;++i)ULongLongVector_push(&result,transform(v->data[i],context));
    return result;
}
static void ULongLongVector_map_inplace(ULongLongVector *v, unsigned long long (*transform)(unsigned long long)) {
    if (!transform) tb_fail("missing transform");
    for (size_t i = 0; i < v->len; ++i) v->data[i] = transform(v->data[i]);
}
static size_t ULongLongVector_find_if(const ULongLongVector *v, bool (*predicate)(unsigned long long)) {
    if (!predicate) tb_fail("missing predicate");
    for (size_t i = 0; i < v->len; ++i) if (predicate(v->data[i])) return i;
    return SIZE_MAX;
}
static size_t ULongLongVector_find_if_ctx(const ULongLongVector *v,bool (*predicate)(unsigned long long,void *),void *context) {
    if(!predicate)tb_fail("missing predicate");
    for(size_t i=0;i<v->len;++i)if(predicate(v->data[i],context))return i;
    return SIZE_MAX;
}
static size_t ULongLongVector_count_if(const ULongLongVector *v, bool (*predicate)(unsigned long long)) {
    if (!predicate) tb_fail("missing predicate");
    size_t count = 0;
    for (size_t i = 0; i < v->len; ++i) if (predicate(v->data[i])) ++count;
    return count;
}
static bool ULongLongVector_any(const ULongLongVector *v, bool (*predicate)(unsigned long long)) {
    return ULongLongVector_find_if(v, predicate) != SIZE_MAX;
}
static bool ULongLongVector_all(const ULongLongVector *v, bool (*predicate)(unsigned long long)) {
    if (!predicate) tb_fail("missing predicate");
    for (size_t i = 0; i < v->len; ++i) if (!predicate(v->data[i])) return false;
    return true;
}
static unsigned long long ULongLongVector_reduce(const ULongLongVector *v, unsigned long long initial,
                           unsigned long long (*combine)(unsigned long long, unsigned long long)) {
    if (!combine) tb_fail("missing reducer");
    unsigned long long result = initial;
    for (size_t i = 0; i < v->len; ++i) result = combine(result, v->data[i]);
    return result;
}
static unsigned long long ULongLongVector_reduce_ctx(const ULongLongVector *v,unsigned long long initial,unsigned long long (*combine)(unsigned long long,unsigned long long,void *),void *context) {
    if(!combine)tb_fail("missing reducer");
    for(size_t i=0;i<v->len;++i)initial=combine(initial,v->data[i],context);
    return initial;
}

/* SPDX-License-Identifier: MIT-0 */
#include <math.h>
/* Numeric helpers for int, long long and double. Floating operands must be finite. */
static int ULongLongVector_compare_asc(const void *a, const void *b) {
    unsigned long long x = *(const unsigned long long *)a, y = *(const unsigned long long *)b;
    return (x > y) - (x < y);
}
static int ULongLongVector_compare_desc(const void *a, const void *b) {
    return ULongLongVector_compare_asc(b, a);
}
static void ULongLongVector_sort_asc(ULongLongVector *v) { ULongLongVector_sort(v, ULongLongVector_compare_asc); }
static void ULongLongVector_sort_desc(ULongLongVector *v) { ULongLongVector_sort(v, ULongLongVector_compare_desc); }
static size_t ULongLongVector_find(const ULongLongVector *v, unsigned long long value) {
    for (size_t i = 0; i < v->len; ++i) if (v->data[i] == value) return i;
    return SIZE_MAX;
}
static bool ULongLongVector_contains(const ULongLongVector *v, unsigned long long value) {
    return ULongLongVector_find(v, value) != SIZE_MAX;
}
static size_t ULongLongVector_count(const ULongLongVector *v, unsigned long long value) {
    size_t count = 0;
    for (size_t i = 0; i < v->len; ++i) if (v->data[i] == value) ++count;
    return count;
}
static size_t ULongLongVector_lower_bound(const ULongLongVector *v, unsigned long long key) {
    size_t left = 0, right = v->len;
    while (left < right) {
        size_t mid = left + (right - left) / 2;
        if (v->data[mid] < key) left = mid + 1;
        else right = mid;
    }
    return left;
}
static size_t ULongLongVector_upper_bound(const ULongLongVector *v, unsigned long long key) {
    size_t left = 0, right = v->len;
    while (left < right) {
        size_t mid = left + (right - left) / 2;
        if (v->data[mid] <= key) left = mid + 1;
        else right = mid;
    }
    return left;
}
static void ULongLongVector_unique_sorted(ULongLongVector *v) {
    size_t written = 0;
    for (size_t i = 0; i < v->len; ++i)
        if (!written || v->data[i] != v->data[written - 1]) v->data[written++] = v->data[i];
    v->len = written;
}
static size_t ULongLongVector_argmin(const ULongLongVector *v) {
    if (!v->len) tb_fail("argmin of empty vector");
    size_t best = 0;
    for (size_t i = 1; i < v->len; ++i) if (v->data[i] < v->data[best]) best = i;
    return best;
}
static size_t ULongLongVector_argmax(const ULongLongVector *v) {
    if (!v->len) tb_fail("argmax of empty vector");
    size_t best = 0;
    for (size_t i = 1; i < v->len; ++i) if (v->data[i] > v->data[best]) best = i;
    return best;
}
static unsigned long long ULongLongVector_min(const ULongLongVector *v) { return v->data[ULongLongVector_argmin(v)]; }
static unsigned long long ULongLongVector_max(const ULongLongVector *v) { return v->data[ULongLongVector_argmax(v)]; }
static double ULongLongVector_mean(const ULongLongVector *v) {
    if (!v->len) tb_fail("mean of empty vector");
    long double result = 0;
    for (size_t i = 0; i < v->len; ++i)
        result += ((long double)v->data[i] - result) / (long double)(i + 1);
    if (!isfinite((double)result)) tb_fail("nonfinite statistic");
    return (double)result;
}
/* Population variance, dividing by n. Original data is unchanged. */
static double ULongLongVector_variance(const ULongLongVector *v) {
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
static double ULongLongVector_stddev(const ULongLongVector *v) { return sqrt(ULongLongVector_variance(v)); }
static double ULongLongVector_median(const ULongLongVector *v) {
    if (!v->len) tb_fail("median of empty vector");
    ULongLongVector copy = ULongLongVector_clone(v);
    ULongLongVector_sort_asc(&copy);
    size_t mid = copy.len / 2;
    long double result = copy.data[mid];
    if (!(copy.len % 2)) result = ((long double)copy.data[mid - 1] + result) / 2;
    ULongLongVector_free(&copy);
    if (!isfinite((double)result)) tb_fail("nonfinite median");
    return (double)result;
}

/* Exact binary lookup on ascending input. */
static size_t ULongLongVector_binary_search(const ULongLongVector *v,unsigned long long key){size_t i=ULongLongVector_lower_bound(v,key);return i<v->len && v->data[i]==key?i:SIZE_MAX;}

/* Exact unsigned accumulation; fails before wraparound. */
static unsigned long long ULongLongVector_sum(const ULongLongVector *v) {
    unsigned long long result=0;
    for(size_t i=0;i<v->len;++i) {
        unsigned long long value=(unsigned long long)v->data[i];
        if(result>ULLONG_MAX-value)tb_fail("unsigned vector sum overflow");
        result+=value;
    }
    return result;
}

/* SPDX-License-Identifier: MIT-0 */
typedef struct { bool ok; unsigned long long value; } ULongLongVectorResult;
static ULongLongVectorResult ULongLongVector_try_get(const ULongLongVector *v,size_t pos){ULongLongVectorResult r={false,(unsigned long long){0}};r.ok=ULongLongVector_get_into(v,pos,&r.value);return r;}
static unsigned long long ULongLongVector_get(const ULongLongVector *v,size_t pos){ULongLongVectorResult r=ULongLongVector_try_get(v,pos);if(!r.ok)tb_fail("vector index out of range");return r.value;}
static ULongLongVectorResult ULongLongVector_try_pop(ULongLongVector *v){ULongLongVectorResult r={false,(unsigned long long){0}};r.ok=ULongLongVector_pop_into(v,&r.value);return r;}
static unsigned long long ULongLongVector_pop(ULongLongVector *v){ULongLongVectorResult r=ULongLongVector_try_pop(v);if(!r.ok)tb_fail("pop of empty vector");return r.value;}
static unsigned long long ULongLongVector_front(const ULongLongVector *v){return ULongLongVector_get(v,0);}
static unsigned long long ULongLongVector_back(const ULongLongVector *v){if(!v->len)tb_fail("back of empty vector");return ULongLongVector_get(v,v->len-1);}
static void ULongLongVector_merge_sort(unsigned long long *data,unsigned long long *scratch,size_t n,int (*compare)(const unsigned long long *,const unsigned long long *)){
    if(n<2)return;
    size_t mid=n/2;
    ULongLongVector_merge_sort(data,scratch,mid,compare);ULongLongVector_merge_sort(data+mid,scratch+mid,n-mid,compare);
    size_t i=0,j=mid,k=0;
    while(i<mid && j<n){if(compare(&data[i],&data[j])<=0)scratch[k++]=data[i++];else scratch[k++]=data[j++];}
    while(i<mid)scratch[k++]=data[i++];
    while(j<n)scratch[k++]=data[j++];
    memcpy(data,scratch,n*sizeof *data);
}
/* Stable, typed comparison; no void-pointer casts required from callers. */
static void ULongLongVector_sort_by(ULongLongVector *v,int (*compare)(const unsigned long long *,const unsigned long long *)){
    if(!compare)tb_fail("null comparator");
    if(v->len<2)return;
    unsigned long long *scratch=tb_resize(NULL,v->len,sizeof *scratch);ULongLongVector_merge_sort(v->data,scratch,v->len,compare);free(scratch);
}

static ULongLongVector read_ulls(size_t count) { ULongLongVector v=ULongLongVector_new();ULongLongVector_reserve(&v,count);for(size_t i=0;i<count;++i) ULongLongVector_push(&v,read_ull());return v;}

static void print_ulls(const ULongLongVector *v,const char *sep,const char *end) {for(size_t i=0;i<v->len;++i){if(i)print_text(sep,"");print_ull(v->data[i],"");}print_text("",end);}

#endif
