/**
 * @file timing.h
 * @author JacobLinCool <hi@jacoblin.cool> (github.com/JacobLinCool)
 * @brief The timing header file of Cimple Lib.
 * @version
 * @date
 *
 * @copyright Copyright (c) 2022 JacobLinCool (MIT License)
 * @see https://github.com/JacobLinCool/Cimple-Lib
 */
#ifndef __CIMPLE_UTILS_TIMING_H
#define __CIMPLE_UTILS_TIMING_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct {
    bool             initialized;
    uint64_t         max_timer;
    uint64_t         timer_size;
    char**           labels;
    struct timespec* __starts;
} __Timing = {
    .initialized = false,
    .max_timer = 16,
    .timer_size = 0,
    .labels = NULL,
    .__starts = NULL,
};

static inline bool __Timing_start(const char* label) {
    if (!__Timing.initialized) {
        __Timing.initialized = true;
        __Timing.labels = (char**)malloc(sizeof(char*) * __Timing.max_timer);
        __Timing.__starts = (struct timespec*)malloc(sizeof(struct timespec) * __Timing.max_timer);
    }

    if (__Timing.timer_size == __Timing.max_timer) {
        __Timing.max_timer <<= 2;
        __Timing.labels = (char**)realloc(__Timing.labels, sizeof(char*) * __Timing.max_timer);
        __Timing.__starts = (struct timespec*)realloc(__Timing.__starts,
                                                      sizeof(struct timespec) * __Timing.max_timer);
    }

    __Timing.labels[__Timing.timer_size] = strdup(label);
    clock_gettime(CLOCK_MONOTONIC, &__Timing.__starts[__Timing.timer_size]);
    __Timing.timer_size++;
    return true;
}

static inline long double __Timing_check(const char* label) {
    for (uint64_t i = 0; i < __Timing.timer_size; i++) {
        if (strcmp(__Timing.labels[i], label) == 0) {
            struct timespec end;
            clock_gettime(CLOCK_MONOTONIC, &end);
            return (long double)((end.tv_sec - __Timing.__starts[i].tv_sec) * 1e3 +
                                 (end.tv_nsec - __Timing.__starts[i].tv_nsec) / 1e6L);
        }
    }
    return (-1.0);
}

static inline bool __Timing_remove(const char* label) {
    for (uint64_t i = 0; i < __Timing.timer_size; i++) {
        if (strcmp(__Timing.labels[i], label) == 0) {
            free(__Timing.labels[i]);
            __Timing.labels[i] = __Timing.labels[__Timing.timer_size - 1];
            __Timing.__starts[i] = __Timing.__starts[__Timing.timer_size - 1];
            __Timing.timer_size--;
            return true;
        }
    }
    return false;
}

static inline size_t __Timing_clear() {
    size_t size = __Timing.timer_size;
    for (uint64_t i = 0; i < __Timing.timer_size; i++) {
        free(__Timing.labels[i]);
    }
    __Timing.timer_size = 0;
    return size;
}

static inline void __Timing_sleep(const uint64_t ms) {
    struct timespec req = {0};
    req.tv_sec = ms / 1000;
    req.tv_nsec = (ms % 1000) * 1e6L;
    nanosleep(&req, NULL);
}

struct {
    /**
     * @brief Start a timer with a label.
     * @param label The label of the timer.
     */
    bool (*start)(const char* label);
    /**
     * @brief Get the time elapsed since the last timing_start() call.
     * @param label The label of the timer.
     * @return long double The time elapsed since the last timing_start() call in milliseconds.
     */
    long double (*check)(const char* label);
    /**
     * @brief Remove a timer with a label.
     * @param label The label of the timer.
     * @return bool True if the timer is removed successfully.
     */
    bool (*remove)(const char* label);
    /**
     * @brief Clear all the timers.
     * @return size_t The number of timers cleared.
     */
    size_t (*clear)();
    /**
     * @brief Sleep for a certain amount of milliseconds.
     * @param ms The amount of milliseconds to sleep.
     */
    void (*sleep)(const uint64_t ms);
} Timing = {
    .start = __Timing_start,
    .check = __Timing_check,
    .remove = __Timing_remove,
    .clear = __Timing_clear,
    .sleep = __Timing_sleep,
};

#endif  // __CIMPLE_UTILS_TIMING_H
