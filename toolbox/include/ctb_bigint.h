/* SPDX-License-Identifier: MIT-0 */
#ifndef CTB_BIGINT_H
#define CTB_BIGINT_H
#include "ctb_string.h"
/* Owned base-10 magnitude, least significant digit first. Zero initializer valid.
   sign is -1, 0 or 1. Use clone, never shallow-copy independent owners. */
typedef struct { unsigned char *digits; size_t len; int sign; } BigInt;
typedef struct { bool ok; BigInt value; } BigIntResult;
typedef struct { BigInt quotient,remainder; } BigIntDivision;
static BigInt bigint_new(void){return (BigInt){0};}
static void bigint_free(BigInt *a){free(a->digits);*a=(BigInt){0};}
static void tb_big_normalize(BigInt *a){while(a->len && !a->digits[a->len-1])--a->len;if(!a->len)a->sign=0;}
static BigInt tb_big_alloc(size_t count){BigInt a={count?tb_resize(NULL,count,1):NULL,count,count?1:0};if(count)memset(a.digits,0,count);return a;}
static BigInt bigint_clone(const BigInt *a){BigInt r=tb_big_alloc(a->len);if(a->len)memcpy(r.digits,a->digits,a->len);r.sign=a->sign;return r;}
static BigIntResult try_bigint_from(const char *text){
    BigIntResult r={false,{0}};size_t n=strlen(text),start=0;int sign=1;
    if(n && (text[0]=='-' || text[0]=='+')){sign=text[0]=='-'?-1:1;start=1;}
    if(start==n)return r;
    for(size_t i=start;i<n;++i)if(text[i]<'0'||text[i]>'9')return r;
    r.value=tb_big_alloc(n-start);r.value.sign=sign;
    for(size_t i=start;i<n;++i)r.value.digits[n-1-i]=(unsigned char)(text[i]-'0');
    tb_big_normalize(&r.value);r.ok=true;return r;
}
static BigInt bigint_from(const char *text){BigIntResult r=try_bigint_from(text);if(!r.ok)tb_fail("invalid decimal integer");return r.value;}
static BigInt bigint_from_ll(long long value){char s[sizeof(long long)*CHAR_BIT+2];snprintf(s,sizeof s,"%lld",value);return bigint_from(s);}
static String bigint_to_string(const BigInt *a){String s={0};string_reserve(&s,tb_add_size(a->len,1));if(a->sign<0)string_push(&s,'-');if(!a->len)string_push(&s,'0');else for(size_t i=a->len;i;--i)string_push(&s,(char)('0'+a->digits[i-1]));return s;}
static int tb_big_abs_compare(const BigInt *a,const BigInt *b){if(a->len!=b->len)return a->len>b->len?1:-1;for(size_t i=a->len;i;--i)if(a->digits[i-1]!=b->digits[i-1])return a->digits[i-1]>b->digits[i-1]?1:-1;return 0;}
static int bigint_compare(const BigInt *a,const BigInt *b){if(a->sign!=b->sign)return (a->sign>b->sign)-(a->sign<b->sign);return a->sign*tb_big_abs_compare(a,b);}
static BigInt bigint_negate(const BigInt *a){BigInt r=bigint_clone(a);r.sign=-r.sign;return r;}
static BigInt tb_big_abs_add(const BigInt *a,const BigInt *b){size_t n=a->len>b->len?a->len:b->len;BigInt r=tb_big_alloc(tb_add_size(n,1));unsigned int carry=0;for(size_t i=0;i<n;++i){unsigned int x=carry+(i<a->len?a->digits[i]:0)+(i<b->len?b->digits[i]:0);r.digits[i]=(unsigned char)(x%10);carry=x/10;}r.digits[n]=(unsigned char)carry;tb_big_normalize(&r);return r;}
/* Magnitudes a >= b. */
static BigInt tb_big_abs_subtract(const BigInt *a,const BigInt *b){BigInt r=tb_big_alloc(a->len);int borrow=0;for(size_t i=0;i<a->len;++i){int x=a->digits[i]-borrow-(i<b->len?b->digits[i]:0);borrow=x<0;if(borrow)x+=10;r.digits[i]=(unsigned char)x;}tb_big_normalize(&r);return r;}
static BigInt bigint_add(const BigInt *a,const BigInt *b){if(!a->sign)return bigint_clone(b);if(!b->sign)return bigint_clone(a);BigInt r;if(a->sign==b->sign){r=tb_big_abs_add(a,b);r.sign=a->sign;}else {int c=tb_big_abs_compare(a,b);r=c>=0?tb_big_abs_subtract(a,b):tb_big_abs_subtract(b,a);if(r.len)r.sign=c>=0?a->sign:b->sign;}return r;}
static BigInt bigint_subtract(const BigInt *a,const BigInt *b){BigInt borrowed=*b;borrowed.sign=-borrowed.sign;return bigint_add(a,&borrowed);}
static BigInt bigint_multiply(const BigInt *a,const BigInt *b){
    if(!a->sign || !b->sign)return bigint_new();
    BigInt r=tb_big_alloc(tb_add_size(a->len,b->len));
    for(size_t i=0;i<a->len;++i){unsigned int carry=0;for(size_t j=0;j<b->len;++j){unsigned int value=r.digits[i+j]+a->digits[i]*b->digits[j]+carry;r.digits[i+j]=(unsigned char)(value%10);carry=value/10;}r.digits[i+b->len]=(unsigned char)carry;}
    tb_big_normalize(&r);r.sign=a->sign*b->sign;return r;
}
/* Division truncates toward zero; remainder has the dividend's sign.
   Both returned objects own independent memory. Division by zero exits. */
static BigIntDivision bigint_divmod(const BigInt *a,const BigInt *b){
    if(!b->sign)tb_fail("big integer division by zero");
    BigIntDivision result={tb_big_alloc(a->len),bigint_new()};
    for(size_t i=a->len;i;--i){
        BigInt *r=&result.remainder;
        r->digits=tb_resize(r->digits,tb_add_size(r->len,1),1);
        if(r->len)memmove(r->digits+1,r->digits,r->len);
        r->digits[0]=a->digits[i-1];++r->len;r->sign=1;tb_big_normalize(r);
        unsigned char digit=0;
        while(tb_big_abs_compare(r,b)>=0){BigInt next=tb_big_abs_subtract(r,b);bigint_free(r);*r=next;++digit;}
        result.quotient.digits[i-1]=digit;
    }
    tb_big_normalize(&result.quotient);if(result.quotient.len)result.quotient.sign=a->sign*b->sign;
    if(result.remainder.len)result.remainder.sign=a->sign;
    return result;
}
static BigInt bigint_divide(const BigInt *a,const BigInt *b){BigIntDivision r=bigint_divmod(a,b);bigint_free(&r.remainder);return r.quotient;}
static BigInt bigint_modulo(const BigInt *a,const BigInt *b){BigIntDivision r=bigint_divmod(a,b);bigint_free(&r.quotient);return r.remainder;}
/* Small-operand helpers share the same storage and signed division rules. */
static BigInt bigint_multiply_small(const BigInt *a,int b){BigInt rhs=bigint_from_ll(b);BigInt r=bigint_multiply(a,&rhs);bigint_free(&rhs);return r;}
static BigIntDivision bigint_divide_small(const BigInt *a,int b){
    if(!b)tb_fail("big integer division by zero");
    unsigned long long divisor=b<0?(unsigned long long)(-(long long)b):(unsigned long long)b,remainder=0;
    BigInt q=tb_big_alloc(a->len);
    for(size_t i=a->len;i;--i){unsigned long long value=remainder*10+a->digits[i-1];q.digits[i-1]=(unsigned char)(value/divisor);remainder=value%divisor;}
    tb_big_normalize(&q);if(q.len)q.sign=a->sign*(b<0?-1:1);
    BigInt r=bigint_from_ll((long long)remainder);if(r.len)r.sign=a->sign;
    return (BigIntDivision){q,r};
}
static BigInt bigint_factorial(unsigned int n){BigInt r=bigint_from_ll(1);for(unsigned int i=2;i<=n;++i){BigInt b=bigint_from_ll(i),next=bigint_multiply(&r,&b);bigint_free(&b);bigint_free(&r);r=next;if(i==UINT_MAX)break;}return r;}
typedef struct {IOStatus status;BigInt value;} BigIntReadResult;
static BigIntReadResult try_read_bigint(void){StringReadResult token=try_read_word();BigIntReadResult r={token.status,{0}};if(r.status==IO_OK){BigIntResult parsed=try_bigint_from(string_cstr(&token.value));if(!parsed.ok)r.status=IO_FORMAT;else r.value=parsed.value;}string_free(&token.value);return r;}
static BigInt read_bigint(void){BigIntReadResult r=try_read_bigint();io_require(r.status);return r.value;}
static void print_bigint(const BigInt *a){String s=bigint_to_string(a);print_string(&s);string_free(&s);}
#endif
