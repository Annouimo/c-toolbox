/* SPDX-License-Identifier: MIT-0 */
#ifndef CTB_NUMBER_H
#define CTB_NUMBER_H
#include "../string.h"
#include "arithmetic.h"
/* Bases 2..36, uppercase digits, no prefix. Handles LLONG_MIN. */
static String integer_to_base(long long value,unsigned int base){if(base<2||base>36)tb_fail("invalid base");char buffer[sizeof(long long)*CHAR_BIT+2];size_t n=0;unsigned long long magnitude=tb_magnitude(value);do{buffer[n++]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"[magnitude%base];magnitude/=base;}while(magnitude);if(value<0)buffer[n++]='-';String s={0};string_reserve(&s,n);while(n)string_push(&s,buffer[--n]);return s;}
static IntegerResult try_integer_from_base(const String *text,unsigned int base){IntegerResult r={false,0};if(base<2||base>36)return r;const char *s=string_cstr(text);size_t i=0;bool negative=false;if(s[i]=='-'||s[i]=='+'){negative=s[i]=='-';++i;}if(!s[i])return r;unsigned long long value=0,limit=negative?(unsigned long long)LLONG_MAX+1:(unsigned long long)LLONG_MAX;for(;s[i];++i){unsigned char c=(unsigned char)s[i];unsigned int digit=c>='0'&&c<='9'?c-'0':c>='a'&&c<='z'?c-'a'+10:c>='A'&&c<='Z'?c-'A'+10:36;if(digit>=base||value>(limit-digit)/base)return r;value=value*base+digit;}r.ok=true;r.value=negative?(value==(unsigned long long)LLONG_MAX+1?LLONG_MIN:-(long long)value):(long long)value;return r;}
static long long integer_from_base(const String *text,unsigned int base){IntegerResult r=try_integer_from_base(text,base);if(!r.ok)tb_fail("invalid or out-of-range integer");return r.value;}
static unsigned int integer_digit_count(long long value,unsigned int base){if(base<2||base>36)tb_fail("invalid base");unsigned long long n=tb_magnitude(value);unsigned int count=0;do{++count;n/=base;}while(n);return count;}
static unsigned int integer_digit_sum(long long value,unsigned int base){if(base<2||base>36)tb_fail("invalid base");unsigned long long n=tb_magnitude(value);unsigned int sum=0;do{sum+=(unsigned int)(n%base);n/=base;}while(n);return sum;}
typedef struct { bool exists; unsigned int value; } DigitResult;
/* Position zero is the least-significant digit. */
static DigitResult integer_digit_at(long long value,unsigned int base,unsigned int position) {
    if(base<2||base>36)tb_fail("invalid base");
    unsigned long long n=tb_magnitude(value);
    if(position>=integer_digit_count(value,base))return (DigitResult){false,0};
    while(position--)n/=base;
    return (DigitResult){true,(unsigned int)(n%base)};
}
/* Reverses the magnitude and preserves the sign; failure reports signed overflow. */
static IntegerResult try_integer_reverse_digits(long long value,unsigned int base) {
    if(base<2||base>36)return (IntegerResult){false,0};
    unsigned long long n=tb_magnitude(value),result=0;
    unsigned long long limit=value<0?(unsigned long long)LLONG_MAX+1:(unsigned long long)LLONG_MAX;
    do { unsigned int digit=(unsigned int)(n%base);n/=base;if(result>(limit-digit)/base)return (IntegerResult){false,0};result=result*base+digit; } while(n);
    long long signed_result=value<0?(result==(unsigned long long)LLONG_MAX+1?LLONG_MIN:-(long long)result):(long long)result;
    return (IntegerResult){true,signed_result};
}
static long long integer_reverse_digits(long long value,unsigned int base) {
    IntegerResult r=try_integer_reverse_digits(value,base);if(!r.ok)tb_fail("reversed integer overflow");return r.value;
}
#endif
