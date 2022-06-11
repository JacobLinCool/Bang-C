/**
 * @file vector.h
 * @author JacobLinCool <me@jacoblin.cool> (github.com/JacobLinCool)
 * @brief Vector, auto-managed dynamic array of any type.
 * @copyright Copyright (c) 2022 JacobLinCool (MIT License)
 * @see https://github.com/JacobLinCool/Universal-Data-Structures
 */

#ifndef __UDS_VECTOR_H
#define __UDS_VECTOR_H

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
        void (*resize)(struct _name * this, size_t size);                                          \
        /** Utilities */                                                                           \
        void (*swap)(struct _name * this, size_t idx1, size_t idx2);                               \
        void (*sort)(struct _name * this, int (*compare)(const _type*, const _type*));             \
        void (*shuffle)(struct _name * this);                                                      \
        struct _name* (*concat)(struct _name * this, struct _name* other);                         \
        struct _name* (*slice)(struct _name * this, size_t start, size_t end);                     \
        struct _name* (*clone)(struct _name * this);                                               \
        struct _name* (*map)(struct _name * this,                                                  \
                             _type (*func)(_type elm, size_t idx, struct _name* vec));             \
        struct _name* (*each)(struct _name * this,                                                 \
                              void (*func)(_type elm, size_t idx, struct _name* vec));             \
    } _name;                                                                                       \
    _name* create_##_name();                                                                       \
    void   insert_##_name(_name* vector, size_t _idx, _type val) {                                 \
          if (vector->size == vector->capacity) {                                                  \
              vector->capacity <<= 1;                                                              \
              vector->data = realloc(vector->data, vector->capacity * sizeof(_type));              \
        }                                                                                        \
          for (size_t i = vector->size; i > _idx; i--) {                                           \
              vector->data[i] = vector->data[i - 1];                                               \
        }                                                                                        \
          vector->data[_idx] = val;                                                                \
          vector->size++;                                                                          \
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
    void resize_##_name(_name* vector, size_t size) {                                              \
        vector->data = realloc(vector->data, size * sizeof(_type));                                \
        vector->capacity = size;                                                                   \
        if (vector->size > size) {                                                                 \
            vector->size = size;                                                                   \
        }                                                                                          \
    }                                                                                              \
    void swap_##_name(_name* vector, size_t idx1, size_t idx2) {                                   \
        if (idx1 >= vector->size || idx2 >= vector->size) {                                        \
            return;                                                                                \
        }                                                                                          \
        _type tmp = vector->data[idx1];                                                            \
        vector->data[idx1] = vector->data[idx2];                                                   \
        vector->data[idx2] = tmp;                                                                  \
    }                                                                                              \
    void sort_##_name(_name* vector, int (*compare)(const _type*, const _type*)) {                 \
        qsort(vector->data, vector->size, sizeof(_type),                                           \
              (int (*)(const void*, const void*))compare);                                         \
    }                                                                                              \
    void shuffle_##_name(_name* vector) {                                                          \
        for (size_t i = 0; i < vector->size; i++) {                                                \
            size_t j = rand() % vector->size;                                                      \
            swap_##_name(vector, i, j);                                                            \
        }                                                                                          \
    }                                                                                              \
    _name* concat_##_name(_name* this, _name* other) {                                             \
        _name* new_vector = create_##_name();                                                      \
        new_vector->size = this->size + other->size;                                               \
        new_vector->capacity = new_vector->size;                                                   \
        new_vector->data = malloc(new_vector->capacity * sizeof(_type));                           \
        memcpy(new_vector->data, this->data, this->size * sizeof(_type));                          \
        memcpy(new_vector->data + this->size, other->data, other->size * sizeof(_type));           \
        return new_vector;                                                                         \
    }                                                                                              \
    _name* slice_##_name(_name* this, size_t start, size_t end) {                                  \
        if (start >= this->size) {                                                                 \
            return NULL;                                                                           \
        }                                                                                          \
        if (end > this->size) {                                                                    \
            end = this->size;                                                                      \
        }                                                                                          \
        _name* new_vector = create_##_name();                                                      \
        new_vector->size = end - start;                                                            \
        new_vector->capacity = new_vector->size;                                                   \
        new_vector->data = malloc(new_vector->capacity * sizeof(_type));                           \
        memcpy(new_vector->data, this->data + start, new_vector->size * sizeof(_type));            \
        return new_vector;                                                                         \
    }                                                                                              \
    _name* clone_##_name(_name* this) {                                                            \
        _name* new_vector = create_##_name();                                                      \
        new_vector->size = this->size;                                                             \
        new_vector->capacity = new_vector->size;                                                   \
        new_vector->data = malloc(new_vector->capacity * sizeof(_type));                           \
        memcpy(new_vector->data, this->data, new_vector->size * sizeof(_type));                    \
        return new_vector;                                                                         \
    }                                                                                              \
    _name* map_##_name(_name* this, _type (*func)(_type elm, size_t idx, struct _name * vec)) {    \
        _name* new_vector = create_##_name();                                                      \
        new_vector->size = this->size;                                                             \
        new_vector->capacity = new_vector->size;                                                   \
        new_vector->data = malloc(new_vector->capacity * sizeof(_type));                           \
        for (size_t i = 0; i < this->size; i++) {                                                  \
            new_vector->data[i] = func(this->data[i], i, this);                                    \
        }                                                                                          \
        return new_vector;                                                                         \
    }                                                                                              \
    _name* each_##_name(_name* this, void (*func)(_type elm, size_t idx, struct _name * vec)) {    \
        for (size_t i = 0; i < this->size; i++) {                                                  \
            func(this->data[i], i, this);                                                          \
        }                                                                                          \
        return this;                                                                               \
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
        vector->resize = &resize_##_name;                                                          \
        vector->swap = &swap_##_name;                                                              \
        vector->sort = &sort_##_name;                                                              \
        vector->shuffle = &shuffle_##_name;                                                        \
        vector->concat = &concat_##_name;                                                          \
        vector->slice = &slice_##_name;                                                            \
        vector->clone = &clone_##_name;                                                            \
        vector->map = &map_##_name;                                                                \
        vector->each = &each_##_name;                                                              \
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

#endif /* __UDS_VECTOR_H */
