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
