#ifndef CTB_VECTOR_CUSTOM_H
#define CTB_VECTOR_CUSTOM_H
#include "../detail/core.h"
#ifndef CTB_JOIN
#define CTB_JOIN_RAW(a,b) a##b
#define CTB_JOIN(a,b) CTB_JOIN_RAW(a,b)
#endif
#define CTB_VECTOR_DEFINE(Name,Type) \
typedef struct { Type *data; size_t len, cap; } Name; \
static Name CTB_JOIN(Name,_new)(void) { return (Name){0}; } \
static void CTB_JOIN(Name,_reserve)(Name *v, size_t needed) { \
if (needed <= v->cap) return; \
size_t cap = tb_capacity(v->cap, needed); \
v->data = tb_resize(v->data, cap, sizeof *v->data); \
v->cap = cap; \
} \
static void CTB_JOIN(Name,_push)(Name *v, Type value) { \
CTB_JOIN(Name,_reserve)(v, tb_add_size(v->len, 1)); \
v->data[v->len++] = value; \
} \
static bool CTB_JOIN(Name,_pop_into)(Name *v, Type *out) { \
if (!v->len) return false; \
--v->len; \
if (out) *out = v->data[v->len]; \
return true; \
} \
static bool CTB_JOIN(Name,_insert)(Name *v, size_t pos, Type value) { \
if (pos > v->len) return false; \
CTB_JOIN(Name,_reserve)(v, tb_add_size(v->len, 1)); \
memmove(v->data + pos + 1, v->data + pos, (v->len - pos) * sizeof *v->data); \
v->data[pos] = value; \
++v->len; \
return true; \
} \
static bool CTB_JOIN(Name,_erase)(Name *v, size_t pos, size_t count) { \
if (pos > v->len || count > v->len - pos) return false; \
if (count) memmove(v->data + pos, v->data + pos + count, \
(v->len - pos - count) * sizeof *v->data); \
v->len -= count; \
return true; \
} \
static Name CTB_JOIN(Name,_clone)(const Name *v) { \
Name copy = {0}; \
if (v->len) { \
CTB_JOIN(Name,_reserve)(&copy, v->len); \
memcpy(copy.data, v->data, v->len * sizeof *v->data); \
} \
copy.len = v->len; \
return copy; \
} \
static void CTB_JOIN(Name,_clear)(Name *v) { v->len = 0; } \
static void CTB_JOIN(Name,_sort)(Name *v, int (*compare)(const void *, const void *)) { \
if (v->len > 1) qsort(v->data, v->len, sizeof *v->data, compare); \
} \
static void CTB_JOIN(Name,_free)(Name *v) { free(v->data); *v = (Name){0}; } \
static void CTB_JOIN(Name,_swap)(Name *a,Name *b) { Name value=*a;*a=*b;*b=value; } \
static bool CTB_JOIN(Name,_get_into)(const Name *v, size_t pos, Type *out) { \
if (pos >= v->len) return false; \
if (out) *out = v->data[pos]; \
return true; \
} \
static bool CTB_JOIN(Name,_set)(Name *v, size_t pos, Type value) { \
if (pos >= v->len) return false; \
v->data[pos] = value; \
return true; \
} \
static bool CTB_JOIN(Name,_front_into)(const Name *v, Type *out) { \
return CTB_JOIN(Name,_get_into)(v, 0, out); \
} \
static bool CTB_JOIN(Name,_back_into)(const Name *v, Type *out) { \
return v->len && CTB_JOIN(Name,_get_into)(v, v->len - 1, out); \
} \
static void CTB_JOIN(Name,_extend)(Name *v, Type const *items, size_t count) { \
if (!count) return; \
size_t length = tb_add_size(v->len, count); \
Type *copy = tb_resize(NULL, count, sizeof *copy); \
memcpy(copy, items, count * sizeof *copy); \
CTB_JOIN(Name,_reserve)(v, length); \
memcpy(v->data + v->len, copy, count * sizeof *copy); \
v->len = length; \
free(copy); \
} \
static void CTB_JOIN(Name,_resize)(Name *v, size_t length, Type fill) { \
CTB_JOIN(Name,_reserve)(v, length); \
for (size_t i = v->len; i < length; ++i) v->data[i] = fill; \
v->len = length; \
} \
static void CTB_JOIN(Name,_reverse)(Name *v) { \
for (size_t i = 0; i < v->len / 2; ++i) { \
Type value = v->data[i]; \
v->data[i] = v->data[v->len - 1 - i]; \
v->data[v->len - 1 - i] = value; \
} \
} \
static size_t CTB_JOIN(Name,_size)(const Name *v) { return v->len; } \
static bool CTB_JOIN(Name,_empty)(const Name *v) { return v->len == 0; } \
static Type CTB_JOIN(Name,_at)(const Name *v, size_t pos) { \
if (pos >= v->len) tb_fail("vector index out of range"); \
return v->data[pos]; \
} \
static void CTB_JOIN(Name,_fill)(Name *v, Type value) { \
for (size_t i = 0; i < v->len; ++i) v->data[i] = value; \
} \
static Name CTB_JOIN(Name,_slice)(const Name *v, size_t pos, size_t count) { \
if (pos > v->len || count > v->len - pos) tb_fail("invalid vector slice"); \
Name result = CTB_JOIN(Name,_new)(); \
if (count) CTB_JOIN(Name,_extend)(&result, v->data + pos, count); \
return result; \
} \
static Name CTB_JOIN(Name,_slice_step)(const Name *v,size_t first,size_t last,size_t step) { \
if(!step || first>last || last>v->len)tb_fail("invalid vector slice"); \
Name result=CTB_JOIN(Name,_new)(); \
size_t count=first==last?0:1+(last-first-1)/step; \
CTB_JOIN(Name,_reserve)(&result,count); \
for(size_t i=first;i<last;) { CTB_JOIN(Name,_push)(&result,v->data[i]);if(step>last-1-i)break;i+=step; } \
return result; \
} \
static void CTB_JOIN(Name,_extend_vector)(Name *v, const Name *other) { \
CTB_JOIN(Name,_extend)(v, other->data, other->len); \
} \
static Name CTB_JOIN(Name,_from)(Type const *items, size_t count) { \
Name result = CTB_JOIN(Name,_new)(); \
CTB_JOIN(Name,_extend)(&result, items, count); \
return result; \
} \
static Name CTB_JOIN(Name,_filter)(const Name *v, bool (*predicate)(Type)) { \
if (!predicate) tb_fail("missing predicate"); \
Name result = CTB_JOIN(Name,_new)(); \
for (size_t i = 0; i < v->len; ++i) \
if (predicate(v->data[i])) CTB_JOIN(Name,_push)(&result, v->data[i]); \
return result; \
} \
static Name CTB_JOIN(Name,_filter_ctx)(const Name *v, bool (*predicate)(Type,void *),void *context) { \
if (!predicate) tb_fail("missing predicate"); \
Name result=CTB_JOIN(Name,_new)(); \
for(size_t i=0;i<v->len;++i)if(predicate(v->data[i],context))CTB_JOIN(Name,_push)(&result,v->data[i]); \
return result; \
} \
static Name CTB_JOIN(Name,_map)(const Name *v, Type (*transform)(Type)) { \
if (!transform) tb_fail("missing transform"); \
Name result = CTB_JOIN(Name,_new)(); \
CTB_JOIN(Name,_reserve)(&result, v->len); \
for (size_t i = 0; i < v->len; ++i) CTB_JOIN(Name,_push)(&result, transform(v->data[i])); \
return result; \
} \
static Name CTB_JOIN(Name,_map_ctx)(const Name *v,Type (*transform)(Type,void *),void *context) { \
if(!transform)tb_fail("missing transform"); \
Name result=CTB_JOIN(Name,_new)();CTB_JOIN(Name,_reserve)(&result,v->len); \
for(size_t i=0;i<v->len;++i)CTB_JOIN(Name,_push)(&result,transform(v->data[i],context)); \
return result; \
} \
static void CTB_JOIN(Name,_map_inplace)(Name *v, Type (*transform)(Type)) { \
if (!transform) tb_fail("missing transform"); \
for (size_t i = 0; i < v->len; ++i) v->data[i] = transform(v->data[i]); \
} \
static size_t CTB_JOIN(Name,_find_if)(const Name *v, bool (*predicate)(Type)) { \
if (!predicate) tb_fail("missing predicate"); \
for (size_t i = 0; i < v->len; ++i) if (predicate(v->data[i])) return i; \
return SIZE_MAX; \
} \
static size_t CTB_JOIN(Name,_find_if_ctx)(const Name *v,bool (*predicate)(Type,void *),void *context) { \
if(!predicate)tb_fail("missing predicate"); \
for(size_t i=0;i<v->len;++i)if(predicate(v->data[i],context))return i; \
return SIZE_MAX; \
} \
static size_t CTB_JOIN(Name,_count_if)(const Name *v, bool (*predicate)(Type)) { \
if (!predicate) tb_fail("missing predicate"); \
size_t count = 0; \
for (size_t i = 0; i < v->len; ++i) if (predicate(v->data[i])) ++count; \
return count; \
} \
static bool CTB_JOIN(Name,_any)(const Name *v, bool (*predicate)(Type)) { \
return CTB_JOIN(Name,_find_if)(v, predicate) != SIZE_MAX; \
} \
static bool CTB_JOIN(Name,_all)(const Name *v, bool (*predicate)(Type)) { \
if (!predicate) tb_fail("missing predicate"); \
for (size_t i = 0; i < v->len; ++i) if (!predicate(v->data[i])) return false; \
return true; \
} \
static Type CTB_JOIN(Name,_reduce)(const Name *v, Type initial, \
Type (*combine)(Type, Type)) { \
if (!combine) tb_fail("missing reducer"); \
Type result = initial; \
for (size_t i = 0; i < v->len; ++i) result = combine(result, v->data[i]); \
return result; \
} \
static Type CTB_JOIN(Name,_reduce_ctx)(const Name *v,Type initial,Type (*combine)(Type,Type,void *),void *context) { \
if(!combine)tb_fail("missing reducer"); \
for(size_t i=0;i<v->len;++i)initial=combine(initial,v->data[i],context); \
return initial; \
} \
typedef struct { bool ok; Type value; } CTB_JOIN(Name,Result); \
static CTB_JOIN(Name,Result) CTB_JOIN(Name,_try_get)(const Name *v,size_t pos){CTB_JOIN(Name,Result) r={0};r.ok=CTB_JOIN(Name,_get_into)(v,pos,&r.value);return r;} \
static Type CTB_JOIN(Name,_get)(const Name *v,size_t pos){CTB_JOIN(Name,Result) r=CTB_JOIN(Name,_try_get)(v,pos);if(!r.ok)tb_fail("vector index out of range");return r.value;} \
static CTB_JOIN(Name,Result) CTB_JOIN(Name,_try_pop)(Name *v){CTB_JOIN(Name,Result) r={0};r.ok=CTB_JOIN(Name,_pop_into)(v,&r.value);return r;} \
static Type CTB_JOIN(Name,_pop)(Name *v){CTB_JOIN(Name,Result) r=CTB_JOIN(Name,_try_pop)(v);if(!r.ok)tb_fail("pop of empty vector");return r.value;} \
static Type CTB_JOIN(Name,_front)(const Name *v){return CTB_JOIN(Name,_get)(v,0);} \
static Type CTB_JOIN(Name,_back)(const Name *v){if(!v->len)tb_fail("back of empty vector");return CTB_JOIN(Name,_get)(v,v->len-1);} \
static void CTB_JOIN(Name,_merge_sort)(Type *data,Type *scratch,size_t n,int (*compare)(const Type *,const Type *)){ \
if(n<2)return; \
size_t mid=n/2; \
CTB_JOIN(Name,_merge_sort)(data,scratch,mid,compare);CTB_JOIN(Name,_merge_sort)(data+mid,scratch+mid,n-mid,compare); \
size_t i=0,j=mid,k=0; \
while(i<mid && j<n){if(compare(&data[i],&data[j])<=0)scratch[k++]=data[i++];else scratch[k++]=data[j++];} \
while(i<mid)scratch[k++]=data[i++]; \
while(j<n)scratch[k++]=data[j++]; \
memcpy(data,scratch,n*sizeof *data); \
} \
static void CTB_JOIN(Name,_sort_by)(Name *v,int (*compare)(const Type *,const Type *)){ \
if(!compare)tb_fail("null comparator"); \
if(v->len<2)return; \
Type *scratch=tb_resize(NULL,v->len,sizeof *scratch);CTB_JOIN(Name,_merge_sort)(v->data,scratch,v->len,compare);free(scratch); \
}

#endif
