/* SPDX-License-Identifier: MIT-0 */
#ifndef CTB_ARRAY_H
#define CTB_ARRAY_H
#include "../vector/int.h"
typedef struct { size_t begin,end; } IndexRange;
typedef struct { bool ok; long long sum; size_t begin,end; } SubarrayResult;
static long long tb_checked_add_ll(long long a,long long b){if((b>0 && a>LLONG_MAX-b)||(b<0 && a<LLONG_MIN-b))tb_fail("sum overflow");return a+b;}
static IndexRange IntVector_longest_equal_run(const IntVector *v){IndexRange best={0,0};size_t start=0;for(size_t i=0;i<v->len;++i){if(i && v->data[i]!=v->data[i-1])start=i;if(i+1-start>best.end-best.begin)best=(IndexRange){start,i+1};}return best;}
/* Nonempty subarray; earliest tie wins. Empty input returns ok=false. */
static SubarrayResult IntVector_max_subarray(const IntVector *v){
    SubarrayResult best={false,0,0,0};long long sum=0;size_t start=0;
    for(size_t i=0;i<v->len;++i){if(!i || sum<0){sum=v->data[i];start=i;}else sum=tb_checked_add_ll(sum,v->data[i]);if(!best.ok || sum>best.sum)best=(SubarrayResult){true,sum,start,i+1};}return best;
}
/* Wide differences avoid signed int subtraction overflow. */
#include "../vector/long_long.h"
static LongLongVector IntVector_difference(const IntVector *v){LongLongVector r={0};if(v->len>1)LongLongVector_reserve(&r,v->len-1);for(size_t i=1;i<v->len;++i)LongLongVector_push(&r,(long long)v->data[i]-v->data[i-1]);return r;}

#include "../vector/index.h"
#include "../vector/double.h"
typedef struct {long long value;size_t index;} tb_LongLongIndexPair;
static int tb_compare_ll_index(const void *a,const void *b){const tb_LongLongIndexPair *x=a,*y=b;if(x->value!=y->value)return (x->value>y->value)-(x->value<y->value);return (x->index>y->index)-(x->index<y->index);}
static IndexVector LongLongVector_argsort(const LongLongVector *v){IndexVector r={0};if(!v->len)return r;tb_LongLongIndexPair *p=tb_resize(NULL,v->len,sizeof *p);for(size_t i=0;i<v->len;++i)p[i]=(tb_LongLongIndexPair){v->data[i],i};qsort(p,v->len,sizeof *p,tb_compare_ll_index);IndexVector_reserve(&r,v->len);for(size_t i=0;i<v->len;++i)IndexVector_push(&r,p[i].index);free(p);return r;}
typedef struct {double value;size_t index;} tb_DoubleIndexPair;
static int tb_compare_double_index(const void *a,const void *b){const tb_DoubleIndexPair *x=a,*y=b;bool xn=isnan(x->value),yn=isnan(y->value);if(xn!=yn)return xn?1:-1;if(!xn){if(x->value<y->value)return -1;if(x->value>y->value)return 1;}return (x->index>y->index)-(x->index<y->index);}
static IndexVector DoubleVector_argsort(const DoubleVector *v){IndexVector r={0};if(!v->len)return r;tb_DoubleIndexPair *p=tb_resize(NULL,v->len,sizeof *p);for(size_t i=0;i<v->len;++i)p[i]=(tb_DoubleIndexPair){v->data[i],i};qsort(p,v->len,sizeof *p,tb_compare_double_index);IndexVector_reserve(&r,v->len);for(size_t i=0;i<v->len;++i)IndexVector_push(&r,p[i].index);free(p);return r;}
typedef struct {int value;size_t index;} tb_IntIndexPair;
static int tb_compare_int_index(const void *a,const void *b){const tb_IntIndexPair *x=a,*y=b;if(x->value!=y->value)return (x->value>y->value)-(x->value<y->value);return (x->index>y->index)-(x->index<y->index);}
static IndexVector IntVector_argsort(const IntVector *v){IndexVector r={0};if(!v->len)return r;tb_IntIndexPair *pairs=tb_resize(NULL,v->len,sizeof *pairs);for(size_t i=0;i<v->len;++i)pairs[i]=(tb_IntIndexPair){v->data[i],i};qsort(pairs,v->len,sizeof *pairs,tb_compare_int_index);IndexVector_reserve(&r,v->len);for(size_t i=0;i<v->len;++i)IndexVector_push(&r,pairs[i].index);free(pairs);return r;}
typedef struct {IntVector values;IndexVector ranks;} IntDiscretization;
/* Sorted unique values and zero-based ranks in original input order. */
static IntDiscretization IntVector_discretize(const IntVector *v){IntDiscretization r={IntVector_clone(v),{0}};IntVector_sort_asc(&r.values);IntVector_unique_sorted(&r.values);IndexVector_reserve(&r.ranks,v->len);for(size_t i=0;i<v->len;++i)IndexVector_push(&r.ranks,IntVector_lower_bound(&r.values,v->data[i]));return r;}
static void int_discretization_free(IntDiscretization *r){IntVector_free(&r->values);IndexVector_free(&r->ranks);}
#endif
