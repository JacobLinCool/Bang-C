/**
 * @file console.h
 * @author JacobLinCool <hi@jacoblin.cool> (github.com/JacobLinCool)
 * @brief The console header file of Cimple Lib.
 * @version 3.0.0
 * @date 2022-04-30
 *
 * @copyright Copyright (c) 2022 JacobLinCool (MIT License)
 * @see https://github.com/JacobLinCool/Cimple-Lib
 */
#ifndef __CIMPLE_UTILS_CONSOLE_H
#define __CIMPLE_UTILS_CONSOLE_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define __FMT_BLACK "\x1b[30m"
#define __FMT_WHITE "\x1b[37m"
#define __FMT_GRAY "\x1b[90m"
#define __FMT_RED "\x1b[91m"
#define __FMT_GREEN "\x1b[92m"
#define __FMT_YELLOW "\x1b[93m"
#define __FMT_BLUE "\x1b[94m"
#define __FMT_MAGENTA "\x1b[95m"
#define __FMT_CYAN "\x1b[96m"
#define __FMT_NONE "\x1b[0m"
#define __FMT_BOLD "\x1b[1m"
#define __FMT_ITALIC "\x1b[3m"
#define __FMT_UNDERLINE "\x1b[4m"
#define __FMT_INVERSE "\x1b[7m"

static inline char* __Console_fmt(const char* format, va_list args) {
    va_list args_clone;
    va_copy(args_clone, args);
    size_t size = vsnprintf(NULL, 0, format, args_clone);
    va_end(args_clone);
    char* str = malloc((size + 1) * sizeof(char));
    va_copy(args_clone, args);
    vsprintf(str, format, args_clone);
    va_end(args_clone);
    str[size] = '\0';
    return str;
}

size_t __Console_gray(const char* str, ...) {
    va_list args;
    va_start(args, str);

    char*  content = __Console_fmt(str, args);
    size_t printed = printf(__FMT_GRAY "%s" __FMT_NONE, content);
    printf("\n");

    free(content);
    va_end(args);

    return printed;
}

size_t __Console_red(const char* str, ...) {
    va_list args;
    va_start(args, str);

    char*  content = __Console_fmt(str, args);
    size_t printed = printf(__FMT_RED "%s" __FMT_NONE, content);
    printf("\n");

    free(content);
    va_end(args);

    return printed;
}

size_t __Console_green(const char* str, ...) {
    va_list args;
    va_start(args, str);

    char*  content = __Console_fmt(str, args);
    size_t printed = printf(__FMT_GREEN "%s" __FMT_NONE, content);
    printf("\n");

    free(content);
    va_end(args);

    return printed;
}

size_t __Console_yellow(const char* str, ...) {
    va_list args;
    va_start(args, str);

    char*  content = __Console_fmt(str, args);
    size_t printed = printf(__FMT_YELLOW "%s" __FMT_NONE, content);
    printf("\n");

    free(content);
    va_end(args);

    return printed;
}

size_t __Console_blue(const char* str, ...) {
    va_list args;
    va_start(args, str);

    char*  content = __Console_fmt(str, args);
    size_t printed = printf(__FMT_BLUE "%s" __FMT_NONE, content);
    printf("\n");

    free(content);
    va_end(args);

    return printed;
}

size_t __Console_magenta(const char* str, ...) {
    va_list args;
    va_start(args, str);

    char*  content = __Console_fmt(str, args);
    size_t printed = printf(__FMT_MAGENTA "%s" __FMT_NONE, content);
    printf("\n");

    free(content);
    va_end(args);

    return printed;
}

size_t __Console_cyan(const char* str, ...) {
    va_list args;
    va_start(args, str);

    char*  content = __Console_fmt(str, args);
    size_t printed = printf(__FMT_CYAN "%s" __FMT_NONE, content);
    printf("\n");

    free(content);
    va_end(args);

    return printed;
}

size_t __Console_success(const char* str, ...) {
    va_list args;
    va_start(args, str);

    char*  content = __Console_fmt(str, args);
    size_t printed = printf(__FMT_GREEN "[SUCCESS]" __FMT_NONE " %s", content);
    printf("\n");

    free(content);
    va_end(args);

    return printed;
}

size_t __Console_info(const char* str, ...) {
    va_list args;

    va_start(args, str);
    char* content = __Console_fmt(str, args);
    va_end(args);
    size_t printed = printf(__FMT_BLUE "[INFO]" __FMT_NONE " %s", content);
    printf("\n");

    free(content);

    return printed;
}

size_t __Console_warn(const char* str, ...) {
    va_list args;
    va_start(args, str);

    char*  content = __Console_fmt(str, args);
    size_t printed = printf(__FMT_YELLOW "[WARNING]" __FMT_NONE " %s", content);
    printf("\n");

    free(content);
    va_end(args);

    return printed;
}

size_t __Console_error(const char* str, ...) {
    va_list args;
    va_start(args, str);

    char*  content = __Console_fmt(str, args);
    size_t printed = printf(__FMT_RED "[ERROR]" __FMT_NONE " %s", content);
    printf("\n");

    free(content);
    va_end(args);

    return printed;
}

size_t __Console_rainbow(const char* str, ...) {
    va_list args;
    va_start(args, str);

    size_t printed = 0;
    char*  content = __Console_fmt(str, args);
    for (size_t i = 0; i < strlen(content); i++) {
        switch (i % 6) {
            case 0:
                printed += printf(__FMT_RED "%c" __FMT_NONE, content[i]);
                break;
            case 1:
                printed += printf(__FMT_GREEN "%c" __FMT_NONE, content[i]);
                break;
            case 2:
                printed += printf(__FMT_YELLOW "%c" __FMT_NONE, content[i]);
                break;
            case 3:
                printed += printf(__FMT_BLUE "%c" __FMT_NONE, content[i]);
                break;
            case 4:
                printed += printf(__FMT_MAGENTA "%c" __FMT_NONE, content[i]);
                break;
            case 5:
                printed += printf(__FMT_CYAN "%c" __FMT_NONE, content[i]);
                break;
            default:
                break;
        }
    }
    printf("\n");

    free(content);
    va_end(args);

    return printed;
}

void __Console_log(const char* str, ...) {
    time_t     raw;
    struct tm* info;

    time(&raw);
    info = localtime(&raw);

    printf("[%04d-%02d-%02d %02d:%02d:%02d] ", info->tm_year + 1900, info->tm_mon + 1,
           info->tm_mday, info->tm_hour, info->tm_min, info->tm_sec);

    va_list args;
    va_start(args, str);

    char* content = __Console_fmt(str, args);
    printf("%s\n", content);

    free(content);
    va_end(args);

    return;
}

struct {
    size_t (*gray)(const char* str, ...);
    size_t (*red)(const char* str, ...);
    size_t (*green)(const char* str, ...);
    size_t (*blue)(const char* str, ...);
    size_t (*yellow)(const char* str, ...);
    size_t (*magenta)(const char* str, ...);
    size_t (*cyan)(const char* str, ...);

    size_t (*success)(const char* str, ...);
    size_t (*info)(const char* str, ...);
    size_t (*warn)(const char* str, ...);
    size_t (*error)(const char* str, ...);

    /** Logs a message to the console, with time prefix */
    void (*log)(const char* str, ...);

    /** Print in rainbow colors */
    size_t (*rainbow)(const char* str, ...);

    /** Color constants */
    struct {
        const char* GRAY;
        const char* RED;
        const char* GREEN;
        const char* BLUE;
        const char* YELLOW;
        const char* MAGENTA;
        const char* CYAN;
    } C;
} Console = {.gray = __Console_gray,
             .red = __Console_red,
             .green = __Console_green,
             .blue = __Console_blue,
             .yellow = __Console_yellow,
             .magenta = __Console_magenta,
             .cyan = __Console_cyan,

             .success = __Console_success,
             .info = __Console_info,
             .warn = __Console_warn,
             .error = __Console_error,
             .log = __Console_log,

             .rainbow = __Console_rainbow,

             .C = {
                 .GRAY = __FMT_GRAY,
                 .RED = __FMT_RED,
                 .GREEN = __FMT_GREEN,
                 .BLUE = __FMT_BLUE,
                 .YELLOW = __FMT_YELLOW,
                 .MAGENTA = __FMT_MAGENTA,
                 .CYAN = __FMT_CYAN,
             }};

#endif  // __CIMPLE_UTILS_CONSOLE_H
