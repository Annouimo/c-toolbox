#ifndef CTB_DEQUE_H
#define CTB_DEQUE_H
#include "ctb_core.h"
#ifndef CTB_JOIN
#define CTB_JOIN_RAW(a,b) a##b
#define CTB_JOIN(a,b) CTB_JOIN_RAW(a,b)
#endif
#define CTB_DEQUE_DEFINE(Name,Type) \
typedef struct { Type *data; size_t len, cap, head; } Name; \
static Name CTB_JOIN(Name,_new)(void) { return (Name){0}; } \
static size_t CTB_JOIN(Name,_size)(const Name *q) { return q->len; } \
static bool CTB_JOIN(Name,_empty)(const Name *q) { return q->len == 0; } \
static size_t CTB_JOIN(Name,_slot)(const Name *q, size_t pos) { \
size_t tail = q->cap - q->head; \
return pos < tail ? q->head + pos : pos - tail; \
} \
static void CTB_JOIN(Name,_reserve)(Name *q, size_t needed) { \
if (needed <= q->cap) return; \
size_t cap = tb_capacity(q->cap, needed); \
Type *data = tb_resize(NULL, cap, sizeof *data); \
for (size_t i = 0; i < q->len; ++i) data[i] = q->data[CTB_JOIN(Name,_slot)(q, i)]; \
free(q->data); \
q->data = data; q->cap = cap; q->head = 0; \
} \
static void CTB_JOIN(Name,_push_back)(Name *q, Type value) { \
CTB_JOIN(Name,_reserve)(q, tb_add_size(q->len, 1)); \
q->data[CTB_JOIN(Name,_slot)(q, q->len)] = value; \
++q->len; \
} \
static void CTB_JOIN(Name,_push_front)(Name *q, Type value) { \
CTB_JOIN(Name,_reserve)(q, tb_add_size(q->len, 1)); \
q->head = q->head ? q->head - 1 : q->cap - 1; \
q->data[q->head] = value; ++q->len; \
} \
static bool CTB_JOIN(Name,_get_into)(const Name *q, size_t pos, Type *out) { \
if (pos >= q->len) return false; \
if (out) *out = q->data[CTB_JOIN(Name,_slot)(q, pos)]; \
return true; \
} \
static bool CTB_JOIN(Name,_front_into)(const Name *q, Type *out) { return CTB_JOIN(Name,_get_into)(q, 0, out); } \
static bool CTB_JOIN(Name,_back_into)(const Name *q, Type *out) { \
return q->len && CTB_JOIN(Name,_get_into)(q, q->len - 1, out); \
} \
static bool CTB_JOIN(Name,_pop_front_into)(Name *q, Type *out) { \
if (!CTB_JOIN(Name,_front_into)(q, out)) return false; \
q->head = q->head + 1 == q->cap ? 0 : q->head + 1; \
--q->len; return true; \
} \
static bool CTB_JOIN(Name,_pop_back_into)(Name *q, Type *out) { \
if (!CTB_JOIN(Name,_back_into)(q, out)) return false; \
--q->len; return true; \
} \
static void CTB_JOIN(Name,_clear)(Name *q) { q->len = q->head = 0; } \
static Name CTB_JOIN(Name,_clone)(const Name *q) { \
Name copy = {0}; \
CTB_JOIN(Name,_reserve)(&copy, q->len); \
for (size_t i = 0; i < q->len; ++i) \
copy.data[i] = q->data[CTB_JOIN(Name,_slot)(q, i)]; \
copy.len = q->len; \
return copy; \
} \
static void CTB_JOIN(Name,_free)(Name *q) { free(q->data); *q = (Name){0}; } \
typedef struct { bool ok; Type value; } CTB_JOIN(Name,Result); \
static CTB_JOIN(Name,Result) CTB_JOIN(Name,_try_get)(const Name *v,size_t pos) { CTB_JOIN(Name,Result) r={false,(Type){0}};r.ok=CTB_JOIN(Name,_get_into)(v,pos,&r.value);return r;} \
static Type CTB_JOIN(Name,_get)(const Name *v,size_t pos) { CTB_JOIN(Name,Result) r=CTB_JOIN(Name,_try_get)(v,pos);if(!r.ok)tb_fail("invalid container access");return r.value;} \
static CTB_JOIN(Name,Result) CTB_JOIN(Name,_try_front)(const Name *v) { CTB_JOIN(Name,Result) r={false,(Type){0}};r.ok=CTB_JOIN(Name,_front_into)(v,&r.value);return r;} \
static Type CTB_JOIN(Name,_front)(const Name *v) { CTB_JOIN(Name,Result) r=CTB_JOIN(Name,_try_front)(v);if(!r.ok)tb_fail("invalid container access");return r.value;} \
static CTB_JOIN(Name,Result) CTB_JOIN(Name,_try_back)(const Name *v) { CTB_JOIN(Name,Result) r={false,(Type){0}};r.ok=CTB_JOIN(Name,_back_into)(v,&r.value);return r;} \
static Type CTB_JOIN(Name,_back)(const Name *v) { CTB_JOIN(Name,Result) r=CTB_JOIN(Name,_try_back)(v);if(!r.ok)tb_fail("invalid container access");return r.value;} \
static CTB_JOIN(Name,Result) CTB_JOIN(Name,_try_pop_front)(Name *v) { CTB_JOIN(Name,Result) r={false,(Type){0}};r.ok=CTB_JOIN(Name,_pop_front_into)(v,&r.value);return r;} \
static Type CTB_JOIN(Name,_pop_front)(Name *v) { CTB_JOIN(Name,Result) r=CTB_JOIN(Name,_try_pop_front)(v);if(!r.ok)tb_fail("invalid container access");return r.value;} \
static CTB_JOIN(Name,Result) CTB_JOIN(Name,_try_pop_back)(Name *v) { CTB_JOIN(Name,Result) r={false,(Type){0}};r.ok=CTB_JOIN(Name,_pop_back_into)(v,&r.value);return r;} \
static Type CTB_JOIN(Name,_pop_back)(Name *v) { CTB_JOIN(Name,Result) r=CTB_JOIN(Name,_try_pop_back)(v);if(!r.ok)tb_fail("invalid container access");return r.value;}

#endif
