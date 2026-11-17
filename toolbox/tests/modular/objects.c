#include "ctb_prefix_sum.h"
#include "ctb_matrix.h"
#include "ctb_date.h"
#include <assert.h>
int main(void){
 IntVector v=IntVector_values(-2,3,3,-1,4,-9);IndexRange r=IntVector_longest_equal_run(&v);assert(r.begin==1 && r.end==3);
 SubarrayResult s=IntVector_max_subarray(&v);assert(s.ok && s.sum==9 && s.begin==1 && s.end==5);
 PrefixSum p=prefix_sum_from(&v);assert(prefix_sum_query(&p,1,5)==9);IntVector_set(&v,1,100);assert(prefix_sum_query(&p,1,5)==9);assert(prefix_sum_query(&p,3,3)==0);prefix_sum_free(&p);IntVector_free(&v);
 v=IntVector_values(INT_MIN,INT_MAX);LongLongVector differences=IntVector_difference(&v);assert(differences.len==1 && differences.data[0]==(long long)INT_MAX-INT_MIN);LongLongVector_free(&differences);IntVector_free(&v);
 v=IntVector_values(1,1,2);int count=1;while(IntVector_next_permutation(&v))++count;assert(count==3 && v.data[0]==1);IntVector_free(&v);
 v=IntVector_new();assert(!IntVector_max_subarray(&v).ok);p=prefix_sum_from(&v);assert(prefix_sum_query(&p,0,0)==0);prefix_sum_free(&p);
 Matrix a=matrix_new(2,3),b=matrix_new(3,2);for(size_t i=0;i<6;++i){a.data[i]=(double)i+1;b.data[i]=(double)i+1;}Matrix c=matrix_multiply(&a,&b);assert(c.rows==2 && c.cols==2 && matrix_get(&c,0,0)==22 && matrix_get(&c,1,1)==64);Matrix t=matrix_transpose(&a);assert(t.rows==3 && t.cols==2 && matrix_get(&t,2,1)==6);Matrix copy=matrix_clone(&a);matrix_set(&a,0,0,99);assert(matrix_get(&copy,0,0)==1);matrix_free(&a);matrix_free(&b);matrix_free(&c);matrix_free(&t);matrix_free(&copy);
 a=matrix_new(2,0);b=matrix_new(0,3);c=matrix_multiply(&a,&b);assert(c.rows==2 && c.cols==3 && c.data[0]==0);matrix_free(&a);matrix_free(&b);matrix_free(&c);
 Date d=date_new(2000,2,28);assert(date_add_days(d,1).day==29);assert(date_add_days(d,2).month==3);assert(!try_date(1900,2,29).ok);assert(date_difference(date_new(1999,12,31),date_new(2000,1,1))==1);
 assert(!try_date_add_days(date_new(1,1,1),LLONG_MIN).ok);assert(!try_date_add_days(date_new(9999,12,31),LLONG_MAX).ok);
 int ordinal=0;for(int year=1;year<=9999;++year)for(int month=1;month<=12;++month){Date first=date_new(year,month,1);assert(date_ordinal(first)==ordinal);Date back=date_add_days(date_new(1,1,1),ordinal);assert(back.year==year && back.month==month && back.day==1);ordinal+=date_month_days(year,month);}
 assert(time_to_seconds(time_from_seconds(86399))==86399);return 0;
}
