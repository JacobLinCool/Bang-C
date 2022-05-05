/**
 * @file xor-list.h
 * @author JacobLinCool <me@jacoblin.cool> (github.com/JacobLinCool)
 * @brief XOR Linked List, memory efficient doubly linked list
 * @copyright Copyright (c) 2022 JacobLinCool (MIT License)
 * @see https://github.com/JacobLinCool/Universal-Data-Structures
 */

#ifndef __UDS_XOR_LIST_H
#define __UDS_XOR_LIST_H

#define StructXORListNode(_name, _type)                                                            \
    typedef struct _name {                                                                         \
        _type         val;                                                                         \
        struct _name* beacon;                                                                      \
        struct _name* (*xor)(struct _name * node_a, struct _name* node_b);                         \
        struct _name* (*left)(struct _name * this, struct _name* right);                           \
        struct _name* (*right)(struct _name * this, struct _name* left);                           \
        void (*insert)(struct _name * this, struct _name* left, struct _name* right);              \
        void (*remove)(struct _name * this, struct _name* adjacent);                               \
        size_t (*traverse)(struct _name * this, struct _name* next, void (*func)(struct _name*));  \
        void (*free)(struct _name * this);                                                         \
    } _name;                                                                                       \
    _name* xor_##_name(_name* left, _name* right) {                                                \
        return (_name*)((unsigned long)left ^ (unsigned long)right);                               \
    }                                                                                              \
    void insert_##_name(_name* this, _name* left, _name* right) {                                  \
        this->beacon = xor_##_name(left, right);                                                   \
        if (left) {                                                                                \
            left->beacon = xor_##_name(this, xor_##_name(left->beacon, right));                    \
        }                                                                                          \
        if (right) {                                                                               \
            right->beacon = xor_##_name(this, xor_##_name(right->beacon, left));                   \
        }                                                                                          \
    }                                                                                              \
    void remove_##_name(_name* this, _name* adjacent) {                                            \
        _name* another = xor_##_name(this->beacon, adjacent);                                      \
        if (adjacent) {                                                                            \
            adjacent->beacon = xor_##_name(another, xor_##_name(adjacent->beacon, this));          \
        }                                                                                          \
        if (another) {                                                                             \
            another->beacon = xor_##_name(adjacent, xor_##_name(another->beacon, this));           \
        }                                                                                          \
        this->beacon = NULL;                                                                       \
    }                                                                                              \
    size_t traverse_##_name(_name* this, _name* next, void (*func)(_name*)) {                      \
        size_t count = 0;                                                                          \
        _name* current = this;                                                                     \
        while (current) {                                                                          \
            (*func)(current);                                                                      \
            count++;                                                                               \
            _name* prev = current;                                                                 \
            current = next;                                                                        \
            if (current) {                                                                         \
                next = xor_##_name(prev, current->beacon);                                         \
            }                                                                                      \
        }                                                                                          \
        return count;                                                                              \
    }                                                                                              \
    void   free_##_name(_name* this) { free(this); }                                               \
    _name* create_##_name(_type val) {                                                             \
        _name* this = malloc(sizeof(_name));                                                       \
        this->val = val;                                                                           \
        this->beacon = NULL;                                                                       \
        this->xor = xor_##_name;                                                                   \
        this->left = xor_##_name;                                                                  \
        this->right = xor_##_name;                                                                 \
        this->insert = insert_##_name;                                                             \
        this->remove = remove_##_name;                                                             \
        this->traverse = traverse_##_name;                                                         \
        this->free = free_##_name;                                                                 \
        return this;                                                                               \
    }

#endif /* __UDS_XOR_LIST_H */
