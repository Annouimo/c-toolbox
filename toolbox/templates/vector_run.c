/* SPDX-License-Identifier: MIT-0 */
static IndexRange @NAME@_longest_run(const @NAME@ *v, @TYPE@ value) {
    IndexRange best = {0, 0};
    size_t start = 0;
    for (size_t i = 0; i < v->len; ++i) {
        if (!(v->data[i] == value)) start = i + 1;
        else if (i + 1 - start > best.end - best.begin)
            best = (IndexRange){start, i + 1};
    }
    return best;
}
