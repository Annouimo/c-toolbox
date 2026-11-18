#include "ctb/matrix/int.h"
#include "ctb/matrix/long_long.h"
#include "ctb/matrix/double.h"
#include "ctb/matrix/char.h"
#include "ctb/matrix/bool.h"
#include "ctb/matrix/custom.h"
#include "ctb/vector/char.h"
#include "ctb/vector/bool.h"
#include <assert.h>

typedef struct { int score; } Cell;
CTB_MATRIX_DEFINE(CellMatrix, Cell)

int main(void) {
    int rows = read_int(), cols = read_int();
    CharMatrix art = read_char_matrix(rows, cols);
    assert(CharMatrix_get(&art, 0, 0) == ' ');
    assert(CharMatrix_get(&art, 1, 3) == ' ');
    assert(!CharMatrix_in_bounds(&art, -1, 0));
    CharMatrix_draw_rect(&art, 0, 0, 3, 4, '#');
    CharMatrix_fill_col(&art, 2, '+');
    CharMatrix_fill_row(&art, 0, '=');
    CharMatrix_fill_rect(&art, 1, 1, 1, 1, '*');
    CharMatrix part = CharMatrix_slice(&art, 1, 1, 2, 2);
    CharMatrix_fill(&part, '!');
    assert(CharMatrix_get(&art, 1, 1) == '*');
    CharMatrix_free(&part);
    print_char_matrix(&art, "", "\n", "\n");
    CharMatrix_free(&art);

    for (size_t h = 0; h <= 5; ++h) {
        for (size_t w = 0; w <= 5; ++w) {
            IntMatrix m = IntMatrix_filled(7, 7, 100);
            IntMatrix_fill_rect(&m, 1, 1, h, w, 2);
            size_t expected = 0;
            for (size_t r = 0; r < h; ++r)
                for (size_t c = 0; c < w; ++c)
                    if (!r || !c || r + 1 == h || c + 1 == w) ++expected;
            assert(IntMatrix_sum_border(&m, 1, 1, h, w) == 2 * (long long)expected);
            assert(IntMatrix_sum_rect(&m, 1, 1, h, w) == 2 * (long long)(h * w));
            IntMatrix_free(&m);
        }
    }

    LongLongMatrix wide = LongLongMatrix_filled(1, 2, (long long)INT_MAX);
    assert(LongLongMatrix_sum_border(&wide, 0, 0, 1, 2) == 2LL * INT_MAX);
    LongLongMatrix_free(&wide);
    DoubleMatrix fractional = DoubleMatrix_filled(1, 2, 0.25);
    assert(DoubleMatrix_sum_rect(&fractional, 0, 0, 1, 2) == 0.5);
    DoubleMatrix_free(&fractional);

    BoolMatrix visited = BoolMatrix_new(1, 1);
    assert(BoolMatrix_try_get(&visited, 0, 0).ok);
    assert(!BoolMatrix_try_get(&visited, 0, 0).value);
    assert(!BoolMatrix_try_get(&visited, 0, 1).ok);
    assert(!BoolMatrix_try_set(&visited, -1, 0, true));
    assert(BoolMatrix_try_set(&visited, 0, 0, true));
    BoolMatrix_resize(&visited, 2, 2, false);
    assert(BoolMatrix_size(&visited) == 4 && BoolMatrix_get(&visited, 0, 0));
    print_bool_matrix(&visited, ",", "|", "!\n");
    BoolMatrix_free(&visited);
    assert(BoolMatrix_empty(&visited));

    CellMatrix cells = CellMatrix_filled(1, 1, (Cell){7});
    assert(CellMatrix_try_get(&cells, 0, 0).value.score == 7);
    CellMatrix_free(&cells);
    CharVector chars = CharVector_values('a', '\0');
    assert(CharVector_get(&chars, 1) == '\0');
    CharVector_free(&chars);
    BoolVector flags = BoolVector_filled(2, true);
    assert(BoolVector_get(&flags, 1));
    assert(!BoolVector_in_bounds(&flags, -1));
    assert(!BoolVector_try_set(&flags, 2, false));
    BoolVector_set(&flags, 0, false);
    print_bools(&flags, "/", "!\n");
    BoolVector_free(&flags);
}
