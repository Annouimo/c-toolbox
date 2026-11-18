#ifndef CTB_VECTOR_INT_H
#define CTB_VECTOR_INT_H
#include "../io.h"
/* SPDX-License-Identifier: MIT-0 */
/* Typed, contiguous storage. Elements are shallow copied; no element destructor. */
typedef struct { int *data; size_t len, cap; } IntVector;
static IntVector IntVector_new(void) { return (IntVector){0}; }
static void IntVector_reserve(IntVector *v, size_t needed) {
    if (needed <= v->cap) return;
    size_t cap = tb_capacity(v->cap, needed);
    v->data = tb_resize(v->data, cap, sizeof *v->data);
    v->cap = cap;
}
static void IntVector_push(IntVector *v, int value) {
    IntVector_reserve(v, tb_add_size(v->len, 1));
    v->data[v->len++] = value;
}
static bool IntVector_pop_into(IntVector *v, int *out) {
    if (!v->len) return false;
    --v->len;
    if (out) *out = v->data[v->len];
    return true;
}
static bool IntVector_insert(IntVector *v, size_t pos, int value) {
    if (pos > v->len) return false;
    IntVector_reserve(v, tb_add_size(v->len, 1));
    memmove(v->data + pos + 1, v->data + pos, (v->len - pos) * sizeof *v->data);
    v->data[pos] = value;
    ++v->len;
    return true;
}
static bool IntVector_erase(IntVector *v, size_t pos, size_t count) {
    if (pos > v->len || count > v->len - pos) return false;
    if (count) memmove(v->data + pos, v->data + pos + count,
                       (v->len - pos - count) * sizeof *v->data);
    v->len -= count;
    return true;
}
static IntVector IntVector_clone(const IntVector *v) {
    IntVector copy = {0};
    if (v->len) {
        IntVector_reserve(&copy, v->len);
        memcpy(copy.data, v->data, v->len * sizeof *v->data);
    }
    copy.len = v->len;
    return copy;
}
static void IntVector_clear(IntVector *v) { v->len = 0; }
static void IntVector_sort(IntVector *v, int (*compare)(const void *, const void *)) {
    if (v->len > 1) qsort(v->data, v->len, sizeof *v->data, compare);
}
static void IntVector_free(IntVector *v) { free(v->data); *v = (IntVector){0}; }
static void IntVector_swap(IntVector *a,IntVector *b) { IntVector value=*a;*a=*b;*b=value; }

/* SPDX-License-Identifier: MIT-0 */
static bool IntVector_get_into(const IntVector *v, size_t pos, int *out) {
    if (pos >= v->len) return false;
    if (out) *out = v->data[pos];
    return true;
}
static bool IntVector_try_set(IntVector *v, size_t pos, int value) {
    if (pos >= v->len) return false;
    v->data[pos] = value;
    return true;
}
static bool IntVector_front_into(const IntVector *v, int *out) {
    return IntVector_get_into(v, 0, out);
}
static bool IntVector_back_into(const IntVector *v, int *out) {
    return v->len && IntVector_get_into(v, v->len - 1, out);
}
/* Snapshot supports extending from the vector's own storage. */
static void IntVector_extend(IntVector *v, int const *items, size_t count) {
    if (!count) return;
    size_t length = tb_add_size(v->len, count);
    int *copy = tb_resize(NULL, count, sizeof *copy);
    memcpy(copy, items, count * sizeof *copy);
    IntVector_reserve(v, length);
    memcpy(v->data + v->len, copy, count * sizeof *copy);
    v->len = length;
    free(copy);
}
static void IntVector_resize(IntVector *v, size_t length, int fill) {
    IntVector_reserve(v, length);
    for (size_t i = v->len; i < length; ++i) v->data[i] = fill;
    v->len = length;
}
static void IntVector_reverse(IntVector *v) {
    for (size_t i = 0; i < v->len / 2; ++i) {
        int value = v->data[i];
        v->data[i] = v->data[v->len - 1 - i];
        v->data[v->len - 1 - i] = value;
    }
}
static size_t IntVector_size(const IntVector *v) { return v->len; }
static bool IntVector_empty(const IntVector *v) { return v->len == 0; }
static int IntVector_at(const IntVector *v, size_t pos) {
    if (pos >= v->len) tb_fail("vector index out of range");
    return v->data[pos];
}
static void IntVector_fill(IntVector *v, int value) {
    for (size_t i = 0; i < v->len; ++i) v->data[i] = value;
}
static IntVector IntVector_slice(const IntVector *v, size_t pos, size_t count) {
    if (pos > v->len || count > v->len - pos) tb_fail("invalid vector slice");
    IntVector result = IntVector_new();
    if (count) IntVector_extend(&result, v->data + pos, count);
    return result;
}
/* Forward half-open slice [first,last) with a positive stride. */
static IntVector IntVector_slice_step(const IntVector *v,size_t first,size_t last,size_t step) {
    if(!step || first>last || last>v->len)tb_fail("invalid vector slice");
    IntVector result=IntVector_new();
    size_t count=first==last?0:1+(last-first-1)/step;
    IntVector_reserve(&result,count);
    for(size_t i=first;i<last;) { IntVector_push(&result,v->data[i]);if(step>last-1-i)break;i+=step; }
    return result;
}
static void IntVector_extend_vector(IntVector *v, const IntVector *other) {
    IntVector_extend(v, other->data, other->len);
}

/* SPDX-License-Identifier: MIT-0 */
/* Eager, same-type operations. Element copies follow the vector's shallow policy. */
static IntVector IntVector_from(int const *items, size_t count) {
    IntVector result = IntVector_new();
    IntVector_extend(&result, items, count);
    return result;
}
/* At least one value; use _new() for empty. Arguments are evaluated once. */
#define IntVector_values(...) IntVector_from((int const[]){__VA_ARGS__}, \
    sizeof((int const[]){__VA_ARGS__}) / sizeof(int))
static IntVector IntVector_filter(const IntVector *v, bool (*predicate)(int)) {
    if (!predicate) tb_fail("missing predicate");
    IntVector result = IntVector_new();
    for (size_t i = 0; i < v->len; ++i)
        if (predicate(v->data[i])) IntVector_push(&result, v->data[i]);
    return result;
}
static IntVector IntVector_filter_ctx(const IntVector *v, bool (*predicate)(int,void *),void *context) {
    if (!predicate) tb_fail("missing predicate");
    IntVector result=IntVector_new();
    for(size_t i=0;i<v->len;++i)if(predicate(v->data[i],context))IntVector_push(&result,v->data[i]);
    return result;
}
static IntVector IntVector_map(const IntVector *v, int (*transform)(int)) {
    if (!transform) tb_fail("missing transform");
    IntVector result = IntVector_new();
    IntVector_reserve(&result, v->len);
    for (size_t i = 0; i < v->len; ++i) IntVector_push(&result, transform(v->data[i]));
    return result;
}
static IntVector IntVector_map_ctx(const IntVector *v,int (*transform)(int,void *),void *context) {
    if(!transform)tb_fail("missing transform");
    IntVector result=IntVector_new();IntVector_reserve(&result,v->len);
    for(size_t i=0;i<v->len;++i)IntVector_push(&result,transform(v->data[i],context));
    return result;
}
static void IntVector_map_inplace(IntVector *v, int (*transform)(int)) {
    if (!transform) tb_fail("missing transform");
    for (size_t i = 0; i < v->len; ++i) v->data[i] = transform(v->data[i]);
}
static size_t IntVector_find_if(const IntVector *v, bool (*predicate)(int)) {
    if (!predicate) tb_fail("missing predicate");
    for (size_t i = 0; i < v->len; ++i) if (predicate(v->data[i])) return i;
    return SIZE_MAX;
}
static size_t IntVector_find_if_ctx(const IntVector *v,bool (*predicate)(int,void *),void *context) {
    if(!predicate)tb_fail("missing predicate");
    for(size_t i=0;i<v->len;++i)if(predicate(v->data[i],context))return i;
    return SIZE_MAX;
}
static size_t IntVector_count_if(const IntVector *v, bool (*predicate)(int)) {
    if (!predicate) tb_fail("missing predicate");
    size_t count = 0;
    for (size_t i = 0; i < v->len; ++i) if (predicate(v->data[i])) ++count;
    return count;
}
static bool IntVector_any(const IntVector *v, bool (*predicate)(int)) {
    return IntVector_find_if(v, predicate) != SIZE_MAX;
}
static bool IntVector_all(const IntVector *v, bool (*predicate)(int)) {
    if (!predicate) tb_fail("missing predicate");
    for (size_t i = 0; i < v->len; ++i) if (!predicate(v->data[i])) return false;
    return true;
}
static int IntVector_reduce(const IntVector *v, int initial,
                           int (*combine)(int, int)) {
    if (!combine) tb_fail("missing reducer");
    int result = initial;
    for (size_t i = 0; i < v->len; ++i) result = combine(result, v->data[i]);
    return result;
}
static int IntVector_reduce_ctx(const IntVector *v,int initial,int (*combine)(int,int,void *),void *context) {
    if(!combine)tb_fail("missing reducer");
    for(size_t i=0;i<v->len;++i)initial=combine(initial,v->data[i],context);
    return initial;
}

/* SPDX-License-Identifier: MIT-0 */
#include <math.h>
/* Numeric helpers for int, long long and double. Floating operands must be finite. */
static int IntVector_compare_asc(const void *a, const void *b) {
    int x = *(const int *)a, y = *(const int *)b;
    return (x > y) - (x < y);
}
static int IntVector_compare_desc(const void *a, const void *b) {
    return IntVector_compare_asc(b, a);
}
static void IntVector_sort_asc(IntVector *v) { IntVector_sort(v, IntVector_compare_asc); }
static void IntVector_sort_desc(IntVector *v) { IntVector_sort(v, IntVector_compare_desc); }
static size_t IntVector_find(const IntVector *v, int value) {
    for (size_t i = 0; i < v->len; ++i) if (v->data[i] == value) return i;
    return SIZE_MAX;
}
static bool IntVector_contains(const IntVector *v, int value) {
    return IntVector_find(v, value) != SIZE_MAX;
}
static size_t IntVector_count(const IntVector *v, int value) {
    size_t count = 0;
    for (size_t i = 0; i < v->len; ++i) if (v->data[i] == value) ++count;
    return count;
}
static size_t IntVector_lower_bound(const IntVector *v, int key) {
    size_t left = 0, right = v->len;
    while (left < right) {
        size_t mid = left + (right - left) / 2;
        if (v->data[mid] < key) left = mid + 1;
        else right = mid;
    }
    return left;
}
static size_t IntVector_upper_bound(const IntVector *v, int key) {
    size_t left = 0, right = v->len;
    while (left < right) {
        size_t mid = left + (right - left) / 2;
        if (v->data[mid] <= key) left = mid + 1;
        else right = mid;
    }
    return left;
}
static void IntVector_unique_sorted(IntVector *v) {
    size_t written = 0;
    for (size_t i = 0; i < v->len; ++i)
        if (!written || v->data[i] != v->data[written - 1]) v->data[written++] = v->data[i];
    v->len = written;
}
static size_t IntVector_argmin(const IntVector *v) {
    if (!v->len) tb_fail("argmin of empty vector");
    size_t best = 0;
    for (size_t i = 1; i < v->len; ++i) if (v->data[i] < v->data[best]) best = i;
    return best;
}
static size_t IntVector_argmax(const IntVector *v) {
    if (!v->len) tb_fail("argmax of empty vector");
    size_t best = 0;
    for (size_t i = 1; i < v->len; ++i) if (v->data[i] > v->data[best]) best = i;
    return best;
}
static int IntVector_min(const IntVector *v) { return v->data[IntVector_argmin(v)]; }
static int IntVector_max(const IntVector *v) { return v->data[IntVector_argmax(v)]; }
static double IntVector_mean(const IntVector *v) {
    if (!v->len) tb_fail("mean of empty vector");
    long double result = 0;
    for (size_t i = 0; i < v->len; ++i)
        result += ((long double)v->data[i] - result) / (long double)(i + 1);
    if (!isfinite((double)result)) tb_fail("nonfinite statistic");
    return (double)result;
}
/* Population variance, dividing by n. Original data is unchanged. */
static double IntVector_variance(const IntVector *v) {
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
static double IntVector_stddev(const IntVector *v) { return sqrt(IntVector_variance(v)); }
static double IntVector_median(const IntVector *v) {
    if (!v->len) tb_fail("median of empty vector");
    IntVector copy = IntVector_clone(v);
    IntVector_sort_asc(&copy);
    size_t mid = copy.len / 2;
    long double result = copy.data[mid];
    if (!(copy.len % 2)) result = ((long double)copy.data[mid - 1] + result) / 2;
    IntVector_free(&copy);
    if (!isfinite((double)result)) tb_fail("nonfinite median");
    return (double)result;
}

/* Exact binary lookup on ascending input. */
static size_t IntVector_binary_search(const IntVector *v,int key){size_t i=IntVector_lower_bound(v,key);return i<v->len && v->data[i]==key?i:SIZE_MAX;}

/* SPDX-License-Identifier: MIT-0 */
/* Exact sequential sum; fails before a signed overflow, including intermediate sums. */
static long long IntVector_sum(const IntVector *v) {
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
static IntVector IntVector_range(int start, int stop, int step) {
    if (!step) tb_fail("range step must not be zero");
    IntVector result = IntVector_new();
    int value = start;
    while (step > 0 ? value < stop : value > stop) {
        IntVector_push(&result, value);
        if ((step > 0 && value > INT_MAX - step) ||
            (step < 0 && value < INT_MIN - step)) break;
        value += step;
    }
    return result;
}

/* SPDX-License-Identifier: MIT-0 */
typedef struct { bool ok; int value; } IntVectorResult;
static IntVectorResult IntVector_try_get(const IntVector *v,size_t pos){IntVectorResult r={0};r.ok=IntVector_get_into(v,pos,&r.value);return r;}
static int IntVector_get(const IntVector *v,size_t pos){IntVectorResult r=IntVector_try_get(v,pos);if(!r.ok)tb_fail("vector index out of range");return r.value;}
static IntVectorResult IntVector_try_pop(IntVector *v){IntVectorResult r={0};r.ok=IntVector_pop_into(v,&r.value);return r;}
static int IntVector_pop(IntVector *v){IntVectorResult r=IntVector_try_pop(v);if(!r.ok)tb_fail("pop of empty vector");return r.value;}
static int IntVector_front(const IntVector *v){return IntVector_get(v,0);}
static int IntVector_back(const IntVector *v){if(!v->len)tb_fail("back of empty vector");return IntVector_get(v,v->len-1);}
static void IntVector_merge_sort(int *data,int *scratch,size_t n,int (*compare)(const int *,const int *)){
    if(n<2)return;
    size_t mid=n/2;
    IntVector_merge_sort(data,scratch,mid,compare);IntVector_merge_sort(data+mid,scratch+mid,n-mid,compare);
    size_t i=0,j=mid,k=0;
    while(i<mid && j<n){if(compare(&data[i],&data[j])<=0)scratch[k++]=data[i++];else scratch[k++]=data[j++];}
    while(i<mid)scratch[k++]=data[i++];
    while(j<n)scratch[k++]=data[j++];
    memcpy(data,scratch,n*sizeof *data);
}
/* Stable, typed comparison; no void-pointer casts required from callers. */
static void IntVector_sort_by(IntVector *v,int (*compare)(const int *,const int *)){
    if(!compare)tb_fail("null comparator");
    if(v->len<2)return;
    int *scratch=tb_resize(NULL,v->len,sizeof *scratch);IntVector_merge_sort(v->data,scratch,v->len,compare);free(scratch);
}

static IntVector IntVector_filled(size_t length, int value) {
    IntVector v = IntVector_new();
    IntVector_resize(&v, length, value);
    return v;
}
static bool IntVector_in_bounds(const IntVector *v, intmax_t pos) {
    return pos >= 0 && (uintmax_t)pos < v->len;
}
static void IntVector_set(IntVector *v, size_t pos, int value) {
    if (!IntVector_try_set(v, pos, value)) tb_fail("vector index out of range");
}

static IntVector read_ints(size_t count) { IntVector v=IntVector_new();IntVector_reserve(&v,count);for(size_t i=0;i<count;++i) IntVector_push(&v,read_int());return v;}

static void print_ints(const IntVector *v,const char *sep,const char *end) {for(size_t i=0;i<v->len;++i){if(i)print_text(sep,"");print_int(v->data[i],"");}print_text("",end);}

#endif
