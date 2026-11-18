/* SPDX-License-Identifier: MIT-0 */
static CharMatrix read_char_matrix(size_t rows, size_t cols) {
    CharMatrix m = CharMatrix_new(rows, cols);
    for (size_t r = 0; r < rows; ++r) {
        String line = r == 0 ? read_next_line_raw() : read_line_raw();
        if (line.len != cols) {
            string_free(&line);
            CharMatrix_free(&m);
            tb_fail("character matrix row width mismatch");
        }
        if (cols) memcpy(m.data + r * cols, string_cstr(&line), cols);
        string_free(&line);
    }
    return m;
}
