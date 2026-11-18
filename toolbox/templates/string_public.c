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
