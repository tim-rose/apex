/*
 * TAP.H --Routines for emitting test results using the TAP protocol.
 *
 * SPDX-FileCopyrightText: 2025 Tim Rose
 * SPDX-License-Identifier: MIT
 *
 * Remarks:
 * This is a re-implementation inspired by Nik Clayton's version.
 */
#ifndef APEX_TAP_H
#define APEX_TAP_H

#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
#define ok(_result, ...) \
    tap_result((_result), __func__, __FILE__, __LINE__, __VA_ARGS__)

#define ok1(_result) \
    tap_result((_result), __func__, __FILE__, __LINE__, "%s", #_result)

#define pass(...) ok(1, __VA_ARGS__)
#define fail(...) ok(0, __VA_ARGS__)

#define skip_start(_test, _n_test, ...)   \
    do {                                  \
        if((_test)) {                     \
            skip(_n_test,  __VA_ARGS__);  \
            break;                        \
        }
#define skip_end } while(0)

#define plan_no_plan()	plan_tests(TAP_NO_PLAN)

    enum TapConstants
    {
        TAP_LINE_MAX = 4096,           /* maximum formatted output record */
        TAP_NO_PLAN = -1               /* sentinel */
    };

    int tap_result(int result, const char *func,
                   const char *file, size_t line, const char *fmt, ...);

    void plan_skip_all(const char *fmt, ...);
    void plan_tests(int n_test);
    void done_testing(void);           /* TODO: not implemented */

    void diag(const char *fmt, ...);

    void skip(size_t n_test, const char *fmt, ...);

    void todo_start(const char *fmt, ...);
    void todo_end(void);

    int exit_status(void);
#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* APEX_TAP_H */
