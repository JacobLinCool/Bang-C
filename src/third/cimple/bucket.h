/**
 * @file bucket.h
 * @author JacobLinCool <hi@jacoblin.cool> (github.com/JacobLinCool)
 * @brief The bucket header file of Cimple Lib.
 * @version
 * @date
 *
 * @copyright Copyright (c) 2022 JacobLinCool (MIT License)
 * @see https://github.com/JacobLinCool/Cimple-Lib
 */
#ifndef __CIMPLE_UTILS_BUCKET_H
#define __CIMPLE_UTILS_BUCKET_H

#include <stdlib.h>

static size_t __Bucket_capacity = 0;
static size_t __Bucket_size = 0;
static void** __Bucket_elements = NULL;

void __Bucket_enlarge() {
    if (__Bucket_elements == NULL) {
        __Bucket_capacity = 128;
        __Bucket_elements = malloc(__Bucket_capacity * sizeof(void*));
    } else {
        __Bucket_capacity <<= 1;
        __Bucket_elements = realloc(__Bucket_elements, __Bucket_capacity * sizeof(void*));
    }
}

/**
 * @brief Add a pointer to the bucket.
 * @param element The pointer to be added.
 */
void* $(void* element) {
    if (__Bucket_size == __Bucket_capacity) {
        __Bucket_enlarge();
    }
    __Bucket_elements[__Bucket_size++] = element;
    return element;
}

/**
 * @brief Get the size of the bucket.
 */
size_t $size() { return __Bucket_size; }
/**
 * @brief Get the capacity of the bucket.
 */
size_t $capacity() { return __Bucket_capacity; }

static int pointer_comp(const void* a, const void* b) { return a - b; }

/**
 * @brief Free all the pointers in the bucket.
 */
void $free() {
    qsort(__Bucket_elements, __Bucket_size, sizeof(void*), pointer_comp);
    void* prev = NULL;
    for (size_t i = 0; i < __Bucket_size; i++) {
        if (prev != __Bucket_elements[i]) {
            free(__Bucket_elements[i]);
            prev = __Bucket_elements[i];
        }
    }
    __Bucket_size = 0;
}

/**
 * @brief Remove a pointer from the bucket.
 * @param element The pointer to be removed.
 */
void $remove(void* element) {
    for (size_t i = 0; i < __Bucket_size; i++) {
        if (__Bucket_elements[i] == element) {
            __Bucket_elements[i] = __Bucket_elements[--__Bucket_size];
            i--;
        }
    }
}

#endif  // __CIMPLE_UTILS_BUCKET_H
