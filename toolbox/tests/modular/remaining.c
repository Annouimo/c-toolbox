#include "ctb/number.h"
#include "ctb/sieve.h"
#include "ctb/search.h"
#include "ctb/complex.h"
#include "ctb/combination.h"
#include "ctb/counter.h"
#include "ctb/array.h"
#include <assert.h>
static bool positive(long long n){return n>=0;}
static double square_minus_two(double x){return x*x-2;}
int main(void){
 for(unsigned int b=2;b<=36;++b){String s=integer_to_base(LLONG_MIN,b);assert(integer_from_base(&s,b)==LLONG_MIN);string_free(&s);s=integer_to_base(LLONG_MAX,b);assert(integer_from_base(&s,b)==LLONG_MAX);string_free(&s);}
 String invalid=string_from("8000000000000000");assert(!try_integer_from_base(&invalid,16).ok);string_free(&invalid);
 assert(integer_digit_sum(-123,10)==6 && integer_digit_count(0,2)==1);
 PrimeSieve sieve=prime_sieve_new(10000);for(size_t i=0;i<=10000;++i)assert(prime_sieve_contains(&sieve,i)==is_prime(i));prime_sieve_free(&sieve);
 assert(integer_first_true(LLONG_MIN,LLONG_MAX,positive).value==0);assert(!integer_first_true(-9,-1,positive).ok);
 RootResult root=bisect_root(square_minus_two,0,2,1e-12,100);assert(root.ok && fabs(root.value-sqrt(2))<1e-11);
 Complex z=complex_multiply(complex_new(1,2),complex_new(3,4));assert(complex_real(z)==-5 && complex_imaginary(z)==10);
 Combination combo=combination_new(5,3);int count=0;while(combination_next(&combo)){assert(combo.indices[0]<combo.indices[1] && combo.indices[1]<combo.indices[2]);++count;}assert(count==10);combination_free(&combo);combo=combination_new(5,0);assert(combination_next(&combo) && !combination_next(&combo));combination_free(&combo);
 Map m=map_new();assert(!map_try_get(&m,"x").ok);map_set(&m,"x",42);assert(map_get(&m,"x")==42);map_free(&m);
 IntCounter c=int_counter_new();assert(int_counter_add(&c,LLONG_MIN,3));assert(int_counter_get(&c,LLONG_MIN)==3);int_counter_free(&c);
 Counter counter=counter_new();assert(!counter_most_common(&counter).ok);counter_add(&counter,"x",4);CounterStringEntry entry=counter_most_common_string(&counter);assert(entry.ok && entry.count==4);counter_free(&counter);assert(string_equal_text(&entry.key,"x"));string_free(&entry.key);
 StringList list={0};string_list_push_text(&list,"x");String popped=string_list_pop(&list);assert(string_equal_text(&popped,"x"));string_free(&popped);string_list_free(&list);
 IntVector v=IntVector_values(8,2,8,1);IndexVector indices=IntVector_argsort(&v);assert(indices.data[0]==3 && indices.data[2]==0 && indices.data[3]==2);IndexVector_free(&indices);IntDiscretization d=IntVector_discretize(&v);assert(d.values.len==3 && d.ranks.data[0]==2 && d.ranks.data[1]==1);int_discretization_free(&d);IntVector_free(&v);
 print_fixed(-0.0001,2,"\n");
 return 0;
}
