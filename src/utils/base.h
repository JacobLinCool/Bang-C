#ifndef __UTILS_BASE_H
#define __UTILS_BASE_H

// #region Include Common Header Files.
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <math.h>
#include <regex.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// #endregion

// #region Type Alias for Standard Types.
typedef int8_t      i8;
typedef int16_t     i16;
typedef int32_t     i32;
typedef int64_t     i64;
typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;
typedef float       f32;
typedef double      f64;
typedef long double f128;
typedef char*       string;
// #endregion

// #region ANSI Escape Colors.
#define CLR_RED "\x1b[91m"
#define CLR_GREEN "\x1b[92m"
#define CLR_YELLOW "\x1b[93m"
#define CLR_BLUE "\x1b[94m"
#define CLR_MAGENTA "\x1b[95m"
#define CLR_CYAN "\x1b[96m"
#define CLR_NONE "\x1b[0m"
// #endregion

// #region Error Handling.
#define THROW(format, args...)                                                                     \
    ({                                                                                             \
        fprintf(stderr, CLR_RED "[ERROR]" CLR_NONE " " format, ##args);                            \
        fprintf(stderr, CLR_RED "[ERROR]" CLR_NONE " at %s:%d (%s)\n", __FILE__, __LINE__,         \
                __FUNCTION__);                                                                     \
        exit(EXIT_FAILURE);                                                                        \
    })

#define THROW_IF(condition, format, args...)                                                       \
    ({                                                                                             \
        if (condition) {                                                                           \
            THROW(format, ##args);                                                                 \
        }                                                                                          \
    })
// #endregion

#endif  // __UTILS_BASE_H
