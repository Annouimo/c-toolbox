/* SPDX-License-Identifier: MIT-0 */
#ifndef CTB_BITS_H
#define CTB_BITS_H
#include "core.h"
/* Bit zero is the least significant bit; operate on unsigned long long. */
static void tb_check_bit(unsigned int bit){if(bit>=sizeof(unsigned long long)*CHAR_BIT)tb_fail("bit index out of range");}
static bool bit_get(unsigned long long value,unsigned int bit){tb_check_bit(bit);return ((value>>bit)&1)!=0;}
static unsigned long long bit_set(unsigned long long value,unsigned int bit){tb_check_bit(bit);return value|(1ULL<<bit);}
static unsigned long long bit_clear(unsigned long long value,unsigned int bit){tb_check_bit(bit);return value&~(1ULL<<bit);}
static unsigned long long bit_toggle(unsigned long long value,unsigned int bit){tb_check_bit(bit);return value^(1ULL<<bit);}
static unsigned int bit_count(unsigned long long value){unsigned int n=0;while(value){value&=value-1;++n;}return n;}
typedef struct { bool exists; unsigned int index; } BitIndexResult;
static BitIndexResult bit_lowest_set_index(unsigned long long value){if(!value)return (BitIndexResult){false,0};unsigned int i=0;while(!(value&1)){value>>=1;++i;}return (BitIndexResult){true,i};}
static BitIndexResult bit_highest_set_index(unsigned long long value){if(!value)return (BitIndexResult){false,0};unsigned int i=0;while(value>>1){value>>=1;++i;}return (BitIndexResult){true,i};}
#endif
