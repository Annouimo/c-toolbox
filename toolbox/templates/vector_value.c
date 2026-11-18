/* SPDX-License-Identifier: MIT-0 */
static size_t @NAME@_find_from(const @NAME@ *v, @TYPE@ value, size_t start) {
    for (size_t i = start; i < v->len; ++i) if (v->data[i] == value) return i;
    return SIZE_MAX;
}
static size_t @NAME@_find(const @NAME@ *v, @TYPE@ value) {
    return @NAME@_find_from(v, value, 0);
}
static bool @NAME@_contains(const @NAME@ *v, @TYPE@ value) {
    return @NAME@_find(v, value) != SIZE_MAX;
}
static size_t @NAME@_count(const @NAME@ *v, @TYPE@ value) {
    size_t count = 0;
    for (size_t i = 0; i < v->len; ++i) if (v->data[i] == value) ++count;
    return count;
}
