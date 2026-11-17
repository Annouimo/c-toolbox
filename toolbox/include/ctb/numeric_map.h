/* SPDX-License-Identifier: MIT-0 */
#ifndef CTB_NUMERIC_MAP_H
#define CTB_NUMERIC_MAP_H
#include "core.h"
/* Integer key -> plain value map. Keys are int/long long; values are scalar/trivially copied. */
static uint64_t ctb_numeric_map_hash(uint64_t x) {
    x^=x>>30;x*=UINT64_C(0xbf58476d1ce4e5b9);x^=x>>27;
    x*=UINT64_C(0x94d049bb133111eb);return x^(x>>31);
}
#define CTB_NUMERIC_MAP_DEFINE(Name,KeyType,ValueType) \
typedef struct { KeyType *keys; ValueType *values; unsigned char *state; size_t len,cap; } Name; \
typedef struct { bool found; ValueType value; } Name##Result; \
static size_t Name##_slot(const Name *m,KeyType key,bool *found) { \
    size_t mask=m->cap-1,pos=(size_t)ctb_numeric_map_hash((uint64_t)key)&mask,first=SIZE_MAX; \
    for(;;) { if(!m->state[pos]) { *found=false;return first==SIZE_MAX?pos:first; } \
        if(m->state[pos]==2) { if(first==SIZE_MAX)first=pos; } \
        else if(m->keys[pos]==key) { *found=true;return pos; } pos=(pos+1)&mask; } } \
static void Name##_reserve(Name *m,size_t needed) { \
    if(m->cap && needed<m->cap-m->cap/3)return; size_t cap=m->cap?m->cap:8; \
    while(needed>=cap-cap/3) { if(cap>SIZE_MAX/2)tb_fail("map size overflow");cap*=2; } \
    Name next={0};next.cap=cap;next.keys=tb_resize(NULL,cap,sizeof *next.keys); \
    next.values=tb_resize(NULL,cap,sizeof *next.values);next.state=tb_resize(NULL,cap,sizeof *next.state); \
    memset(next.state,0,cap);for(size_t i=0;i<m->cap;++i)if(m->state[i]==1) { bool found=false;size_t p=Name##_slot(&next,m->keys[i],&found); \
        next.state[p]=1;next.keys[p]=m->keys[i];next.values[p]=m->values[i];++next.len; } \
    free(m->keys);free(m->values);free(m->state);*m=next; } \
static Name Name##_new(void) { return (Name){0}; } \
static size_t Name##_size(const Name *m) { return m->len; } \
static void Name##_put(Name *m,KeyType key,ValueType value) { \
    bool found=false;if(!m->cap)Name##_reserve(m,1);size_t p=Name##_slot(m,key,&found); \
    if(!found) { if(m->len>=m->cap-m->cap/3) { Name##_reserve(m,m->len+1);p=Name##_slot(m,key,&found); } \
        ++m->len; } m->state[p]=1;m->keys[p]=key;m->values[p]=value; } \
static Name##Result Name##_get(const Name *m,KeyType key) { \
    Name##Result r={false,(ValueType){0}};if(!m->cap)return r;bool found=false;size_t p=Name##_slot(m,key,&found); \
    if(found) { r.found=true;r.value=m->values[p]; }return r; } \
static bool Name##_contains(const Name *m,KeyType key) { return Name##_get(m,key).found; } \
static bool Name##_remove(Name *m,KeyType key) { if(!m->cap)return false;bool found=false;size_t p=Name##_slot(m,key,&found); \
    if(!found)return false;m->state[p]=2;--m->len;return true; } \
static void Name##_free(Name *m) { free(m->keys);free(m->values);free(m->state);*m=(Name){0}; }
CTB_NUMERIC_MAP_DEFINE(IntLongLongMap,int,long long)
CTB_NUMERIC_MAP_DEFINE(IntIntMap,int,int)
CTB_NUMERIC_MAP_DEFINE(IntDoubleMap,int,double)
CTB_NUMERIC_MAP_DEFINE(LongLongMap,long long,long long)
CTB_NUMERIC_MAP_DEFINE(LongLongIntMap,long long,int)
CTB_NUMERIC_MAP_DEFINE(LongLongDoubleMap,long long,double)
#endif
