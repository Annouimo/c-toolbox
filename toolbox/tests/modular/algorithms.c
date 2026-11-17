#include "ctb/math/arithmetic.h"
#include "ctb/math/bits.h"
#include "ctb/vector/int.h"
#include <assert.h>
static int parity(const int *a,const int *b){return (*a%2)-(*b%2);}
int main(void){
 assert(gcd(LLONG_MIN,0)==(unsigned long long)LLONG_MAX+1);
 assert(gcd(-12,18)==6);assert(lcm(-12,18)==36);assert(lcm(0,0)==0);
 assert(factorial(0)==1);assert(factorial(20)==2432902008176640000ULL);assert(!try_factorial(21).ok);
 assert(integer_power(-2,3)==-8);assert(integer_power(0,0)==1);
 assert(integer_power(LLONG_MIN,1)==LLONG_MIN);assert(!try_integer_power(LLONG_MIN,2).ok);assert(!try_integer_power(LLONG_MAX,2).ok);
 assert(is_prime(2)&&is_prime(97)&&!is_prime(1)&&!is_prime(121));
 assert(round_digits(1.25,1)==1.3);assert(round_digits(-1.25,1)==-1.3);
 assert(nearly_equal(1,1+1e-10,1e-9,0));assert(!nearly_equal(INFINITY,-INFINITY,0,0));
 assert(bit_get(bit_set(0,63),63));assert(bit_clear(7,1)==5);assert(bit_toggle(5,0)==4);assert(bit_count(ULLONG_MAX)==sizeof(unsigned long long)*CHAR_BIT);
 IntVector v=IntVector_values(3,2,1,4);assert(IntVector_front(&v)==3);assert(IntVector_back(&v)==4);
 assert(!IntVector_try_get(&v,4).ok);IntVector_sort_by(&v,parity);
 assert(v.data[0]==2&&v.data[1]==4&&v.data[2]==3&&v.data[3]==1);
 assert(IntVector_pop(&v)==1);IntVector_clear(&v);assert(!IntVector_try_pop(&v).ok);IntVector_free(&v);
 return 0;
}
