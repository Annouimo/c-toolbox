#ifndef CTB_HEAP_H
#define CTB_HEAP_H
#include "core.h"
#ifndef CTB_JOIN
#define CTB_JOIN_RAW(a,b) a##b
#define CTB_JOIN(a,b) CTB_JOIN_RAW(a,b)
#endif
#define CTB_HEAP_DEFINE(Name,Type) \
typedef struct { \
Type *data; \
size_t len, cap; \
int (*compare)(const void *, const void *); \
} Name; \
static Name CTB_JOIN(Name,_new)(int (*compare)(const void *, const void *)) { \
if (!compare) tb_fail("heap needs a comparator"); \
Name h = {0}; h.compare = compare; return h; \
} \
static void CTB_JOIN(Name,_reserve)(Name *h, size_t needed) { \
if (needed <= h->cap) return; \
size_t cap = tb_capacity(h->cap, needed); \
h->data = tb_resize(h->data, cap, sizeof *h->data); \
h->cap = cap; \
} \
static void CTB_JOIN(Name,_sift_down)(Name *h, size_t pos) { \
Type value = h->data[pos]; \
while (pos < h->len / 2) { \
size_t child = pos * 2 + 1; \
if (child + 1 < h->len && \
h->compare(&h->data[child + 1], &h->data[child]) < 0) ++child; \
if (h->compare(&value, &h->data[child]) <= 0) break; \
h->data[pos] = h->data[child]; pos = child; \
} \
h->data[pos] = value; \
} \
static void CTB_JOIN(Name,_push)(Name *h, Type value) { \
if (!h->compare) tb_fail("heap needs a comparator"); \
CTB_JOIN(Name,_reserve)(h, tb_add_size(h->len, 1)); \
size_t pos = h->len++; \
while (pos) { \
size_t parent = (pos - 1) / 2; \
if (h->compare(&h->data[parent], &value) <= 0) break; \
h->data[pos] = h->data[parent]; pos = parent; \
} \
h->data[pos] = value; \
} \
static bool CTB_JOIN(Name,_peek_into)(const Name *h, Type *out) { \
if (!h->len) return false; \
if (out) *out = h->data[0]; \
return true; \
} \
static bool CTB_JOIN(Name,_pop_into)(Name *h, Type *out) { \
if (!CTB_JOIN(Name,_peek_into)(h, out)) return false; \
--h->len; \
if (h->len) { \
h->data[0] = h->data[h->len]; \
CTB_JOIN(Name,_sift_down)(h, 0); \
} \
return true; \
} \
static Name CTB_JOIN(Name,_from)(Type const *items, size_t count, \
int (*compare)(const void *, const void *)) { \
Name h = CTB_JOIN(Name,_new)(compare); \
CTB_JOIN(Name,_reserve)(&h, count); \
if (count) memcpy(h.data, items, count * sizeof *h.data); \
h.len = count; \
for (size_t i = count / 2; i > 0; --i) CTB_JOIN(Name,_sift_down)(&h, i - 1); \
return h; \
} \
static Name CTB_JOIN(Name,_clone)(const Name *h) { \
Name copy = *h; \
copy.data = NULL; copy.cap = 0; \
CTB_JOIN(Name,_reserve)(&copy, h->len); \
if (h->len) memcpy(copy.data, h->data, h->len * sizeof *h->data); \
return copy; \
} \
static void CTB_JOIN(Name,_clear)(Name *h) { h->len = 0; } \
static void CTB_JOIN(Name,_free)(Name *h) { free(h->data); *h = (Name){0}; } \
static size_t CTB_JOIN(Name,_size)(const Name *h) { return h->len; } \
static bool CTB_JOIN(Name,_empty)(const Name *h) { return h->len == 0; } \
typedef struct { bool ok; Type value; } CTB_JOIN(Name,Result); \
static CTB_JOIN(Name,Result) CTB_JOIN(Name,_try_peek)(const Name *v) { CTB_JOIN(Name,Result) r={false,(Type){0}};r.ok=CTB_JOIN(Name,_peek_into)(v,&r.value);return r;} \
static Type CTB_JOIN(Name,_peek)(const Name *v) { CTB_JOIN(Name,Result) r=CTB_JOIN(Name,_try_peek)(v);if(!r.ok)tb_fail("invalid container access");return r.value;} \
static CTB_JOIN(Name,Result) CTB_JOIN(Name,_try_pop)(Name *v) { CTB_JOIN(Name,Result) r={false,(Type){0}};r.ok=CTB_JOIN(Name,_pop_into)(v,&r.value);return r;} \
static Type CTB_JOIN(Name,_pop)(Name *v) { CTB_JOIN(Name,Result) r=CTB_JOIN(Name,_try_pop)(v);if(!r.ok)tb_fail("invalid container access");return r.value;}

#endif
