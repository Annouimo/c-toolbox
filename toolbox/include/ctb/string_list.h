#ifndef CTB_STRING_LIST_H
#define CTB_STRING_LIST_H
#include "string.h"
#include "vector/index.h"
/* SPDX-License-Identifier: MIT-0 */
typedef struct { String *data; size_t len, cap; } StringList;
/* Literal, nonempty delimiter; preserves empty fields; result owns all strings. */
static StringList string_split(const String *s, const char *delimiter) {
    if (!*delimiter) tb_fail("empty split delimiter");
    StringList list = {0};
    size_t start = 0, width = strlen(delimiter);
    for (;;) {
        size_t pos = string_find(s, delimiter, start);
        size_t end = pos == SIZE_MAX ? s->len : pos;
        if (list.len == list.cap) {
            list.cap = tb_capacity(list.cap, tb_add_size(list.len, 1));
            list.data = tb_resize(list.data, list.cap, sizeof *list.data);
        }
        list.data[list.len++] = string_slice(s, start, end - start);
        if (pos == SIZE_MAX) break;
        start = pos + width;
    }
    return list;
}
static String string_join(const StringList *list, const char *separator) {
    String result = {0};
    for (size_t i = 0; i < list->len; ++i) {
        if (i) string_append(&result, separator);
        string_append(&result, string_cstr(&list->data[i]));
    }
    return result;
}
static void string_list_free(StringList *list) {
    for (size_t i = 0; i < list->len; ++i) string_free(&list->data[i]);
    free(list->data);
    *list = (StringList){0};
}

/* SPDX-License-Identifier: MIT-0 */
/* Canonical owning string sequence. Copy by default; _take explicitly moves. */
static StringList string_list_new(void) { return (StringList){0}; }
static size_t string_list_size(const StringList *list) { return list->len; }
static bool string_list_empty(const StringList *list) { return list->len == 0; }
static void string_list_reserve(StringList *list, size_t needed) {
    if (needed <= list->cap) return;
    size_t cap = tb_capacity(list->cap, needed);
    list->data = tb_resize(list->data, cap, sizeof *list->data);
    list->cap = cap;
}
static const String *string_list_get(const StringList *list, size_t pos) {
    if (pos >= list->len) tb_fail("string list index out of range");
    return &list->data[pos];
}
static String *string_list_at(StringList *list, size_t pos) {
    if (pos >= list->len) tb_fail("string list index out of range");
    return &list->data[pos];
}
static void string_list_push_take(StringList *list, String *value) {
    String moved = *value;
    *value = string_new(); /* before reserve: value may be a list element */
    string_list_reserve(list, tb_add_size(list->len, 1));
    list->data[list->len++] = moved;
}
static void string_list_push(StringList *list, const String *value) {
    String copy = string_clone(value);
    string_list_push_take(list, &copy);
}
static void string_list_push_text(StringList *list, const char *value) {
    String copy = string_from(value);
    string_list_push_take(list, &copy);
}
static bool string_list_insert(StringList *list, size_t pos, const String *value) {
    if (pos > list->len) return false;
    String copy = string_clone(value);
    string_list_reserve(list, tb_add_size(list->len, 1));
    memmove(list->data + pos + 1, list->data + pos,
            (list->len - pos) * sizeof *list->data);
    list->data[pos] = copy; ++list->len; return true;
}
static bool string_list_set(StringList *list, size_t pos, const String *value) {
    if (pos >= list->len) return false;
    String copy = string_clone(value);
    string_free(&list->data[pos]); list->data[pos] = copy;
    return true;
}
static bool string_list_erase(StringList *list, size_t pos, size_t count) {
    if (pos > list->len || count > list->len - pos) return false;
    for (size_t i = pos; i < pos + count; ++i) string_free(&list->data[i]);
    if (count) memmove(list->data + pos, list->data + pos + count,
                       (list->len - pos - count) * sizeof *list->data);
    list->len -= count; return true;
}
/* out, when non-NULL, must be initialized and must not point inside list. */
static bool string_list_pop_into(StringList *list, String *out) {
    if (!list->len) return false;
    String value = list->data[--list->len];
    if (out) { string_free(out); *out = value; }
    else string_free(&value);
    return true;
}
static void string_list_clear(StringList *list) {
    (void)string_list_erase(list, 0, list->len);
}
static StringList string_list_slice(const StringList *list, size_t pos, size_t count) {
    if (pos > list->len || count > list->len - pos) tb_fail("invalid string list slice");
    StringList result = string_list_new();
    string_list_reserve(&result, count);
    for (size_t i = 0; i < count; ++i) string_list_push(&result, &list->data[pos + i]);
    return result;
}
static StringList string_list_clone(const StringList *list) {
    return string_list_slice(list, 0, list->len);
}
static size_t string_list_find_text(const StringList *list, const char *text, size_t start) {
    for (size_t i = start; i < list->len; ++i) {
        if (strcmp(string_cstr(&list->data[i]), text) == 0) return i;
    }
    return SIZE_MAX;
}
static bool string_list_contains_text(const StringList *list, const char *text) {
    return string_list_find_text(list, text, 0) != SIZE_MAX;
}
static size_t string_list_count_text(const StringList *list, const char *text) {
    size_t count = 0;
    for (size_t i = 0; i < list->len; ++i) {
        if (strcmp(string_cstr(&list->data[i]), text) == 0) ++count;
    }
    return count;
}
static size_t string_list_find(const StringList *list, const String *value) {
    return string_list_find_text(list, string_cstr(value), 0);
}
static bool string_list_contains(const StringList *list, const String *value) {
    return string_list_find(list, value) != SIZE_MAX;
}
static int tb_string_compare(const void *a, const void *b) {
    return strcmp(string_cstr((const String *)a), string_cstr((const String *)b));
}
static void tb_string_list_merge_sort(String *data,String *scratch,size_t n,
                                      int (*compare)(const void *,const void *)) {
    if(n<2)return;
    size_t mid=n/2;
    tb_string_list_merge_sort(data,scratch,mid,compare);
    tb_string_list_merge_sort(data+mid,scratch+mid,n-mid,compare);
    size_t i=0,j=mid,k=0;
    while(i<mid && j<n) {
        if(compare(&data[i],&data[j])<=0)scratch[k++]=data[i++];
        else scratch[k++]=data[j++];
    }
    while(i<mid)scratch[k++]=data[i++];
    while(j<n)scratch[k++]=data[j++];
    memcpy(data,scratch,n*sizeof *data);
}
static void string_list_sort_by(StringList *list, int (*compare)(const void *, const void *)) {
    if(!compare)tb_fail("null comparator");
    if(list->len<2)return;
    String *scratch=tb_resize(NULL,list->len,sizeof *scratch);
    tb_string_list_merge_sort(list->data,scratch,list->len,compare);
    free(scratch);
}
static void string_list_sort(StringList *list) { string_list_sort_by(list, tb_string_compare); }
static void string_list_reverse(StringList *list) {
    for (size_t i = 0; i < list->len / 2; ++i) {
        String value = list->data[i];
        list->data[i] = list->data[list->len - i - 1];
        list->data[list->len - i - 1] = value;
    }
}
static void string_list_swap(StringList *a,StringList *b) { StringList value=*a;*a=*b;*b=value; }
/* Adjacent equals only; sort first for global deduplication. */
static void string_list_unique_sorted(StringList *list) {
    size_t written = 0;
    for (size_t i = 0; i < list->len; ++i) {
        if (written && !strcmp(string_cstr(&list->data[written - 1]),
                              string_cstr(&list->data[i]))) {
            string_free(&list->data[i]);
        } else {
            if (written != i) {
                list->data[written] = list->data[i];
                list->data[i] = string_new();
            }
            ++written;
        }
    }
    list->len = written;
}
static StringList string_split_whitespace(const String *s) {
    StringList words = string_list_new();
    size_t pos = 0;
    while (pos < s->len) {
        while (pos < s->len && isspace((unsigned char)s->data[pos])) ++pos;
        size_t start = pos;
        while (pos < s->len && !isspace((unsigned char)s->data[pos])) ++pos;
        if (pos > start) {
            String word = string_slice(s, start, pos - start);
            string_list_push_take(&words, &word);
        }
    }
    return words;
}

/* SPDX-License-Identifier: MIT-0 */
static StringList string_split_s(const String *s, const String *delimiter) {
    return string_split(s, string_cstr(delimiter));
}
static String string_join_s(const StringList *list, const String *separator) {
    return string_join(list, string_cstr(separator));
}

typedef struct { const StringList *list; int (*compare)(const void *,const void *); } StringSortContext;
static int tb_string_index_compare(const StringSortContext *ctx,size_t a,size_t b) {
    int order=ctx->compare(&ctx->list->data[a],&ctx->list->data[b]);
    return order?order:(a<b?-1:a>b?1:0);
}
static void tb_string_index_merge_sort(size_t *data,size_t *scratch,size_t n,const StringSortContext *ctx) {
    if(n<2)return;
    size_t mid=n/2;
    tb_string_index_merge_sort(data,scratch,mid,ctx);tb_string_index_merge_sort(data+mid,scratch+mid,n-mid,ctx);
    size_t i=0,j=mid,k=0;
    while(i<mid && j<n)scratch[k++]=tb_string_index_compare(ctx,data[i],data[j])<=0?data[i++]:data[j++];
    while(i<mid)scratch[k++]=data[i++];
    while(j<n)scratch[k++]=data[j++];
    memcpy(data,scratch,n*sizeof *data);
}
static IndexVector string_list_argsort_by(const StringList *list,int (*compare)(const void *,const void *)) {
    if(!compare)tb_fail("null comparator");
    IndexVector indices=IndexVector_new();IndexVector_reserve(&indices,list->len);
    for(size_t i=0;i<list->len;++i)IndexVector_push(&indices,i);
    if(indices.len>1) { size_t *scratch=tb_resize(NULL,indices.len,sizeof *scratch);StringSortContext ctx={list,compare};
        tb_string_index_merge_sort(indices.data,scratch,indices.len,&ctx);free(scratch); }
    return indices;
}
static IndexVector string_list_argsort(const StringList *list) { return string_list_argsort_by(list,tb_string_compare); }

/* SPDX-License-Identifier: MIT-0 */
static StringList read_words(size_t count){StringList result={0};for(size_t i=0;i<count;++i){String s=read_word();string_list_push_take(&result,&s);}return result;}

/* SPDX-License-Identifier: MIT-0 */
/* Reapply each deletion pattern until reaching a fixed point across the set. */
static size_t string_remove_patterns(String *text,const StringList *patterns) {
    size_t total=0,pass;
    do { pass=0;for(size_t i=0;i<patterns->len;++i)pass+=string_remove_repeated(text,string_cstr(&patterns->data[i]));total+=pass; } while(pass);
    return total;
}
typedef struct {bool ok;String value;} StringResult;
static StringResult string_list_try_pop(StringList *list){StringResult r={false,{0}};r.ok=string_list_pop_into(list,&r.value);return r;}
static String string_list_pop(StringList *list){StringResult r=string_list_try_pop(list);if(!r.ok)tb_fail("pop of empty string list");return r.value;}
static void print_strings(const StringList *words,const char *separator,const char *end){for(size_t i=0;i<words->len;++i){if(i)print_text(separator,"");print_string(&words->data[i],"");}print_text("",end);}

#endif
