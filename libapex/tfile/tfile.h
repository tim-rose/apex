/*
 * TFILE.H --An API for writing timestamped data into timestamp-named files.
 *
 */
#ifndef APEX_TFILE_H
#define APEX_TFILE_H

#include <stdio.h>
#include <time.h>
#include <apex.h>

#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
    typedef struct TFILE_t
    {
        FILE *fp;                      /* current open file pointer, if any */
        const char path[FILENAME_MAX]; /* current open file name */
        const char *name_template;     /* strftime(3) spec */
        const char *prologue;          /* strftime(3) spec */
        const char *epilogue;          /* strftime(3) spec */
    } TFILE;

    TFILE *tfopen(const char *name_template, time_t t,
                  const char *prologue, const char *epilogue)
        STRFTIME_ATTRIBUTE(1) STRFTIME_ATTRIBUTE(3) STRFTIME_ATTRIBUTE(4);

    size_t tfwrite(const void *ptr, size_t size, size_t nitems,
                   TFILE * tfp, time_t t);
    size_t tfwrite_time(const char *record_template, TFILE * tfp, time_t t)
        STRFTIME_ATTRIBUTE(1);

    int tfprintf(TFILE * tfp, time_t t, const char *format, ...)
        PRINTF_ATTRIBUTE(3, 4);
    int tfclose(TFILE * tfp, time_t t);

#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* APEX_TFILE_H */
