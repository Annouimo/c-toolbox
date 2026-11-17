/* SPDX-License-Identifier: MIT-0 */
#ifndef CTB_SEARCH_H
#define CTB_SEARCH_H
#include "ctb_math.h"
/* Inclusive bounds; predicate must be false then true. */
static IntegerResult integer_first_true(long long begin,long long end,bool (*predicate)(long long)){if(!predicate||begin>end)tb_fail("invalid search");while(begin<end){unsigned long long width=(unsigned long long)end-(unsigned long long)begin;long long mid=begin+(long long)(width/2);if(predicate(mid))end=mid;else begin=mid+1;}return (IntegerResult){predicate(begin),begin};}
typedef struct {bool ok;double value;} RootResult;
static RootResult bisect_root(double (*function)(double),double begin,double end,double tolerance,unsigned int iterations){if(!function||!isfinite(begin)||!isfinite(end)||begin>end||!isfinite(tolerance)||tolerance<=0||!iterations)tb_fail("invalid root search");double left=function(begin),right=function(end);if(!isfinite(left)||!isfinite(right))return (RootResult){false,0};if(left==0)return (RootResult){true,begin};if(right==0)return (RootResult){true,end};if((left<0)==(right<0))return (RootResult){false,0};for(unsigned int i=0;i<iterations;++i){double mid=begin/2+end/2,value=function(mid);if(!isfinite(value))return (RootResult){false,mid};if(value==0||fabs(end-begin)<=tolerance)return (RootResult){true,mid};if(mid==begin||mid==end)return (RootResult){false,mid};if((left<0)==(value<0)){begin=mid;left=value;}else end=mid;}return (RootResult){false,begin/2+end/2};}
#endif
