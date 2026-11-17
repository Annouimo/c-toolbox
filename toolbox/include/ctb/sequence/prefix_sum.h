/* SPDX-License-Identifier: MIT-0 */
#ifndef CTB_PREFIX_SUM_H
#define CTB_PREFIX_SUM_H
#include "algorithms.h"
/* Owns a snapshot, independent of later source mutations. */
typedef struct { long long *data; size_t len; } PrefixSum;
static PrefixSum prefix_sum_from(const IntVector *v){PrefixSum r={tb_resize(NULL,tb_add_size(v->len,1),sizeof(long long)),v->len};r.data[0]=0;for(size_t i=0;i<v->len;++i)r.data[i+1]=tb_checked_add_ll(r.data[i],v->data[i]);return r;}
static long long prefix_sum_query(const PrefixSum *p,size_t begin,size_t end){if(!p->data || begin>end || end>p->len)tb_fail("invalid prefix sum interval");long long a=p->data[end],b=p->data[begin];if((b>0 && a<LLONG_MIN+b)||(b<0 && a>LLONG_MAX+b))tb_fail("range sum overflow");return a-b;}
static void prefix_sum_free(PrefixSum *p){free(p->data);*p=(PrefixSum){0};}
#endif
