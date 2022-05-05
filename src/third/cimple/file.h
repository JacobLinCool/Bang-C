/**
 * @file file.h
 * @author JacobLinCool <hi@jacoblin.cool> (github.com/JacobLinCool)
 * @brief The file header file of Cimple Lib.
 * @version 3.0.0
 * @date 2022-05-01
 *
 * @copyright Copyright (c) 2022 JacobLinCool (MIT License)
 * @see https://github.com/JacobLinCool/Cimple-Lib
 */
#ifndef __CIMPLE_UTILS_FILE_H
#define __CIMPLE_UTILS_FILE_H

#ifdef DEBUG
#define CHECK_NULL(ptr)                                                                            \
    ({                                                                                             \
        if (ptr == NULL) {                                                                         \
            fprintf(stderr, "Error: %s:%s: %s is NULL.\n", __FILE__, __func__, #ptr);              \
            exit(EXIT_FAILURE);                                                                    \
        }                                                                                          \
    })
#else
#define CHECK_NULL(ptr) (void)0
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* __file_read(const char* path) {
    CHECK_NULL(path);

    FILE* file = fopen(path, "r");
    if (file == NULL) return NULL;

    fseek(file, 0, SEEK_END);
    int32_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    uint8_t bom[3];
    fread(bom, 1, 3, file);
    if (memcmp(bom, "\xEF\xBB\xBF", 3) == 0)
        size -= 3;
    else
        rewind(file);

    char* content = calloc(size + 1, sizeof(char));
    fread(content, sizeof(char), size, file);
    fclose(file);

    return content;
}

uint64_t __file_write(const char* path, const char* content) {
    CHECK_NULL(path);
    CHECK_NULL(content);

    FILE* file = fopen(path, "w");
    if (file == NULL) return false;

    uint64_t wrote = fwrite(content, sizeof(char), strlen(content), file);
    fclose(file);

    return wrote;
}

uint64_t __file_append(const char* path, const char* content) {
    CHECK_NULL(path);
    CHECK_NULL(content);

    FILE* file = fopen(path, "a");
    if (file == NULL) return false;

    uint64_t wrote = fwrite(content, sizeof(char), strlen(content), file);
    fclose(file);

    return wrote;
}

bool __file_remove(const char* path) {
    CHECK_NULL(path);
    return remove(path) == 0;
}

struct {
    /**
     * @brief Read a text file. (UTF-8, ASCII)
     *
     * @param path The path of the file.
     * @return char* The content of the file.
     */
    char* (*read)(const char* path);
    /**
     * @brief Write a text file. (ASCII)
     *
     * @param path The path of the file.
     * @param content The content of the file.
     * @return uint64_t The number of bytes wrote.
     */
    uint64_t (*write)(const char* path, const char* content);
    /**
     * @brief Append a text file. (ASCII)
     *
     * @param path The path of the file.
     * @param content The content of the file.
     * @return uint64_t The number of bytes wrote.
     */
    uint64_t (*append)(const char* path, const char* content);
    /**
     * @brief Remove a file.
     *
     * @param path The path of the file.
     * @return bool True if success, false otherwise.
     */
    bool (*remove)(const char* path);
} File = {
    .read = __file_read,
    .write = __file_write,
    .append = __file_append,
    .remove = __file_remove,
};

#endif  // __CIMPLE_UTILS_FILE_H
