#ifndef CTB_VEC_DOUBLE_H
#define CTB_VEC_DOUBLE_H
#include "ctb_io.h"
/* SPDX-License-Identifier: MIT-0 */
/* Typed, contiguous storage. Elements are shallow copied; no element destructor. */
typedef struct { double *data; size_t len, cap; } DoubleVector;
static DoubleVector DoubleVector_new(void) { return (DoubleVector){0}; }
static void DoubleVector_reserve(DoubleVector *v, size_t needed) {
    if (needed <= v->cap) return;
    size_t cap = tb_capacity(v->cap, needed);
    v->data = tb_resize(v->data, cap, sizeof *v->data);
    v->cap = cap;
}
static void DoubleVector_push(DoubleVector *v, double value) {
    DoubleVector_reserve(v, tb_add_size(v->len, 1));
    v->data[v->len++] = value;
}
static bool DoubleVector_pop(DoubleVector *v, double *out) {
    if (!v->len) return false;
    --v->len;
    if (out) *out = v->data[v->len];
    return true;
}
static bool DoubleVector_insert(DoubleVector *v, size_t pos, double value) {
    if (pos > v->len) return false;
    DoubleVector_reserve(v, tb_add_size(v->len, 1));
    memmove(v->data + pos + 1, v->data + pos, (v->len - pos) * sizeof *v->data);
    v->data[pos] = value;
    ++v->len;
    return true;
}
static bool DoubleVector_erase(DoubleVector *v, size_t pos, size_t count) {
    if (pos > v->len || count > v->len - pos) return false;
    if (count) memmove(v->data + pos, v->data + pos + count,
                       (v->len - pos - count) * sizeof *v->data);
    v->len -= count;
    return true;
}
static DoubleVector DoubleVector_clone(const DoubleVector *v) {
    DoubleVector copy = {0};
    if (v->len) {
        DoubleVector_reserve(&copy, v->len);
        memcpy(copy.data, v->data, v->len * sizeof *v->data);
    }
    copy.len = v->len;
    return copy;
}
static void DoubleVector_clear(DoubleVector *v) { v->len = 0; }
static void DoubleVector_sort(DoubleVector *v, int (*compare)(const void *, const void *)) {
    if (v->len > 1) qsort(v->data, v->len, sizeof *v->data, compare);
}
static void DoubleVector_free(DoubleVector *v) { free(v->data); *v = (DoubleVector){0}; }

/* SPDX-License-Identifier: MIT-0 */
static bool DoubleVector_get(const DoubleVector *v, size_t pos, double *out) {
    if (pos >= v->len) return false;
    if (out) *out = v->data[pos];
    return true;
}
static bool DoubleVector_set(DoubleVector *v, size_t pos, double value) {
    if (pos >= v->len) return false;
    v->data[pos] = value;
    return true;
}
static bool DoubleVector_front(const DoubleVector *v, double *out) {
    return DoubleVector_get(v, 0, out);
}
static bool DoubleVector_back(const DoubleVector *v, double *out) {
    return v->len && DoubleVector_get(v, v->len - 1, out);
}
/* Snapshot supports extending from the vector's own storage. */
static void DoubleVector_extend(DoubleVector *v, double const *items, size_t count) {
    if (!count) return;
    size_t length = tb_add_size(v->len, count);
    double *copy = tb_resize(NULL, count, sizeof *copy);
    memcpy(copy, items, count * sizeof *copy);
    DoubleVector_reserve(v, length);
    memcpy(v->data + v->len, copy, count * sizeof *copy);
    v->len = length;
    free(copy);
}
static void DoubleVector_resize(DoubleVector *v, size_t length, double fill) {
    DoubleVector_reserve(v, length);
    for (size_t i = v->len; i < length; ++i) v->data[i] = fill;
    v->len = length;
}
static void DoubleVector_reverse(DoubleVector *v) {
    for (size_t i = 0; i < v->len / 2; ++i) {
        double value = v->data[i];
        v->data[i] = v->data[v->len - 1 - i];
        v->data[v->len - 1 - i] = value;
    }
}
static size_t DoubleVector_size(const DoubleVector *v) { return v->len; }
static bool DoubleVector_empty(const DoubleVector *v) { return v->len == 0; }
static double DoubleVector_at(const DoubleVector *v, size_t pos) {
    if (pos >= v->len) tb_fail("vector index out of range");
    return v->data[pos];
}
static void DoubleVector_fill(DoubleVector *v, double value) {
    for (size_t i = 0; i < v->len; ++i) v->data[i] = value;
}
static DoubleVector DoubleVector_slice(const DoubleVector *v, size_t pos, size_t count) {
    if (pos > v->len || count > v->len - pos) tb_fail("invalid vector slice");
    DoubleVector result = DoubleVector_new();
    if (count) DoubleVector_extend(&result, v->data + pos, count);
    return result;
}
static void DoubleVector_extend_vector(DoubleVector *v, const DoubleVector *other) {
    DoubleVector_extend(v, other->data, other->len);
}

/* SPDX-License-Identifier: MIT-0 */
/* Eager, same-type operations. Element copies follow the vector's shallow policy. */
static DoubleVector DoubleVector_from(double const *items, size_t count) {
    DoubleVector result = DoubleVector_new();
    DoubleVector_extend(&result, items, count);
    return result;
}
/* At least one value; use _new() for empty. Arguments are evaluated once. */
#define DoubleVector_values(...) DoubleVector_from((double const[]){__VA_ARGS__}, \
    sizeof((double const[]){__VA_ARGS__}) / sizeof(double))
static DoubleVector DoubleVector_filter(const DoubleVector *v, bool (*predicate)(double)) {
    if (!predicate) tb_fail("missing predicate");
    DoubleVector result = DoubleVector_new();
    for (size_t i = 0; i < v->len; ++i)
        if (predicate(v->data[i])) DoubleVector_push(&result, v->data[i]);
    return result;
}
static DoubleVector DoubleVector_map(const DoubleVector *v, double (*transform)(double)) {
    if (!transform) tb_fail("missing transform");
    DoubleVector result = DoubleVector_new();
    DoubleVector_reserve(&result, v->len);
    for (size_t i = 0; i < v->len; ++i) DoubleVector_push(&result, transform(v->data[i]));
    return result;
}
static void DoubleVector_map_inplace(DoubleVector *v, double (*transform)(double)) {
    if (!transform) tb_fail("missing transform");
    for (size_t i = 0; i < v->len; ++i) v->data[i] = transform(v->data[i]);
}
static size_t DoubleVector_find_if(const DoubleVector *v, bool (*predicate)(double)) {
    if (!predicate) tb_fail("missing predicate");
    for (size_t i = 0; i < v->len; ++i) if (predicate(v->data[i])) return i;
    return SIZE_MAX;
}
static size_t DoubleVector_count_if(const DoubleVector *v, bool (*predicate)(double)) {
    if (!predicate) tb_fail("missing predicate");
    size_t count = 0;
    for (size_t i = 0; i < v->len; ++i) if (predicate(v->data[i])) ++count;
    return count;
}
static bool DoubleVector_any(const DoubleVector *v, bool (*predicate)(double)) {
    return DoubleVector_find_if(v, predicate) != SIZE_MAX;
}
static bool DoubleVector_all(const DoubleVector *v, bool (*predicate)(double)) {
    if (!predicate) tb_fail("missing predicate");
    for (size_t i = 0; i < v->len; ++i) if (!predicate(v->data[i])) return false;
    return true;
}
static double DoubleVector_reduce(const DoubleVector *v, double initial,
                           double (*combine)(double, double)) {
    if (!combine) tb_fail("missing reducer");
    double result = initial;
    for (size_t i = 0; i < v->len; ++i) result = combine(result, v->data[i]);
    return result;
}

/* SPDX-License-Identifier: MIT-0 */
#include <math.h>
/* Numeric helpers for int, long long and double. Floating operands must be finite. */
static int DoubleVector_compare_asc(const void *a, const void *b) {
    double x = *(const double *)a, y = *(const double *)b;
    return (x > y) - (x < y);
}
static int DoubleVector_compare_desc(const void *a, const void *b) {
    return DoubleVector_compare_asc(b, a);
}
static void DoubleVector_sort_asc(DoubleVector *v) { DoubleVector_sort(v, DoubleVector_compare_asc); }
static void DoubleVector_sort_desc(DoubleVector *v) { DoubleVector_sort(v, DoubleVector_compare_desc); }
static size_t DoubleVector_find(const DoubleVector *v, double value) {
    for (size_t i = 0; i < v->len; ++i) if (v->data[i] == value) return i;
    return SIZE_MAX;
}
static bool DoubleVector_contains(const DoubleVector *v, double value) {
    return DoubleVector_find(v, value) != SIZE_MAX;
}
static size_t DoubleVector_count(const DoubleVector *v, double value) {
    size_t count = 0;
    for (size_t i = 0; i < v->len; ++i) if (v->data[i] == value) ++count;
    return count;
}
static size_t DoubleVector_lower_bound(const DoubleVector *v, double key) {
    size_t left = 0, right = v->len;
    while (left < right) {
        size_t mid = left + (right - left) / 2;
        if (v->data[mid] < key) left = mid + 1;
        else right = mid;
    }
    return left;
}
static size_t DoubleVector_upper_bound(const DoubleVector *v, double key) {
    size_t left = 0, right = v->len;
    while (left < right) {
        size_t mid = left + (right - left) / 2;
        if (v->data[mid] <= key) left = mid + 1;
        else right = mid;
    }
    return left;
}
static void DoubleVector_unique_sorted(DoubleVector *v) {
    size_t written = 0;
    for (size_t i = 0; i < v->len; ++i)
        if (!written || v->data[i] != v->data[written - 1]) v->data[written++] = v->data[i];
    v->len = written;
}
static size_t DoubleVector_argmin(const DoubleVector *v) {
    if (!v->len) tb_fail("argmin of empty vector");
    size_t best = 0;
    for (size_t i = 1; i < v->len; ++i) if (v->data[i] < v->data[best]) best = i;
    return best;
}
static size_t DoubleVector_argmax(const DoubleVector *v) {
    if (!v->len) tb_fail("argmax of empty vector");
    size_t best = 0;
    for (size_t i = 1; i < v->len; ++i) if (v->data[i] > v->data[best]) best = i;
    return best;
}
static double DoubleVector_min(const DoubleVector *v) { return v->data[DoubleVector_argmin(v)]; }
static double DoubleVector_max(const DoubleVector *v) { return v->data[DoubleVector_argmax(v)]; }
static double DoubleVector_mean(const DoubleVector *v) {
    if (!v->len) tb_fail("mean of empty vector");
    long double result = 0;
    for (size_t i = 0; i < v->len; ++i)
        result += ((long double)v->data[i] - result) / (long double)(i + 1);
    if (!isfinite((double)result)) tb_fail("nonfinite statistic");
    return (double)result;
}
/* Population variance, dividing by n. Original data is unchanged. */
static double DoubleVector_variance(const DoubleVector *v) {
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
static double DoubleVector_stddev(const DoubleVector *v) { return sqrt(DoubleVector_variance(v)); }
static double DoubleVector_median(const DoubleVector *v) {
    if (!v->len) tb_fail("median of empty vector");
    DoubleVector copy = DoubleVector_clone(v);
    DoubleVector_sort_asc(&copy);
    size_t mid = copy.len / 2;
    long double result = copy.data[mid];
    if (!(copy.len % 2)) result = ((long double)copy.data[mid - 1] + result) / 2;
    DoubleVector_free(&copy);
    if (!isfinite((double)result)) tb_fail("nonfinite median");
    return (double)result;
}

/* SPDX-License-Identifier: MIT-0 */
static double DoubleVector_sum(const DoubleVector *v) {
    long double total = 0;
    for (size_t i = 0; i < v->len; ++i) total += (long double)v->data[i];
    double result = (double)total;
    if (!isfinite(result)) tb_fail("floating sum outside double range");
    return result;
}

static DoubleVector read_doubles(size_t count) { DoubleVector v=DoubleVector_new();DoubleVector_reserve(&v,count);for(size_t i=0;i<count;++i) DoubleVector_push(&v,read_double());return v;}
static void print_doubles(const DoubleVector *v,const char *sep,const char *end) {for(size_t i=0;i<v->len;++i){if(i)print_text(sep);print_double(v->data[i]);}print_text(end);}

#endif
