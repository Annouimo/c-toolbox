#ifndef CTB_STRING_H
#define CTB_STRING_H
#include "io.h"
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
static void string_swap(String *a,String *b) { String value=*a;*a=*b;*b=value; }
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

/* SPDX-License-Identifier: MIT-0 */
static void string_push(String *s, char ch);
static String string_slice_step(const String *s,size_t first,size_t last,size_t step) {
    if(!step || first>last || last>s->len)tb_fail("invalid string slice");
    String result=string_new();
    size_t count=first==last?0:1+(last-first-1)/step;
    string_reserve(&result,count);
    for(size_t i=first;i<last;) { string_push(&result,s->data[i]);if(step>last-1-i)break;i+=step; }
    return result;
}
static String string_repeat_char(char ch,size_t count) {
    if(!ch && count)tb_fail("NUL is not a text character");
    String result=string_new();
    string_reserve(&result,count);if(count)memset(result.data,ch,count);result.data[count]='\0';result.len=count;return result;
}
static String string_repeat(const String *s,size_t count) {
    if(count && s->len>SIZE_MAX/count)tb_fail("string size overflow");
    size_t length=s->len*count;String result=string_new();string_reserve(&result,length);
    for(size_t i=0;i<count;++i)if(s->len)memcpy(result.data+i*s->len,string_cstr(s),s->len);
    result.data[length]='\0';result.len=length;return result;
}
static void string_append_repeat(String *s,const String *part,size_t count) {
    String repeated=string_repeat(part,count);string_append(s,string_cstr(&repeated));string_free(&repeated);
}
static void string_append_slice(String *s,const String *source,size_t first,size_t count) {
    String slice=string_slice(source,first,count);string_append(s,string_cstr(&slice));string_free(&slice);
}
static void string_pad_left(String *s,size_t width,char fill) {
    if(!fill)tb_fail("NUL is not a text character");
    if(width<=s->len)return;
    size_t padding=width-s->len;string_reserve(s,width);memmove(s->data+padding,s->data,s->len+1);
    memset(s->data,fill,padding);s->len=width;
}
static void string_pad_right(String *s,size_t width,char fill) {
    if(!fill)tb_fail("NUL is not a text character");
    if(width<=s->len)return;
    size_t padding=width-s->len;string_reserve(s,width);memset(s->data+s->len,fill,padding);
    s->len=width;s->data[width]='\0';
}
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

/* SPDX-License-Identifier: MIT-0 */
typedef struct { IOStatus status; String value; } StringReadResult;
typedef struct { bool ok; int value; } StringIntResult;
typedef struct { bool ok; long long value; } StringLongLongResult;
typedef struct { bool ok; double value; } StringDoubleResult;
static StringIntResult string_parse_int(const String *s) {
    StringIntResult r={false,0};if(!s->len)return r;char *end;errno=0;long long x=strtoll(string_cstr(s),&end,10);
    if(errno==ERANGE || end==string_cstr(s) || *end || x<INT_MIN || x>INT_MAX)return r;
    r.ok=true;r.value=(int)x;return r;
}
static StringLongLongResult string_parse_ll(const String *s) {
    StringLongLongResult r={false,0};if(!s->len)return r;char *end;errno=0;long long x=strtoll(string_cstr(s),&end,10);
    if(errno==ERANGE || end==string_cstr(s) || *end)return r;
    r.ok=true;r.value=x;return r;
}
static StringDoubleResult string_parse_double(const String *s) {
    StringDoubleResult r={false,0};if(!s->len)return r;char *end;errno=0;double x=strtod(string_cstr(s),&end);
    if(errno==ERANGE || end==string_cstr(s) || *end || !isfinite(x))return r;
    r.ok=true;r.value=x;return r;
}
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
static void print_string(const String *s,const char *end){print_text(string_cstr(s),end);}

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

/* SPDX-License-Identifier: MIT-0 */
typedef struct {size_t begin,end;} StringRun;
static StringRun string_longest_char_run(const String *s,char target){StringRun best={0,0};size_t start=0;for(size_t i=0;i<s->len;++i){if(s->data[i]!=target)start=i+1;else if(i+1-start>best.end-best.begin)best=(StringRun){start,i+1};}return best;}

static size_t string_find_char(const String *s, char ch, size_t start) {
    if (start >= s->len) return SIZE_MAX;
    const char *found = memchr(s->data + start, (unsigned char)ch, s->len - start);
    return found ? (size_t)(found - s->data) : SIZE_MAX;
}
static bool string_contains_char(const String *s, char ch) {
    return string_find_char(s, ch, 0) != SIZE_MAX;
}
static size_t string_count_char(const String *s, char ch) {
    size_t count = 0;
    for (size_t i = 0; i < s->len; ++i) {
        if (s->data[i] == ch) ++count;
    }
    return count;
}
static size_t string_replace_char(String *s, char from, char to) {
    if (!to) tb_fail("NUL is not a text character");
    size_t count = 0;
    for (size_t i = 0; i < s->len; ++i) {
        if (s->data[i] == from) {
            s->data[i] = to;
            ++count;
        }
    }
    return count;
}
static size_t string_remove_char(String *s, char ch) {
    if (!s->len) return 0;
    size_t kept = 0;
    for (size_t i = 0; i < s->len; ++i) {
        if (s->data[i] != ch) s->data[kept++] = s->data[i];
    }
    size_t removed = s->len - kept;
    s->len = kept;
    s->data[kept] = '\0';
    return removed;
}
static int tb_compare_string_bytes(const void *a, const void *b) {
    unsigned char x = *(const unsigned char *)a;
    unsigned char y = *(const unsigned char *)b;
    return (x > y) - (x < y);
}
static void string_sort_chars(String *s) {
    if (s->len > 1) qsort(s->data, s->len, sizeof *s->data, tb_compare_string_bytes);
}
static size_t string_unique_chars(String *s) {
    if (!s->len) return 0;
    bool seen[(size_t)UCHAR_MAX + 1] = {false};
    size_t kept = 0;
    for (size_t i = 0; i < s->len; ++i) {
        unsigned char ch = (unsigned char)s->data[i];
        if (!seen[ch]) {
            seen[ch] = true;
            s->data[kept++] = s->data[i];
        }
    }
    size_t removed = s->len - kept;
    s->len = kept;
    s->data[kept] = '\0';
    return removed;
}

typedef struct { bool ok; char value; } StringCharResult;
static StringCharResult string_try_pop_char(String *s) {
    StringCharResult result = {false, 0};
    result.ok = string_pop(s, &result.value);
    return result;
}
static char string_pop_char(String *s) {
    StringCharResult result = string_try_pop_char(s);
    if (!result.ok) tb_fail("pop of empty string");
    return result.value;
}

#endif
