#include "../../src/utils/file.h"

#include <assert.h>

int main() {
    string test_string = "Hello World!\nH e l l o W o r l d!\n";
    string path = "testfile.txt";

    File.write(path, test_string);
    string content = File.read(path);
    assert(strcmp(content, test_string) == 0);
    File.append(path, "Hello World!\n");
    content = File.read(path);
    assert(strcmp(content, "Hello World!\nH e l l o W o r l d!\nHello World!\n") == 0);
    File.remove(path);

    return EXIT_SUCCESS;
}
