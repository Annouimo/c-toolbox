/* SPDX-License-Identifier: MIT-0 */
/* Advances lexicographically; at the last permutation resets to ascending. */
static bool IntVector_next_permutation(IntVector *v){if(v->len<2)return false;size_t i=v->len-1;while(i && v->data[i-1]>=v->data[i])--i;if(!i){IntVector_reverse(v);return false;}size_t j=v->len-1;while(v->data[j]<=v->data[i-1])--j;int t=v->data[j];v->data[j]=v->data[i-1];v->data[i-1]=t;for(size_t a=i,b=v->len-1;a<b;++a,--b){t=v->data[a];v->data[a]=v->data[b];v->data[b]=t;}return true;}
