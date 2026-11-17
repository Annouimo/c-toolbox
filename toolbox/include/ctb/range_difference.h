/* SPDX-License-Identifier: MIT-0 */
#ifndef CTB_RANGE_DIFFERENCE_H
#define CTB_RANGE_DIFFERENCE_H
#include "vec_int.h"
#include "vec_ll.h"
/* Range-add difference buffer. The extra slot stores the right boundary sentinel. */
typedef struct { LongLongVector diff; size_t len; } RangeDifference;
static RangeDifference range_difference_new(size_t len) {
    RangeDifference r={LongLongVector_new(),len};LongLongVector_resize(&r.diff,tb_add_size(len,1),0);return r;
}
static RangeDifference range_difference_from_ints(const IntVector *values) {
    RangeDifference r=range_difference_new(values->len);
    if(values->len)r.diff.data[0]=values->data[0];
    for(size_t i=1;i<values->len;++i)r.diff.data[i]=(long long)values->data[i]-values->data[i-1];
    return r;
}
static bool range_difference_add(RangeDifference *r,size_t first,size_t last,long long value) {
    if(first>last||last>r->len)return false;if(first==last||!value)return true;
    long long a=r->diff.data[first],b=r->diff.data[last];
    if((value>0&&a>LLONG_MAX-value)||(value<0&&a<LLONG_MIN-value))return false;
    if((value<0&&b>LLONG_MAX+value)||(value>0&&b<LLONG_MIN+value))return false;
    r->diff.data[first]=a+value;r->diff.data[last]=b-value;return true;
}
static LongLongVector range_difference_materialize(const RangeDifference *r) {
    LongLongVector values=LongLongVector_new();LongLongVector_reserve(&values,r->len);long long running=0;
    for(size_t i=0;i<r->len;++i) {
        long long d=r->diff.data[i];if((d>0&&running>LLONG_MAX-d)||(d<0&&running<LLONG_MIN-d))tb_fail("range difference overflow");
        running+=d;LongLongVector_push(&values,running);
    }
    return values;
}
static void range_difference_free(RangeDifference *r){LongLongVector_free(&r->diff);r->len=0;}
#endif
