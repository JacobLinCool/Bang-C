#ifndef __RANDOM_STRING_H__
#define __RANDOM_STRING_H__

#include "../utils/all.h"
char* random_string(size_t len) {
    char* name = calloc(len + 1, sizeof(char));
    for (size_t i = 0; i < len; i++) {
        name[i] = 'a' + rand() % 26;
    }
    return name;
}

#endif  // __RANDOM_STRING_H__
