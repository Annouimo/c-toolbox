/* SPDX-License-Identifier: MIT-0 */
#ifndef CTB_SIEVE_H
#define CTB_SIEVE_H
#include "ctb_core.h"
typedef struct { unsigned char *composite;size_t limit; } PrimeSieve;
static PrimeSieve prime_sieve_new(size_t limit){size_t n=tb_add_size(limit,1);PrimeSieve s={tb_resize(NULL,n,1),limit};memset(s.composite,0,n);s.composite[0]=1;if(limit)s.composite[1]=1;for(size_t p=2;p<=limit/p;++p)if(!s.composite[p]){size_t m=p*p;for(;;){s.composite[m]=1;if(limit-m<p)break;m+=p;}}return s;}
static bool prime_sieve_contains(const PrimeSieve *s,size_t value){if(!s->composite||value>s->limit)tb_fail("sieve query out of range");return !s->composite[value];}
static void prime_sieve_free(PrimeSieve *s){free(s->composite);*s=(PrimeSieve){0};}
#endif
