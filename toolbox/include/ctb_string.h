#ifndef CTB_STRING_H
#define CTB_STRING_H
#include "ctb_io.h"
/* SPDX-License-Identifier: MIT-0 */
/* Owns data. Zero initialization is valid. Positions and lengths count bytes. */
typedef struct { char *data; size_t len, cap; } String;
static String string_new(void) { return (String){0}; }
static const char *string_cstr(const String *s) { return s->data ? s->data : ""; }
static void string_reserve(String *s, size_t length) {
    size_t needed = tb_add_size(length, 1);
    if (needed <= s->cap) return;
    size_t cap = tb_capacity(s->cap, needed);
    s->data = tb_resize(s->data, cap, 1);
    s->cap = cap;
    s->data[s->len] = '\0';
}
static String string_from(const char *text) {
    String s = {0};
    size_t n = strlen(text);
    string_reserve(&s, n);
    memcpy(s.data, text, n + 1);
    s.len = n;
    return s;
}
static String string_clone(const String *s) { return string_from(string_cstr(s)); }
static void string_free(String *s) { free(s->data); *s = (String){0}; }
/* text may refer to s.data; snapshot before reserve/memmove. */
static bool string_insert(String *s, size_t pos, const char *text) {
    if (pos > s->len) return false;
    size_t n = strlen(text);
    if (!n) return true;
    size_t length = tb_add_size(s->len, n);
    char *copy = tb_resize(NULL, n, 1);
    memcpy(copy, text, n);
    string_reserve(s, length);
    memmove(s->data + pos + n, s->data + pos, s->len - pos + 1);
    memcpy(s->data + pos, copy, n);
    s->len = length;
    free(copy);
    return true;
}
static void string_append(String *s, const char *text) {
    (void)string_insert(s, s->len, text);
}
static bool string_erase(String *s, size_t pos, size_t count) {
    if (pos > s->len || count > s->len - pos) return false;
    if (count) memmove(s->data + pos, s->data + pos + count, s->len - pos - count + 1);
    s->len -= count;
    return true;
}
/* Invalid slice is a programming error; empty slices are valid. */
static String string_slice(const String *s, size_t pos, size_t count) {
    if (pos > s->len || count > s->len - pos) tb_fail("invalid string slice");
    String result = {0};
    string_reserve(&result, count);
    memcpy(result.data, string_cstr(s) + pos, count);
    result.data[count] = '\0';
    result.len = count;
    return result;
}
static size_t string_find(const String *s, const char *needle, size_t start) {
    if (start > s->len) return SIZE_MAX;
    const char *base = string_cstr(s);
    const char *p = strstr(base + start, needle);
    return p ? (size_t)(p - base) : SIZE_MAX;
}
static void string_replace_all(String *s, const char *old, const char *replacement) {
    if (!*old) return;
    String result = {0};
    size_t start = 0, width = strlen(old), pos;
    while ((pos = string_find(s, old, start)) != SIZE_MAX) {
        String part = string_slice(s, start, pos - start);
        string_append(&result, string_cstr(&part));
        string_free(&part);
        string_append(&result, replacement);
        start = pos + width;
    }
    string_append(&result, string_cstr(s) + start);
    string_free(s);
    *s = result;
}
/* Reads an entire line, removes LF and optional CR; false only at immediate EOF. */
static bool string_readline(String *s, FILE *input) {
    int ch;
    bool read_any = false;
    s->len = 0;
    string_reserve(s, 0);
    s->data[0] = '\0';
    while ((ch = fgetc(input)) != EOF && ch != '\n') {
        read_any = true;
        string_reserve(s, tb_add_size(s->len, 1));
        s->data[s->len++] = (char)ch;
    }
    if (s->len && s->data[s->len - 1] == '\r') --s->len;
    s->data[s->len] = '\0';
    return ch != EOF || read_any;
}
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
static void string_clear(String *s) {
    s->len = 0;
    if (s->data) s->data[0] = '\0';
}
static void string_assign(String *s, const char *text) {
    String copy = string_from(text); /* permits text inside s.data */
    string_free(s);
    *s = copy;
}
static void string_push(String *s, char ch) {
    if (!ch) tb_fail("NUL is not a text character");
    string_reserve(s, tb_add_size(s->len, 1));
    s->data[s->len++] = ch;
    s->data[s->len] = '\0';
}
static bool string_pop(String *s, char *out) {
    if (!s->len) return false;
    char ch = s->data[--s->len];
    s->data[s->len] = '\0';
    if (out) *out = ch;
    return true;
}
static bool string_contains(const String *s, const char *text) {
    return string_find(s, text, 0) != SIZE_MAX;
}
static bool string_starts_with(const String *s, const char *text) {
    size_t n = strlen(text);
    return n <= s->len && !memcmp(string_cstr(s), text, n);
}
static bool string_ends_with(const String *s, const char *text) {
    size_t n = strlen(text);
    return n <= s->len && !memcmp(string_cstr(s) + s->len - n, text, n);
}
/* Non-overlapping occurrences; empty needle counts as zero. */
static size_t string_count(const String *s, const char *needle) {
    size_t width = strlen(needle), start = 0, count = 0, pos;
    if (!width) return 0;
    while ((pos = string_find(s, needle, start)) != SIZE_MAX) {
        ++count;
        start = pos + width;
    }
    return count;
}
static bool string_remove_first(String *s, const char *needle) {
    if (!*needle) return false;
    size_t pos = string_find(s, needle, 0);
    return pos != SIZE_MAX && string_erase(s, pos, strlen(needle));
}
/* Single pass over original matches; does not revisit new boundary matches. */
static void string_remove_all(String *s, const char *needle) {
    string_replace_all(s, needle, "");
}
/* Restarts from the beginning after every deletion. Returns deletion count. */
static size_t string_remove_repeated(String *s, const char *needle) {
    if (!*needle) return 0;
    String copy = string_from(needle); /* needle may alias s */
    size_t count = 0;
    while (string_remove_first(s, copy.data)) ++count;
    string_free(&copy);
    return count;
}
static void string_trim(String *s) {
    size_t first = 0, end = s->len;
    const char *text = string_cstr(s);
    while (first < end && isspace((unsigned char)text[first])) ++first;
    while (end > first && isspace((unsigned char)text[end - 1])) --end;
    (void)string_erase(s, end, s->len - end);
    (void)string_erase(s, 0, first);
}
/* Byte-wise case conversion in the current C locale, not Unicode case folding. */
static void string_lower(String *s) {
    for (size_t i = 0; i < s->len; ++i)
        s->data[i] = (char)tolower((unsigned char)s->data[i]);
}
static void string_upper(String *s) {
    for (size_t i = 0; i < s->len; ++i)
        s->data[i] = (char)toupper((unsigned char)s->data[i]);
}
/* Whitespace-delimited token of arbitrary length; consumes one ending space. */
static bool string_read_token(String *s, FILE *input) {
    string_clear(s);
    int ch;
    do { ch = fgetc(input); } while (ch != EOF && isspace((unsigned char)ch));
    if (ch == EOF) return false;
    do {
        string_push(s, (char)ch);
        ch = fgetc(input);
    } while (ch != EOF && !isspace((unsigned char)ch));
    return true;
}

/* SPDX-License-Identifier: MIT-0 */
#include <stdarg.h>
static String tb_string_vformat(const char *format, va_list args) {
    va_list copy;
    va_copy(copy, args);
    int needed = vsnprintf(NULL, 0, format, copy);
    va_end(copy);
    if (needed < 0) tb_fail("formatting failed");
    String result = string_new();
    string_reserve(&result, (size_t)needed);
    va_copy(copy, args);
    int written = vsnprintf(result.data, result.cap, format, copy);
    va_end(copy);
    if (written != needed) { string_free(&result); tb_fail("inconsistent formatted length"); }
    if (memchr(result.data, 0, (size_t)needed)) {
        string_free(&result); tb_fail("NUL in formatted text");
    }
    result.len = (size_t)needed;
    return result;
}
static String string_format(const char *format, ...) {
    va_list args;
    va_start(args, format);
    String result = tb_string_vformat(format, args);
    va_end(args);
    return result;
}
static void string_append_format(String *s, const char *format, ...) {
    va_list args;
    va_start(args, format);
    String part = tb_string_vformat(format, args);
    va_end(args);
    string_append(s, string_cstr(&part));
    string_free(&part);
}
static String string_from_int(int value) { return string_format("%d", value); }
static String string_from_ll(long long value) { return string_format("%lld", value); }
static String string_from_double(double value, int precision) {
    if (precision < 0) tb_fail("precision must be nonnegative");
    return string_format("%.*f", precision, value);
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
static bool string_list_pop(StringList *list, String *out) {
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
static size_t string_list_find(const StringList *list, const String *value) {
    for (size_t i = 0; i < list->len; ++i)
        if (!strcmp(string_cstr(&list->data[i]), string_cstr(value))) return i;
    return SIZE_MAX;
}
static bool string_list_contains(const StringList *list, const String *value) {
    return string_list_find(list, value) != SIZE_MAX;
}
static int tb_string_compare(const void *a, const void *b) {
    return strcmp(string_cstr((const String *)a), string_cstr((const String *)b));
}
static void string_list_sort_by(StringList *list, int (*compare)(const void *, const void *)) {
    if (list->len > 1) qsort(list->data, list->len, sizeof *list->data, compare);
}
static void string_list_sort(StringList *list) { string_list_sort_by(list, tb_string_compare); }
static void string_list_reverse(StringList *list) {
    for (size_t i = 0; i < list->len / 2; ++i) {
        String value = list->data[i];
        list->data[i] = list->data[list->len - i - 1];
        list->data[list->len - i - 1] = value;
    }
}
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
static size_t string_size(const String *s) { return s->len; }
static bool string_empty(const String *s) { return s->len == 0; }
static char string_at(const String *s, size_t pos) {
    if (pos >= s->len) tb_fail("string index out of range");
    return s->data[pos];
}
static bool string_set(String *s, size_t pos, char ch) {
    if (pos >= s->len || !ch) return false;
    s->data[pos] = ch; return true;
}
static int string_compare(const String *a, const String *b) {
    return strcmp(string_cstr(a), string_cstr(b));
}
static bool string_equal(const String *a, const String *b) { return string_compare(a, b) == 0; }
static bool string_equal_text(const String *a, const char *b) { return !strcmp(string_cstr(a), b); }
static void string_reverse(String *s) {
    for (size_t i = 0; i < s->len / 2; ++i) {
        char ch = s->data[i]; s->data[i] = s->data[s->len - i - 1];
        s->data[s->len - i - 1] = ch;
    }
}
static bool string_is_palindrome(const String *s) {
    for (size_t i = 0; i < s->len / 2; ++i)
        if (s->data[i] != s->data[s->len - i - 1]) return false;
    return true;
}
static size_t string_count_overlapping(const String *s, const char *needle) {
    if (!*needle) return 0;
    size_t start = 0, count = 0, pos;
    while ((pos = string_find(s, needle, start)) != SIZE_MAX) {
        ++count; start = pos + 1;
    }
    return count;
}
/* _s variants accept String operands directly; original forms accept literals. */
static void string_assign_s(String *s, const String *value) { string_assign(s, string_cstr(value)); }
static void string_append_s(String *s, const String *value) { string_append(s, string_cstr(value)); }
static bool string_insert_s(String *s, size_t pos, const String *value) {
    return string_insert(s, pos, string_cstr(value));
}
static size_t string_find_s(const String *s, const String *needle, size_t start) {
    return string_find(s, string_cstr(needle), start);
}
static bool string_contains_s(const String *s, const String *needle) {
    return string_contains(s, string_cstr(needle));
}
static size_t string_count_s(const String *s, const String *needle) {
    return string_count(s, string_cstr(needle));
}
static void string_replace_all_s(String *s, const String *old, const String *replacement) {
    string_replace_all(s, string_cstr(old), string_cstr(replacement));
}
static bool string_remove_first_s(String *s, const String *needle) {
    return string_remove_first(s, string_cstr(needle));
}
static void string_remove_all_s(String *s, const String *needle) {
    string_remove_all(s, string_cstr(needle));
}
static size_t string_remove_repeated_s(String *s, const String *needle) {
    return string_remove_repeated(s, string_cstr(needle));
}
static StringList string_split_s(const String *s, const String *delimiter) {
    return string_split(s, string_cstr(delimiter));
}
static String string_join_s(const StringList *list, const String *separator) {
    return string_join(list, string_cstr(separator));
}

/* SPDX-License-Identifier: MIT-0 */
typedef struct { IOStatus status; String value; } StringReadResult;
static StringReadResult io_try_read_word(IOReader *r) {
    StringReadResult result={IO_OK,{0}};char *token=tb_io_token(r,&result.status);
    if(result.status==IO_OK)result.value=string_from(token);
    free(token);return result;
}
/* Reads one physical line/remainder, preserving spaces and empty lines. */
static StringReadResult io_try_read_line_raw(IOReader *r) {
    StringReadResult result={IO_OK,{0}};int c;bool any=false;
    while((c=fgetc(r->stream))!=EOF) {
        any=true;if(c=='\n')break;
        if(c=='\r'){int next=fgetc(r->stream);if(next!=EOF && next!='\n' && ungetc(next,r->stream)==EOF)result.status=IO_STREAM_ERROR;break;}
        if(!c)result.status=IO_FORMAT;else string_push(&result.value,(char)c);
    }
    r->line_pending=false;
    if(ferror(r->stream))result.status=IO_STREAM_ERROR;
    else if(!any)result.status=IO_EOF;
    if(result.status!=IO_OK)string_free(&result.value);
    return result;
}
static StringReadResult io_try_read_line(IOReader *r) {
    for(;;){StringReadResult result=io_try_read_line_raw(r);if(result.status!=IO_OK)return result;string_trim(&result.value);if(result.value.len)return result;string_free(&result.value);}
}
static StringReadResult try_read_word(void){return io_try_read_word(tb_input());}
static StringReadResult try_read_line(void){return io_try_read_line(tb_input());}
static StringReadResult try_read_line_raw(void){return io_try_read_line_raw(tb_input());}
static String read_word(void){StringReadResult r=try_read_word();io_require(r.status);return r.value;}
static String read_line(void){StringReadResult r=try_read_line();io_require(r.status);return r.value;}
static String read_line_raw(void){StringReadResult r=try_read_line_raw();io_require(r.status);return r.value;}
static String read_rest(void){return read_line_raw();}
static void print_string(const String *s){print_text(string_cstr(s));}
static StringList read_words(size_t count){StringList result={0};for(size_t i=0;i<count;++i){String s=read_word();string_list_push_take(&result,&s);}return result;}

/* After a token, reject extra nonspace data before crossing into next line. */
static IOStatus io_finish_line(IOReader *r) {
    if(!r->line_pending)return IO_OK;
    int c;
    while((c=fgetc(r->stream))!=EOF){
        if(c=='\n')break;
        if(c=='\r'){int n=fgetc(r->stream);if(n!=EOF && n!='\n' && ungetc(n,r->stream)==EOF)return IO_STREAM_ERROR;break;}
        if(!isspace((unsigned char)c)){if(ungetc(c,r->stream)==EOF)return IO_STREAM_ERROR;return IO_EXTRA_DATA;}
    }
    r->line_pending=false;return ferror(r->stream)?IO_STREAM_ERROR:IO_OK;
}
static StringReadResult try_read_next_line_raw(void){IOStatus s=io_finish_line(tb_input());if(s!=IO_OK){StringReadResult r={s,{0}};return r;}return try_read_line_raw();}
static StringReadResult try_read_next_line(void){IOStatus s=io_finish_line(tb_input());if(s!=IO_OK){StringReadResult r={s,{0}};return r;}return try_read_line();}
static String read_next_line_raw(void){StringReadResult r=try_read_next_line_raw();io_require(r.status);return r.value;}
static String read_next_line(void){StringReadResult r=try_read_next_line();io_require(r.status);return r.value;}

#endif
