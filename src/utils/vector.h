/**
 * @file vector.h
 * @author JacobLinCool <me@jacoblin.cool> (github.com/JacobLinCool)
 * @brief Vector, auto-managed dynamic array of any type.
 * @copyright Copyright (c) 2022 JacobLinCool (MIT License)
 */

#ifndef __C_VECTOR_H
#define __C_VECTOR_H

/**
 * @example Create A Vector Structure
 * StructVector(IntVector, int, INT32_MIN);
 * ^ Create an "int" vector named "IntVector" with fallback value INT32_MIN
 * @see https://github.com/JacobLinCool/Universal-Data-Structures
 */

#define StructVector(_name, _type, _fallback)                                                      \
    typedef struct _name {                                                                         \
        _type* data;                                                                               \
        size_t size;                                                                               \
        size_t capacity;                                                                           \
        void (*insert)(struct _name * this, size_t idx, _type elm);                                \
        _type (*remove)(struct _name * this, size_t idx);                                          \
        void (*set)(struct _name * this, size_t idx, _type elm);                                   \
        _type (*get)(struct _name * this, size_t idx);                                             \
        void (*push)(struct _name * this, _type elm);                                              \
        _type (*pop)(struct _name * this);                                                         \
        _type (*back)(struct _name * this);                                                        \
        void (*unshift)(struct _name * this, _type elm);                                           \
        _type (*shift)(struct _name * this);                                                       \
        _type (*front)(struct _name * this);                                                       \
        void (*clear)(struct _name * this);                                                        \
        void (*free)(struct _name * this);                                                         \
    } _name;                                                                                       \
    void insert_##_name(_name* vector, size_t _idx, _type val) {                                   \
        if (vector->size == vector->capacity) {                                                    \
            vector->capacity <<= 1;                                                                \
            vector->data = realloc(vector->data, vector->capacity * sizeof(_type));                \
        }                                                                                          \
        for (size_t i = vector->size; i > _idx; i--) {                                             \
            vector->data[i] = vector->data[i - 1];                                                 \
        }                                                                                          \
        vector->data[_idx] = val;                                                                  \
        vector->size++;                                                                            \
    }                                                                                              \
    _type remove_##_name(_name* vector, size_t _idx) {                                             \
        if (_idx >= vector->size || _idx < 0) {                                                    \
            return _fallback;                                                                      \
        }                                                                                          \
        _type val = vector->data[_idx];                                                            \
        for (size_t i = _idx; i < vector->size - 1; i++) {                                         \
            vector->data[i] = vector->data[i + 1];                                                 \
        }                                                                                          \
        vector->size--;                                                                            \
        return val;                                                                                \
    }                                                                                              \
    void set_##_name(_name* vector, size_t _idx, _type val) {                                      \
        if (_idx >= vector->size || _idx < 0) {                                                    \
            return;                                                                                \
        }                                                                                          \
        vector->data[_idx] = val;                                                                  \
    }                                                                                              \
    _type get_##_name(_name* vector, size_t _idx) {                                                \
        if (_idx >= vector->size || _idx < 0) {                                                    \
            return _fallback;                                                                      \
        }                                                                                          \
        return vector->data[_idx];                                                                 \
    }                                                                                              \
    void  push_##_name(_name* vector, _type val) { insert_##_name(vector, vector->size, val); }    \
    _type pop_##_name(_name* vector) { return remove_##_name(vector, vector->size - 1); }          \
    _type back_##_name(_name* vector) { return get_##_name(vector, vector->size - 1); }            \
    void  unshift_##_name(_name* vector, _type val) { insert_##_name(vector, 0, val); }            \
    _type shift_##_name(_name* vector) { return remove_##_name(vector, 0); }                       \
    _type front_##_name(_name* vector) { return get_##_name(vector, 0); }                          \
    void  clear_##_name(_name* vector) { vector->size = 0; }                                       \
    void  free_##_name(_name* vector) {                                                            \
        free(vector->data);                                                                       \
        free(vector);                                                                             \
    }                                                                                              \
    _name* create_##_name() {                                                                      \
        _name* vector = malloc(sizeof(_name));                                                     \
        vector->size = 0;                                                                          \
        vector->capacity = 16;                                                                     \
        vector->data = malloc(vector->capacity * sizeof(_type));                                   \
        vector->insert = &insert_##_name;                                                          \
        vector->remove = &remove_##_name;                                                          \
        vector->set = &set_##_name;                                                                \
        vector->get = &get_##_name;                                                                \
        vector->push = &push_##_name;                                                              \
        vector->pop = &pop_##_name;                                                                \
        vector->back = &back_##_name;                                                              \
        vector->unshift = &unshift_##_name;                                                        \
        vector->shift = &shift_##_name;                                                            \
        vector->front = &front_##_name;                                                            \
        vector->clear = &clear_##_name;                                                            \
        vector->free = &free_##_name;                                                              \
        return vector;                                                                             \
    }

#define SV_2(_name, _type, ...) StructVector(_name, _type, 0)
#define SV_SELECT(arg1, arg2, arg3, arg4, ...) arg4
#define SV(_name, args_type_fallback...)                                                           \
    SV_SELECT(_name, ##args_type_fallback, StructVector, SV_2)(_name, ##args_type_fallback)

#define VectorInspect(_vector, _format)                                                            \
    ({                                                                                             \
        printf("\n----------------------------------------\n");                                    \
        printf("Vector \"%s\":\n", #_vector);                                                      \
        printf("\tSize: %zu\n", _vector->size);                                                    \
        printf("\tCapacity: %zu\n", _vector->capacity);                                            \
        printf("\tFront: " _format "\n", _vector->front(_vector));                                 \
        printf("\tBack: " _format "\n", _vector->back(_vector));                                   \
        printf("\tData: \n");                                                                      \
        for (size_t i = 0; i < _vector->size; i++) {                                               \
            if (i % 10 == 0) {                                                                     \
                printf("\t");                                                                      \
            }                                                                                      \
            printf("\t[" _format "] ", _vector->get(_vector, i));                                  \
            if (i % 10 == 9) {                                                                     \
                printf("\n");                                                                      \
            }                                                                                      \
        }                                                                                          \
        printf("\n----------------------------------------\n");                                    \
    })

#define VectorShuffle(_vector)                                                                     \
    ({                                                                                             \
        for (size_t i = 0; i < _vector->size; i++) {                                               \
            size_t j = rand() % _vector->size;                                                     \
                                                                                                   \
            typeof(_vector->data[i]) tmp = _vector->data[i];                                       \
            _vector->data[i] = _vector->data[j];                                                   \
            _vector->data[j] = tmp;                                                                \
        }                                                                                          \
    })

#endif /* __C_VECTOR_H */
