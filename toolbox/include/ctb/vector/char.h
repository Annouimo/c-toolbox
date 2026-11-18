#ifndef CTB_VECTOR_CHAR_H
#define CTB_VECTOR_CHAR_H
#include "../io.h"
/* SPDX-License-Identifier: MIT-0 */
/* Typed, contiguous storage. Elements are shallow copied; no element destructor. */
typedef struct { char *data; size_t len, cap; } CharVector;
static CharVector CharVector_new(void) { return (CharVector){0}; }
static void CharVector_reserve(CharVector *v, size_t needed) {
    if (needed <= v->cap) return;
    size_t cap = tb_capacity(v->cap, needed);
    v->data = tb_resize(v->data, cap, sizeof *v->data);
    v->cap = cap;
}
static void CharVector_push(CharVector *v, char value) {
    CharVector_reserve(v, tb_add_size(v->len, 1));
    v->data[v->len++] = value;
}
static bool CharVector_pop_into(CharVector *v, char *out) {
    if (!v->len) return false;
    --v->len;
    if (out) *out = v->data[v->len];
    return true;
}
static bool CharVector_insert(CharVector *v, size_t pos, char value) {
    if (pos > v->len) return false;
    CharVector_reserve(v, tb_add_size(v->len, 1));
    memmove(v->data + pos + 1, v->data + pos, (v->len - pos) * sizeof *v->data);
    v->data[pos] = value;
    ++v->len;
    return true;
}
static bool CharVector_erase(CharVector *v, size_t pos, size_t count) {
    if (pos > v->len || count > v->len - pos) return false;
    if (count) memmove(v->data + pos, v->data + pos + count,
                       (v->len - pos - count) * sizeof *v->data);
    v->len -= count;
    return true;
}
static CharVector CharVector_clone(const CharVector *v) {
    CharVector copy = {0};
    if (v->len) {
        CharVector_reserve(&copy, v->len);
        memcpy(copy.data, v->data, v->len * sizeof *v->data);
    }
    copy.len = v->len;
    return copy;
}
static void CharVector_clear(CharVector *v) { v->len = 0; }
static void CharVector_sort(CharVector *v, int (*compare)(const void *, const void *)) {
    if (v->len > 1) qsort(v->data, v->len, sizeof *v->data, compare);
}
static void CharVector_free(CharVector *v) { free(v->data); *v = (CharVector){0}; }
static void CharVector_swap(CharVector *a,CharVector *b) { CharVector value=*a;*a=*b;*b=value; }

/* SPDX-License-Identifier: MIT-0 */
static bool CharVector_get_into(const CharVector *v, size_t pos, char *out) {
    if (pos >= v->len) return false;
    if (out) *out = v->data[pos];
    return true;
}
static bool CharVector_try_set(CharVector *v, size_t pos, char value) {
    if (pos >= v->len) return false;
    v->data[pos] = value;
    return true;
}
static bool CharVector_front_into(const CharVector *v, char *out) {
    return CharVector_get_into(v, 0, out);
}
static bool CharVector_back_into(const CharVector *v, char *out) {
    return v->len && CharVector_get_into(v, v->len - 1, out);
}
/* Snapshot supports extending from the vector's own storage. */
static void CharVector_extend(CharVector *v, char const *items, size_t count) {
    if (!count) return;
    size_t length = tb_add_size(v->len, count);
    char *copy = tb_resize(NULL, count, sizeof *copy);
    memcpy(copy, items, count * sizeof *copy);
    CharVector_reserve(v, length);
    memcpy(v->data + v->len, copy, count * sizeof *copy);
    v->len = length;
    free(copy);
}
static void CharVector_resize(CharVector *v, size_t length, char fill) {
    CharVector_reserve(v, length);
    for (size_t i = v->len; i < length; ++i) v->data[i] = fill;
    v->len = length;
}
static void CharVector_reverse(CharVector *v) {
    for (size_t i = 0; i < v->len / 2; ++i) {
        char value = v->data[i];
        v->data[i] = v->data[v->len - 1 - i];
        v->data[v->len - 1 - i] = value;
    }
}
static size_t CharVector_size(const CharVector *v) { return v->len; }
static bool CharVector_empty(const CharVector *v) { return v->len == 0; }
static char CharVector_at(const CharVector *v, size_t pos) {
    if (pos >= v->len) tb_fail("vector index out of range");
    return v->data[pos];
}
static void CharVector_fill(CharVector *v, char value) {
    for (size_t i = 0; i < v->len; ++i) v->data[i] = value;
}
static CharVector CharVector_slice(const CharVector *v, size_t pos, size_t count) {
    if (pos > v->len || count > v->len - pos) tb_fail("invalid vector slice");
    CharVector result = CharVector_new();
    if (count) CharVector_extend(&result, v->data + pos, count);
    return result;
}
/* Forward half-open slice [first,last) with a positive stride. */
static CharVector CharVector_slice_step(const CharVector *v,size_t first,size_t last,size_t step) {
    if(!step || first>last || last>v->len)tb_fail("invalid vector slice");
    CharVector result=CharVector_new();
    size_t count=first==last?0:1+(last-first-1)/step;
    CharVector_reserve(&result,count);
    for(size_t i=first;i<last;) { CharVector_push(&result,v->data[i]);if(step>last-1-i)break;i+=step; }
    return result;
}
static void CharVector_extend_vector(CharVector *v, const CharVector *other) {
    CharVector_extend(v, other->data, other->len);
}

/* SPDX-License-Identifier: MIT-0 */
/* Eager, same-type operations. Element copies follow the vector's shallow policy. */
static CharVector CharVector_from(char const *items, size_t count) {
    CharVector result = CharVector_new();
    CharVector_extend(&result, items, count);
    return result;
}
/* At least one value; use _new() for empty. Arguments are evaluated once. */
#define CharVector_values(...) CharVector_from((char const[]){__VA_ARGS__}, \
    sizeof((char const[]){__VA_ARGS__}) / sizeof(char))
static CharVector CharVector_filter(const CharVector *v, bool (*predicate)(char)) {
    if (!predicate) tb_fail("missing predicate");
    CharVector result = CharVector_new();
    for (size_t i = 0; i < v->len; ++i)
        if (predicate(v->data[i])) CharVector_push(&result, v->data[i]);
    return result;
}
static CharVector CharVector_filter_ctx(const CharVector *v, bool (*predicate)(char,void *),void *context) {
    if (!predicate) tb_fail("missing predicate");
    CharVector result=CharVector_new();
    for(size_t i=0;i<v->len;++i)if(predicate(v->data[i],context))CharVector_push(&result,v->data[i]);
    return result;
}
static CharVector CharVector_map(const CharVector *v, char (*transform)(char)) {
    if (!transform) tb_fail("missing transform");
    CharVector result = CharVector_new();
    CharVector_reserve(&result, v->len);
    for (size_t i = 0; i < v->len; ++i) CharVector_push(&result, transform(v->data[i]));
    return result;
}
static CharVector CharVector_map_ctx(const CharVector *v,char (*transform)(char,void *),void *context) {
    if(!transform)tb_fail("missing transform");
    CharVector result=CharVector_new();CharVector_reserve(&result,v->len);
    for(size_t i=0;i<v->len;++i)CharVector_push(&result,transform(v->data[i],context));
    return result;
}
static void CharVector_map_inplace(CharVector *v, char (*transform)(char)) {
    if (!transform) tb_fail("missing transform");
    for (size_t i = 0; i < v->len; ++i) v->data[i] = transform(v->data[i]);
}
static size_t CharVector_find_if(const CharVector *v, bool (*predicate)(char)) {
    if (!predicate) tb_fail("missing predicate");
    for (size_t i = 0; i < v->len; ++i) if (predicate(v->data[i])) return i;
    return SIZE_MAX;
}
static size_t CharVector_find_if_ctx(const CharVector *v,bool (*predicate)(char,void *),void *context) {
    if(!predicate)tb_fail("missing predicate");
    for(size_t i=0;i<v->len;++i)if(predicate(v->data[i],context))return i;
    return SIZE_MAX;
}
static size_t CharVector_count_if(const CharVector *v, bool (*predicate)(char)) {
    if (!predicate) tb_fail("missing predicate");
    size_t count = 0;
    for (size_t i = 0; i < v->len; ++i) if (predicate(v->data[i])) ++count;
    return count;
}
static bool CharVector_any(const CharVector *v, bool (*predicate)(char)) {
    return CharVector_find_if(v, predicate) != SIZE_MAX;
}
static bool CharVector_all(const CharVector *v, bool (*predicate)(char)) {
    if (!predicate) tb_fail("missing predicate");
    for (size_t i = 0; i < v->len; ++i) if (!predicate(v->data[i])) return false;
    return true;
}
static char CharVector_reduce(const CharVector *v, char initial,
                           char (*combine)(char, char)) {
    if (!combine) tb_fail("missing reducer");
    char result = initial;
    for (size_t i = 0; i < v->len; ++i) result = combine(result, v->data[i]);
    return result;
}
static char CharVector_reduce_ctx(const CharVector *v,char initial,char (*combine)(char,char,void *),void *context) {
    if(!combine)tb_fail("missing reducer");
    for(size_t i=0;i<v->len;++i)initial=combine(initial,v->data[i],context);
    return initial;
}

/* SPDX-License-Identifier: MIT-0 */
static size_t CharVector_find_from(const CharVector *v, char value, size_t start) {
    for (size_t i = start; i < v->len; ++i) if (v->data[i] == value) return i;
    return SIZE_MAX;
}
static size_t CharVector_find(const CharVector *v, char value) {
    return CharVector_find_from(v, value, 0);
}
static bool CharVector_contains(const CharVector *v, char value) {
    return CharVector_find(v, value) != SIZE_MAX;
}
static size_t CharVector_count(const CharVector *v, char value) {
    size_t count = 0;
    for (size_t i = 0; i < v->len; ++i) if (v->data[i] == value) ++count;
    return count;
}

/* SPDX-License-Identifier: MIT-0 */
static IndexRange CharVector_longest_run(const CharVector *v, char value) {
    IndexRange best = {0, 0};
    size_t start = 0;
    for (size_t i = 0; i < v->len; ++i) {
        if (!(v->data[i] == value)) start = i + 1;
        else if (i + 1 - start > best.end - best.begin)
            best = (IndexRange){start, i + 1};
    }
    return best;
}

/* SPDX-License-Identifier: MIT-0 */
typedef struct { bool ok; char value; } CharVectorResult;
static CharVectorResult CharVector_try_get(const CharVector *v,size_t pos){CharVectorResult r={0};r.ok=CharVector_get_into(v,pos,&r.value);return r;}
static char CharVector_get(const CharVector *v,size_t pos){CharVectorResult r=CharVector_try_get(v,pos);if(!r.ok)tb_fail("vector index out of range");return r.value;}
static CharVectorResult CharVector_try_pop(CharVector *v){CharVectorResult r={0};r.ok=CharVector_pop_into(v,&r.value);return r;}
static char CharVector_pop(CharVector *v){CharVectorResult r=CharVector_try_pop(v);if(!r.ok)tb_fail("pop of empty vector");return r.value;}
static char CharVector_front(const CharVector *v){return CharVector_get(v,0);}
static char CharVector_back(const CharVector *v){if(!v->len)tb_fail("back of empty vector");return CharVector_get(v,v->len-1);}
static void CharVector_merge_sort(char *data,char *scratch,size_t n,int (*compare)(const char *,const char *)){
    if(n<2)return;
    size_t mid=n/2;
    CharVector_merge_sort(data,scratch,mid,compare);CharVector_merge_sort(data+mid,scratch+mid,n-mid,compare);
    size_t i=0,j=mid,k=0;
    while(i<mid && j<n){if(compare(&data[i],&data[j])<=0)scratch[k++]=data[i++];else scratch[k++]=data[j++];}
    while(i<mid)scratch[k++]=data[i++];
    while(j<n)scratch[k++]=data[j++];
    memcpy(data,scratch,n*sizeof *data);
}
/* Stable, typed comparison; no void-pointer casts required from callers. */
static void CharVector_sort_by(CharVector *v,int (*compare)(const char *,const char *)){
    if(!compare)tb_fail("null comparator");
    if(v->len<2)return;
    char *scratch=tb_resize(NULL,v->len,sizeof *scratch);CharVector_merge_sort(v->data,scratch,v->len,compare);free(scratch);
}

static CharVector CharVector_filled(size_t length, char value) {
    CharVector v = CharVector_new();
    CharVector_resize(&v, length, value);
    return v;
}
static bool CharVector_in_bounds(const CharVector *v, intmax_t pos) {
    return pos >= 0 && (uintmax_t)pos < v->len;
}
static void CharVector_set(CharVector *v, size_t pos, char value) {
    if (!CharVector_try_set(v, pos, value)) tb_fail("vector index out of range");
}

static CharVector read_chars(size_t count) { CharVector v=CharVector_new();CharVector_reserve(&v,count);for(size_t i=0;i<count;++i) CharVector_push(&v,read_char());return v;}

static void print_chars(const CharVector *v,const char *sep,const char *end) {for(size_t i=0;i<v->len;++i){if(i)print_text(sep,"");print_char(v->data[i],"");}print_text("",end);}

#endif
