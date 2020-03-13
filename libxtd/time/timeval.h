/*
 * TIMEVAL.H --Definitions for timeval functions.
 *
 */
#ifndef TIMEVAL_H
#define TIMEVAL_H

#include <sys/time.h>

#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
#define PRI_TIMEVALUE	"{%ld, %d}"

    typedef struct timeval TimeValue, *TimeValuePtr;

    int tv_set(TimeValuePtr tv, double t);
    int tv_cmp(TimeValuePtr t1, TimeValuePtr t2);
    TimeValuePtr tv_normalise(TimeValuePtr t);
    TimeValuePtr tv_diff(TimeValuePtr t1, TimeValuePtr t2);
    TimeValuePtr tv_diff_r(TimeValuePtr t1, TimeValuePtr t2,
                           TimeValuePtr result);
    TimeValuePtr tv_sum(TimeValuePtr t1, TimeValuePtr t2);
    TimeValuePtr tv_sum_r(TimeValuePtr t1, TimeValuePtr t2,
                          TimeValuePtr result);
    TimeValuePtr tv_scale(TimeValuePtr t1, double scale);
    TimeValuePtr tv_scale_r(TimeValuePtr t1, double scale,
                            TimeValuePtr result);


    char *tv_strftime(TimeValuePtr tv, const char *fmt);
    char *tv_strftime_r(TimeValuePtr tv, const char *fmt,
                        char *text, size_t size);

    extern TimeValue tv_min, tv_max;
#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* TIMEVAL_H */
