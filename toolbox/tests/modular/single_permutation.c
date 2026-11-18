#include "c_toolbox.h"
int main(void) {
    int t = read_int();
    if (t < 0) tb_fail("negative test count");
    BoolMatrix method = BoolMatrix_filled(3, 3, true);
    for (int i = 0; i < t; i++) {
        int ABC = read_char()-'A';
        int XYZ = read_char()-'X';
        BoolMatrix_set(&method, ABC, XYZ, false);
    }
    IntVector arrangement = IntVector_range(0, 3, 1);
    StringList output = string_list_new();
    do {
        bool is_valid = true;
        for (int i = 0; i < 3; i++) {
            if (!BoolMatrix_get(&method, i, IntVector_get(&arrangement, i))) {
                is_valid = false;
                break;
            }
        }
        if (is_valid) {
            StringList str_list = string_list_new();
            for (int i = 0; i < 3; i++) {
                String str = string_format("%c-%c", 'A'+i, 'X'+IntVector_get(&arrangement, i));
                string_list_push(&str_list, &str);
                string_free(&str);
            }
            String format_str = string_join(&str_list, " ");
            string_list_push(&output, &format_str);
            string_free(&format_str);
            string_list_free(&str_list);
        }
    }while (IntVector_next_permutation(&arrangement));
    print_int(string_list_size(&output), "\n");
    print_strings(&output, "\n", "");
    string_list_free(&output);
    IntVector_free(&arrangement);
    BoolMatrix_free(&method);
    return 0;
}
