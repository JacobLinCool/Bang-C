#ifndef __UTILS_FILE_H
#define __UTILS_FILE_H

#include "base.h"

typedef struct {
    /** Read a text file. */
    string (*read)(const restrict string path);
    /** Write a text file. */
    u64 (*write)(const restrict string path, const restrict string content);
    /** Append a text file. */
    u64 (*append)(const restrict string path, const restrict string content);
    /** Remove a file. */
    bool (*remove)(const restrict string path);
} FileUtils;

string __file_read(const restrict string path) {
    THROW_IF(path == NULL, "Cannot read a null path.");

    FILE* file = fopen(path, "r");
    if (file == NULL) return NULL;

    fseek(file, 0, SEEK_END);
    i32 size = ftell(file);
    fseek(file, 0, SEEK_SET);

    u8 bom[3];
    fread(bom, 1, 3, file);
    if (memcmp(bom, "\xEF\xBB\xBF", 3) == 0)
        size -= 3;
    else
        rewind(file);

    string content = calloc(size + 1, sizeof(char));
    fread(content, sizeof(char), size, file);
    fclose(file);

    return content;
}

u64 __file_write(const restrict string path, const restrict string content) {
    THROW_IF(path == NULL, "Cannot write to a null path.");
    THROW_IF(content == NULL, "Cannot write a null content.");

    FILE* file = fopen(path, "w");
    if (file == NULL) return false;

    u64 wrote = fwrite(content, sizeof(char), strlen(content), file);
    fclose(file);

    return wrote;
}

u64 __file_append(const restrict string path, const restrict string content) {
    THROW_IF(path == NULL, "Cannot append to a null path.");
    THROW_IF(content == NULL, "Cannot append a null content.");

    FILE* file = fopen(path, "a");
    if (file == NULL) return false;

    u64 wrote = fwrite(content, sizeof(char), strlen(content), file);
    fclose(file);

    return wrote;
}

bool __file_remove(const restrict string path) {
    THROW_IF(path == NULL, "Cannot remove a null path.");

    return remove(path) == 0;
}

FileUtils File = {
    .read = __file_read, .write = __file_write, .append = __file_append, .remove = __file_remove};

#endif  // __UTILS_FILE_H
