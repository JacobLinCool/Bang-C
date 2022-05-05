/**
 * @file string.h
 * @author JacobLinCool <hi@jacoblin.cool> (github.com/JacobLinCool)
 * @brief The string header file of Cimple Lib.
 * @version 3.0.0
 * @date 2022-02-25
 *
 * @copyright Copyright (c) 2022 JacobLinCool (MIT License)
 * @see https://github.com/JacobLinCool/Cimple-Lib
 */
#ifndef __CIMPLE_UTILS_STR_H
#define __CIMPLE_UTILS_STR_H

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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* __String_format(const char* format, ...) {
    CHECK_NULL(format);
    va_list args;
    va_start(args, format);
    size_t length = vsnprintf(NULL, 0, format, args);
    va_end(args);

    char* string = malloc(sizeof(char) * (length + 1));
    va_start(args, format);
    vsprintf(string, format, args);
    va_end(args);

    return string;
}

char* __String_trim(const char* string, const char* charset) {
    CHECK_NULL(string);
    char* chars = (char*)charset;
    if (chars == NULL) chars = " \t\n\r\f\v";

    size_t start = strspn(string, chars);
    size_t end = strlen(string);
    while (end > start && strchr(chars, string[end - 1])) end--;

    char* trimmed = malloc(sizeof(char) * (end - start + 1));
    strncpy(trimmed, string + start, end - start);
    trimmed[end - start] = '\0';

    return trimmed;
}

char* __String_substring(const char* string, size_t start, size_t end) {
    CHECK_NULL(string);
    size_t length = strlen(string);
    if (start > length) start = length;
    if (end > length) end = length;
    if (start < 0) start = 0;
    if (end < 0) end = 0;
    if (start > end) {
        size_t temp = start;
        start = end;
        end = temp;
    }

    char* substring = malloc(sizeof(char) * (end - start + 1));
    strncpy(substring, string + start, end - start);
    substring[end - start] = '\0';

    return substring;
}

size_t* __String_search(const char* string, const char* pattern, size_t* match_size) {
    CHECK_NULL(string);
    CHECK_NULL(pattern);
    size_t  length = strlen(string);
    size_t* positions = malloc(sizeof(size_t) * length);
    size_t  count = 0;
    size_t  pattern_length = strlen(pattern);

    for (size_t i = 0; i < length; i++) {
        if (strncmp(string + i, pattern, pattern_length) == 0) {
            positions[count++] = i;
            i += pattern_length - 1;
        }
    }

    positions = realloc(positions, sizeof(size_t) * count);
    if (match_size != NULL) *match_size = count;
    return positions;
}

char* __String_repeat(const char* string, size_t times, const char* separator) {
    CHECK_NULL(string);
    char* sep = (char*)separator;
    if (sep == NULL) sep = "";
    size_t length = strlen(string);
    size_t new_length = length * times + strlen(sep) * (times - 1);

    char* repeated = malloc(sizeof(char) * (new_length + 1));

    for (size_t i = 0; i < times; i++) {
        strcpy(repeated + i * length, string);
        strcpy(repeated + i * length + length, sep);
    }
    repeated[length] = '\0';

    return repeated;
}

char* __String_replace(const char* string, const char* old, const char* new) {
    CHECK_NULL(string);
    CHECK_NULL(old);
    CHECK_NULL(new);
    size_t  length = strlen(string);
    size_t  match_size = 0;
    size_t* matches = __String_search(string, old, &match_size);

    size_t old_length = strlen(old);
    size_t new_length = strlen(new);

    char* replaced = malloc(sizeof(char) * (length + (new_length - old_length) * match_size + 1));
    strncpy(replaced, string, match_size ? matches[0] : length);

    size_t idx = match_size ? matches[0] : length;
    for (size_t i = 0; i < match_size; i++) {
        memcpy(replaced + idx, new, new_length);
        idx += new_length;
        memcpy(replaced + idx, string + matches[i] + old_length,
               i < match_size - 1 ? matches[i + 1] - matches[i] - old_length
                                  : length - matches[i] - old_length);
        idx += i < match_size - 1 ? matches[i + 1] - matches[i] - old_length
                                  : length - matches[i] - old_length;
    }
    replaced[length + (new_length - old_length) * match_size] = '\0';

    free(matches);
    return replaced;
}

char* __String_upper(const char* string) {
    CHECK_NULL(string);
    size_t length = strlen(string);
    char*  upper = malloc(sizeof(char) * (length + 1));

    for (size_t i = 0; i < length; i++) {
        upper[i] = string[i] >= 'a' && string[i] <= 'z' ? string[i] - 'a' + 'A' : string[i];
    }
    upper[length] = '\0';

    return upper;
}

char* __String_lower(const char* string) {
    CHECK_NULL(string);
    size_t length = strlen(string);
    char*  lower = malloc(sizeof(char) * (length + 1));

    for (size_t i = 0; i < length; i++) {
        lower[i] = string[i] >= 'A' && string[i] <= 'Z' ? string[i] - 'A' + 'a' : string[i];
    }
    lower[length] = '\0';

    return lower;
}

char* __String_reverse(const char* string) {
    CHECK_NULL(string);
    size_t length = strlen(string);
    char*  reversed = malloc(sizeof(char) * (length + 1));

    for (size_t i = 0; i < length; i++) {
        reversed[i] = string[length - i - 1];
    }
    reversed[length] = '\0';

    return reversed;
}

char* __String_pad(const char* string, size_t min_length, const char* padding) {
    CHECK_NULL(string);
    size_t length = strlen(string);
    if (min_length <= length) return strdup(string);

    char* pad = (char*)padding;
    if (pad == NULL) pad = " ";
    size_t pad_length = strlen(pad);

    char* padded = malloc(sizeof(char) * (min_length + 1));
    strncpy(padded + (min_length - length), string, length);
    for (size_t i = 0; i < min_length - length; i += pad_length) {
        strncpy(padded + i * pad_length, pad,
                i + pad_length > min_length ? min_length - i : pad_length);
    }
    padded[min_length] = '\0';

    return padded;
}

char** __String_split(const char* string, const char* separator, size_t* count) {
    CHECK_NULL(string);
    size_t length = strlen(string);
    char*  sep = (char*)separator;
    size_t sep_length = sep == NULL ? 0 : strlen(sep);
    if (sep == NULL || sep_length == 0) {
        sep = " ";
        sep_length = 1;
    }

    size_t size = 0;
    char** slices = malloc(sizeof(char*) * (length / sep_length + 1));

    size_t start_idx = 0;
    for (size_t i = 0; i < length; i++) {
        if (strncmp(string + i, sep, sep_length) == 0) {
            slices[size++] = __String_substring(string, start_idx, i);
            i += sep_length - 1;
            start_idx = i + 1;
        }
    }
    slices[size++] = __String_substring(string, start_idx, length);

    slices = realloc(slices, sizeof(char*) * size);
    if (count != NULL) *count = size;
    return slices;
}

/**
 * @brief String Utility Functions.
 */
struct {
    /**
     * Construct a new string with IO formatting.
     */
    char* (*format)(const char* format, ...);
    /**
     * Trim the chars from the beginning and end of a string and returns the new string.
     * If charset is NULL, the default charset ( \\t\\n\\r\\f\\v ) is used.
     */
    char* (*trim)(const char* string, const char* charset);
    /**
     * Extract a substring from a string and returns the new string.
     */
    char* (*substring)(const char* string, size_t start, size_t end);
    /**
     * Search for a substring in a string and returns the index of occurrences, with size `count`.
     */
    size_t* (*search)(const char* string, const char* pattern, size_t* count);
    /**
     * Repeat a string with separator for a given number of times and returns the new string.
     */
    char* (*repeat)(const char* string, size_t times, const char* separator);
    /**
     * Replace a substring in a string with another substring and returns the new string.
     */
    char* (*replace)(const char* string, const char* old, const char* new);
    /**
     * Make a string uppercase and returns the new string.
     */
    char* (*upper)(const char* string);
    /**
     * Make a string lowercase and returns the new string.
     */
    char* (*lower)(const char* string);
    /**
     * Reverse a string and returns the new string.
     */
    char* (*reverse)(const char* string);
    /**
     * Pad a string with a given string and returns the new string.
     */
    char* (*pad)(const char* string, size_t length, const char* pad);
    /**
     * Split a string with a given separator and returns the new string array.
     */
    char** (*split)(const char* string, const char* delimiter, size_t* count);
} String = {
    .format = __String_format,
    .trim = __String_trim,
    .substring = __String_substring,
    .search = __String_search,
    .repeat = __String_repeat,
    .replace = __String_replace,
    .upper = __String_upper,
    .lower = __String_lower,
    .reverse = __String_reverse,
    .pad = __String_pad,
    .split = __String_split,
};

#endif  // __CIMPLE_UTILS_STR_H
