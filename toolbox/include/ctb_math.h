/* SPDX-License-Identifier: MIT-0 */
#ifndef CTB_MATH_H
#define CTB_MATH_H
#include "ctb_core.h"
#include <math.h>
typedef struct { bool ok; unsigned long long value; } UnsignedResult;
typedef struct { bool ok; long long value; } IntegerResult;
static unsigned long long tb_magnitude(long long x) {return x<0?(unsigned long long)(-(x+1))+1:(unsigned long long)x;}
static unsigned long long gcd(long long a,long long b) {
    unsigned long long x=tb_magnitude(a),y=tb_magnitude(b);
    while(y){unsigned long long r=x%y;x=y;y=r;}return x;
}
static UnsignedResult try_lcm(long long a,long long b) {
    unsigned long long x=tb_magnitude(a),y=tb_magnitude(b);
    if(!x || !y)return (UnsignedResult){true,0};
    x/=gcd(a,b);if(x>ULLONG_MAX/y)return (UnsignedResult){false,0};return (UnsignedResult){true,x*y};
}
static unsigned long long lcm(long long a,long long b) {UnsignedResult r=try_lcm(a,b);if(!r.ok)tb_fail("lcm overflow");return r.value;}
static UnsignedResult try_factorial(unsigned int n) {
    unsigned long long value=1;
    for(unsigned int i=2;i<=n;++i){if(value>ULLONG_MAX/i)return (UnsignedResult){false,0};value*=i;if(i==UINT_MAX)break;}
    return (UnsignedResult){true,value};
}
static unsigned long long factorial(unsigned int n){UnsignedResult r=try_factorial(n);if(!r.ok)tb_fail("factorial overflow");return r.value;}
static bool tb_multiply_fits(long long a,long long b) {
    if(a>0)return b>0?a<=LLONG_MAX/b:b>=LLONG_MIN/a;
    if(a<0)return b>0?a>=LLONG_MIN/b:b==0 || a>=LLONG_MAX/b;
    return true;
}
static IntegerResult try_integer_power(long long base,unsigned int exponent) {
    long long value=1;
    while(exponent){if(exponent&1){if(!tb_multiply_fits(value,base))return (IntegerResult){false,0};value*=base;}exponent>>=1;if(exponent){if(!tb_multiply_fits(base,base))return (IntegerResult){false,0};base*=base;}}
    return (IntegerResult){true,value};
}
static long long integer_power(long long base,unsigned int exponent){IntegerResult r=try_integer_power(base,exponent);if(!r.ok)tb_fail("integer power overflow");return r.value;}
/* Absolute or relative tolerance; finite nonnegative tolerances required. */
static bool nearly_equal(double a,double b,double absolute,double relative) {
    if(!isfinite(absolute)||!isfinite(relative)||absolute<0||relative<0)tb_fail("invalid tolerance");
    if(a==b)return true;
    if(!isfinite(a)||!isfinite(b))return false;
    double difference=fabs(a-b),scale=fmax(fabs(a),fabs(b));
    if(difference<=absolute)return true;
    return scale && fabs(a/scale-b/scale)<=relative;
}
/* Halfway values round away from zero, as C99 round(). */
static double round_digits(double value,int digits) {
    double factor=pow(10.0,(double)digits);
    if(!isfinite(value)||!isfinite(factor)||factor==0 || !isfinite(value*factor))tb_fail("rounding range error");
    return round(value*factor)/factor;
}
static bool is_prime(unsigned long long n){if(n<2)return false;if(n%2==0)return n==2;for(unsigned long long d=3;d<=n/d;d+=2)if(n%d==0)return false;return true;}
/* Coefficients in ascending degree order; empty polynomial is zero. */
static double polynomial_value(const double *coefficients,size_t count,double x){double value=0;while(count)value=value*x+coefficients[--count];return value;}
#endif
