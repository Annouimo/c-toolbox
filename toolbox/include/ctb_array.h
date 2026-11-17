/* SPDX-License-Identifier: MIT-0 */
#ifndef CTB_ARRAY_H
#define CTB_ARRAY_H
#include "ctb_vec_int.h"
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
#include "ctb_vec_ll.h"
static LongLongVector IntVector_difference(const IntVector *v){LongLongVector r={0};if(v->len>1)LongLongVector_reserve(&r,v->len-1);for(size_t i=1;i<v->len;++i)LongLongVector_push(&r,(long long)v->data[i]-v->data[i-1]);return r;}
/* Advances lexicographically; at the last permutation resets to ascending. */
static bool IntVector_next_permutation(IntVector *v){if(v->len<2)return false;size_t i=v->len-1;while(i && v->data[i-1]>=v->data[i])--i;if(!i){IntVector_reverse(v);return false;}size_t j=v->len-1;while(v->data[j]<=v->data[i-1])--j;int t=v->data[j];v->data[j]=v->data[i-1];v->data[i-1]=t;for(size_t a=i,b=v->len-1;a<b;++a,--b){t=v->data[a];v->data[a]=v->data[b];v->data[b]=t;}return true;}
#endif
