/**
 * @file timing.h
 * @author JacobLinCool <jacoblincool@gmail.com> (github.com/JacobLinCool)
 * @brief The timing header file of Jacob's Helper Library.
 * @version 2.0.0
 * @date 2022-02-25
 *
 * @copyright Copyright (c) 2022 JacobLinCool (MIT License)
 *
 */
#ifndef JACOB_HELPER_TIMING_H
#define JACOB_HELPER_TIMING_H

#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

 // #region Type Alias for Standard Types.
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float f32;
typedef double f64;
typedef long double f128;
typedef char* string;
// #endregion

static bool _timing_initialized = false;
static u32 _timing_max_length = 100;
static u32 _timing_length = 0;
static string* _timing_labels = NULL;
static struct timespec* _timing_starts = NULL;

/**
 * @brief Start a timing with a label.
 * @param label The label of the timing.
 */
static inline void timing_start(const string label) {
    if (!_timing_initialized) {
        _timing_initialized = true;
        _timing_labels = (string*)malloc(sizeof(string) * _timing_max_length);
        _timing_starts = (struct timespec*)malloc(sizeof(struct timespec) * _timing_max_length);
    }

    if (_timing_length == _timing_max_length) {
        _timing_max_length *= 2;
        _timing_labels = (string*)realloc(_timing_labels, sizeof(string) * _timing_max_length);
        _timing_starts = (struct timespec*)realloc(_timing_starts, sizeof(struct timespec) * _timing_max_length);
    }

    _timing_labels[_timing_length] = (string)malloc(sizeof(char) * (strlen(label) + 1));
    strcpy(_timing_labels[_timing_length], label);
    clock_gettime(CLOCK_MONOTONIC, &_timing_starts[_timing_length]);
    _timing_length++;
}

/**
 * @brief Get the time elapsed since the last timing_start() call.
 * @param label The label of the timing.
 * @return f128 The time elapsed since the last timing_start() call in milliseconds.
 */
static inline f128 timing_check(const string label) {
    for (u32 i = 0; i < _timing_length; i++) {
        if (strcmp(_timing_labels[i], label) == 0) {
            struct timespec end;
            clock_gettime(CLOCK_MONOTONIC, &end);
            return (f128)((end.tv_sec - _timing_starts[i].tv_sec) * 1e3 + (end.tv_nsec - _timing_starts[i].tv_nsec) / 1e6L);
        }
    }
    return (-1.0);
}

/**
 * @brief Sleep for a certain amount of milliseconds.
 * @param ms The amount of milliseconds to sleep.
 */
static inline void xsleep(u64 ms) {
    struct timespec req = { 0 };
    req.tv_sec = ms / 1000;
    req.tv_nsec = (ms % 1000) * 1e6L;
    nanosleep(&req, NULL);
}

#endif // JACOB_HELPER_TIMING_H
