#include "c_toolbox.h"
#include <assert.h>

int main(int argc, char **argv) {
    if (argc == 2) {
        if (argv[1][0] == 'i') {
            IntVector v = IntVector_filled(1, INT_MAX);
            IntVector_add_at(&v, 0, 1);
        } else if (argv[1][0] == 'm') {
            LongLongMatrix m = LongLongMatrix_filled(1, 1, LLONG_MIN);
            LongLongMatrix_add_at(&m, 0, 0, -1);
        } else if (argv[1][0] == 'd') {
            DoubleVector v = DoubleVector_filled(1, 1e308);
            DoubleVector_add_at(&v, 0, 1e308);
        } else if (argv[1][0] == 'v') {
            IntVector v = IntVector_new();
            IntVector_add_at(&v, 0, 1);
        } else if (argv[1][0] == 'n') {
            IntMatrix m = IntMatrix_new(1, 1);
            IntMatrix_add_at(&m, 1, 0, 1);
        } else {
            (void)read_bools(1);
        }
        return 0;
    }

    String s = string_new();
    assert(string_find_char(&s, 'a', 0) == SIZE_MAX);
    assert(string_unique_chars(&s) == 0);
    assert(!string_try_pop_char(&s).ok);
    assert(string_remove_char(&s, 'a') == 0);
    string_assign(&s, "banana");
    assert(string_find_char(&s, 'a', 2) == 3);
    assert(string_find_char(&s, '\0', 0) == SIZE_MAX);
    assert(string_find_char(&s, 'a', 6) == SIZE_MAX);
    assert(string_contains_char(&s, 'b'));
    assert(string_count_char(&s, 'a') == 3);
    assert(string_unique_chars(&s) == 3 && string_equal_text(&s, "ban"));
    assert(string_pop_char(&s) == 'n');
    string_assign(&s, "banana");
    string_sort_chars(&s);
    assert(string_equal_text(&s, "aaabnn"));
    assert(string_replace_char(&s, 'a', 'z') == 3);
    assert(string_remove_char(&s, 'n') == 2 && string_equal_text(&s, "zzzb"));
    string_free(&s);

    StringList list = string_list_new();
    string_list_push_text(&list, "abc");
    string_list_push_text(&list, "abc");
    assert(string_list_find_text(&list, "abc", 1) == 1);
    assert(!string_list_contains_text(&list, "ab"));
    assert(string_list_count_text(&list, "abc") == 2);
    string_list_free(&list);

    IntVector ints = IntVector_values(1, 1, 1, 0, 0, 1, 0, 0);
    IndexRange run = IntVector_longest_run(&ints, 0);
    assert(run.begin == 3 && run.end == 5);
    run = IntVector_longest_run(&ints, 2);
    assert(run.begin == 0 && run.end == 0);
    IntVector_add_at(&ints, 3, -2);
    assert(IntVector_get(&ints, 3) == -2);
    assert(IntVector_find_from(&ints, 0, 5) == 6);
    IntVector_free(&ints);

    LongLongVector wide = LongLongVector_filled(1, LLONG_MIN);
    LongLongVector_add_at(&wide, 0, LLONG_MAX);
    assert(LongLongVector_contains(&wide, -1));
    LongLongVector_free(&wide);
    DoubleVector reals = DoubleVector_filled(1, 0.25);
    DoubleVector_add_at(&reals, 0, 0.5);
    assert(DoubleVector_find(&reals, 0.75) == 0);
    DoubleVector_free(&reals);
    CharVector chars = CharVector_filled(2, '\0');
    assert(CharVector_count(&chars, '\0') == 2);
    CharVector_free(&chars);
    IndexVector indices = IndexVector_filled(2, 7);
    assert(IndexVector_find_from(&indices, 7, 1) == 1);
    IndexVector_free(&indices);

    BoolVector flags = read_bools(3);
    assert(BoolVector_count(&flags, true) == 2);
    BoolVector_free(&flags);
    BoolMatrix grid = read_bool_matrix(2, 2);
    print_bool_matrix(&grid, " ", "\n", "\n");
    BoolMatrix_free(&grid);

    IntMatrix counts = IntMatrix_filled(2, 2, 0);
    IntMatrix_add_at(&counts, 1, 0, 4);
    assert(IntMatrix_sum_border(&counts, 1, 0, 1, 2) == 4);
    IntMatrix_free(&counts);
    LongLongMatrix large = LongLongMatrix_filled(1, 1, LLONG_MAX);
    LongLongMatrix_add_at(&large, 0, 0, LLONG_MIN);
    assert(LongLongMatrix_get(&large, 0, 0) == -1);
    LongLongMatrix_free(&large);
    DoubleMatrix decimals = DoubleMatrix_filled(1, 1, 0.25);
    DoubleMatrix_add_at(&decimals, 0, 0, 0.5);
    assert(DoubleMatrix_get(&decimals, 0, 0) == 0.75);
    DoubleMatrix_free(&decimals);
    return 0;
}
