/* SPDX-License-Identifier: MIT-0 */
#ifndef CTB_COMPLEX_H
#define CTB_COMPLEX_H
#include "ctb_io.h"
#include <complex.h>
typedef double complex Complex;
static Complex complex_new(double real,double imaginary){return real+imaginary*I;}
static double complex_real(Complex z){return creal(z);}
static double complex_imaginary(Complex z){return cimag(z);}
static Complex complex_add(Complex a,Complex b){return a+b;}
static Complex complex_subtract(Complex a,Complex b){return a-b;}
static Complex complex_multiply(Complex a,Complex b){return a*b;}
static Complex complex_divide(Complex a,Complex b){if(creal(b)==0 && cimag(b)==0)tb_fail("complex division by zero");return a/b;}
static double complex_magnitude(Complex z){return cabs(z);}
static Complex read_complex(void){double real=read_double();double imaginary=read_double();return complex_new(real,imaginary);}
static void print_complex(Complex z,int digits){print_fixed(creal(z),digits);double imaginary=cimag(z);print_char(signbit(imaginary)?'-':'+');print_fixed(fabs(imaginary),digits);print_char('i');}
#endif
