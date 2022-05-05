/**
 * @file deque.h
 * @author JacobLinCool <me@jacoblin.cool> (github.com/JacobLinCool)
 * @brief Deque, double ended queue
 * @copyright Copyright (c) 2022 JacobLinCool (MIT License)
 * @see https://github.com/JacobLinCool/Universal-Data-Structures
 */

#include "xor-list.h"  // underlying data structure

#ifndef __UDS_DEQUE_H
#define __UDS_DEQUE_H

#define StructDeque(_name, _type, _fallback)                                                       \
    StructXORListNode(_name##_Node, _type);                                                        \
    typedef struct _name {                                                                         \
        _name##_Node* head;                                                                        \
        _name##_Node* tail;                                                                        \
        size_t        size;                                                                        \
        void (*unshift)(struct _name * this, _type elm);                                           \
        void (*push)(struct _name * this, _type elm);                                              \
        _type (*shift)(struct _name * this);                                                       \
        _type (*pop)(struct _name * this);                                                         \
        _type (*front)(struct _name * this);                                                       \
        _type (*back)(struct _name * this);                                                        \
        void (*reverse)(struct _name * this);                                                      \
        void (*concat)(struct _name * this, struct _name* other);                                  \
        void (*clear)(struct _name * this);                                                        \
        void (*free)(struct _name * this);                                                         \
    } _name;                                                                                       \
    void unshift_##_name(_name* this, _type elm) {                                                 \
        _name##_Node* node = create_##_name##_Node(elm);                                           \
        if (this->head) {                                                                          \
            node->insert(node, NULL, this->head);                                                  \
        }                                                                                          \
        this->head = node;                                                                         \
        if (!this->tail) {                                                                         \
            this->tail = node;                                                                     \
        }                                                                                          \
        this->size++;                                                                              \
    }                                                                                              \
    void push_##_name(_name* this, _type elm) {                                                    \
        _name##_Node* node = create_##_name##_Node(elm);                                           \
        if (this->tail) {                                                                          \
            node->insert(node, this->tail, NULL);                                                  \
        }                                                                                          \
        this->tail = node;                                                                         \
        if (!this->head) {                                                                         \
            this->head = node;                                                                     \
        }                                                                                          \
        this->size++;                                                                              \
    }                                                                                              \
    _type shift_##_name(_name* this) {                                                             \
        if (!this->head) {                                                                         \
            return _fallback;                                                                      \
        }                                                                                          \
        _name##_Node* node = this->head;                                                           \
        _type         val = node->val;                                                             \
        this->head = node->beacon;                                                                 \
        node->remove(node, this->head);                                                            \
        if (!this->head) {                                                                         \
            this->tail = NULL;                                                                     \
        }                                                                                          \
        node->free(node);                                                                          \
        this->size--;                                                                              \
        return val;                                                                                \
    }                                                                                              \
    _type pop_##_name(_name* this) {                                                               \
        if (!this->tail) {                                                                         \
            return _fallback;                                                                      \
        }                                                                                          \
        _name##_Node* node = this->tail;                                                           \
        _type         val = node->val;                                                             \
        this->tail = node->beacon;                                                                 \
        node->remove(node, this->tail);                                                            \
        if (!this->tail) {                                                                         \
            this->head = NULL;                                                                     \
        }                                                                                          \
        node->free(node);                                                                          \
        this->size--;                                                                              \
        return val;                                                                                \
    }                                                                                              \
    _type front_##_name(_name* this) {                                                             \
        if (!this->head) {                                                                         \
            return _fallback;                                                                      \
        }                                                                                          \
        return this->head->val;                                                                    \
    }                                                                                              \
    _type back_##_name(_name* this) {                                                              \
        if (!this->tail) {                                                                         \
            return _fallback;                                                                      \
        }                                                                                          \
        return this->tail->val;                                                                    \
    }                                                                                              \
    void reverse_##_name(_name* this) {                                                            \
        if (!this->head) {                                                                         \
            return;                                                                                \
        }                                                                                          \
        _name##_Node* node = this->head;                                                           \
        this->head = this->tail;                                                                   \
        this->tail = node;                                                                         \
    }                                                                                              \
    void concat_##_name(_name* this, _name* other) {                                               \
        if (!other->head) {                                                                        \
            return;                                                                                \
        }                                                                                          \
        if (!this->head) {                                                                         \
            this->head = other->head;                                                              \
            this->tail = other->tail;                                                              \
            this->size = other->size;                                                              \
            other->head = NULL;                                                                    \
            other->tail = NULL;                                                                    \
            other->size = 0;                                                                       \
            return;                                                                                \
        }                                                                                          \
        _name##_Node* tail_beacon = this->tail->beacon;                                            \
        _name##_Node* head_beacon = other->head->beacon;                                           \
        this->tail->beacon = this->tail->xor (tail_beacon, other->head);                           \
        other->head->beacon = other->head->xor (head_beacon, this->tail);                          \
        this->tail = other->tail;                                                                  \
        this->size += other->size;                                                                 \
        other->head = NULL;                                                                        \
        other->tail = NULL;                                                                        \
        other->size = 0;                                                                           \
    }                                                                                              \
    void clear_##_name(_name* this) {                                                              \
        while (this->head) {                                                                       \
            this->shift(this);                                                                     \
        }                                                                                          \
    }                                                                                              \
    void free_##_name(_name* this) {                                                               \
        this->clear(this);                                                                         \
        free(this);                                                                                \
    }                                                                                              \
    _name* create_##_name() {                                                                      \
        _name* this = (_name*)malloc(sizeof(_name));                                               \
        this->head = NULL;                                                                         \
        this->tail = NULL;                                                                         \
        this->size = 0;                                                                            \
        this->unshift = unshift_##_name;                                                           \
        this->push = push_##_name;                                                                 \
        this->shift = shift_##_name;                                                               \
        this->pop = pop_##_name;                                                                   \
        this->front = front_##_name;                                                               \
        this->back = back_##_name;                                                                 \
        this->reverse = reverse_##_name;                                                           \
        this->concat = concat_##_name;                                                             \
        this->clear = clear_##_name;                                                               \
        this->free = free_##_name;                                                                 \
        return this;                                                                               \
    }

#define DequeInspect(_deque, _format)                                                              \
    ({                                                                                             \
        printf("\n----------------------------------------\n");                                    \
        printf("Deque \"%s\":\n", #_deque);                                                        \
        printf("\tSize: \t%zu\n", _deque->size);                                                   \
        printf("\tFront: \t" _format "\n", _deque->front(_deque));                                 \
        printf("\tBack: \t" _format "\n", _deque->back(_deque));                                   \
        printf("----------------------------------------\n");                                      \
    })

#endif /* __UDS_DEQUE_H */
