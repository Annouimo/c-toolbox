/* SPDX-License-Identifier: MIT-0 */
#ifndef CTB_BITS_H
#define CTB_BITS_H
#include "ctb_core.h"
/* Bit zero is the least significant bit; operate on unsigned long long. */
static void tb_check_bit(unsigned int bit){if(bit>=sizeof(unsigned long long)*CHAR_BIT)tb_fail("bit index out of range");}
static bool bit_get(unsigned long long value,unsigned int bit){tb_check_bit(bit);return ((value>>bit)&1)!=0;}
static unsigned long long bit_set(unsigned long long value,unsigned int bit){tb_check_bit(bit);return value|(1ULL<<bit);}
static unsigned long long bit_clear(unsigned long long value,unsigned int bit){tb_check_bit(bit);return value&~(1ULL<<bit);}
static unsigned long long bit_toggle(unsigned long long value,unsigned int bit){tb_check_bit(bit);return value^(1ULL<<bit);}
static unsigned int bit_count(unsigned long long value){unsigned int n=0;while(value){value&=value-1;++n;}return n;}
#endif
