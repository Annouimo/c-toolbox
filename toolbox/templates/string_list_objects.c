/* SPDX-License-Identifier: MIT-0 */
static StringList string_split_s(const String *s, const String *delimiter) {
    return string_split(s, string_cstr(delimiter));
}
static String string_join_s(const StringList *list, const String *separator) {
    return string_join(list, string_cstr(separator));
}
