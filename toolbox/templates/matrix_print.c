/* SPDX-License-Identifier: MIT-0 */
static void print_@SUFFIX@_matrix(const @NAME@ *m@PRECISION@,
                                  const char *elem_sep, const char *row_sep, const char *end) {
    for (size_t r = 0; r < m->rows; ++r) {
        if (r) print_text(row_sep, "");
        for (size_t c = 0; c < m->cols; ++c) {
            if (c) print_text(elem_sep, "");
            @TYPE@ value = m->data[r * m->cols + c];
            @PRINT@;
        }
    }
    print_text("", end);
}
