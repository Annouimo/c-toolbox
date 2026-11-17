#ifndef CTB_VEC_LL_H
#define CTB_VEC_LL_H
#include "io.h"
/* SPDX-License-Identifier: MIT-0 */
/* Typed, contiguous storage. Elements are shallow copied; no element destructor. */
typedef struct { long long *data; size_t len, cap; } LongLongVector;
static LongLongVector LongLongVector_new(void) { return (LongLongVector){0}; }
static void LongLongVector_reserve(LongLongVector *v, size_t needed) {
    if (needed <= v->cap) return;
    size_t cap = tb_capacity(v->cap, needed);
    v->data = tb_resize(v->data, cap, sizeof *v->data);
    v->cap = cap;
}
static void LongLongVector_push(LongLongVector *v, long long value) {
    LongLongVector_reserve(v, tb_add_size(v->len, 1));
    v->data[v->len++] = value;
}
static bool LongLongVector_pop_into(LongLongVector *v, long long *out) {
    if (!v->len) return false;
    --v->len;
    if (out) *out = v->data[v->len];
    return true;
}
static bool LongLongVector_insert(LongLongVector *v, size_t pos, long long value) {
    if (pos > v->len) return false;
    LongLongVector_reserve(v, tb_add_size(v->len, 1));
    memmove(v->data + pos + 1, v->data + pos, (v->len - pos) * sizeof *v->data);
    v->data[pos] = value;
    ++v->len;
    return true;
}
static bool LongLongVector_erase(LongLongVector *v, size_t pos, size_t count) {
    if (pos > v->len || count > v->len - pos) return false;
    if (count) memmove(v->data + pos, v->data + pos + count,
                       (v->len - pos - count) * sizeof *v->data);
    v->len -= count;
    return true;
}
static LongLongVector LongLongVector_clone(const LongLongVector *v) {
    LongLongVector copy = {0};
    if (v->len) {
        LongLongVector_reserve(&copy, v->len);
        memcpy(copy.data, v->data, v->len * sizeof *v->data);
    }
    copy.len = v->len;
    return copy;
}
static void LongLongVector_clear(LongLongVector *v) { v->len = 0; }
static void LongLongVector_sort(LongLongVector *v, int (*compare)(const void *, const void *)) {
    if (v->len > 1) qsort(v->data, v->len, sizeof *v->data, compare);
}
static void LongLongVector_free(LongLongVector *v) { free(v->data); *v = (LongLongVector){0}; }
static void LongLongVector_swap(LongLongVector *a,LongLongVector *b) { LongLongVector value=*a;*a=*b;*b=value; }

/* SPDX-License-Identifier: MIT-0 */
static bool LongLongVector_get_into(const LongLongVector *v, size_t pos, long long *out) {
    if (pos >= v->len) return false;
    if (out) *out = v->data[pos];
    return true;
}
static bool LongLongVector_set(LongLongVector *v, size_t pos, long long value) {
    if (pos >= v->len) return false;
    v->data[pos] = value;
    return true;
}
static bool LongLongVector_front_into(const LongLongVector *v, long long *out) {
    return LongLongVector_get_into(v, 0, out);
}
static bool LongLongVector_back_into(const LongLongVector *v, long long *out) {
    return v->len && LongLongVector_get_into(v, v->len - 1, out);
}
/* Snapshot supports extending from the vector's own storage. */
static void LongLongVector_extend(LongLongVector *v, long long const *items, size_t count) {
    if (!count) return;
    size_t length = tb_add_size(v->len, count);
    long long *copy = tb_resize(NULL, count, sizeof *copy);
    memcpy(copy, items, count * sizeof *copy);
    LongLongVector_reserve(v, length);
    memcpy(v->data + v->len, copy, count * sizeof *copy);
    v->len = length;
    free(copy);
}
static void LongLongVector_resize(LongLongVector *v, size_t length, long long fill) {
    LongLongVector_reserve(v, length);
    for (size_t i = v->len; i < length; ++i) v->data[i] = fill;
    v->len = length;
}
static void LongLongVector_reverse(LongLongVector *v) {
    for (size_t i = 0; i < v->len / 2; ++i) {
        long long value = v->data[i];
        v->data[i] = v->data[v->len - 1 - i];
        v->data[v->len - 1 - i] = value;
    }
}
static size_t LongLongVector_size(const LongLongVector *v) { return v->len; }
static bool LongLongVector_empty(const LongLongVector *v) { return v->len == 0; }
static long long LongLongVector_at(const LongLongVector *v, size_t pos) {
    if (pos >= v->len) tb_fail("vector index out of range");
    return v->data[pos];
}
static void LongLongVector_fill(LongLongVector *v, long long value) {
    for (size_t i = 0; i < v->len; ++i) v->data[i] = value;
}
static LongLongVector LongLongVector_slice(const LongLongVector *v, size_t pos, size_t count) {
    if (pos > v->len || count > v->len - pos) tb_fail("invalid vector slice");
    LongLongVector result = LongLongVector_new();
    if (count) LongLongVector_extend(&result, v->data + pos, count);
    return result;
}
/* Forward half-open slice [first,last) with a positive stride. */
static LongLongVector LongLongVector_slice_step(const LongLongVector *v,size_t first,size_t last,size_t step) {
    if(!step || first>last || last>v->len)tb_fail("invalid vector slice");
    LongLongVector result=LongLongVector_new();
    size_t count=first==last?0:1+(last-first-1)/step;
    LongLongVector_reserve(&result,count);
    for(size_t i=first;i<last;) { LongLongVector_push(&result,v->data[i]);if(step>last-1-i)break;i+=step; }
    return result;
}
static void LongLongVector_extend_vector(LongLongVector *v, const LongLongVector *other) {
    LongLongVector_extend(v, other->data, other->len);
}

/* SPDX-License-Identifier: MIT-0 */
/* Eager, same-type operations. Element copies follow the vector's shallow policy. */
static LongLongVector LongLongVector_from(long long const *items, size_t count) {
    LongLongVector result = LongLongVector_new();
    LongLongVector_extend(&result, items, count);
    return result;
}
/* At least one value; use _new() for empty. Arguments are evaluated once. */
#define LongLongVector_values(...) LongLongVector_from((long long const[]){__VA_ARGS__}, \
    sizeof((long long const[]){__VA_ARGS__}) / sizeof(long long))
static LongLongVector LongLongVector_filter(const LongLongVector *v, bool (*predicate)(long long)) {
    if (!predicate) tb_fail("missing predicate");
    LongLongVector result = LongLongVector_new();
    for (size_t i = 0; i < v->len; ++i)
        if (predicate(v->data[i])) LongLongVector_push(&result, v->data[i]);
    return result;
}
static LongLongVector LongLongVector_filter_ctx(const LongLongVector *v, bool (*predicate)(long long,void *),void *context) {
    if (!predicate) tb_fail("missing predicate");
    LongLongVector result=LongLongVector_new();
    for(size_t i=0;i<v->len;++i)if(predicate(v->data[i],context))LongLongVector_push(&result,v->data[i]);
    return result;
}
static LongLongVector LongLongVector_map(const LongLongVector *v, long long (*transform)(long long)) {
    if (!transform) tb_fail("missing transform");
    LongLongVector result = LongLongVector_new();
    LongLongVector_reserve(&result, v->len);
    for (size_t i = 0; i < v->len; ++i) LongLongVector_push(&result, transform(v->data[i]));
    return result;
}
static LongLongVector LongLongVector_map_ctx(const LongLongVector *v,long long (*transform)(long long,void *),void *context) {
    if(!transform)tb_fail("missing transform");
    LongLongVector result=LongLongVector_new();LongLongVector_reserve(&result,v->len);
    for(size_t i=0;i<v->len;++i)LongLongVector_push(&result,transform(v->data[i],context));
    return result;
}
static void LongLongVector_map_inplace(LongLongVector *v, long long (*transform)(long long)) {
    if (!transform) tb_fail("missing transform");
    for (size_t i = 0; i < v->len; ++i) v->data[i] = transform(v->data[i]);
}
static size_t LongLongVector_find_if(const LongLongVector *v, bool (*predicate)(long long)) {
    if (!predicate) tb_fail("missing predicate");
    for (size_t i = 0; i < v->len; ++i) if (predicate(v->data[i])) return i;
    return SIZE_MAX;
}
static size_t LongLongVector_find_if_ctx(const LongLongVector *v,bool (*predicate)(long long,void *),void *context) {
    if(!predicate)tb_fail("missing predicate");
    for(size_t i=0;i<v->len;++i)if(predicate(v->data[i],context))return i;
    return SIZE_MAX;
}
static size_t LongLongVector_count_if(const LongLongVector *v, bool (*predicate)(long long)) {
    if (!predicate) tb_fail("missing predicate");
    size_t count = 0;
    for (size_t i = 0; i < v->len; ++i) if (predicate(v->data[i])) ++count;
    return count;
}
static bool LongLongVector_any(const LongLongVector *v, bool (*predicate)(long long)) {
    return LongLongVector_find_if(v, predicate) != SIZE_MAX;
}
static bool LongLongVector_all(const LongLongVector *v, bool (*predicate)(long long)) {
    if (!predicate) tb_fail("missing predicate");
    for (size_t i = 0; i < v->len; ++i) if (!predicate(v->data[i])) return false;
    return true;
}
static long long LongLongVector_reduce(const LongLongVector *v, long long initial,
                           long long (*combine)(long long, long long)) {
    if (!combine) tb_fail("missing reducer");
    long long result = initial;
    for (size_t i = 0; i < v->len; ++i) result = combine(result, v->data[i]);
    return result;
}
static long long LongLongVector_reduce_ctx(const LongLongVector *v,long long initial,long long (*combine)(long long,long long,void *),void *context) {
    if(!combine)tb_fail("missing reducer");
    for(size_t i=0;i<v->len;++i)initial=combine(initial,v->data[i],context);
    return initial;
}

/* SPDX-License-Identifier: MIT-0 */
#include <math.h>
/* Numeric helpers for int, long long and double. Floating operands must be finite. */
static int LongLongVector_compare_asc(const void *a, const void *b) {
    long long x = *(const long long *)a, y = *(const long long *)b;
    return (x > y) - (x < y);
}
static int LongLongVector_compare_desc(const void *a, const void *b) {
    return LongLongVector_compare_asc(b, a);
}
static void LongLongVector_sort_asc(LongLongVector *v) { LongLongVector_sort(v, LongLongVector_compare_asc); }
static void LongLongVector_sort_desc(LongLongVector *v) { LongLongVector_sort(v, LongLongVector_compare_desc); }
static size_t LongLongVector_find(const LongLongVector *v, long long value) {
    for (size_t i = 0; i < v->len; ++i) if (v->data[i] == value) return i;
    return SIZE_MAX;
}
static bool LongLongVector_contains(const LongLongVector *v, long long value) {
    return LongLongVector_find(v, value) != SIZE_MAX;
}
static size_t LongLongVector_count(const LongLongVector *v, long long value) {
    size_t count = 0;
    for (size_t i = 0; i < v->len; ++i) if (v->data[i] == value) ++count;
    return count;
}
static size_t LongLongVector_lower_bound(const LongLongVector *v, long long key) {
    size_t left = 0, right = v->len;
    while (left < right) {
        size_t mid = left + (right - left) / 2;
        if (v->data[mid] < key) left = mid + 1;
        else right = mid;
    }
    return left;
}
static size_t LongLongVector_upper_bound(const LongLongVector *v, long long key) {
    size_t left = 0, right = v->len;
    while (left < right) {
        size_t mid = left + (right - left) / 2;
        if (v->data[mid] <= key) left = mid + 1;
        else right = mid;
    }
    return left;
}
static void LongLongVector_unique_sorted(LongLongVector *v) {
    size_t written = 0;
    for (size_t i = 0; i < v->len; ++i)
        if (!written || v->data[i] != v->data[written - 1]) v->data[written++] = v->data[i];
    v->len = written;
}
static size_t LongLongVector_argmin(const LongLongVector *v) {
    if (!v->len) tb_fail("argmin of empty vector");
    size_t best = 0;
    for (size_t i = 1; i < v->len; ++i) if (v->data[i] < v->data[best]) best = i;
    return best;
}
static size_t LongLongVector_argmax(const LongLongVector *v) {
    if (!v->len) tb_fail("argmax of empty vector");
    size_t best = 0;
    for (size_t i = 1; i < v->len; ++i) if (v->data[i] > v->data[best]) best = i;
    return best;
}
static long long LongLongVector_min(const LongLongVector *v) { return v->data[LongLongVector_argmin(v)]; }
static long long LongLongVector_max(const LongLongVector *v) { return v->data[LongLongVector_argmax(v)]; }
static double LongLongVector_mean(const LongLongVector *v) {
    if (!v->len) tb_fail("mean of empty vector");
    long double result = 0;
    for (size_t i = 0; i < v->len; ++i)
        result += ((long double)v->data[i] - result) / (long double)(i + 1);
    if (!isfinite((double)result)) tb_fail("nonfinite statistic");
    return (double)result;
}
/* Population variance, dividing by n. Original data is unchanged. */
static double LongLongVector_variance(const LongLongVector *v) {
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
static double LongLongVector_stddev(const LongLongVector *v) { return sqrt(LongLongVector_variance(v)); }
static double LongLongVector_median(const LongLongVector *v) {
    if (!v->len) tb_fail("median of empty vector");
    LongLongVector copy = LongLongVector_clone(v);
    LongLongVector_sort_asc(&copy);
    size_t mid = copy.len / 2;
    long double result = copy.data[mid];
    if (!(copy.len % 2)) result = ((long double)copy.data[mid - 1] + result) / 2;
    LongLongVector_free(&copy);
    if (!isfinite((double)result)) tb_fail("nonfinite median");
    return (double)result;
}

/* Exact binary lookup on ascending input. */
static size_t LongLongVector_binary_search(const LongLongVector *v,long long key){size_t i=LongLongVector_lower_bound(v,key);return i<v->len && v->data[i]==key?i:SIZE_MAX;}

/* SPDX-License-Identifier: MIT-0 */
/* Exact sequential sum; fails before a signed overflow, including intermediate sums. */
static long long LongLongVector_sum(const LongLongVector *v) {
    long long result = 0;
    for (size_t i = 0; i < v->len; ++i) {
        long long value = v->data[i];
        if ((value > 0 && result > LLONG_MAX - value) ||
            (value < 0 && result < LLONG_MIN - value)) tb_fail("integer sum overflow");
        result += value;
    }
    return result;
}

/* SPDX-License-Identifier: MIT-0 */
/* Integer [start, stop), like range. No signed overflow on the last increment. */
static LongLongVector LongLongVector_range(long long start, long long stop, long long step) {
    if (!step) tb_fail("range step must not be zero");
    LongLongVector result = LongLongVector_new();
    long long value = start;
    while (step > 0 ? value < stop : value > stop) {
        LongLongVector_push(&result, value);
        if ((step > 0 && value > LLONG_MAX - step) ||
            (step < 0 && value < LLONG_MIN - step)) break;
        value += step;
    }
    return result;
}

/* SPDX-License-Identifier: MIT-0 */
typedef struct { bool ok; long long value; } LongLongVectorResult;
static LongLongVectorResult LongLongVector_try_get(const LongLongVector *v,size_t pos){LongLongVectorResult r={0};r.ok=LongLongVector_get_into(v,pos,&r.value);return r;}
static long long LongLongVector_get(const LongLongVector *v,size_t pos){LongLongVectorResult r=LongLongVector_try_get(v,pos);if(!r.ok)tb_fail("vector index out of range");return r.value;}
static LongLongVectorResult LongLongVector_try_pop(LongLongVector *v){LongLongVectorResult r={0};r.ok=LongLongVector_pop_into(v,&r.value);return r;}
static long long LongLongVector_pop(LongLongVector *v){LongLongVectorResult r=LongLongVector_try_pop(v);if(!r.ok)tb_fail("pop of empty vector");return r.value;}
static long long LongLongVector_front(const LongLongVector *v){return LongLongVector_get(v,0);}
static long long LongLongVector_back(const LongLongVector *v){if(!v->len)tb_fail("back of empty vector");return LongLongVector_get(v,v->len-1);}
static void LongLongVector_merge_sort(long long *data,long long *scratch,size_t n,int (*compare)(const long long *,const long long *)){
    if(n<2)return;
    size_t mid=n/2;
    LongLongVector_merge_sort(data,scratch,mid,compare);LongLongVector_merge_sort(data+mid,scratch+mid,n-mid,compare);
    size_t i=0,j=mid,k=0;
    while(i<mid && j<n){if(compare(&data[i],&data[j])<=0)scratch[k++]=data[i++];else scratch[k++]=data[j++];}
    while(i<mid)scratch[k++]=data[i++];
    while(j<n)scratch[k++]=data[j++];
    memcpy(data,scratch,n*sizeof *data);
}
/* Stable, typed comparison; no void-pointer casts required from callers. */
static void LongLongVector_sort_by(LongLongVector *v,int (*compare)(const long long *,const long long *)){
    if(!compare)tb_fail("null comparator");
    if(v->len<2)return;
    long long *scratch=tb_resize(NULL,v->len,sizeof *scratch);LongLongVector_merge_sort(v->data,scratch,v->len,compare);free(scratch);
}

static LongLongVector read_lls(size_t count) { LongLongVector v=LongLongVector_new();LongLongVector_reserve(&v,count);for(size_t i=0;i<count;++i) LongLongVector_push(&v,read_ll());return v;}

static void print_lls(const LongLongVector *v,const char *sep,const char *end) {for(size_t i=0;i<v->len;++i){if(i)print_text(sep,"");print_ll(v->data[i],"");}print_text("",end);}

#endif
