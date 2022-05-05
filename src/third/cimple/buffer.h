/**
 * @file buffer.h
 * @author JacobLinCool <hi@jacoblin.cool> (github.com/JacobLinCool)
 * @brief The buffer header file of Cimple Lib.
 * @version 3.0.0
 * @date 2022-05-03
 *
 * @copyright Copyright (c) 2022 JacobLinCool (MIT License)
 * @see https://github.com/JacobLinCool/Cimple-Lib
 */
#ifndef __CIMPLE_UTILS_BUFFER_H
#define __CIMPLE_UTILS_BUFFER_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

bool __buffer_get_bit(const void* buffer, size_t bit) {
    if (buffer == NULL) {
        return false;
    }

    const uint8_t* buffer_ptr = buffer;
    size_t         byte = bit / 8;
    size_t         offset = bit % 8;
    return (buffer_ptr[byte] >> offset) & 1;
}

void __buffer_set_bit(void* buffer, size_t bit, bool value) {
    if (buffer == NULL) {
        return;
    }

    uint8_t* buffer_ptr = buffer;
    size_t   byte = bit / 8;
    size_t   offset = bit % 8;
    if (value) {
        buffer_ptr[byte] |= 1 << offset;
    } else {
        buffer_ptr[byte] &= ~(1 << offset);
    }
}

char* __buffer_stringify(const void* buffer, size_t size, size_t col_size, size_t row_size) {
    if (buffer == NULL) {
        return NULL;
    }

    size_t cols = (size + col_size - 1) / col_size;
    size_t rows = (cols + row_size - 1) / row_size;
    char*  str = malloc(size + cols + rows + 1);

    size_t idx = 0;
    for (size_t i = 0; i < size; i++) {
        str[idx++] = __buffer_get_bit(buffer, i) ? '1' : '0';
        if (i % col_size == col_size - 1) {
            str[idx++] = ' ';
        }
        if (i % (col_size * row_size) == (col_size * row_size) - 1 && i != size - 1) {
            str[idx++] = '\n';
        }
    }
    str[idx] = '\0';

    return str;
}

void __buffer_parse(void* buffer, const char* str, size_t size) {
    if (buffer == NULL || str == NULL) {
        return;
    }

    size_t idx = 0;
    for (size_t i = 0; i < size; i++) {
        if (str[idx] != '0' && str[idx] != '1') {
            idx++, i--;
            continue;
        }
        __buffer_set_bit(buffer, i, str[idx++] == '1');
    }
}

bool __buffer_endian() { return (*(uint16_t*)"\x01\x02") == 0x0201; }

void* __buffer_reverse(const void* buffer, size_t size) {
    if (buffer == NULL) {
        return NULL;
    }

    uint8_t* reversed = malloc(size);
    for (size_t i = 0; i < size; i++) {
        reversed[i] = ((uint8_t*)buffer)[size - i - 1];
    }
    return reversed;
}

void* __buffer_to_big(const void* buffer, size_t size) {
    if (buffer == NULL) {
        return NULL;
    }

    size_t   blocks = (size + 7) / 8;
    size_t   padding = blocks * 8 - size;
    uint8_t* big = calloc(blocks, 1);
    for (size_t b = 0; b < blocks; b++) {
        size_t len = b == blocks - 1 ? 8 - padding : 8;
        for (size_t i = 0; i < len; i++) {
            size_t pos = size - 1 - b * 8 - (7 - i) + (b == blocks - 1 ? padding : 0);
            if (pos < size && pos >= 0) {
                __buffer_set_bit(big, pos, __buffer_get_bit(buffer, b * 8 + i));
            }
        }
    }

    return big;
}

void* __buffer_to_little(const void* buffer, size_t size) {
    if (buffer == NULL) {
        return NULL;
    }

    size_t   blocks = (size + 7) / 8;
    size_t   padding = blocks * 8 - size;
    uint8_t* little = calloc(blocks, 1);
    for (size_t b = 0; b < blocks; b++) {
        size_t len = b == blocks - 1 ? 8 - padding : 8;
        for (size_t i = 0; i < len; i++) {
            size_t pos = b * 8 + i;
            if (pos < size && pos >= 0) {
                __buffer_set_bit(little, pos,
                                 __buffer_get_bit(buffer, size - 1 - b * 8 - (7 - i) +
                                                              (b == blocks - 1 ? padding : 0)));
            }
        }
    }

    return little;
}

/**
 * @brief Buffer utility functions.
 */
struct {
    /**
     * @brief Get bit from buffer
     * @param buffer The buffer
     * @param bit The bit to be get
     */
    bool (*get_bit)(const void* buffer, size_t bit);
    /**
     * @brief Set bit in buffer
     * @param buffer The buffer
     * @param bit The bit to be set
     * @param value The value to be set
     */
    void (*set_bit)(void* buffer, size_t bit, bool value);
    /**
     * @brief Stringify the buffer
     * @param buffer The buffer
     * @param size The size of the buffer, in bits
     * @param col_size The size of each column, in bits
     * @param row_size The size of each row, in columns
     */
    char* (*stringify)(const void* buffer, size_t size, size_t col_size, size_t row_size);
    /**
     * @brief Parse a stringified buffer into a buffer
     * @param buffer The buffer
     * @param str The string to be parsed
     * @param size The size of the buffer, in bits
     */
    void (*parse)(void* buffer, const char* str, size_t size);
    /**
     * @brief Get the endian of the system
     * @return true if the system is little endian, false otherwise
     */
    bool (*endian)();
    /**
     * @brief Reverse the buffer
     * @param buffer The buffer
     * @param size The size of the buffer, in bytes
     */
    void* (*reverse)(const void* buffer, size_t size);
    /**
     * @brief Convert a little endian buffer to big endian
     * @param buffer The buffer
     * @param size The size of the buffer, in bits
     */
    void* (*to_big)(const void* buffer, size_t size);
    /**
     * @brief Convert a big endian buffer to little endian
     * @param buffer The buffer
     * @param size The size of the buffer, in bits
     */
    void* (*to_little)(const void* buffer, size_t size);
} Buffer = {
    .get_bit = __buffer_get_bit,
    .set_bit = __buffer_set_bit,
    .stringify = __buffer_stringify,
    .parse = __buffer_parse,
    .endian = __buffer_endian,
    .reverse = __buffer_reverse,
    .to_big = __buffer_to_big,
    .to_little = __buffer_to_little,
};

#endif  // __CIMPLE_UTILS_BUFFER_H
