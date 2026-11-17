#include "ctb/sequence/prefix_sum.h"
#include "ctb/matrix/double.h"
#include "ctb/date.h"
#include <assert.h>
int main(void){
 IntVector v=IntVector_values(-2,3,3,-1,4,-9);IndexRange r=IntVector_longest_equal_run(&v);assert(r.begin==1 && r.end==3);
 SubarrayResult s=IntVector_max_subarray(&v);assert(s.ok && s.sum==9 && s.begin==1 && s.end==5);
 PrefixSum p=prefix_sum_from(&v);assert(prefix_sum_query(&p,1,5)==9);IntVector_set(&v,1,100);assert(prefix_sum_query(&p,1,5)==9);assert(prefix_sum_query(&p,3,3)==0);prefix_sum_free(&p);IntVector_free(&v);
 v=IntVector_values(INT_MIN,INT_MAX);LongLongVector differences=IntVector_difference(&v);assert(differences.len==1 && differences.data[0]==(long long)INT_MAX-INT_MIN);LongLongVector_free(&differences);IntVector_free(&v);
 v=IntVector_values(1,1,2);int count=1;while(IntVector_next_permutation(&v))++count;assert(count==3 && v.data[0]==1);IntVector_free(&v);
 v=IntVector_new();assert(!IntVector_max_subarray(&v).ok);p=prefix_sum_from(&v);assert(prefix_sum_query(&p,0,0)==0);prefix_sum_free(&p);
 DoubleMatrix a=DoubleMatrix_new(2,3),b=DoubleMatrix_new(3,2);for(size_t i=0;i<6;++i){a.data[i]=(double)i+1;b.data[i]=(double)i+1;}DoubleMatrix c=DoubleMatrix_multiply(&a,&b);assert(c.rows==2 && c.cols==2 && DoubleMatrix_get(&c,0,0)==22 && DoubleMatrix_get(&c,1,1)==64);DoubleMatrix t=DoubleMatrix_transpose(&a);assert(t.rows==3 && t.cols==2 && DoubleMatrix_get(&t,2,1)==6);DoubleMatrix copy=DoubleMatrix_clone(&a);DoubleMatrix_set(&a,0,0,99);assert(DoubleMatrix_get(&copy,0,0)==1);DoubleMatrix_free(&a);DoubleMatrix_free(&b);DoubleMatrix_free(&c);DoubleMatrix_free(&t);DoubleMatrix_free(&copy);
 a=DoubleMatrix_new(2,0);b=DoubleMatrix_new(0,3);c=DoubleMatrix_multiply(&a,&b);assert(c.rows==2 && c.cols==3 && c.data[0]==0);DoubleMatrix_free(&a);DoubleMatrix_free(&b);DoubleMatrix_free(&c);
 Date d=date_new(2000,2,28);assert(date_add_days(d,1).day==29);assert(date_add_days(d,2).month==3);assert(!try_date(1900,2,29).ok);assert(date_difference(date_new(1999,12,31),date_new(2000,1,1))==1);
 assert(!try_date_add_days(date_new(1,1,1),LLONG_MIN).ok);assert(!try_date_add_days(date_new(9999,12,31),LLONG_MAX).ok);
 int ordinal=0;for(int year=1;year<=9999;++year)for(int month=1;month<=12;++month){Date first=date_new(year,month,1);assert(date_ordinal(first)==ordinal);Date back=date_add_days(date_new(1,1,1),ordinal);assert(back.year==year && back.month==month && back.day==1);ordinal+=date_month_days(year,month);}
 assert(time_to_seconds(time_from_seconds(86399))==86399);return 0;
}
