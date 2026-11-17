/* SPDX-License-Identifier: MIT-0 */
#ifndef CTB_BYTE_VECTOR_H
#define CTB_BYTE_VECTOR_H
#include "string.h"
/* Raw bytes may contain NUL. Use String only for text with no embedded NUL. */
typedef struct { unsigned char *data; size_t len,cap; } ByteVector;
static ByteVector ByteVector_new(void) { return (ByteVector){0}; }
static size_t ByteVector_size(const ByteVector *v) { return v->len; }
static void ByteVector_reserve(ByteVector *v,size_t count) {
    if(count<=v->cap)return;size_t cap=tb_capacity(v->cap,count);v->data=tb_resize(v->data,cap,sizeof *v->data);v->cap=cap;
}
static void ByteVector_push(ByteVector *v,unsigned char value) { ByteVector_reserve(v,tb_add_size(v->len,1));v->data[v->len++]=value; }
static unsigned char ByteVector_get(const ByteVector *v,size_t pos) { if(pos>=v->len)tb_fail("byte vector index out of range");return v->data[pos]; }
static bool ByteVector_set(ByteVector *v,size_t pos,unsigned char value) { if(pos>=v->len)return false;v->data[pos]=value;return true; }
/* Extend with zero bytes when writing beyond the current end. */
static void ByteVector_write_at(ByteVector *v,size_t pos,unsigned char value) {
    size_t length=tb_add_size(pos,1);ByteVector_reserve(v,length);
    if(length>v->len)memset(v->data+v->len,0,length-v->len);
    v->data[pos]=value;v->len=length;
}
static ByteVector ByteVector_clone(const ByteVector *v) {
    ByteVector r=ByteVector_new();ByteVector_reserve(&r,v->len);
    if(v->len)memcpy(r.data,v->data,v->len);r.len=v->len;return r;
}
static ByteVector ByteVector_slice(const ByteVector *v,size_t first,size_t last) {
    if(first>last||last>v->len)tb_fail("invalid byte vector slice");ByteVector r=ByteVector_new();
    ByteVector_reserve(&r,last-first);if(last>first)memcpy(r.data,v->data+first,last-first);r.len=last-first;return r;
}
static void ByteVector_swap(ByteVector *a,ByteVector *b) { ByteVector x=*a;*a=*b;*b=x; }
static String byte_vector_to_string(const ByteVector *v) {
    size_t n=0;while(n<v->len&&v->data[n])++n;String s=string_new();string_reserve(&s,n);
    if(n)memcpy(s.data,v->data,n);s.data[n]='\0';s.len=n;return s;
}
static void ByteVector_print(const ByteVector *v,const char *end) {
    if(v->len&&fwrite(v->data,1,v->len,stdout)!=v->len)tb_fail("output error");
    if(fputs(end,stdout)==EOF)tb_fail("output error");
}
static void ByteVector_free(ByteVector *v) { free(v->data);*v=(ByteVector){0}; }
#endif
