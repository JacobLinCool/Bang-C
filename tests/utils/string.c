#include "../../src/utils/string.h"

#include <assert.h>

int main() {
    string test_string = "  Hello World!  ";
    string empty_string = "";
    assert(strcmp(String.trim(test_string), "Hello World!") == 0);
    assert(strcmp(String.trim(empty_string), "") == 0);
    assert(strcmp(String.lower(test_string), "  hello world!  ") == 0);
    assert(strcmp(String.lower(empty_string), "") == 0);
    assert(strcmp(String.upper(test_string), "  HELLO WORLD!  ") == 0);
    assert(strcmp(String.upper(empty_string), "") == 0);

    return EXIT_SUCCESS;
}
