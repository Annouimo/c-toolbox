#include "ctb/math/bigint.h"
#include <assert.h>
int main(void){
 assert(!try_bigint_from("").ok);assert(!try_bigint_from("+").ok);assert(!try_bigint_from("12x").ok);
 BigIntResult zero=try_bigint_from("-000");assert(zero.ok && zero.value.sign==0);bigint_free(&zero.value);
 BigInt f=bigint_factorial(100);String text=bigint_to_string(&f);assert(text.len==158);assert(!strncmp(string_cstr(&text),"933262154439441526816992388562",30));string_free(&text);bigint_free(&f);
 for(;;){StringReadResult token=try_read_word();if(token.status==IO_EOF)break;io_require(token.status);BigInt a=bigint_from(string_cstr(&token.value));string_free(&token.value);BigInt b=read_bigint();
 BigInt sum=bigint_add(&a,&b),difference=bigint_subtract(&a,&b),product=bigint_multiply(&a,&b);BigIntDivision division=bigint_divmod(&a,&b);
 BigInt small=bigint_from_ll(INT_MIN);BigIntDivision fast=bigint_divide_small(&a,INT_MIN),slow=bigint_divmod(&a,&small);assert(bigint_compare(&fast.quotient,&slow.quotient)==0 && bigint_compare(&fast.remainder,&slow.remainder)==0);bigint_free(&small);bigint_free(&fast.quotient);bigint_free(&fast.remainder);bigint_free(&slow.quotient);bigint_free(&slow.remainder);
 print_bigint(&sum,"");print_char(' ',"");print_bigint(&difference,"");print_char(' ',"");print_bigint(&product,"");print_char(' ',"");print_bigint(&division.quotient,"");print_char(' ',"");print_bigint(&division.remainder,"\n");
 bigint_free(&a);bigint_free(&b);bigint_free(&sum);bigint_free(&difference);bigint_free(&product);bigint_free(&division.quotient);bigint_free(&division.remainder);
 }return 0;
}
