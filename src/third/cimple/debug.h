/**
 * @file debug.h
 * @author JacobLinCool <hi@jacoblin.cool> (github.com/JacobLinCool)
 * @brief The debug header file of Cimple Lib.
 * @version 3.0.0
 * @date 2022-02-28
 *
 * @copyright Copyright (c) 2022 JacobLinCool (MIT License)
 * @see https://github.com/JacobLinCool/Cimple-Lib
 */
#ifndef __CIMPLE_UTILS_DEBUG_H
#define __CIMPLE_UTILS_DEBUG_H

#include <stdio.h>
#include <stdlib.h>

#ifdef DEBUG
#define DEBUG_ONLY(expr) ({expr})
#define THROW(format, args...)                                                                     \
    ({                                                                                             \
        fprintf(stderr, "\x1b[91m[ERROR]\x1b[0m " format, ##args);                                 \
        fprintf(stderr, "\x1b[91m[ERROR]\x1b[0m at %s:%s (Line %d)\n", __FILE__, __FUNCTION__,     \
                __LINE__);                                                                         \
        exit(EXIT_FAILURE);                                                                        \
    })
#define DEBUG_PRINT(format, args...) ({ printf("\x1b[93m[DEBUG]\x1b[0m " format, ##args); })
#else
#define DEBUG_ONLY(expr) ((void)0)
#define THROW(format, args...)                                                                     \
    ({                                                                                             \
        fprintf(stderr, "\x1b[91m[ERROR]\x1b[0m " format, ##args);                                 \
        exit(EXIT_FAILURE);                                                                        \
    })
#define DEBUG_PRINT(format, args...) ((void)0)
#endif

#define THROW_IF(condition, format, args...)                                                       \
    ({                                                                                             \
        if (condition) {                                                                           \
            THROW(format, ##args);                                                                 \
        }                                                                                          \
    })

#endif  // __CIMPLE_UTILS_DEBUG_H
