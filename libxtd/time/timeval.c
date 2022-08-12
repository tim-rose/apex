/*
 * timeval.c --timeval manipulation functions.
 *
 * Contents:
 * tv_set()        --Initialise a timeval from an epoch expressed as a double.
 * tv_cmp()        --Compare two normalised timevals.
 * tv_normalise()  --Normalise the values in a timeval struct.
 * tv_diff()       --Calculate the difference between two timevals.
 * tv_diff_r()     --Calculate the difference between two timevals (reentrant).
 * tv_sum()        --Calculate the sum of two timevals.
 * tv_sum_r()      --Calculate the sum of two timevals (reentrant).
 * tv_scale()      --Multiply a tv duration by a scale factor.
 * tv_scale_r()    --Multiply a tv duration by a scale factor (reentrant).
 * tv_strftime()   --Format a TimeValue using a strftime spec.
 * tv_strftime_r() --Format a TimeValue using a strftime spec.
 *
 * Remarks:
 * Actually, sys/time.h includes some macros for doing similar things to this,
 * buy I want to have them as functions too, and I have more paranoid
 * normalisation requirements (sys/time's macros assume that the operands
 * are normalised).
 *
 */
#include <errno.h>
#include <stdio.h>
#include <xtd.h>
#include <xtd/date.h>
#include <xtd/timeval.h>


TimeValue tv_min = { TIME_T_MIN, 0 };
TimeValue tv_max = { TIME_T_MAX, 0 };

/*
 * tv_set() --Initialise a timeval from an epoch expressed as a double.
 *
 * Returns: (int)
 * Success: 1; Failure: 0.
 *
 * Remarks:
 * This routine can fail if t is very too large/negative for a time_t.
 * If it fails, it will set errno to EDOM.
 */
int tv_set(TimeValuePtr tv, double t)
{
    if (t > TIME_T_MAX || t < -1 * TIME_T_MAX)
    {
        errno = EDOM;
        return 0;                      /*  */
    }
    tv->tv_sec = t;
    if (t > 0)
    {
        tv->tv_usec = (t - tv->tv_sec) * 1000000;
    }
    else
    {
        tv->tv_sec -= 1;
        tv->tv_usec = 1 - (t - tv->tv_sec) * 1000000;
    }
    return 1;
}

/*
 * tv_cmp() --Compare two normalised timevals.
 *
 * Returns: (int)
 * Equal: 0; Not Equal: The difference as a signed value (-ve, +ve).
 */
int tv_cmp(TimeValuePtr t1, TimeValuePtr t2)
{
    int result = t1->tv_sec - t2->tv_sec;

    if (result == 0)
    {
        result = t1->tv_usec - t2->tv_usec;
    }
    return result;
}

/*
 * tv_normalise() --Normalise the values in a timeval struct.
 *
 * Remarks:
 * timeval structs store the seconds, microseconds part of a
 * timestamp as separate fields, and after arithmetic operations
 * it is possible that the usec component will be outside its
 * allowed range (i.e. 0..999999).  This routine modifies the struct
 * in-place, adjusting the fields  to restore the usec to its range.
 */
TimeValuePtr tv_normalise(TimeValuePtr t)
{
    if (t->tv_usec < 0)
    {
        int usec = -t->tv_usec;

        t->tv_sec -= usec / 1000000 + 1;
        t->tv_usec = 1000000 - usec % 1000000;
    }
    else if (t->tv_usec >= 1000000)
    {
        int usec = t->tv_usec;

        t->tv_sec += usec / 1000000;
        t->tv_usec = usec % 1000000;
    }
    return t;
}

/*
 * tv_diff() --Calculate the difference between two timevals.
 */
TimeValuePtr tv_diff(TimeValuePtr t1, TimeValuePtr t2)
{
    static TimeValue result_buf;
    return tv_diff_r(t1, t2, &result_buf);
}

/*
 * tv_diff_r() --Calculate the difference between two timevals (reentrant).
 *
 * Remarks:
 * The calculated result is "normalised" such that the tv_usec value
 * is always +ve.
 */
TimeValuePtr tv_diff_r(TimeValuePtr t1, TimeValuePtr t2, TimeValuePtr result)
{
    result->tv_sec = t1->tv_sec - t2->tv_sec;
    result->tv_usec = t1->tv_usec - t2->tv_usec;
    tv_normalise(result);
    return result;
}

/*
 * tv_sum() --Calculate the sum of two timevals.
 */
TimeValuePtr tv_sum(TimeValuePtr t1, TimeValuePtr t2)
{
    static TimeValue result_buf;
    return tv_sum_r(t1, t2, &result_buf);
}

/*
 * tv_sum_r() --Calculate the sum of two timevals (reentrant).
 *
 * Remarks:
 * The calculated result is "normalised" such that the tv_usec value
 * is always within range. It's OK if t1 == result.
 */
TimeValuePtr tv_sum_r(TimeValuePtr t1, TimeValuePtr t2, TimeValuePtr result)
{
    result->tv_sec = t1->tv_sec + t2->tv_sec;
    result->tv_usec = t1->tv_usec + t2->tv_usec;
    tv_normalise(result);
    return result;
}

/*
 * tv_scale() --Multiply a tv duration by a scale factor.
 */
TimeValuePtr tv_scale(TimeValuePtr t1, double scale)
{
    static TimeValue result_buf;
    return tv_scale_r(t1, scale, &result_buf);
}

/*
 * tv_scale_r() --Multiply a tv duration by a scale factor (reentrant).
 */
TimeValuePtr tv_scale_r(TimeValuePtr t1, double scale, TimeValuePtr result)
{
    result->tv_sec = (t1->tv_sec - 1) * scale;
    result->tv_usec = (t1->tv_usec + 1000000) * scale;
    tv_normalise(result);
    return result;
}

/*
 * tv_strftime() --Format a TimeValue using a strftime spec.
 */
char *tv_strftime(TimeValuePtr tv, const char *fmt)
{
    static char text[NAME_MAX];

    return tv_strftime_r(tv, fmt, text, sizeof(text));
}

/*
 * tv_strftime_r() --Format a TimeValue using a strftime spec.
 */
char *tv_strftime_r(TimeValuePtr tv, const char *fmt, char *text, size_t size)
{
    sprintf(fmt_time(text, size, fmt, tv->tv_sec), ".%03d",
            (int) tv->tv_usec / 1000);
    return text;
}
