/* SPDX-License-Identifier: MIT-0 */
#ifndef CTB_COMBINATION_H
#define CTB_COMBINATION_H
#include "../detail/core.h"
/* Owns indices, which are borrowed by the caller until next/free. */
typedef struct {size_t *indices,n,k;bool started,finished;} Combination;
static Combination combination_new(size_t n,size_t k){if(k>n)tb_fail("combination size exceeds population");Combination c={k?tb_resize(NULL,k,sizeof(size_t)):NULL,n,k,false,false};for(size_t i=0;i<k;++i)c.indices[i]=i;return c;}
static bool combination_next(Combination *c){if(c->finished)return false;if(!c->started){c->started=true;return true;}size_t i=c->k;while(i && c->indices[i-1]==c->n-c->k+i-1)--i;if(!i){c->finished=true;return false;}++c->indices[i-1];for(size_t j=i;j<c->k;++j)c->indices[j]=c->indices[j-1]+1;return true;}
static void combination_free(Combination *c){free(c->indices);*c=(Combination){NULL,0,0,true,true};}
#endif
