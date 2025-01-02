/*
 * TIMEVAL.H --Definitions for timeval functions.
 *
 */
#ifndef APEX_TIMEVAL_H
#define APEX_TIMEVAL_H

#include <sys/time.h>

#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
#define PRI_TIMEVALUE	"{%ld, %d}"

    typedef struct timeval TimeValue;

    int tv_set(TimeValue *tv, double t);
    int tv_cmp(TimeValue *t1, TimeValue *t2);
    TimeValue *tv_normalise(TimeValue *t);
    TimeValue *tv_diff(TimeValue *t1, TimeValue *t2);
    TimeValue *tv_diff_r(TimeValue *t1, TimeValue *t2,
                           TimeValue *result);
    TimeValue *tv_sum(TimeValue *t1, TimeValue *t2);
    TimeValue *tv_sum_r(TimeValue *t1, TimeValue *t2,
                          TimeValue *result);
    TimeValue *tv_scale(TimeValue *t1, double scale);
    TimeValue *tv_scale_r(TimeValue *t1, double scale,
                            TimeValue *result);


    char *tv_strftime(TimeValue *tv, const char *fmt);
    char *tv_strftime_r(TimeValue *tv, const char *fmt,
                        char *text, size_t size);

    extern TimeValue tv_min, tv_max;
#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* APEX_TIMEVAL_H */
